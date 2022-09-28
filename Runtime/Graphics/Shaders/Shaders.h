#pragma once

#include <string>
#include "../Graphics/GraphicsUtils.h"

namespace ShadersUtils
{
	inline void CompileShaderFromSource(const std::string& shaderCode, ShaderType type, ID3DBlob** out_blob, const D3D_SHADER_MACRO* macro = nullptr)
	{
		uint32_t compileFlags = D3DCOMPILE_PACK_MATRIX_ROW_MAJOR;
#ifdef _DEBUG
		compileFlags |= D3DCOMPILE_DEBUG;
		compileFlags |= D3DCOMPILE_ENABLE_STRICTNESS;
		compileFlags |= D3DCOMPILE_SKIP_OPTIMIZATION;
		compileFlags |= D3DCOMPILE_WARNINGS_ARE_ERRORS;
#else
		compileFlags |= D3DCOMPILE_OPTIMIZATION_LEVEL3;
#endif

		std::string entryPoint = "_main";
		std::string shaderVersion = "_5_0";
		static const std::map<ShaderType, const char*> shaderTypesPrefixes = {
			{ShaderType::Vertex,    "vs"},
			{ShaderType::Pixel,     "ps"},
			{ShaderType::Geometry,  "gs"},
			{ShaderType::Hull,      "hs"},
			{ShaderType::Domain,    "ds"},
			{ShaderType::Compute,   "cs"},
		};

		entryPoint = shaderTypesPrefixes.at(type) + entryPoint;
		shaderVersion = shaderTypesPrefixes.at(type) + shaderVersion;

		ComPtr<ID3DBlob> error = nullptr;
		if (FAILED(D3DCompile(shaderCode.c_str(),
			shaderCode.size(),
			nullptr,
			macro,
			nullptr,
			entryPoint.c_str(),
			shaderVersion.c_str(),
			compileFlags,
			0,
			out_blob,
			&error)))
			MessageBoxA(NULL, (const char*)error->GetBufferPointer(), NULL, NULL);
	}

	inline void CompileShaderFromFile(const std::wstring& fileName, ShaderType type, ID3DBlob** out_blob, const D3D_SHADER_MACRO* macro = nullptr)
	{
		uint32_t compileFlags = D3DCOMPILE_PACK_MATRIX_ROW_MAJOR;
#ifdef _DEBUG
		compileFlags |= D3DCOMPILE_DEBUG;
		compileFlags |= D3DCOMPILE_ENABLE_STRICTNESS;
		compileFlags |= D3DCOMPILE_SKIP_OPTIMIZATION;
		compileFlags |= D3DCOMPILE_WARNINGS_ARE_ERRORS;
#else
		compileFlags |= D3DCOMPILE_OPTIMIZATION_LEVEL3;
#endif

		std::string entryPoint = "_main";
		std::string shaderVersion = "_5_0";
		static const std::map<ShaderType, const char*> shaderTypesPrefixes = {
			{ShaderType::Vertex,    "vs"},
			{ShaderType::Pixel,     "ps"},
			{ShaderType::Geometry,  "gs"},
			{ShaderType::Hull,      "hs"},
			{ShaderType::Domain,    "ds"},
			{ShaderType::Compute,   "cs"},
		};

		entryPoint = shaderTypesPrefixes.at(type) + entryPoint;
		shaderVersion = shaderTypesPrefixes.at(type) + shaderVersion;

		ComPtr<ID3DBlob> error = nullptr;
		if (FAILED(D3DCompileFromFile(fileName.c_str(), macro, nullptr, entryPoint.c_str(), shaderVersion.c_str(), compileFlags, 0, out_blob, &error)))
			if (error)
				MessageBoxA(NULL, (const char*)error->GetBufferPointer(), NULL, NULL);
			else
				throw std::runtime_error("Unable to compile shader from file");
	}
};
