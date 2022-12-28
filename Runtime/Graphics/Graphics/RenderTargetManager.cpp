#include "RenderTargetManager.h"

// for example, I think it is enough
static const size_t maxDescriptorsCount = 100;

RenderTargetManager::RenderTargetManager(ComPtr<ID3D12Device> device)
	: _device(device)
{
	assert(device);

	D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc = {};
	rtvHeapDesc.NumDescriptors = maxDescriptorsCount;
	rtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
	rtvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	rtvHeapDesc.NodeMask = 0;
	ThrowIfFailed(device->CreateDescriptorHeap(&rtvHeapDesc, IID_PPV_ARGS(&_rtvDescriptorHeap)));

	D3D12_DESCRIPTOR_HEAP_DESC dsvHeapDesc = {};
	dsvHeapDesc.NumDescriptors = maxDescriptorsCount;
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
	size_t descriptorsCount = _rtvDescriptors.size();
	if (descriptorsCount == maxDescriptorsCount)
		throw std::runtime_error("Not enough space for RT descriptors");

	D3D12_HEAP_PROPERTIES heapProperties = {};
	heapProperties.Type = D3D12_HEAP_TYPE_DEFAULT;
	heapProperties.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;

	D3D12_RESOURCE_DESC resourceDesc = {};
	resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	resourceDesc.Format = format;
	resourceDesc.Width = width;
	resourceDesc.Height = height;
	resourceDesc.DepthOrArraySize = 1;
	resourceDesc.MipLevels = 1;
	resourceDesc.Alignment = 0;
	resourceDesc.SampleDesc.Count = 1;
	resourceDesc.SampleDesc.Quality = 0;
	resourceDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
	if (!isUAV)
		resourceDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET;
	else
		resourceDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET | D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;

	D3D12_CLEAR_VALUE defaultClearValue = {};
	if (!clearValue)
	{
		defaultClearValue.Format = format;
		defaultClearValue.Color[0] = 0.0f;
		defaultClearValue.Color[1] = 0.0f;
		defaultClearValue.Color[2] = 0.0f;
		defaultClearValue.Color[3] = 1.0f;
	}
	else
	{
		defaultClearValue = *clearValue;
	}

	ComPtr<ID3D12Resource> resource = nullptr;

	ThrowIfFailed(_device->CreateCommittedResource(&heapProperties,
		D3D12_HEAP_FLAG_NONE,
		&resourceDesc,
		D3D12_RESOURCE_STATE_RENDER_TARGET,
		&defaultClearValue,
		IID_PPV_ARGS(&resource)));

	if (!rtName.empty())
		resource->SetName(rtName.c_str());

	UINT incSize = _device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	D3D12_CPU_DESCRIPTOR_HANDLE handle = _rtvDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
	handle.ptr += descriptorsCount * incSize;

	_device->CreateRenderTargetView(resource.Get(), nullptr, handle);

	_rtvDescriptors[descriptorsCount] = handle;
	std::shared_ptr<RenderTarget> outPtr = std::make_shared<RenderTarget>(descriptorsCount, resource, defaultClearValue);
	return outPtr;
}

std::shared_ptr<DepthStencil> RenderTargetManager::CreateDepthStencil(UINT64 width,
	UINT height,
	DXGI_FORMAT format,
	DXGI_FORMAT viewFormat,
	const std::wstring& dsName /*= L""*/,
	bool isUAV /*= false*/,
	const D3D12_CLEAR_VALUE* clearValue /*= nullptr*/)
{
	size_t descriptorsCount = _dsvDescriptors.size();
	if (descriptorsCount == maxDescriptorsCount)
		throw std::runtime_error("Not enough space for DS descriptors");

	D3D12_HEAP_PROPERTIES heapProperties = {};
	heapProperties.Type = D3D12_HEAP_TYPE_DEFAULT;
	heapProperties.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;

	D3D12_RESOURCE_DESC resourceDesc = {};
	resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	resourceDesc.Format = format;
	resourceDesc.Width = width;
	resourceDesc.Height = height;
	resourceDesc.DepthOrArraySize = 1;
	resourceDesc.MipLevels = 1;
	resourceDesc.Alignment = 0;
	resourceDesc.SampleDesc.Count = 1;
	resourceDesc.SampleDesc.Quality = 0;
	resourceDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
	if (!isUAV)
		resourceDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;
	else
		resourceDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL | D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;

	D3D12_CLEAR_VALUE defaultClearValue = {};
	if (!clearValue)
	{
		defaultClearValue.Format = viewFormat;
		defaultClearValue.DepthStencil.Depth = 1.0f;
		defaultClearValue.DepthStencil.Stencil = 0;
	}
	else
	{
		defaultClearValue = *clearValue;
	}

	ComPtr<ID3D12Resource> resource = nullptr;

	ThrowIfFailed(_device->CreateCommittedResource(&heapProperties,
		D3D12_HEAP_FLAG_NONE,
		&resourceDesc,
		D3D12_RESOURCE_STATE_COMMON,
		&defaultClearValue,
		IID_PPV_ARGS(&resource)));

	if (!dsName.empty())
		resource->SetName(dsName.c_str());

	UINT incSize = _device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);
	D3D12_CPU_DESCRIPTOR_HANDLE handle = _dsvDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
	handle.ptr += descriptorsCount * incSize;

	D3D12_DEPTH_STENCIL_VIEW_DESC viewDescription = {};
	viewDescription.Format = viewFormat;
	viewDescription.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;

	_device->CreateDepthStencilView(resource.Get(), &viewDescription, handle);

	_dsvDescriptors[descriptorsCount] = handle;
	std::shared_ptr<DepthStencil> outPtr = std::make_shared<DepthStencil>(descriptorsCount, resource, defaultClearValue);
	return outPtr;
}

std::vector<std::shared_ptr<RenderTarget>> RenderTargetManager::CreateRenderTargetsForSwapChain(ComPtr<IDXGISwapChain> swapChain)
{
	D3D12_CPU_DESCRIPTOR_HANDLE handle = _rtvDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
	size_t descriptorsCount = _rtvDescriptors.size();
	UINT incSize = _device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	handle.ptr += descriptorsCount * incSize;

	DXGI_SWAP_CHAIN_DESC swapChainDesc = {};
	ThrowIfFailed(swapChain->GetDesc(&swapChainDesc));

	std::vector<std::shared_ptr<RenderTarget>> outVector = {};

	D3D12_CLEAR_VALUE defaultClearValue = {};
	defaultClearValue.Format = swapChainDesc.BufferDesc.Format;
	defaultClearValue.Color[0] = 0.0f;
	defaultClearValue.Color[1] = 0.0f;
	defaultClearValue.Color[2] = 0.0f;
	defaultClearValue.Color[3] = 1.0f;

	for (UINT i = 0; i < swapChainDesc.BufferCount; ++i)
	{
		ComPtr<ID3D12Resource> resource = nullptr;
		ThrowIfFailed(swapChain->GetBuffer(i, IID_PPV_ARGS(&resource)));
		D3D12_RENDER_TARGET_VIEW_DESC viewDesc = {};
		viewDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
		viewDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;
		_device->CreateRenderTargetView(resource.Get(), &viewDesc, handle);
		resource->SetName(L"SwapChain RT");

		_rtvDescriptors[descriptorsCount] = handle;
		outVector.emplace_back(std::make_shared<RenderTarget>(descriptorsCount, resource, defaultClearValue));
		descriptorsCount++;
		handle.ptr += incSize;
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
