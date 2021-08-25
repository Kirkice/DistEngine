/* $Header: /common/Core/include/D3D12.h			    8/25/21 18:26p Kirk 			    $ */
/*--------------------------------------------------------------------------------------------*
*                                                                                             *
*						Project Name : DistEngine                                             *
*                                                                                             *
*						File Name : D3D12.h											          *
*                                                                                             *
*                       Programmer : Kirk                                                     *
*                                                                                             *
*---------------------------------------------------------------------------------------------*/
#pragma once

#include "../../Core/include/PCH.h"

namespace render
{
	namespace d3d12
	{
		// Constants
		const uint64 RenderLatency = 2;

		// Externals
		extern ID3D12Device5* Device;
		extern ID3D12GraphicsCommandList4* CmdList;
		extern ID3D12CommandQueue* GfxQueue;
		extern D3D_FEATURE_LEVEL FeatureLevel;
		extern IDXGIFactory4* Factory;
		extern IDXGIAdapter1* Adapter;

		extern uint64 CurrentCPUFrame; 
		extern uint64 CurrentGPUFrame;
		extern uint64 CurrFrameIdx;

		// Lifetime
		void Initialize(D3D_FEATURE_LEVEL minFeatureLevel, uint32 adapterIdx);
		void Shutdown();

		// Frame submission synchronization
		void BeginFrame();
		void EndFrame(IDXGISwapChain4* swapChain, uint32 syncIntervals);
		void FlushGPU();

		void DeferredRelease_(IUnknown* resource, bool forceDeferred = false);

		template<typename T> void DeferredRelease(T*& resource, bool forceDeferred = false)
		{
			IUnknown* base = resource;
			DeferredRelease_(base, forceDeferred);
			resource = nullptr;
		}

		template<typename T> void Release(T*& resource)
		{
			if (resource != nullptr) {
				resource->Release();
				resource = nullptr;
			}
		}

		void DeferredCreateSRV(ID3D12Resource* resource, const D3D12_SHADER_RESOURCE_VIEW_DESC& desc, uint32 descriptorIdx);

	}
}
