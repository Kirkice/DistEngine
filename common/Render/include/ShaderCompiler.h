/* $Header: /common/Render/include/ShaderCompiler.h	     8/25/21 18:26p Kirk 			    $ */
/*--------------------------------------------------------------------------------------------*
*                                                                                             *
*						Project Name : DistEngine                                             *
*                                                                                             *
*						File Name : ShaderCompiler.h								          *
*                                                                                             *
*                       Programmer : Kirk                                                     *
*                                                                                             *
*---------------------------------------------------------------------------------------------*/
#pragma once

using namespace core;

#include "../../Core/include/PCH.h"
#include "../../Core/include/Assert.h"
#include "../../Core/include/MurmurHash.h"
#include "../../Core/include/Containers.h"
#include "../../Core/include/InterfacePointers.h"

namespace render
{
	class Compiler
	{
	public:
		static const uint32 MaxDefines = 16;
		static const uint32 BufferSize = 1024;

		Compiler();

		void Add(const std::string& name, uint32 value);
		void Reset();
		void MakeDefines(D3D_SHADER_MACRO defines[MaxDefines + 1]) const;

	private:
		uint32 nameOffsets[MaxDefines];
		uint32 defineOffsets[MaxDefines];
		char buffer[BufferSize];
		uint32 numDefines;
		uint32 bufferIdx;
	};

	enum class ShaderType
	{
		Vertex = 0,						//顶点着色器
		Hull,							//外壳着色器
		Domain,							//域着色器
		Geometry,						//几何着色器
		Pixel,							//像素着色器
		Compute,						//计算着色器
		Library,		

		NumTypes
	};

	class CompiledShader
	{

	public:

		std::wstring FilePath;
		std::string FunctionName;
		Compiler funCompiler;
		Array<uint8> ByteCode;
		ShaderType Type;
		Hash ByteCodeHash;

		CompiledShader(const wchar* filePath, const char* functionName,const Compiler& compileOptions, ShaderType type) : FilePath(filePath), funCompiler(compileOptions),Type(type)
		{
			if (functionName != nullptr)
				FunctionName = functionName;
		}
	};

	class CompiledShaderPtr
	{
	public:

		CompiledShaderPtr() : ptr(nullptr)
		{
		}

		CompiledShaderPtr(const CompiledShader* ptr_) : ptr(ptr_)
		{
		}

		const CompiledShader* operator->() const
		{
			Assert_(ptr != nullptr);
			return ptr;
		}

		const CompiledShader& operator*() const
		{
			Assert_(ptr != nullptr);
			return *ptr;
		}

		bool Valid() const
		{
			return ptr != nullptr;
		}

		D3D12_SHADER_BYTECODE ByteCode() const
		{
			Assert_(ptr != nullptr);
			D3D12_SHADER_BYTECODE byteCode;
			byteCode.pShaderBytecode = ptr->ByteCode.Data();
			byteCode.BytecodeLength = ptr->ByteCode.Size();
			return byteCode;
		}

	private:

		const CompiledShader* ptr;
	};

	typedef CompiledShaderPtr ShaderPtr;

	CompiledShaderPtr CompileFromFile(const wchar* path, const char* functionName, ShaderType type,
		const Compiler& compileOpts = Compiler());

	bool UpdateShaders(bool updateAll);
	void ShutdownShaders();
}