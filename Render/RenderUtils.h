#pragma once
#include "../Core/Mathf/Mathf.h"
#include "GraphicsCore.h"

using namespace Mathf;

using Microsoft::WRL::ComPtr;
using namespace DirectX;
using namespace DirectX::PackedVector;

class RenderUtils
{
//public:
//	static ID3D12Resource* GetMatBuffer();
//
//	static void DIST_SET_SRV(
//		UINT RootParameterIndex,
//		D3D12_GPU_VIRTUAL_ADDRESS BufferLocation
//	);
//
//	static void DIST_SET_DESCRIPTABLE_TABLE(
//		UINT RootParameterIndex,
//		D3D12_GPU_DESCRIPTOR_HANDLE BaseDescriptor
//	);
//
//	static void DIST_RESOURCE_BARRIER_READ_COPY_DEST(ID3D12Resource* Resource);
//
//	static void DIST_RESOURCE_BARRIER_READ_COPY_SOURCE(ID3D12Resource* Resource);
//
//	static void DIST_CLEAR_RTV(
//		_In_  D3D12_CPU_DESCRIPTOR_HANDLE RenderTargetView,
//		_In_  const FLOAT ColorRGBA[4],
//		_In_  UINT NumRects,
//		_In_reads_(NumRects)  const D3D12_RECT* pRects);
//
//	static void DIST_CLEAR_DSV(
//		_In_  D3D12_CPU_DESCRIPTOR_HANDLE DepthStencilView,
//		_In_  D3D12_CLEAR_FLAGS ClearFlags,
//		_In_  FLOAT Depth,
//		_In_  UINT8 Stencil,
//		_In_  UINT NumRects,
//		_In_reads_(NumRects)  const D3D12_RECT* pRects
//	);
};