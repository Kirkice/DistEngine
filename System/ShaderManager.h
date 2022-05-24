#pragma once
#include "ResourcesManager.h"
#include "../Core/Mathf/Mathf.h"
#include "d3dApp.h"

using Microsoft::WRL::ComPtr;
using namespace DirectX;
using namespace DirectX::PackedVector;

class ShaderManager
{
public:
	ShaderManager();
	~ShaderManager();

	void Build();
	//ComPtr<ID3DBlob> GetShader(std::string str);
	//std::vector<D3D12_INPUT_ELEMENT_DESC> GetCommonLayout();
	//std::vector<D3D12_INPUT_ELEMENT_DESC> GetSkinnedLayout();

	std::unordered_map<std::string, ComPtr<ID3DBlob>> mShaders;
	std::vector<D3D12_INPUT_ELEMENT_DESC> mInputLayout;
	std::vector<D3D12_INPUT_ELEMENT_DESC> mSkinnedInputLayout;

private:
	void BuildShaders();
	void BuildInputLayout();
};