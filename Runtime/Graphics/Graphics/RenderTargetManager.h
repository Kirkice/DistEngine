#pragma once
#include "DX12Utils.h"

struct RenderTarget
{
	RenderTarget(uint64_t id, ComPtr<ID3D12Resource> resource, D3D12_CLEAR_VALUE clearValue)
		: _id(id)
		, _texture(resource)
		, _clearValue(clearValue)
	{
	}


	RenderTarget(const RenderTarget&) = delete;
	RenderTarget(RenderTarget&&) = delete;
	RenderTarget& operator=(const RenderTarget&) = delete;
	RenderTarget& operator=(RenderTarget&&) = delete;

	const uint64_t                  _id;
	const ComPtr<ID3D12Resource>    _texture;
	const D3D12_CLEAR_VALUE         _clearValue;
};

struct DepthStencil
{
	DepthStencil(uint64_t id, ComPtr<ID3D12Resource> resource, D3D12_CLEAR_VALUE clearValue)
		: _id(id)
		, _texture(resource)
		, _clearValue(clearValue)
	{
	}

	DepthStencil(const DepthStencil&) = delete;
	DepthStencil(DepthStencil&&) = delete;
	DepthStencil& operator=(const DepthStencil&) = delete;
	DepthStencil& operator=(DepthStencil&&) = delete;

	const uint64_t                  _id;
	const ComPtr<ID3D12Resource>    _texture;
	const D3D12_CLEAR_VALUE         _clearValue;
};

class RenderTargetManager
{
public:
	RenderTargetManager(ComPtr<ID3D12Device> device);

	std::shared_ptr<RenderTarget> CreateRenderTarget(DXGI_FORMAT format,
		UINT64 width,
		UINT height,
		const std::wstring& rtName = L"",
		bool isUAV = false,
		const D3D12_CLEAR_VALUE* clearValue = nullptr);

	std::shared_ptr<DepthStencil> CreateDepthStencil(UINT64 width,
		UINT height,
		DXGI_FORMAT format,
		DXGI_FORMAT viewFormat,
		const std::wstring& dsName = L"",
		bool isUAV = false,
		const D3D12_CLEAR_VALUE* clearValue = nullptr);

	std::vector<std::shared_ptr<RenderTarget>> CreateRenderTargetsForSwapChain(ComPtr<IDXGISwapChain> swapChain);

	void BindRenderTargets(RenderTarget* renderTargets[8],
		DepthStencil* depthStencil,
		CommandList& cmdList);

	void ClearRenderTarget(RenderTarget& renderTarget,
		CommandList& cmdList);

	void ClearDepthStencil(DepthStencil& depthStencil,
		CommandList& cmdList);

private:
	static constexpr size_t kMaxDescriptors = 100;

	void EnsureDescriptorBudget(size_t nextIndex, size_t additionalDescriptors = 1) const;
	static D3D12_RESOURCE_DESC MakeTextureDesc(DXGI_FORMAT format, UINT64 width, UINT height, D3D12_RESOURCE_FLAGS flags);
	static D3D12_CLEAR_VALUE MakeColorClearValue(DXGI_FORMAT format, const D3D12_CLEAR_VALUE* overrideValue);
	static D3D12_CLEAR_VALUE MakeDepthClearValue(DXGI_FORMAT format, const D3D12_CLEAR_VALUE* overrideValue);
	ComPtr<ID3D12Resource> CreateTexture(const D3D12_RESOURCE_DESC& desc, const D3D12_CLEAR_VALUE& clearValue, D3D12_RESOURCE_STATES initialState, const std::wstring& resourceName) const;
	D3D12_CPU_DESCRIPTOR_HANDLE GetHeapHandle(D3D12_DESCRIPTOR_HEAP_TYPE type, size_t index) const;

	ComPtr<ID3D12DescriptorHeap>    _rtvDescriptorHeap = nullptr;
	ComPtr<ID3D12DescriptorHeap>    _dsvDescriptorHeap = nullptr;
	ComPtr<ID3D12Device>            _device = nullptr;

	std::unordered_map<uint64_t, D3D12_CPU_DESCRIPTOR_HANDLE> _rtvDescriptors;
	std::unordered_map<uint64_t, D3D12_CPU_DESCRIPTOR_HANDLE> _dsvDescriptors;

};
