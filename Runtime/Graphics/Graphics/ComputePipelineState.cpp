#include "ComputePipelineState.h"

ComputePipelineState::ComputePipelineState()
	: ComputePipelineState(RootSignature{})
{
}

ComputePipelineState::ComputePipelineState(const RootSignature& pRootSignature)
{
	_description.pRootSignature = pRootSignature.GetInternal().Get();
}

void ComputePipelineState::SetRootSignature(const RootSignature& pRootSignature)
{
	_description.pRootSignature = pRootSignature.GetInternal().Get();
}

void ComputePipelineState::SetShaderCode(ComPtr<ID3DBlob> pShaderCode)
{
	_description.CS = { pShaderCode->GetBufferPointer(), pShaderCode->GetBufferSize() };
}

ComPtr<ID3D12PipelineState> ComputePipelineState::GetPSO()
{
	assert(_pso);
	return _pso;
}

void ComputePipelineState::Finalize(ComPtr<ID3D12Device> pDevice)
{
	ThrowIfFailed(pDevice->CreateComputePipelineState(&_description, IID_PPV_ARGS(&_pso)));
}
