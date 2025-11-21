#include "PipelineStateObject.h"

PipelineStateObject::PipelineStateObject()
{

}


PipelineStateObject::~PipelineStateObject()
{

}

/// <summary>
/// Build
/// </summary>
/// <param name="mShaderManager"></param>
/// <param name="mRootSignature"></param>
/// <param name="VS"></param>
/// <param name="PS"></param>
/// <param name="name"></param>
/// <param name="RTVFormat"></param>
/// <param name="MsAAState"></param>
/// <param name="MsAAQuilty"></param>
/// <param name="DSVFormat"></param>
void PipelineStateObject::Build(
	DistShader::ShaderManager& mShaderManager,
	RootSignature& mRootSignature,
	const std::string& VS,
	const std::string& PS,
	int NumRenderTargets,
	DXGI_FORMAT RTVFormat,
	bool MsAAState,
	UINT MsAAQuilty,
	DXGI_FORMAT DSVFormat
)
{
	InitializeDefaults(mShaderManager, mRootSignature);
	ApplyShaders(mShaderManager, VS, PS);
	PSODesc.NumRenderTargets = NumRenderTargets;
	PSODesc.RTVFormats[0] = RTVFormat;
	PSODesc.SampleDesc.Count = MsAAState ? 4 : 1;
	PSODesc.SampleDesc.Quality = MsAAState ? (MsAAQuilty - 1) : 0;
	PSODesc.DSVFormat = DSVFormat;
}

/// <summary>
/// BuildDefault
/// </summary>
/// <param name="mShaderManager"></param>
/// <param name="mRootSignature"></param>
void PipelineStateObject::BuildDefault(
	DistShader::ShaderManager& mShaderManager,
	RootSignature& mRootSignature
)
{
	Build(mShaderManager, mRootSignature, "litVS", "litPS", 1);
}

/// <summary>
/// SetShader
/// </summary>
/// <param name="mShaderManager"></param>
/// <param name="mRootSignature"></param>
/// <param name="VS"></param>
/// <param name="PS"></param>
void PipelineStateObject::SetShader(
	DistShader::ShaderManager& mShaderManager,
	RootSignature& mRootSignature,
	const std::string& VS,
	const std::string& PS
)
{
	PSODesc.InputLayout = { mShaderManager.mInputLayout.data(), (UINT)mShaderManager.mInputLayout.size() };
	PSODesc.pRootSignature = mRootSignature.GetSignature();
	ApplyShaders(mShaderManager, VS, PS);
}

/// <summary>
/// SetRasterizerState  
/// </summary>
void PipelineStateObject::SetRasterizerState(
	D3D12_FILL_MODE fillMode, 
	D3D12_CULL_MODE CullMode
)
{
	PSODesc.RasterizerState.FillMode = fillMode;
	PSODesc.RasterizerState.CullMode = CullMode;
}

/// <summary>
/// SetRasterizerDepth
/// </summary>
/// <param name="DepthBias"></param>
/// <param name="DepthBiasClamp"></param>
/// <param name="SlopeScaledDepthBias"></param>
void PipelineStateObject::SetRasterizerDepth(
	int DepthBias,
	float DepthBiasClamp,
	float SlopeScaledDepthBias
)
{
	PSODesc.RasterizerState.DepthBias = DepthBias;
	PSODesc.RasterizerState.DepthBiasClamp = DepthBiasClamp;
	PSODesc.RasterizerState.SlopeScaledDepthBias = SlopeScaledDepthBias;
}

/// <summary>
/// SetPrimitiveTopologyType
/// </summary>
/// <param name="Type"></param>
void PipelineStateObject::SetPrimitiveTopologyType(D3D12_PRIMITIVE_TOPOLOGY_TYPE Type)
{
	PSODesc.PrimitiveTopologyType = Type;
}

/// <summary>
/// SetBlend
/// </summary>
/// <param name="enable"></param>
/// <param name="SrcBlend"></param>
/// <param name="DestBlend"></param>
/// <param name="BlendOp"></param>
void PipelineStateObject::SetBlend(
	bool enable, 
	D3D12_BLEND SrcBlend,
	D3D12_BLEND DestBlend,
	D3D12_BLEND_OP BlendOp
)
{
	PSODesc.BlendState.RenderTarget[0] = MakeBlendDesc(enable, SrcBlend, DestBlend, BlendOp);
}

/// <summary>
/// SetDefaultBlend
/// </summary>
void PipelineStateObject::SetDefaultBlend()
{
	PSODesc.BlendState.RenderTarget[0] = MakeBlendDesc(
		true,
		D3D12_BLEND_SRC_ALPHA,
		D3D12_BLEND_INV_SRC_ALPHA,
		D3D12_BLEND_OP_ADD);
}

/// <summary>
/// SetDepthStencilState
/// </summary>
/// <param name="DepthEnable"></param>
/// <param name="Mask"></param>
void PipelineStateObject::SetDepthStencilState(
	bool DepthEnable, 
	D3D12_DEPTH_WRITE_MASK Mask,
	D3D12_COMPARISON_FUNC DepthFunc
)
{
	PSODesc.DepthStencilState.DepthEnable = DepthEnable;
	PSODesc.DepthStencilState.DepthWriteMask = Mask;
	PSODesc.DepthStencilState.DepthFunc = DepthFunc;
}

/// <summary>
/// GetPSODesc
/// </summary>
/// <returns></returns>
const D3D12_GRAPHICS_PIPELINE_STATE_DESC* PipelineStateObject::GetPSODesc() const
{
	return &PSODesc;
}

void PipelineStateObject::InitializeDefaults(DistShader::ShaderManager& mShaderManager, RootSignature& mRootSignature)
{
	ZeroMemory(&PSODesc, sizeof(D3D12_GRAPHICS_PIPELINE_STATE_DESC));
	PSODesc.InputLayout = { mShaderManager.mInputLayout.data(), (UINT)mShaderManager.mInputLayout.size() };
	PSODesc.pRootSignature = mRootSignature.GetSignature();
	PSODesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
	PSODesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
	PSODesc.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
	PSODesc.SampleMask = UINT_MAX;
	PSODesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	PSODesc.NumRenderTargets = 1;
	PSODesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
	PSODesc.SampleDesc.Count = 1;
	PSODesc.SampleDesc.Quality = 0;
	PSODesc.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
}

void PipelineStateObject::ApplyShaders(DistShader::ShaderManager& mShaderManager, const std::string& VS, const std::string& PS)
{
	PSODesc.VS =
	{
		reinterpret_cast<BYTE*>(mShaderManager.mShaders[VS]->GetBufferPointer()),
		mShaderManager.mShaders[VS]->GetBufferSize()
	};
	PSODesc.PS =
	{
		reinterpret_cast<BYTE*>(mShaderManager.mShaders[PS]->GetBufferPointer()),
		mShaderManager.mShaders[PS]->GetBufferSize()
	};
}

D3D12_RENDER_TARGET_BLEND_DESC PipelineStateObject::MakeBlendDesc(bool enable, D3D12_BLEND SrcBlend, D3D12_BLEND DestBlend, D3D12_BLEND_OP BlendOp)
{
	D3D12_RENDER_TARGET_BLEND_DESC desc{};
	desc.BlendEnable = enable;
	desc.LogicOpEnable = false;
	desc.SrcBlend = SrcBlend;
	desc.DestBlend = DestBlend;
	desc.BlendOp = BlendOp;
	desc.SrcBlendAlpha = D3D12_BLEND_ONE;
	desc.DestBlendAlpha = D3D12_BLEND_ZERO;
	desc.BlendOpAlpha = D3D12_BLEND_OP_ADD;
	desc.LogicOp = D3D12_LOGIC_OP_NOOP;
	desc.RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
	return desc;
}