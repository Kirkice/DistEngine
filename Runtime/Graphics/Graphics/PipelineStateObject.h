#pragma once
#include "RootSignature.h"
#include "../Shaders/ShaderManager.h"
#include "../File/ResourcesManager.h"
#include "../Core/Mathf/Mathf.h"
#include "DX12GameApp.h"

using Microsoft::WRL::ComPtr;
using namespace DirectX;
using namespace DirectX::PackedVector;

class PipelineStateObject
{
public:
	PipelineStateObject();
	~PipelineStateObject();

	void Build(
		ShaderManager& mShaderManager,
		RootSignature& mRootSignature,
		std::string VS,
		std::string PS,
		int NumRenderTargets,
		DXGI_FORMAT RTVFormat = DXGI_FORMAT_R8G8B8A8_UNORM,
		bool MsAAState = false,
		UINT MsAAQuilty = 0,
		DXGI_FORMAT DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT
	);

	void BuildDefault(
		ShaderManager& mShaderManager,
		RootSignature& mRootSignature
	);

	void SetShader(
		ShaderManager& mShaderManager,
		RootSignature& mRootSignature,
		std::string VS,
		std::string PS
	);

	void SetBlend(
		bool enable,
		D3D12_BLEND SrcBlend,
		D3D12_BLEND DestBlend,
		D3D12_BLEND_OP BlendOp
	);

	void SetDefaultBlend();

	void SetRasterizerState(
		D3D12_FILL_MODE fillMode, 
		D3D12_CULL_MODE CullMode
	);

	void SetRasterizerDepth(
		int DepthBias,
		float DepthBiasClamp,
		float SlopeScaledDepthBias
	);

	void SetPrimitiveTopologyType(D3D12_PRIMITIVE_TOPOLOGY_TYPE Type);

	void SetDepthStencilState(
		bool DepthEnable,
		D3D12_DEPTH_WRITE_MASK Mask,
		D3D12_COMPARISON_FUNC DepthFunc = D3D12_COMPARISON_FUNC_LESS
	);

	void SetGraphicsPipelineState(
		Microsoft::WRL::ComPtr<ID3D12Device> md3dDevice,
		std::unordered_map<std::string, ComPtr<ID3D12PipelineState>> mPSOs,
		std::string Name
	);

	D3D12_GRAPHICS_PIPELINE_STATE_DESC* GetPSODesc();

private:
	D3D12_GRAPHICS_PIPELINE_STATE_DESC PSODesc;
};