#include "RenderCore.h"
#include "UploadBuffer.h"
#include "GeometryGenerator.h"
#include "GraphicsCore.h"

using Microsoft::WRL::ComPtr;
using namespace DirectX;
using namespace DirectX::PackedVector;

RenderCore::RenderCore(HINSTANCE hInstance) : GraphicsCore(hInstance)
{
}

RenderCore::~RenderCore()
{
	if (md3dDevice != nullptr)
		FlushCommandQueue();
}

void RenderCore::Draw(const GameTimer& gt)
{
	ID3D12DescriptorHeap* descriptorHeaps[] = { mSrvDescriptorHeap.GetDescriptorHeap().Get() };
	mCommandList->SetDescriptorHeaps(_countof(descriptorHeaps), descriptorHeaps);
	mCommandList->SetGraphicsRootSignature(mRootSignature.GetSignature());
	auto matBuffer = mCurrFrameResource->PBRMaterialBuffer->Resource();

	//	Draw ShadowMap
	DrawShadowMap(matBuffer);

	//	SSAO
	DrawSSAO(matBuffer);

	mCommandList->SetGraphicsRootSignature(mRootSignature.GetSignature());
	matBuffer = mCurrFrameResource->PBRMaterialBuffer->Resource();
	mCommandList->SetGraphicsRootShaderResourceView(3, matBuffer->GetGPUVirtualAddress());
	mCommandList->RSSetViewports(1, &mScreenViewport);
	mCommandList->RSSetScissorRects(1, &mScissorRect);

	//	BindGBuffer
	BindGBuffer();







	//mCommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(CurrentBackBuffer(), D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET));
	//mCommandList->ClearRenderTargetView(CurrentBackBufferView(), SolidColor, 0, nullptr);
	//mCommandList->ClearDepthStencilView(DepthStencilView(), D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.0f, 0, 0, nullptr);

	//mCommandList->OMSetRenderTargets(1, &CurrentBackBufferView(), true, &DepthStencilView());

	mCommandList->SetGraphicsRootDescriptorTable(5, mSrvDescriptorHeap.GetDescriptorHeap()->GetGPUDescriptorHandleForHeapStart());

	auto passCB = mCurrFrameResource->PassCB->Resource();
	mCommandList->SetGraphicsRootConstantBufferView(2, passCB->GetGPUVirtualAddress());


	mCommandList->SetGraphicsRootDescriptorTable(4, mCubeMapTextures["DGarden_specularIBL"]->GpuHandle);
	mCommandList->SetGraphicsRootDescriptorTable(4, mCubeMapTextures["DGarden_diffuseIBL"]->GpuHandle);





	//DrawOpaque();

	////SkyBox
	//auto skyMatBuufer = mCurrFrameResource->SkyBoxMaterialBuffer->Resource();
	//mCommandList->SetGraphicsRootShaderResourceView(3, skyMatBuufer->GetGPUVirtualAddress());
	//RenderCore::DrawSkyBox();
	//mCommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(CurrentBackBuffer(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT));

	//RenderCore::CopyColorPass();
}

/// <summary>
/// 绑定GBuffer到渲染管线
/// </summary>
void RenderCore::BindGBuffer()
{
	//	ResourceBarrier
	std::array<D3D12_RESOURCE_BARRIER, 4> barriers = {
	CD3DX12_RESOURCE_BARRIER::Transition(mGBuffer->GetGBuffer0(), D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, D3D12_RESOURCE_STATE_RENDER_TARGET),
	CD3DX12_RESOURCE_BARRIER::Transition(mGBuffer->GetGBuffer1(), D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, D3D12_RESOURCE_STATE_RENDER_TARGET),
	CD3DX12_RESOURCE_BARRIER::Transition(mGBuffer->GetGBuffer2(), D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, D3D12_RESOURCE_STATE_RENDER_TARGET),
	CD3DX12_RESOURCE_BARRIER::Transition(mGBuffer->GetGBuffer3(), D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, D3D12_RESOURCE_STATE_RENDER_TARGET),
	};
	mCommandList->ResourceBarrier((UINT)barriers.size(), barriers.data());

	//	ClearRenderTargetView
	float Black[4] = { 0,0,0,1 };
	mCommandList->ClearRenderTargetView(mGBuffer->GBuffer0CPUSrv(), Black, 0, nullptr);
	mCommandList->ClearRenderTargetView(mGBuffer->GBuffer1CPUSrv(), Black, 0, nullptr);
	mCommandList->ClearRenderTargetView(mGBuffer->GBuffer2CPUSrv(), Black, 0, nullptr);
	mCommandList->ClearRenderTargetView(mGBuffer->GBuffer3CPUSrv(), Black, 0, nullptr);

	D3D12_CPU_DESCRIPTOR_HANDLE rtCPUDescriptors[4] = {
		mGBuffer->GBuffer0CPUSrv(),
		mGBuffer->GBuffer1CPUSrv(),
		mGBuffer->GBuffer2CPUSrv(),
		mGBuffer->GBuffer3CPUSrv(),
	};

	mCommandList->OMSetRenderTargets(4, rtCPUDescriptors, FALSE, nullptr);
}

void RenderCore::CopyColorPass()
{
	mCommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(mRenderTarget->Resource(), D3D12_RESOURCE_STATE_GENERIC_READ, D3D12_RESOURCE_STATE_COPY_DEST));

	mCommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(CurrentBackBuffer(), D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_COPY_SOURCE));

	mCommandList->CopyResource(mRenderTarget->Resource(), CurrentBackBuffer());

	mCommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(CurrentBackBuffer(), D3D12_RESOURCE_STATE_COPY_SOURCE, D3D12_RESOURCE_STATE_PRESENT));

	mCommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(mRenderTarget->Resource(),
		D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_GENERIC_READ));
}


//	DrawShadowMap
void RenderCore::DrawShadowMap(ID3D12Resource* matBuffer)
{
	mCommandList->SetGraphicsRootShaderResourceView(3, matBuffer->GetGPUVirtualAddress());

	mCommandList->SetGraphicsRootDescriptorTable(5, mSrvDescriptorHeap.GetDescriptorHeap()->GetGPUDescriptorHandleForHeapStart());
	DrawSceneToShadowMap();
}

void RenderCore::DrawSSAO(ID3D12Resource* matBuffer)
{
	RenderCore::DrawDepthNormal();
	mCommandList->SetGraphicsRootSignature(mSsaoRootSignature.GetSignature());
	mSsao->ComputeSsao(mCommandList.Get(), mCurrFrameResource, 2);
}

//	DrawDepthNormal
void RenderCore::DrawDepthNormal()
{
	DrawNormalsAndDepth();
}

//	DrawOpaque
void RenderCore::DrawOpaque()
{
	mCommandList->SetPipelineState(mPSOs["litOpaque"].Get());
	DrawRenderItems(mCommandList.Get(), mRitemLayer[(int)RenderLayer::Opaque]);
}

//	DrawSkyBox
void RenderCore::DrawSkyBox()
{
	if (renderSkyBox)
	{
		mCommandList->SetPipelineState(mPSOs["sky"].Get());
		DrawRenderItems(mCommandList.Get(), mRitemLayer[(int)RenderLayer::Sky]);
	}
}