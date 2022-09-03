#pragma once
#include "../File/ResourcesManager.h"
#include "../Core/Mathf/Mathf.h"
#include "DX12Utils.h"

using Microsoft::WRL::ComPtr;
using namespace DirectX;
using namespace DirectX::PackedVector;

class ShaderManager
{
public:
	ShaderManager();
	~ShaderManager();

	void Build();

	std::unordered_map<std::string, ComPtr<ID3DBlob>> mShaders;
	std::vector<D3D12_INPUT_ELEMENT_DESC> mInputLayout;
	std::vector<D3D12_INPUT_ELEMENT_DESC> mSkinnedInputLayout;

private:
	void BuildShaders();
	void BuildInputLayout();
};