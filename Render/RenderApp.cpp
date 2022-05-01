#include "RenderApp.h"
#include "RenderScene/DefaultSceneRender.h"

using Microsoft::WRL::ComPtr;
using namespace DirectX;
using namespace DirectX::PackedVector;

namespace Dist
{
	RenderApp::RenderApp(HINSTANCE hInstance) : InputSystem(hInstance)
	{
	}

	RenderApp::~RenderApp()
	{
		if (md3dDevice != nullptr)
			FlushCommandQueue();
	}

	bool RenderApp::Initialize()
	{
		if (!SystemApp::Initialize())
			return false;

		ThrowIfFailed(mCommandList->Reset(mDirectCmdListAlloc.Get(), nullptr));

		//	初始化Scene
		m_SceneRender.InitScene(md3dDevice, mCommandList);

		//	初始化SceneRender
		m_SceneRender.InitSceneRender(md3dDevice, mCommandList, mDepthStencilBuffer, SwapChainBufferCount, mDsvHeap, mDsvDescriptorSize, mRtvHeap, mRtvDescriptorSize, mCbvSrvUavDescriptorSize, mClientWidth, mClientHeight);

		//	PSO
		m_SceneRender.BuildPSO(md3dDevice, mBackBufferFormat, m4xMsaaState, m4xMsaaQuality, mDepthStencilFormat);

		// Execute the initialization commands.
		ThrowIfFailed(mCommandList->Close());
		ID3D12CommandList* cmdsLists[] = { mCommandList.Get() };
		mCommandQueue->ExecuteCommandLists(_countof(cmdsLists), cmdsLists);

		// Wait until initialization is complete.
		FlushCommandQueue();

		return true;
	}

	void RenderApp::CreateRtvAndDsvDescriptorHeaps()
	{
		// Add +1 for screen normal map, +2 for ambient maps.
		D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc;
		rtvHeapDesc.NumDescriptors = SwapChainBufferCount + 3;
		rtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
		rtvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
		rtvHeapDesc.NodeMask = 0;
		ThrowIfFailed(md3dDevice->CreateDescriptorHeap(
			&rtvHeapDesc, IID_PPV_ARGS(mRtvHeap.GetAddressOf())));

		// Add +1 DSV for shadow map.
		D3D12_DESCRIPTOR_HEAP_DESC dsvHeapDesc;
		dsvHeapDesc.NumDescriptors = 2;
		dsvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
		dsvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
		dsvHeapDesc.NodeMask = 0;
		ThrowIfFailed(md3dDevice->CreateDescriptorHeap(
			&dsvHeapDesc, IID_PPV_ARGS(mDsvHeap.GetAddressOf())));

		D3D12_DESCRIPTOR_HEAP_DESC SrvHeapDesc;
		SrvHeapDesc.NumDescriptors = 1;
		SrvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
		SrvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
		SrvHeapDesc.NodeMask = 0;
		ThrowIfFailed(md3dDevice->CreateDescriptorHeap(
			&SrvHeapDesc, IID_PPV_ARGS(mSrvHeap.GetAddressOf())));
	}

	void RenderApp::OnResize()
	{
		EngineApp::OnResize();

		mCamera.SetLens((mCamera.GetFovY() / 180) * Dist::Pi, AspectRatio(), mCamera.GetNearZ(), mCamera.GetFarZ());

		if (m_SceneRender.mSsao != nullptr)
		{
			m_SceneRender.mSsao->OnResize(mClientWidth, mClientHeight);

			// Resources changed, so need to rebuild descriptors.
			m_SceneRender.mSsao->RebuildDescriptors(mDepthStencilBuffer.Get());
		}
	}

	void RenderApp::Update(const GameTimer& gt)
	{
		OnKeyboardInput(gt);
		mCamera.SetLens((mCamera.GetFovY() / 180) * Dist::Pi, AspectRatio(), mCamera.GetNearZ(), mCamera.GetFarZ());

		// Cycle through the circular frame resource array.
		m_SceneRender.mCurrFrameResourceIndex = (m_SceneRender.mCurrFrameResourceIndex + 1) % gNumFrameResources;
		m_SceneRender.mCurrFrameResource = m_SceneRender.mFrameResources[m_SceneRender.mCurrFrameResourceIndex].get();

		// Has the GPU finished processing the commands of the current frame resource?
		// If not, wait until the GPU has completed commands up to this fence point.
		if (m_SceneRender.mCurrFrameResource->Fence != 0 && mFence->GetCompletedValue() < m_SceneRender.mCurrFrameResource->Fence)
		{
			HANDLE eventHandle = CreateEventEx(nullptr, false, false, EVENT_ALL_ACCESS);
			ThrowIfFailed(mFence->SetEventOnCompletion(m_SceneRender.mCurrFrameResource->Fence, eventHandle));
			WaitForSingleObject(eventHandle, INFINITE);
			CloseHandle(eventHandle);
		}

		//	更新
		m_SceneRender.UpdateScene(gt);
		m_SceneRender.UpdateSceneRender(gt,mClientWidth,mClientHeight);
	}

	void RenderApp::Draw(const GameTimer& gt)
	{
		//Init    PSO / RootSignature / Material  /  descriptorHeaps
		auto cmdListAlloc = m_SceneRender.mCurrFrameResource->CmdListAlloc;

		ThrowIfFailed(cmdListAlloc->Reset());
		ThrowIfFailed(mCommandList->Reset(cmdListAlloc.Get(), m_SceneRender.mPSOs["opaque"].Get()));

		ID3D12DescriptorHeap* descriptorHeaps[] = { m_SceneRender.mSrvDescriptorHeap.Get() };
		mCommandList->SetDescriptorHeaps(_countof(descriptorHeaps), descriptorHeaps);
		mCommandList->SetGraphicsRootSignature(m_SceneRender.mRootSignature.Get());
		auto matBuffer = m_SceneRender.mCurrFrameResource->PBRMaterialBuffer->Resource();

		ThrowIfFailed(mCommandList->Close());
		ID3D12CommandList* cmdsLists[] = { mCommandList.Get() };
		mCommandQueue->ExecuteCommandLists(_countof(cmdsLists), cmdsLists);

		ThrowIfFailed(mSwapChain->Present(0, 0));
		mCurrBackBuffer = (mCurrBackBuffer + 1) % SwapChainBufferCount;

		m_SceneRender.mCurrFrameResource->Fence = ++mCurrentFence;

		mCommandQueue->Signal(mFence.Get(), mCurrentFence);
	}
}