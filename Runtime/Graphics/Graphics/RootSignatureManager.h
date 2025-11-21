#pragma once
#include "DX12Utils.h"
#include <vector>

class RootParameter
{
public:
	RootParameter() = default;

	RootParameter(const RootParameter&) = delete;
	RootParameter(RootParameter&& other)
		: _parameter(other._parameter), _ranges(std::move(other._ranges))
	{
		if (_parameter.ParameterType == D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE)
		{
			_parameter.DescriptorTable.pDescriptorRanges = _ranges.empty() ? nullptr : _ranges.data();
		}
		other._parameter.DescriptorTable.pDescriptorRanges = nullptr;
		other._parameter.DescriptorTable.NumDescriptorRanges = 0;
	}

	RootParameter& operator=(const RootParameter&) = delete;
	RootParameter& operator=(RootParameter&& other)
	{
		if (this != &other)
		{
			_parameter = other._parameter;
			_ranges = std::move(other._ranges);

			if (_parameter.ParameterType == D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE)
			{
				_parameter.DescriptorTable.pDescriptorRanges = _ranges.empty() ? nullptr : _ranges.data();
			}
			other._parameter.DescriptorTable.pDescriptorRanges = nullptr;
			other._parameter.DescriptorTable.NumDescriptorRanges = 0;
		}
		return *this;
	}

	void InitAsConstants(UINT valuesCount, UINT shaderRegister, D3D12_SHADER_VISIBILITY visibility = D3D12_SHADER_VISIBILITY_ALL);

	void InitAsCBV(UINT shaderRegister, D3D12_SHADER_VISIBILITY visibility = D3D12_SHADER_VISIBILITY_ALL);
	void InitAsSRV(UINT shaderRegister, D3D12_SHADER_VISIBILITY visibility = D3D12_SHADER_VISIBILITY_ALL);
	void InitAsUAV(UINT shaderRegister, D3D12_SHADER_VISIBILITY visibility = D3D12_SHADER_VISIBILITY_ALL);

	void InitAsDescriptorsTable(UINT numRanges, D3D12_SHADER_VISIBILITY visibility = D3D12_SHADER_VISIBILITY_ALL);

	void InitTableRange(UINT rangeNumber, UINT baseRegister, UINT descriptorsCount, D3D12_DESCRIPTOR_RANGE_TYPE rangeType);

	operator D3D12_ROOT_PARAMETER& ();
	operator const D3D12_ROOT_PARAMETER& () const;

private:
	D3D12_ROOT_PARAMETER                        _parameter = {};
	std::vector<D3D12_DESCRIPTOR_RANGE>         _ranges;
};

//////////////////////////////////////////////////////////////////////////

class StaticSampler
{
public:
	StaticSampler();

	StaticSampler(const StaticSampler&) = default;
	StaticSampler(StaticSampler&&) = default;
	StaticSampler& operator=(const StaticSampler&) = default;
	StaticSampler& operator=(StaticSampler&&) = default;

	operator const D3D12_STATIC_SAMPLER_DESC& () const;
	operator D3D12_STATIC_SAMPLER_DESC& ();

private:
	D3D12_STATIC_SAMPLER_DESC _description;

};

//////////////////////////////////////////////////////////////////////////

class RootSignature
{
public:
	RootSignature() = default;

	RootSignature(const RootSignature&) = delete;
	RootSignature(RootSignature&&) = delete;
	RootSignature operator=(const RootSignature&) = delete;
	RootSignature operator=(RootSignature&&) = delete;

	RootParameter& operator[] (size_t parameter);
	const RootParameter& operator[] (size_t parameter) const;

	void Init(UINT parametersCount, UINT staticSamplersCount);
	void InitStaticSampler(UINT staticSampler, const StaticSampler& samplerDesc);
	void Finalize(ComPtr<ID3D12Device> pDevice, D3D12_ROOT_SIGNATURE_FLAGS flags = D3D12_ROOT_SIGNATURE_FLAG_NONE);
	ComPtr<ID3D12RootSignature> GetInternal() const;

private:
	ComPtr<ID3D12RootSignature> _signature = nullptr;
	std::vector<RootParameter> _parameters;
	std::vector<StaticSampler> _samplers;
	D3D12_ROOT_SIGNATURE_DESC _description = {};
};
