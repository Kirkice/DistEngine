#pragma once
#include "RootSignature.h"
#include "../Shaders/ShaderManager.h"
#include "../File/ResourcesManager.h"
#include "../Core/Mathf/Mathf.h"
#include "DX12Utils.h"

using Microsoft::WRL::ComPtr;
using namespace DirectX;
using namespace DirectX::PackedVector;

class PipelineStateObject
{
public:
	PipelineStateObject();
	~PipelineStateObject();

	void Build(
		DistShader::ShaderManager& mShaderManager,
		RootSignature& mRootSignature,
		const std::string& VS,
		const std::string& PS,
		int NumRenderTargets,
		DXGI_FORMAT RTVFormat = DXGI_FORMAT_R8G8B8A8_UNORM,
		bool MsAAState = false,
		UINT MsAAQuilty = 0,
		DXGI_FORMAT DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT
	);

	void BuildDefault(
		DistShader::ShaderManager& mShaderManager,
		RootSignature& mRootSignature
	);

	void SetShader(
		DistShader::ShaderManager& mShaderManager,
		RootSignature& mRootSignature,
		const std::string& VS,
		const std::string& PS
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

	const D3D12_GRAPHICS_PIPELINE_STATE_DESC* GetPSODesc() const;

private:
	void InitializeDefaults(DistShader::ShaderManager& mShaderManager, RootSignature& mRootSignature);
	void ApplyShaders(DistShader::ShaderManager& mShaderManager, const std::string& VS, const std::string& PS);
	static D3D12_RENDER_TARGET_BLEND_DESC MakeBlendDesc(bool enable, D3D12_BLEND SrcBlend, D3D12_BLEND DestBlend, D3D12_BLEND_OP BlendOp);

private:
	D3D12_GRAPHICS_PIPELINE_STATE_DESC PSODesc;
};