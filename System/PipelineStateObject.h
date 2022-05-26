#pragma once
#include "RootSignature.h"
#include "ShaderManager.h"
#include "ResourcesManager.h"
#include "../Core/Mathf/Mathf.h"
#include "d3dApp.h"

using Microsoft::WRL::ComPtr;
using namespace DirectX;
using namespace DirectX::PackedVector;

class PipelineStateObject
{
public:
	PipelineStateObject();
	~PipelineStateObject();

	void Build(
		Microsoft::WRL::ComPtr<ID3D12Device> md3dDevice,
		ShaderManager& mShaderManager,
		RootSignature& mRootSignature,
		std::string VS,
		std::string PS,
		std::string name,
		DXGI_FORMAT RTVFormat = DXGI_FORMAT_R8G8B8A8_UNORM,
		bool MsAAState = false,
		UINT MsAAQuilty = 0,
		DXGI_FORMAT DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT
	);

	void BuildDefault();

	void SetShader();

	void SetRasterizerState();

	void SetBlend();



private:
	
};