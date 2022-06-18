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
	ShaderManager& mShaderManager,
	RootSignature& mRootSignature,
	std::string VS,
	std::string PS,
	int NumRenderTargets,
	DXGI_FORMAT RTVFormat,
	bool MsAAState,
	UINT MsAAQuilty,
	DXGI_FORMAT DSVFormat
)
{
	ZeroMemory(&PSODesc, sizeof(D3D12_GRAPHICS_PIPELINE_STATE_DESC));
	PSODesc.InputLayout = { mShaderManager.getInstance().mInputLayout.data(), (UINT)mShaderManager.getInstance().mInputLayout.size() };
	PSODesc.pRootSignature = mRootSignature.GetSignature();
	PSODesc.VS =
	{
		reinterpret_cast<BYTE*>(mShaderManager.getInstance().mShaders[VS]->GetBufferPointer()),
		mShaderManager.getInstance().mShaders[VS]->GetBufferSize()
	};
	PSODesc.PS =
	{
		reinterpret_cast<BYTE*>(mShaderManager.getInstance().mShaders[PS]->GetBufferPointer()),
		mShaderManager.getInstance().mShaders[PS]->GetBufferSize()
	};

	PSODesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
	PSODesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
	PSODesc.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
	PSODesc.SampleMask = UINT_MAX;
	PSODesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
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
	ShaderManager& mShaderManager,
	RootSignature& mRootSignature
)
{
	ZeroMemory(&PSODesc, sizeof(D3D12_GRAPHICS_PIPELINE_STATE_DESC));
	PSODesc.InputLayout = { mShaderManager.getInstance().mInputLayout.data(), (UINT)mShaderManager.getInstance().mInputLayout.size() };
	PSODesc.pRootSignature = mRootSignature.GetSignature();
	PSODesc.VS =
	{
		reinterpret_cast<BYTE*>(mShaderManager.getInstance().mShaders["litVS"]->GetBufferPointer()),
		mShaderManager.getInstance().mShaders["litVS"]->GetBufferSize()
	};
	PSODesc.PS =
	{
		reinterpret_cast<BYTE*>(mShaderManager.getInstance().mShaders["litPS"]->GetBufferPointer()),
		mShaderManager.getInstance().mShaders["litPS"]->GetBufferSize()
	};

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

/// <summary>
/// SetShader
/// </summary>
/// <param name="mShaderManager"></param>
/// <param name="mRootSignature"></param>
/// <param name="VS"></param>
/// <param name="PS"></param>
void PipelineStateObject::SetShader(
	ShaderManager& mShaderManager,
	RootSignature& mRootSignature,
	std::string VS,
	std::string PS
)
{
	PSODesc.InputLayout = { mShaderManager.getInstance().mInputLayout.data(), (UINT)mShaderManager.getInstance().mInputLayout.size() };
	PSODesc.pRootSignature = mRootSignature.GetSignature();
	PSODesc.VS =
	{
		reinterpret_cast<BYTE*>(mShaderManager.getInstance().mShaders[VS]->GetBufferPointer()),
		mShaderManager.getInstance().mShaders[VS]->GetBufferSize()
	};
	PSODesc.PS =
	{
		reinterpret_cast<BYTE*>(mShaderManager.getInstance().mShaders[PS]->GetBufferPointer()),
		mShaderManager.getInstance().mShaders[PS]->GetBufferSize()
	};
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
	D3D12_RENDER_TARGET_BLEND_DESC AlphaBlendDesc;
	AlphaBlendDesc.BlendEnable = enable;
	AlphaBlendDesc.LogicOpEnable = false;
	AlphaBlendDesc.SrcBlend = SrcBlend;
	AlphaBlendDesc.DestBlend = DestBlend;
	AlphaBlendDesc.BlendOp = BlendOp;
	AlphaBlendDesc.SrcBlendAlpha = D3D12_BLEND_ONE;
	AlphaBlendDesc.DestBlendAlpha = D3D12_BLEND_ZERO;
	AlphaBlendDesc.BlendOpAlpha = D3D12_BLEND_OP_ADD;
	AlphaBlendDesc.LogicOp = D3D12_LOGIC_OP_NOOP;
	AlphaBlendDesc.RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
	PSODesc.BlendState.RenderTarget[0] = AlphaBlendDesc;
}

/// <summary>
/// SetDefaultBlend
/// </summary>
void PipelineStateObject::SetDefaultBlend()
{
	D3D12_RENDER_TARGET_BLEND_DESC AlphaBlendDesc;
	AlphaBlendDesc.BlendEnable = true;
	AlphaBlendDesc.LogicOpEnable = false;
	AlphaBlendDesc.SrcBlend = D3D12_BLEND_SRC_ALPHA;
	AlphaBlendDesc.DestBlend = D3D12_BLEND_INV_SRC_ALPHA;
	AlphaBlendDesc.BlendOp = D3D12_BLEND_OP_ADD;
	AlphaBlendDesc.SrcBlendAlpha = D3D12_BLEND_ONE;
	AlphaBlendDesc.DestBlendAlpha = D3D12_BLEND_ZERO;
	AlphaBlendDesc.BlendOpAlpha = D3D12_BLEND_OP_ADD;
	AlphaBlendDesc.LogicOp = D3D12_LOGIC_OP_NOOP;
	AlphaBlendDesc.RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
	PSODesc.BlendState.RenderTarget[0] = AlphaBlendDesc;
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
D3D12_GRAPHICS_PIPELINE_STATE_DESC* PipelineStateObject::GetPSODesc()
{
	return &PSODesc;
}

/// <summary>
/// SetGraphicsPipelineState
/// </summary>
/// <param name="md3dDevice"></param>
void PipelineStateObject::SetGraphicsPipelineState(
	Microsoft::WRL::ComPtr<ID3D12Device> md3dDevice,
	std::unordered_map<std::string, ComPtr<ID3D12PipelineState>> mPSOs,
	std::string Name
)
{
	ThrowIfFailed(md3dDevice->CreateGraphicsPipelineState(&PSODesc, IID_PPV_ARGS(&mPSOs[Name])));
}