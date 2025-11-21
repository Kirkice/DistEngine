#include "ComputePipelineState.h"

ComputePipelineState::ComputePipelineState()
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

ComPtr<ID3D12PipelineState> ComputePipelineState::GetPSO() const
{
	assert(_pso);
	return _pso;
}

void ComputePipelineState::Finalize(ComPtr<ID3D12Device> pDevice)
{
	assert(_description.pRootSignature != nullptr);
	ThrowIfFailed(pDevice->CreateComputePipelineState(&_description, IID_PPV_ARGS(&_pso)));
}
