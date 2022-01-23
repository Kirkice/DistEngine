#pragma once

#ifdef _GAMING_XBOX_SCARLETT
#include <d3d12_xs.h>
#elif (defined(_XBOX_ONE) && defined(_TITLE)) || defined(_GAMING_XBOX)
#include <d3d12_x.h>
#else
#include <d3d12.h>
#endif

#include <functional>

#ifdef NTDDI_WIN10_FE
#include <ocidl.h>
#else
#include <OCIdl.h>
#endif
#include <wincodec.h>
#pragma comment(lib,"uuid.lib")


namespace DirectX
{
	HRESULT __cdecl SaveDDSTextureToFile(
		_In_ ID3D12CommandQueue* pCommandQueue,
		_In_ ID3D12Resource* pSource,
		_In_z_ const wchar_t* fileName,
		D3D12_RESOURCE_STATES beforeState = D3D12_RESOURCE_STATE_RENDER_TARGET,
		D3D12_RESOURCE_STATES afterState = D3D12_RESOURCE_STATE_RENDER_TARGET) noexcept;

	HRESULT __cdecl SaveWICTextureToFile(
		_In_ ID3D12CommandQueue* pCommandQ,
		_In_ ID3D12Resource* pSource,
		REFGUID guidContainerFormat,
		_In_z_ const wchar_t* fileName,
		D3D12_RESOURCE_STATES beforeState = D3D12_RESOURCE_STATE_RENDER_TARGET,
		D3D12_RESOURCE_STATES afterState = D3D12_RESOURCE_STATE_RENDER_TARGET,
		_In_opt_ const GUID* targetFormat = nullptr,
		_In_opt_ std::function<void __cdecl(IPropertyBag2*)> setCustomProps = nullptr,
		bool forceSRGB = false);
}
