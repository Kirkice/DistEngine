#pragma once
#include "../Core/Mathf/Mathf.h"
#include "DX12GameApp.h"

using Microsoft::WRL::ComPtr;
using namespace DirectX;
using namespace DirectX::PackedVector;

class RootSignature
{
	//RootSignatureType
public: enum class RootSignatureType : int
	{
		Default,
		SSAO,
	};

public:
	RootSignature();
	~RootSignature();

	void Build(
		RootSignatureType type,
		Microsoft::WRL::ComPtr<ID3D12Device> md3dDevice,
		UINT baseShaderRegister0,
		UINT registerSpace0,
		UINT offsetInDescriptorsFromTableStart0,
		UINT baseShaderRegister1,
		UINT registerSpace1,
		UINT offsetInDescriptorsFromTableStart1
	);
	ID3D12RootSignature* GetSignature();

private:
	ComPtr<ID3D12RootSignature> mRootSignature;
	std::array<const CD3DX12_STATIC_SAMPLER_DESC, 7> GetStaticSamplers();
	std::array<const CD3DX12_STATIC_SAMPLER_DESC, 4> GetSSAOSamplers();
};