#pragma once
#include "Mathf.h"
#include "d3dUtil.h"

class RenderTexture
{
public:
	RenderTexture(DXGI_FORMAT format) noexcept;

	void SetDevice(_In_ ID3D12Device* device, D3D12_CPU_DESCRIPTOR_HANDLE srvDescriptor, D3D12_CPU_DESCRIPTOR_HANDLE rtvDescriptor);

	void SizeResources(size_t width, size_t height);

	void ReleaseDevice() noexcept;

	void TransitionTo(_In_ ID3D12GraphicsCommandList* commandList, D3D12_RESOURCE_STATES afterState);

	void BeginScene(_In_ ID3D12GraphicsCommandList* commandList)
	{
		TransitionTo(commandList, D3D12_RESOURCE_STATE_RENDER_TARGET);
	}

	void EndScene(_In_ ID3D12GraphicsCommandList* commandList)
	{
		TransitionTo(commandList, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
	}

	void SetClearColor(DirectX::FXMVECTOR color)
	{
		DirectX::XMStoreFloat4(reinterpret_cast<DirectX::XMFLOAT4*>(m_clearColor), color);
	}

	ID3D12Resource* GetResource() const noexcept { return m_resource.Get(); }
	D3D12_RESOURCE_STATES GetCurrentState() const noexcept { return m_state; }

	void SetWindow(const RECT& rect);

	DXGI_FORMAT GetFormat() const noexcept { return m_format; }

private:
	Microsoft::WRL::ComPtr<ID3D12Device>                m_device;
	Microsoft::WRL::ComPtr<ID3D12Resource>              m_resource;
	D3D12_RESOURCE_STATES                               m_state;
	D3D12_CPU_DESCRIPTOR_HANDLE                         m_srvDescriptor;
	D3D12_CPU_DESCRIPTOR_HANDLE                         m_rtvDescriptor;
	float                                               m_clearColor[4];

	DXGI_FORMAT                                         m_format;

	size_t                                              m_width;
	size_t                                              m_height;
};