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

		//	加载纹理资源
		mResourceManager.LoadTexture2D(md3dDevice, mSrvHeap, mCommandList);

		//	初始化Scene
		m_SceneRender.InitScene(md3dDevice, mCommandList);

		//	初始化SceneRender
		m_SceneRender.InitSceneRender(md3dDevice, mCommandList, mDepthStencilBuffer, SwapChainBufferCount, mDsvHeap, mDsvDescriptorSize, mRtvHeap, mRtvDescriptorSize, mCbvSrvUavDescriptorSize, mSrvHeap, mResourceManager,mClientWidth, mClientHeight);

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

		//mCamera.SetLens((mCamera.GetFovY() / 180) * Dist::Pi, AspectRatio(), mCamera.GetNearZ(), mCamera.GetFarZ());

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
		//mCamera.SetLens((mCamera.GetFovY() / 180) * Dist::Pi, AspectRatio(), mCamera.GetNearZ(), mCamera.GetFarZ());

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
		auto matBuffer = m_SceneRender.mCurrFrameResource->PBRMaterialBuffer->Resource();
		DrawShadowMap(matBuffer);
	}

	void RenderApp::DrawRenderItems(ID3D12GraphicsCommandList* cmdList, const std::vector<PBRRenderItem*>& ritems)
	{
		UINT objCBByteSize = SystemUtils::CalcConstantBufferByteSize(sizeof(ObjectConstants));

		auto objectCB = m_SceneRender.mCurrFrameResource->ObjectCB->Resource();

		for (size_t i = 0; i < ritems.size(); ++i)
		{
			auto ri = ritems[i];

			cmdList->IASetVertexBuffers(0, 1, &ri->Geo->VertexBufferView());
			cmdList->IASetIndexBuffer(&ri->Geo->IndexBufferView());
			cmdList->IASetPrimitiveTopology(ri->PrimitiveType);

			D3D12_GPU_VIRTUAL_ADDRESS objCBAddress = objectCB->GetGPUVirtualAddress() + ri->ObjCBIndex * objCBByteSize;

			cmdList->SetGraphicsRootConstantBufferView(0, objCBAddress);

			cmdList->DrawIndexedInstanced(ri->IndexCount, 1, ri->StartIndexLocation, ri->BaseVertexLocation, 0);
		}
	}

	void RenderApp::DrawShadowMap(ID3D12Resource* matBuffer)
	{
		mCommandList->SetGraphicsRootShaderResourceView(3, matBuffer->GetGPUVirtualAddress());
		mCommandList->SetGraphicsRootDescriptorTable(5, mSrvHeap->GetGPUDescriptorHandleForHeapStart());
		DrawSceneToShadowMap();
	}

	void RenderApp::DrawSceneToShadowMap()
	{
		mCommandList->RSSetViewports(1, &m_SceneRender.mShadowMapPass->Viewport());
		mCommandList->RSSetScissorRects(1, &m_SceneRender.mShadowMapPass->ScissorRect());

		// Change to DEPTH_WRITE.
		mCommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(m_SceneRender.mShadowMapPass->Resource(),
			D3D12_RESOURCE_STATE_GENERIC_READ, D3D12_RESOURCE_STATE_DEPTH_WRITE));

		// Clear the back buffer and depth buffer.
		mCommandList->ClearDepthStencilView(m_SceneRender.mShadowMapPass->Dsv(),
			D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.0f, 0, 0, nullptr);

		// Specify the buffers we are going to render to.
		mCommandList->OMSetRenderTargets(0, nullptr, false, &m_SceneRender.mShadowMapPass->Dsv());

		// Bind the pass constant buffer for the shadow map pass.
		UINT passCBByteSize = SystemUtils::CalcConstantBufferByteSize(sizeof(PassConstants));
		auto passCB = m_SceneRender.mCurrFrameResource->PassCB->Resource();
		D3D12_GPU_VIRTUAL_ADDRESS passCBAddress = passCB->GetGPUVirtualAddress() + 1 * passCBByteSize;
		mCommandList->SetGraphicsRootConstantBufferView(2, passCBAddress);

		mCommandList->SetPipelineState(m_SceneRender.mPSOs["shadow_opaque"].Get());
		DrawRenderItems(mCommandList.Get(), m_SceneRender.mRitemLayer[(int)RenderLayer::Opaque]);

		// Change back to GENERIC_READ so we can read the texture in a shader.
		mCommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(m_SceneRender.mShadowMapPass->Resource(),
			D3D12_RESOURCE_STATE_DEPTH_WRITE, D3D12_RESOURCE_STATE_GENERIC_READ));
	}
}