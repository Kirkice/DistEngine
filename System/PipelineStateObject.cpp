#include "PipelineStateObject.h"

PipelineStateObject::PipelineStateObject()
{

}


PipelineStateObject::~PipelineStateObject()
{

}

void PipelineStateObject::Build(
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
)
{
	D3D12_GRAPHICS_PIPELINE_STATE_DESC PsoDesc;
	ZeroMemory(&PsoDesc, sizeof(D3D12_GRAPHICS_PIPELINE_STATE_DESC));
	PsoDesc.InputLayout = { mShaderManager.mInputLayout.data(), (UINT)mShaderManager.mInputLayout.size() };
	PsoDesc.pRootSignature = mRootSignature.GetSignature();
	PsoDesc.VS =
	{
		reinterpret_cast<BYTE*>(mShaderManager.mShaders[VS]->GetBufferPointer()),
		mShaderManager.mShaders[VS]->GetBufferSize()
	};
	PsoDesc.PS =
	{
		reinterpret_cast<BYTE*>(mShaderManager.mShaders[PS]->GetBufferPointer()),
		mShaderManager.mShaders[PS]->GetBufferSize()
	};

	PsoDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
	PsoDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
	PsoDesc.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
	PsoDesc.SampleMask = UINT_MAX;
	PsoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	PsoDesc.NumRenderTargets = 1;
	PsoDesc.RTVFormats[0] = RTVFormat;
	PsoDesc.SampleDesc.Count = MsAAState ? 4 : 1;
	PsoDesc.SampleDesc.Quality = MsAAState ? (MsAAQuilty - 1) : 0;
	PsoDesc.DSVFormat = DSVFormat;
	ThrowIfFailed(md3dDevice->CreateGraphicsPipelineState(&PsoDesc, IID_PPV_ARGS(&PSO[name])));
}

void PipelineStateObject::BuildDefault()
{

}