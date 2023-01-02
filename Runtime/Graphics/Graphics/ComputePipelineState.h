#pragma once
#include "RootSignatureManager.h"

class ComputePipelineState
{
public:
	ComputePipelineState();
	ComputePipelineState(const RootSignature& pRootSignature);

	void SetRootSignature(const RootSignature& pRootSignature);
	void SetShaderCode(ComPtr<ID3DBlob> pShaderCode);

	ComPtr<ID3D12PipelineState> GetPSO();
	void Finalize(ComPtr<ID3D12Device> pDevice);

private:
	D3D12_COMPUTE_PIPELINE_STATE_DESC   _description = {};
	ComPtr<ID3D12PipelineState>         _pso = nullptr;
};
