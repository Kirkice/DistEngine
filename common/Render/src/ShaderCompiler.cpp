/* $Header: /common/Render/src/ShaderCompiler.cpp		8/21/21 18:26p Kirk 			    $ */
/*--------------------------------------------------------------------------------------------*
*                                                                                             *
*						Project Name : DistEngine                                             *
*                                                                                             *
*						File Name : ShaderCompiler.cpp										  *
*                                                                                             *
*                       Programmer : Kirk                                                     *
*                                                                                             *
*---------------------------------------------------------------------------------------------*/
#include "../../Core/include/PCH.h"

#include "../../Render/include/ShaderCompiler.h"
#include "../../Render/include/D3D12.h"

#include "../../Core/include/Utility.h"
#include "../../Core/include/Expections.h"
#include "../../Core/include/InterfacePointers.h"
#include "../../Core/include/FileIO.h"
#include "../../Core/include/MurmurHash.h"
#include "../../Core/include/Containers.h"

using std::vector;
using std::wstring;
using std::string;
using std::map;
using namespace core;

namespace render
{
	static const uint64 CacheVersion = 1;

	static const char* TypeStrings[] = { "vertex", "hull", "domain", "geometry", "pixel", "compute", "lib" };
	StaticAssert_(ArraySize_(TypeStrings) == uint64(ShaderType::NumTypes));

	static const char* ProfileStrings[] =
	{
		"vs_6_1", "hs_6_1", "ds_6_1", "gs_6_1", "ps_6_1", "cs_6_1", "lib_6_5",
	};

	StaticAssert_(ArraySize_(ProfileStrings) == uint64(ShaderType::NumTypes));

	static Hash MakeCompilerHash()
	{
		HMODULE module = LoadLibrary(L"dxcompiler.dll");

		if (module == nullptr)
			throw Exception(L"Failed to load compiler DLL");

		wchar dllPath[1024] = { };
		GetModuleFileName(module, dllPath, ArraySize_(dllPath));

		File dllFile(dllPath, FileOpenMode::Read);
		uint64 fileSize = dllFile.Size();
		Array<uint8> fileData(fileSize);
		dllFile.Read(fileSize, fileData.Data());

		return GenerateHash(fileData.Data(), int32(fileSize));
	}

	static Hash CompilerHash = MakeCompilerHash();

	static string GetExpandedShaderCode(const wchar* path, GrowableList<wstring>& filePaths)
	{
		for (uint64 i = 0; i < filePaths.Count(); ++i)
			if (filePaths[i] == path)
				return string();

		filePaths.Add(path);

		string fileContents = ReadFileAsString(path);

		wstring fileDirectory = GetDirectoryFromFilePath(path);
		if (fileDirectory.length() > 0)
			fileDirectory += L"\\";

		// Look for includes
		size_t lineStart = 0;
		while (true)
		{
			size_t lineEnd = fileContents.find('\n', lineStart);
			size_t lineLength = 0;
			if (lineEnd == string::npos)
				lineLength = string::npos;
			else
				lineLength = lineEnd - lineStart;

			string line = fileContents.substr(lineStart, lineLength);
			if (line.find("#include") == 0)
			{
				wstring fullIncludePath;
				size_t startQuote = line.find('\"');
				if (startQuote != -1)
				{
					size_t endQuote = line.find('\"', startQuote + 1);
					string includePath = line.substr(startQuote + 1, endQuote - startQuote - 1);
					fullIncludePath = fileDirectory + AnsiToWString(includePath.c_str());
				}
				else
				{
					startQuote = line.find('<');
					if (startQuote == -1)
						throw Exception(L"Malformed include statement: \"" + AnsiToWString(line.c_str()) + L"\" in file " + path);
					size_t endQuote = line.find('>', startQuote + 1);
					string includePath = line.substr(startQuote + 1, endQuote - startQuote - 1);
					fullIncludePath = SampleFrameworkDir() + L"Shaders\\" + AnsiToWString(includePath.c_str());
				}

				if (FileExists(fullIncludePath.c_str()) == false)
					throw Exception(L"Couldn't find #included file \"" + fullIncludePath + L"\" in file " + path);

				string includeCode = GetExpandedShaderCode(fullIncludePath.c_str(), filePaths);
				fileContents.insert(lineEnd + 1, includeCode);
				lineEnd += includeCode.length();
			}

			if (lineEnd == string::npos)
				break;

			lineStart = lineEnd + 1;
		}

		return fileContents;
	}

	static const wstring baseCacheDir = L"ShaderCache\\";

#if _DEBUG
	static const wstring cacheSubDir = L"Debug\\";
#else
	static const std::wstring cacheSubDir = L"Release\\";
#endif

	static const wstring cacheDir = baseCacheDir + cacheSubDir;

	static string MakeDefinesString(const D3D_SHADER_MACRO* defines)
	{
		string definesString = "";
		while (defines && defines->Name != nullptr && defines != nullptr)
		{
			if (definesString.length() > 0)
				definesString += "|";
			definesString += defines->Name;
			definesString += "=";
			definesString += defines->Definition;
			++defines;
		}

		return definesString;
	}

	static wstring MakeShaderCacheName(const std::string& shaderCode, const char* functionName,
		const char* profile, const D3D_SHADER_MACRO* defines)
	{
		string hashString = shaderCode;
		hashString += "\n";
		if (functionName != nullptr)
		{
			hashString += functionName;
			hashString += "\n";
		}
		hashString += profile;
		hashString += "\n";

		hashString += MakeDefinesString(defines);

		hashString += ToAnsiString(CacheVersion);

		Hash codeHash = GenerateHash(hashString.data(), int(hashString.length()), 0);
		codeHash = CombineHashes(codeHash, CompilerHash);

		return cacheDir + codeHash.ToString() + L".cache";
	}

	static HRESULT CompileShaderDXC(const wchar* path, const D3D_SHADER_MACRO* defines, const char* functionName,
		const char* profileString, IDxcBlobPtr& compiledShader, IDxcBlobEncodingPtr& errorMessages)
	{
		IDxcLibraryPtr library;
		DXCall(DxcCreateInstance(CLSID_DxcLibrary, IID_PPV_ARGS(&library)));

		IDxcBlobEncodingPtr sourceCode;
		DXCall(library->CreateBlobFromFile(path, nullptr, &sourceCode));

		IDxcCompilerPtr compiler;
		DXCall(DxcCreateInstance(CLSID_DxcCompiler, IID_PPV_ARGS(&compiler)));

		// Convert the defines to wide strings
		uint64 numDefines = 0;
		while (defines && defines[numDefines].Name)
			++numDefines;

		const uint64 extraDefines = 2;
		uint64 totalNumDefines = numDefines + extraDefines;

		Array<DxcDefine> dxcDefines(totalNumDefines);
		Array<wstring> defineStrings(numDefines * 2);
		for (uint64 i = 0; i < numDefines; ++i)
		{
			defineStrings[i * 2 + 0] = AnsiToWString(defines[i].Name);
			defineStrings[i * 2 + 1] = AnsiToWString(defines[i].Definition);
			dxcDefines[i].Name = defineStrings[i * 2 + 0].c_str();
			dxcDefines[i].Value = defineStrings[i * 2 + 1].c_str();
		}

		dxcDefines[numDefines + 0].Name = L"DXC_";
		dxcDefines[numDefines + 0].Value = L"1";
		dxcDefines[numDefines + 1].Name = L"SM60_";
		dxcDefines[numDefines + 1].Value = L"1";

		wstring frameworkShaderDir = SampleFrameworkDir() + L"Shaders";
		wchar expandedFrameworkShaderDir[1024] = { };
		GetFullPathName(frameworkShaderDir.c_str(), ArraySize_(expandedFrameworkShaderDir), expandedFrameworkShaderDir, nullptr);

		const wchar* arguments[] =
		{
			L"/O3",
			L"-all_resources_bound",
			L"-WX",
			L"-I",
			expandedFrameworkShaderDir,

			// LLVM debug info seems to make the RT PSO croak
			#if Debug_ && !EnableDXR_
				L"/Zi",
			#endif
		};

		IDxcIncludeHandlerPtr includeHandler;
		DXCall(library->CreateIncludeHandler(&includeHandler));

		IDxcOperationResultPtr operationResult;
		DXCall(compiler->Compile(sourceCode.Get(), path, functionName ? AnsiToWString(functionName).c_str() : L"",
			AnsiToWString(profileString).c_str(), arguments,
			ArraySize_(arguments), dxcDefines.Data(), uint32(dxcDefines.Size()),
			includeHandler.Get(), &operationResult));

		HRESULT hr = S_OK;
		operationResult->GetStatus(&hr);
		if (SUCCEEDED(hr))
			DXCall(operationResult->GetResult(&compiledShader));
		else
			operationResult->GetErrorBuffer(&errorMessages);

		return hr;
	}

	static void CompileShader(const wchar* path, const char* functionName, ShaderType type,
		const D3D_SHADER_MACRO* defines, GrowableList<wstring>& filePaths,
		Array<uint8>& byteCode)
	{
		if (FileExists(path) == false)
		{
			Assert_(false);
			throw Exception(L"Shader file " + std::wstring(path) + L" does not exist");
		}

		uint64 profileIdx = uint64(type);
		Assert_(profileIdx < ArraySize_(ProfileStrings));
		const char* profileString = ProfileStrings[profileIdx];

		// Make a hash off the expanded shader code
		string shaderCode = GetExpandedShaderCode(path, filePaths);
		wstring cacheName = MakeShaderCacheName(shaderCode, functionName, profileString, defines);

		if (FileExists(cacheName.c_str()))
		{
			ReadFileAsByteArray(cacheName.c_str(), byteCode);
			return;
		}

		if (type == ShaderType::Library)
		{
			WriteLog("Compiling shader library %ls %s\n", GetFileName(path).c_str(),
				MakeDefinesString(defines).c_str());
		}
		else
		{
			WriteLog("Compiling %s shader %ls_%s %s\n", TypeStrings[uint64(type)],
				GetFileName(path).c_str(), functionName, MakeDefinesString(defines).c_str());
		}

		// Loop until we succeed, or an exception is thrown
		while (true)
		{
			IDxcBlobPtr compiledShader;
			IDxcBlobEncodingPtr errorMessages;

			HRESULT hr = CompileShaderDXC(path, defines, functionName, profileString, compiledShader, errorMessages);
			if (FAILED(hr))
			{
				if (errorMessages)
				{
					const char* errMsgStr = reinterpret_cast<const char*>(errorMessages->GetBufferPointer());
					std::wstring fullMessage = MakeString(L"Error compiling shader file \"%s\" - %hs", path, errMsgStr);

					// Pop up a message box allowing user to retry compilation
					int32 retVal = MessageBoxW(nullptr, fullMessage.c_str(), L"Shader Compilation Error", MB_RETRYCANCEL);
					if (retVal != IDRETRY)
						throw DXException(hr, fullMessage.c_str());
				}
				else
				{
					Assert_(false);
					throw DXException(hr);
				}
			}
			else
			{
				// Create the cache directory if it doesn't exist
				if (DirectoryExists(baseCacheDir.c_str()) == false)
					Win32Call(CreateDirectory(baseCacheDir.c_str(), nullptr));

				if (DirectoryExists(cacheDir.c_str()) == false)
					Win32Call(CreateDirectory(cacheDir.c_str(), nullptr));

				File cacheFile(cacheName.c_str(), FileOpenMode::Write);

				// Write the compiled shader to disk
				const uint64 shaderSize = compiledShader->GetBufferSize();
				cacheFile.Write(shaderSize, compiledShader->GetBufferPointer());

				// Return the compiled shader bytecode
				byteCode.Init(shaderSize);
				memcpy(byteCode.Data(), compiledShader->GetBufferPointer(), shaderSize);

				return;
			}
		}
	}

	struct ShaderFile
	{
		wstring FilePath;
		uint64 TimeStamp;
		GrowableList<CompiledShader*> Shaders;

		ShaderFile(const wstring& filePath) : TimeStamp(0), FilePath(filePath)
		{
		}
	};

	static GrowableList<ShaderFile*> ShaderFiles;
	static GrowableList<CompiledShader*> CompiledShaders;
	static SRWLOCK ShaderFilesLock = SRWLOCK_INIT;
	static SRWLOCK CompiledShadersLock = SRWLOCK_INIT;

	static void CompileShader(CompiledShader* shader)
	{
		Assert_(shader != nullptr);

		const char* functionName = shader->Type != ShaderType::Library ? shader->FunctionName.c_str() : nullptr;

		GrowableList<wstring> filePaths;
		D3D_SHADER_MACRO defines[Compiler::MaxDefines + 1];
		shader->funCompiler.MakeDefines(defines);
		CompileShader(shader->FilePath.c_str(), functionName, shader->Type, defines, filePaths, shader->ByteCode);
		shader->ByteCodeHash = GenerateHash(shader->ByteCode.Data(), int(shader->ByteCode.Size()));

		for (uint64 fileIdx = 0; fileIdx < filePaths.Count(); ++fileIdx)
		{
			const wstring& filePath = filePaths[fileIdx];
			ShaderFile* shaderFile = nullptr;
			const uint64 numShaderFiles = ShaderFiles.Count();
			for (uint64 shaderFileIdx = 0; shaderFileIdx < numShaderFiles; ++shaderFileIdx)
			{
				if (ShaderFiles[shaderFileIdx]->FilePath == filePath)
				{
					shaderFile = ShaderFiles[shaderFileIdx];
					break;
				}
			}
			if (shaderFile == nullptr)
			{
				shaderFile = new ShaderFile(filePath);

				AcquireSRWLockExclusive(&ShaderFilesLock);

				ShaderFiles.Add(shaderFile);

				ReleaseSRWLockExclusive(&ShaderFilesLock);
			}

			bool containsShader = false;
			for (uint64 shaderIdx = 0; shaderIdx < shaderFile->Shaders.Count(); ++shaderIdx)
			{
				if (shaderFile->Shaders[shaderIdx] == shader)
				{
					containsShader = true;
					break;
				}
			}

			if (containsShader == false)
				shaderFile->Shaders.Add(shader);
		}
	}

	CompiledShaderPtr CompileFromFile(const wchar* path, const char* functionName,
		ShaderType type, const Compiler& compiler)
	{
		if (type == ShaderType::Library)
		{
			Assert_(functionName == nullptr);
		}

		CompiledShader* compiledShader = new CompiledShader(path, functionName, compiler, type);
		CompileShader(compiledShader);

		AcquireSRWLockExclusive(&CompiledShadersLock);

		CompiledShaders.Add(compiledShader);

		ReleaseSRWLockExclusive(&CompiledShadersLock);

		return compiledShader;
	}

	bool UpdateShaders(bool updateAll)
	{
		uint64 numShaderFiles = ShaderFiles.Count();
		if (numShaderFiles == 0)
			return false;

		static uint64 currFile = 0;

		const uint64 numShadersToCheck = updateAll ? numShaderFiles : 1;
		bool shaderChanged = false;

		for (uint64 i = 0; i < numShadersToCheck; ++i)
		{
			currFile = (currFile + 1) % uint64(numShaderFiles);

			ShaderFile* file = ShaderFiles[currFile];
			const uint64 newTimeStamp = GetFileTimestamp(file->FilePath.c_str());
			if (file->TimeStamp == 0)
			{
				file->TimeStamp = newTimeStamp;
				return false;
			}

			if (file->TimeStamp < newTimeStamp)
			{
				WriteLog("Hot-swapping shaders for %ls\n", file->FilePath.c_str());
				file->TimeStamp = newTimeStamp;
				for (uint64 fileIdx = 0; fileIdx < file->Shaders.Count(); ++fileIdx)
				{
					// Retry a few times to avoid file conflicts with text editors
					const uint64 NumRetries = 10;
					for (uint64 retryCount = 0; retryCount < NumRetries; ++retryCount)
					{
						try
						{
							CompiledShader* shader = file->Shaders[fileIdx];
							CompileShader(shader);
							break;
						}
						catch (Win32Exception& exception)
						{
							if (retryCount == NumRetries - 1)
								throw exception;
							Sleep(15);
						}
					}
				}

				shaderChanged = true;
			}
		}

		return shaderChanged;
	}

	void ShutdownShaders()
	{
		for (uint64 i = 0; i < ShaderFiles.Count(); ++i)
			delete ShaderFiles[i];

		for (uint64 i = 0; i < CompiledShaders.Count(); ++i)
			delete CompiledShaders[i];
	}

	// == CompileOptions ==============================================================================

	Compiler::Compiler()
	{
		Reset();
	}

	void Compiler::Add(const std::string& name, uint32 value)
	{
		Assert_(numDefines < MaxDefines);

		nameOffsets[numDefines] = bufferIdx;
		for (uint32 i = 0; i < name.length(); ++i)
			buffer[bufferIdx++] = name[i];
		++bufferIdx;

		std::string stringVal = ToAnsiString(value);
		defineOffsets[numDefines] = bufferIdx;
		for (uint32 i = 0; i < stringVal.length(); ++i)
			buffer[bufferIdx++] = stringVal[i];
		++bufferIdx;

		++numDefines;
	}

	void Compiler::Reset()
	{
		numDefines = 0;
		bufferIdx = 0;

		for (uint32 i = 0; i < MaxDefines; ++i)
		{
			nameOffsets[i] = 0xFFFFFFFF;
			defineOffsets[i] = 0xFFFFFFFF;
		}

		ZeroMemory(buffer, BufferSize);
	}

	void Compiler::MakeDefines(D3D_SHADER_MACRO defines[MaxDefines + 1]) const
	{
		for (uint32 i = 0; i < numDefines; ++i)
		{
			defines[i].Name = buffer + nameOffsets[i];
			defines[i].Definition = buffer + defineOffsets[i];
		}

		defines[numDefines].Name = nullptr;
		defines[numDefines].Definition = nullptr;
	}
}