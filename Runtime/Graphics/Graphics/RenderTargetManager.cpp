#include "RenderTargetManager.h"

#include <stdexcept>

RenderTargetManager::RenderTargetManager(ComPtr<ID3D12Device> device)
	: _device(device)
{
	assert(device);

	D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc = {};
	rtvHeapDesc.NumDescriptors = static_cast<UINT>(kMaxDescriptors);
	rtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
	rtvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	rtvHeapDesc.NodeMask = 0;
	ThrowIfFailed(device->CreateDescriptorHeap(&rtvHeapDesc, IID_PPV_ARGS(&_rtvDescriptorHeap)));

	D3D12_DESCRIPTOR_HEAP_DESC dsvHeapDesc = {};
	dsvHeapDesc.NumDescriptors = static_cast<UINT>(kMaxDescriptors);
	dsvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
	dsvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	dsvHeapDesc.NodeMask = 0;
	ThrowIfFailed(device->CreateDescriptorHeap(&dsvHeapDesc, IID_PPV_ARGS(&_dsvDescriptorHeap)));
}

std::shared_ptr<RenderTarget> RenderTargetManager::CreateRenderTarget(DXGI_FORMAT format,
	UINT64 width,
	UINT height,
	const std::wstring& rtName /*= L""*/,
	bool isUAV /*= false*/,
	const D3D12_CLEAR_VALUE* clearValue /*= nullptr*/)
{
	const size_t descriptorIndex = _rtvDescriptors.size();
	EnsureDescriptorBudget(descriptorIndex);

	const auto flags = isUAV
		? D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET | D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS
		: D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET;
	const auto resourceDesc = MakeTextureDesc(format, width, height, flags);
	const auto resolvedClearValue = MakeColorClearValue(format, clearValue);
	auto resource = CreateTexture(resourceDesc, resolvedClearValue, D3D12_RESOURCE_STATE_RENDER_TARGET, rtName);

	const auto handle = GetHeapHandle(D3D12_DESCRIPTOR_HEAP_TYPE_RTV, descriptorIndex);
	_device->CreateRenderTargetView(resource.Get(), nullptr, handle);

	_rtvDescriptors[descriptorIndex] = handle;
	return std::make_shared<RenderTarget>(descriptorIndex, resource, resolvedClearValue);
}

std::shared_ptr<DepthStencil> RenderTargetManager::CreateDepthStencil(UINT64 width,
	UINT height,
	DXGI_FORMAT format,
	DXGI_FORMAT viewFormat,
	const std::wstring& dsName /*= L""*/,
	bool isUAV /*= false*/,
	const D3D12_CLEAR_VALUE* clearValue /*= nullptr*/)
{
	const size_t descriptorIndex = _dsvDescriptors.size();
	EnsureDescriptorBudget(descriptorIndex);

	const auto flags = isUAV
		? D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL | D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS
		: D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;
	const auto resourceDesc = MakeTextureDesc(format, width, height, flags);
	const auto resolvedClearValue = MakeDepthClearValue(viewFormat, clearValue);
	auto resource = CreateTexture(resourceDesc, resolvedClearValue, D3D12_RESOURCE_STATE_COMMON, dsName);

	const auto handle = GetHeapHandle(D3D12_DESCRIPTOR_HEAP_TYPE_DSV, descriptorIndex);

	D3D12_DEPTH_STENCIL_VIEW_DESC viewDescription = {};
	viewDescription.Format = viewFormat;
	viewDescription.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;

	_device->CreateDepthStencilView(resource.Get(), &viewDescription, handle);

	_dsvDescriptors[descriptorIndex] = handle;
	return std::make_shared<DepthStencil>(descriptorIndex, resource, resolvedClearValue);
}

std::vector<std::shared_ptr<RenderTarget>> RenderTargetManager::CreateRenderTargetsForSwapChain(ComPtr<IDXGISwapChain> swapChain)
{
	DXGI_SWAP_CHAIN_DESC swapChainDesc = {};
	ThrowIfFailed(swapChain->GetDesc(&swapChainDesc));

	const size_t startIndex = _rtvDescriptors.size();
	EnsureDescriptorBudget(startIndex, swapChainDesc.BufferCount);

	std::vector<std::shared_ptr<RenderTarget>> outVector = {};
	outVector.reserve(swapChainDesc.BufferCount);

	const auto clearValue = MakeColorClearValue(swapChainDesc.BufferDesc.Format, nullptr);

	for (UINT i = 0; i < swapChainDesc.BufferCount; ++i)
	{
		ComPtr<ID3D12Resource> resource = nullptr;
		ThrowIfFailed(swapChain->GetBuffer(i, IID_PPV_ARGS(&resource)));
		D3D12_RENDER_TARGET_VIEW_DESC viewDesc = {};
		viewDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
		viewDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;

		const size_t descriptorIndex = startIndex + i;
		const auto handle = GetHeapHandle(D3D12_DESCRIPTOR_HEAP_TYPE_RTV, descriptorIndex);
		_device->CreateRenderTargetView(resource.Get(), &viewDesc, handle);
		resource->SetName(L"SwapChain RT");

		_rtvDescriptors[descriptorIndex] = handle;
		outVector.emplace_back(std::make_shared<RenderTarget>(descriptorIndex, resource, clearValue));
	}

	return outVector;
}

void RenderTargetManager::BindRenderTargets(RenderTarget* renderTargets[8],
	DepthStencil* depthStencil,
	CommandList& cmdList)
{
	D3D12_CPU_DESCRIPTOR_HANDLE rtCPUDescriptors[8] = {};
	UINT descriptorsCount = 0;
	for (UINT i = 0; i < 8; ++i)
	{
		if (!renderTargets[i])
			break;

		descriptorsCount++;
		rtCPUDescriptors[i] = _rtvDescriptors.at(renderTargets[i]->_id);
	}

	D3D12_CPU_DESCRIPTOR_HANDLE dsCPUDescriptor = {};
	if (depthStencil)
		dsCPUDescriptor = _dsvDescriptors.at(depthStencil->_id);

	cmdList.GetInternal()->OMSetRenderTargets(descriptorsCount, rtCPUDescriptors, FALSE, depthStencil ? &dsCPUDescriptor : nullptr);
}

void RenderTargetManager::ClearRenderTarget(RenderTarget& renderTarget, CommandList& cmdList)
{
	cmdList.GetInternal()->ClearRenderTargetView(_rtvDescriptors.at(renderTarget._id),
		renderTarget._clearValue.Color,
		0,
		nullptr);
}

void RenderTargetManager::ClearDepthStencil(DepthStencil& depthStencil, CommandList& cmdList)
{
	cmdList.GetInternal()->ClearDepthStencilView(_dsvDescriptors.at(depthStencil._id),
		D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL,
		depthStencil._clearValue.DepthStencil.Depth,
		depthStencil._clearValue.DepthStencil.Stencil,
		0,
		nullptr);
}

void RenderTargetManager::EnsureDescriptorBudget(size_t nextIndex, size_t additionalDescriptors) const
{
	if (nextIndex + additionalDescriptors > kMaxDescriptors)
	{
		throw std::runtime_error("Not enough space for descriptors");
	}
}

D3D12_RESOURCE_DESC RenderTargetManager::MakeTextureDesc(DXGI_FORMAT format, UINT64 width, UINT height, D3D12_RESOURCE_FLAGS flags)
{
	D3D12_RESOURCE_DESC desc = {};
	desc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	desc.Format = format;
	desc.Width = width;
	desc.Height = height;
	desc.DepthOrArraySize = 1;
	desc.MipLevels = 1;
	desc.Alignment = 0;
	desc.SampleDesc.Count = 1;
	desc.SampleDesc.Quality = 0;
	desc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
	desc.Flags = flags;
	return desc;
}

D3D12_CLEAR_VALUE RenderTargetManager::MakeColorClearValue(DXGI_FORMAT format, const D3D12_CLEAR_VALUE* overrideValue)
{
	if (overrideValue)
	{
		return *overrideValue;
	}

	D3D12_CLEAR_VALUE clearValue = {};
	clearValue.Format = format;
	clearValue.Color[0] = 0.0f;
	clearValue.Color[1] = 0.0f;
	clearValue.Color[2] = 0.0f;
	clearValue.Color[3] = 1.0f;
	return clearValue;
}

D3D12_CLEAR_VALUE RenderTargetManager::MakeDepthClearValue(DXGI_FORMAT format, const D3D12_CLEAR_VALUE* overrideValue)
{
	if (overrideValue)
	{
		return *overrideValue;
	}

	D3D12_CLEAR_VALUE clearValue = {};
	clearValue.Format = format;
	clearValue.DepthStencil.Depth = 1.0f;
	clearValue.DepthStencil.Stencil = 0;
	return clearValue;
}

ComPtr<ID3D12Resource> RenderTargetManager::CreateTexture(const D3D12_RESOURCE_DESC& desc, const D3D12_CLEAR_VALUE& clearValue, D3D12_RESOURCE_STATES initialState, const std::wstring& resourceName) const
{
	D3D12_HEAP_PROPERTIES heapProperties = {};
	heapProperties.Type = D3D12_HEAP_TYPE_DEFAULT;
	heapProperties.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;

	ComPtr<ID3D12Resource> resource;
	ThrowIfFailed(_device->CreateCommittedResource(&heapProperties,
		D3D12_HEAP_FLAG_NONE,
		&desc,
		initialState,
		&clearValue,
		IID_PPV_ARGS(&resource)));

	if (!resourceName.empty())
	{
		resource->SetName(resourceName.c_str());
	}

	return resource;
}

D3D12_CPU_DESCRIPTOR_HANDLE RenderTargetManager::GetHeapHandle(D3D12_DESCRIPTOR_HEAP_TYPE type, size_t index) const
{
	ID3D12DescriptorHeap* heap = (type == D3D12_DESCRIPTOR_HEAP_TYPE_RTV) ? _rtvDescriptorHeap.Get() : _dsvDescriptorHeap.Get();
	D3D12_CPU_DESCRIPTOR_HANDLE handle = heap->GetCPUDescriptorHandleForHeapStart();
	const UINT increment = _device->GetDescriptorHandleIncrementSize(type);
	handle.ptr += index * increment;
	return handle;
}
