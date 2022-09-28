#include "RootSignatureManager.h"

void RootParameter::InitAsConstants(UINT valuesCount, UINT shaderRegister, D3D12_SHADER_VISIBILITY visibility /*= D3D12_SHADER_VISIBILITY_ALL*/)
{
	_parameter.ParameterType = D3D12_ROOT_PARAMETER_TYPE_32BIT_CONSTANTS;
	_parameter.ShaderVisibility = visibility;
	_parameter.Constants.Num32BitValues = valuesCount;
	_parameter.Constants.ShaderRegister = shaderRegister;
	_parameter.Constants.RegisterSpace = 0;
}

void RootParameter::InitAsCBV(UINT shaderRegister, D3D12_SHADER_VISIBILITY visibility /*= D3D12_SHADER_VISIBILITY_ALL*/)
{
	_parameter.ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	_parameter.ShaderVisibility = visibility;
	_parameter.Descriptor.ShaderRegister = shaderRegister;
	_parameter.Descriptor.RegisterSpace = 0;
}

void RootParameter::InitAsSRV(UINT shaderRegister, D3D12_SHADER_VISIBILITY visibility /*= D3D12_SHADER_VISIBILITY_ALL*/)
{
	_parameter.ParameterType = D3D12_ROOT_PARAMETER_TYPE_SRV;
	_parameter.ShaderVisibility = visibility;
	_parameter.Descriptor.ShaderRegister = shaderRegister;
	_parameter.Descriptor.RegisterSpace = 0;
}

void RootParameter::InitAsUAV(UINT shaderRegister, D3D12_SHADER_VISIBILITY visibility /*= D3D12_SHADER_VISIBILITY_ALL*/)
{
	_parameter.ParameterType = D3D12_ROOT_PARAMETER_TYPE_UAV;
	_parameter.ShaderVisibility = visibility;
	_parameter.Descriptor.ShaderRegister = shaderRegister;
	_parameter.Descriptor.RegisterSpace = 0;
}

void RootParameter::InitAsDescriptorsTable(UINT numRanges, D3D12_SHADER_VISIBILITY visibility /*= D3D12_SHADER_VISIBILITY_ALL*/)
{
	_parameter.ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	_parameter.ShaderVisibility = visibility;
	_parameter.DescriptorTable.NumDescriptorRanges = numRanges;

	_ranges.reset(new D3D12_DESCRIPTOR_RANGE[numRanges]);
	_parameter.DescriptorTable.pDescriptorRanges = _ranges.get();
}

void RootParameter::InitTableRange(UINT rangeNumber, UINT baseRegister, UINT descriptorsCount, D3D12_DESCRIPTOR_RANGE_TYPE rangeType)
{
	if (!_ranges || _parameter.DescriptorTable.NumDescriptorRanges <= rangeNumber)
	{
		assert(false);
		return;
	}

	_ranges[rangeNumber].RangeType = rangeType;
	_ranges[rangeNumber].BaseShaderRegister = baseRegister;
	_ranges[rangeNumber].NumDescriptors = descriptorsCount;
	_ranges[rangeNumber].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;
	_ranges[rangeNumber].RegisterSpace = 0;
}

RootParameter::operator D3D12_ROOT_PARAMETER& ()
{
	return _parameter;
}

RootParameter::operator const D3D12_ROOT_PARAMETER& () const
{
	return _parameter;
}

//////////////////////////////////////////////////////////////////////////

StaticSampler::StaticSampler()
{
	_description.Filter = D3D12_FILTER_ANISOTROPIC;
	_description.AddressU = D3D12_TEXTURE_ADDRESS_MODE_MIRROR;
	_description.AddressV = D3D12_TEXTURE_ADDRESS_MODE_MIRROR;
	_description.AddressW = D3D12_TEXTURE_ADDRESS_MODE_MIRROR;
	_description.MipLODBias = 0;
	_description.MaxAnisotropy = 8;
	_description.ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;
	_description.BorderColor = D3D12_STATIC_BORDER_COLOR_OPAQUE_WHITE;
	_description.MinLOD = 0.0f;
	_description.MaxLOD = D3D12_FLOAT32_MAX;
	_description.ShaderRegister = 0;
	_description.RegisterSpace = 0;
	_description.ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
}

StaticSampler::operator const D3D12_STATIC_SAMPLER_DESC& () const
{
	return _description;
}

StaticSampler::operator D3D12_STATIC_SAMPLER_DESC& ()
{
	return _description;
}

//////////////////////////////////////////////////////////////////////////

void RootSignature::Init(UINT parametersCount, UINT staticSamplersCount)
{
	_parameters.reset(new RootParameter[parametersCount]);
	_samplers.reset(new StaticSampler[staticSamplersCount]);

	_description.NumParameters = parametersCount;
	_description.NumStaticSamplers = staticSamplersCount;
}

void RootSignature::InitStaticSampler(UINT staticSampler, const StaticSampler& samplerDesc)
{
	assert(_description.NumStaticSamplers > staticSampler);
	_samplers[staticSampler] = samplerDesc;
}

RootParameter& RootSignature::operator[] (size_t parameter)
{
	assert(_description.NumParameters > parameter);
	return _parameters[parameter];
}

const RootParameter& RootSignature::operator[] (size_t parameter) const
{
	assert(_description.NumParameters > parameter);
	return _parameters[parameter];
}

void RootSignature::Finalize(ComPtr<ID3D12Device> pDevice, D3D12_ROOT_SIGNATURE_FLAGS flags /*= D3D12_ROOT_SIGNATURE_FLAG_NONE*/)
{
	// create more optimal way to do this
	std::unique_ptr<D3D12_ROOT_PARAMETER[]> descParameters;
	descParameters.reset(new D3D12_ROOT_PARAMETER[_description.NumParameters]);
	for (UINT i = 0; i < _description.NumParameters; ++i)
		descParameters[i] = _parameters[i];

	std::unique_ptr<D3D12_STATIC_SAMPLER_DESC[]> descSamplers;
	descSamplers.reset(new D3D12_STATIC_SAMPLER_DESC[_description.NumStaticSamplers]);
	for (UINT i = 0; i < _description.NumStaticSamplers; ++i)
		descSamplers[i] = _samplers[i];

	_description.Flags = flags;
	_description.pParameters = descParameters.get();
	_description.pStaticSamplers = descSamplers.get();

	ComPtr<ID3DBlob> signatureBlob = nullptr;
	ComPtr<ID3DBlob> errorBlob = nullptr;
	if (FAILED(D3D12SerializeRootSignature(&_description, D3D_ROOT_SIGNATURE_VERSION_1, &signatureBlob, &errorBlob)))
		MessageBoxA(NULL, (const char*)errorBlob->GetBufferPointer(), NULL, NULL);

	ThrowIfFailed(pDevice->CreateRootSignature(0, signatureBlob->GetBufferPointer(), signatureBlob->GetBufferSize(), IID_PPV_ARGS(&_signature)));
}

ComPtr<ID3D12RootSignature> RootSignature::GetInternal() const
{
	return _signature;
}
