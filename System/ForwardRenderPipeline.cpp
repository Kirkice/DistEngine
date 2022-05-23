#include "ForwardRenderPipeline.h"
#include "UploadBuffer.h"
#include "GeometryGenerator.h"
#include "GraphicsCore.h"

using Microsoft::WRL::ComPtr;
using namespace DirectX;
using namespace DirectX::PackedVector;

ForwardRenderer::ForwardRenderer(HINSTANCE hInstance) : GraphicsCore(hInstance)
{
}

ForwardRenderer::~ForwardRenderer()
{
	if (md3dDevice != nullptr)
		FlushCommandQueue();
} 

void ForwardRenderer::Draw(const GameTimer& gt)
{
	//Init    PSO / RootSignature / Material  /  descriptorHeaps
	auto cmdListAlloc = mCurrFrameResource->CmdListAlloc;

	ThrowIfFailed(cmdListAlloc->Reset());
	ThrowIfFailed(mCommandList->Reset(cmdListAlloc.Get(), mPSOs["opaque"].Get()));

	ID3D12DescriptorHeap* descriptorHeaps[] = { mSrvDescriptorHeap.Get() };
	mCommandList->SetDescriptorHeaps(_countof(descriptorHeaps), descriptorHeaps);
	mCommandList->SetGraphicsRootSignature(mRootSignature.GetSignature());
	auto matBuffer = mCurrFrameResource->PBRMaterialBuffer->Resource();

	////Draw ShadowMap
	//DrawShadowMap(matBuffer);

	////SSAO
	//DrawSSAO(matBuffer);

	mCommandList->SetGraphicsRootSignature(mRootSignature.GetSignature());

	matBuffer = mCurrFrameResource->PBRMaterialBuffer->Resource();
	mCommandList->SetGraphicsRootShaderResourceView(3, matBuffer->GetGPUVirtualAddress());


	mCommandList->RSSetViewports(1, &mScreenViewport);
	mCommandList->RSSetScissorRects(1, &mScissorRect);

	mCommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(CurrentBackBuffer(), D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET));

	mCommandList->ClearRenderTargetView(CurrentBackBufferView(), SolidColor, 0, nullptr);
	mCommandList->ClearDepthStencilView(DepthStencilView(), D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.0f, 0, 0, nullptr);

	mCommandList->OMSetRenderTargets(1, &CurrentBackBufferView(), true, &DepthStencilView());

	mCommandList->SetGraphicsRootDescriptorTable(5, mSrvDescriptorHeap->GetGPUDescriptorHandleForHeapStart());

	auto passCB = mCurrFrameResource->PassCB->Resource();
	mCommandList->SetGraphicsRootConstantBufferView(2, passCB->GetGPUVirtualAddress());


	mCommandList->SetGraphicsRootDescriptorTable(4, mCubeMapTextures["DGarden_specularIBL"]->GpuHandle);
	mCommandList->SetGraphicsRootDescriptorTable(4, mCubeMapTextures["DGarden_diffuseIBL"]->GpuHandle);

	ForwardRenderer::DrawOpaque();

	//SkyBox
	auto skyMatBuufer = mCurrFrameResource->SkyBoxMaterialBuffer->Resource();
	mCommandList->SetGraphicsRootShaderResourceView(3, skyMatBuufer->GetGPUVirtualAddress());
	ForwardRenderer::DrawSkyBox();


	//ImGui_ImplDX12_NewFrame();
	//ImGui_ImplWin32_NewFrame();
	//ImGui::NewFrame();

	//ImGui::Begin("DirectX12 Texture Test");
	//ImGui::Text("DGarden_specularIBL = %d", mCubeMapTextures["DGarden_specularIBL"]->TexIndex);
	//ImGui::Text("DGarden_diffuseIBL = %d", mCubeMapTextures["DGarden_diffuseIBL"]->TexIndex);
	//ImGui::End();
	//mCommandList->SetDescriptorHeaps(1, mSrvHeap.GetAddressOf());
	//ImGui::Render();
	//ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), mCommandList.Get());


	mCommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(CurrentBackBuffer(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT));


	ThrowIfFailed(mCommandList->Close());
	ID3D12CommandList* cmdsLists[] = { mCommandList.Get() };
	mCommandQueue->ExecuteCommandLists(_countof(cmdsLists), cmdsLists);

	ThrowIfFailed(mSwapChain->Present(0, 0));
	mCurrBackBuffer = (mCurrBackBuffer + 1) % SwapChainBufferCount;

	mCurrFrameResource->Fence = ++mCurrentFence;

	mCommandQueue->Signal(mFence.Get(), mCurrentFence);
}


//	DrawShadowMap
void ForwardRenderer::DrawShadowMap(ID3D12Resource* matBuffer)
{
	mCommandList->SetGraphicsRootShaderResourceView(3, matBuffer->GetGPUVirtualAddress());

	//mCommandList->SetGraphicsRootDescriptorTable(4, mNullSrv);

	mCommandList->SetGraphicsRootDescriptorTable(5, mSrvDescriptorHeap->GetGPUDescriptorHandleForHeapStart());
	DrawSceneToShadowMap();
}

void ForwardRenderer::DrawSSAO(ID3D12Resource* matBuffer)
{
	ForwardRenderer::DrawDepthNormal();
	mCommandList->SetGraphicsRootSignature(mSsaoRootSignature.GetSignature());
	mSsao->ComputeSsao(mCommandList.Get(), mCurrFrameResource, 2);
}

//	DrawDepthNormal
void ForwardRenderer::DrawDepthNormal()
{
	DrawNormalsAndDepth();
}

//	DrawOpaque
void ForwardRenderer::DrawOpaque()
{
	mCommandList->SetPipelineState(mPSOs["litOpaque"].Get());
	DrawRenderItems(mCommandList.Get(), mRitemLayer[(int)RenderLayer::Opaque]);
}

//	DrawSkyBox
void ForwardRenderer::DrawSkyBox()
{
	if (renderSkyBox)
	{
		mCommandList->SetPipelineState(mPSOs["sky"].Get());
		DrawRenderItems(mCommandList.Get(), mRitemLayer[(int)RenderLayer::Sky]);
	}
}