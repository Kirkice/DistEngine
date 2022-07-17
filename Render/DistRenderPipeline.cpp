#include "UploadBuffer.h"
#include "GeometryGenerator.h"
#include "GraphicsCore.h"
#include "DistRenderPipeline.h"

using Microsoft::WRL::ComPtr;
using namespace DirectX;
using namespace DirectX::PackedVector;

DistRenderPipeline::DistRenderPipeline(HINSTANCE hInstance) : GraphicsCore(hInstance)
{
}

DistRenderPipeline::~DistRenderPipeline()
{
	if (md3dDevice != nullptr)
		FlushCommandQueue();
}

void DistRenderPipeline::Render()
{
	//	获取材质Buffer
	auto MatBuffer = GetMatBuffer();

	//	绘制阴影Pass
	RenderShadowPass(MatBuffer);

	mCommandList->SetGraphicsRootSignature(mRootSignature.GetSignature());

	MatBuffer = mCurrFrameResource->PBRMaterialBuffer->Resource();
	mCommandList->SetGraphicsRootShaderResourceView(3, MatBuffer->GetGPUVirtualAddress());

	mCommandList->RSSetViewports(1, &mScreenViewport);
	mCommandList->RSSetScissorRects(1, &mScissorRect);

	mCommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(CurrentBackBuffer(), D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET));

	mCommandList->ClearRenderTargetView(CurrentBackBufferView(), mSceneManager.getInstance().mCameraSetting.SolidColor, 0, nullptr);
	mCommandList->ClearDepthStencilView(DepthStencilView(), D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.0f, 0, 0, nullptr);

	mCommandList->OMSetRenderTargets(1, &CurrentBackBufferView(), true, &DepthStencilView());


	mCommandList->SetGraphicsRootDescriptorTable(5, mSrvDescriptorHeap.GetDescriptorHeap()->GetGPUDescriptorHandleForHeapStart());

	auto passCB = mCurrFrameResource->PassCB->Resource();
	mCommandList->SetGraphicsRootConstantBufferView(2, passCB->GetGPUVirtualAddress());


	mCommandList->SetGraphicsRootDescriptorTable(4, mCubeMapTextures["Sky_specularIBL"]->GpuHandle);
	mCommandList->SetGraphicsRootDescriptorTable(4, mCubeMapTextures["Sky_diffuseIBL"]->GpuHandle);

	MatBuffer = mCurrFrameResource->PBRMaterialBuffer->Resource();
	mCommandList->SetGraphicsRootShaderResourceView(3, MatBuffer->GetGPUVirtualAddress());

	RenderOpaquePass();

	RenderAxisPass();

	//SkyBox
	auto skyMatBuffer = mCurrFrameResource->SkyBoxMaterialBuffer->Resource();
	mCommandList->SetGraphicsRootShaderResourceView(3, skyMatBuffer->GetGPUVirtualAddress());
	RenderSkyBoxPass();

	//DrawGizmos
	mCommandList->SetGraphicsRootShaderResourceView(3, MatBuffer->GetGPUVirtualAddress());
	RenderGizmosPass();

	mCommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(CurrentBackBuffer(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT));

	CopyColorPass();

	RenderPostProcessPass(MatBuffer);

}

/// <summary>
/// 获取MatBuffer
/// </summary>
/// <returns></returns>
ID3D12Resource* DistRenderPipeline::GetMatBuffer()
{
	ID3D12DescriptorHeap* descriptorHeaps[] = { mSrvDescriptorHeap.GetDescriptorHeap().Get() };
	mCommandList->SetDescriptorHeaps(_countof(descriptorHeaps), descriptorHeaps);
	mCommandList->SetGraphicsRootSignature(mRootSignature.GetSignature());
	return mCurrFrameResource->PBRMaterialBuffer->Resource();
}

/// <summary>
/// 绘制阴影Pass
/// </summary>
/// <param name="matBuffer"></param>
void DistRenderPipeline::RenderShadowPass(ID3D12Resource* matBuffer)
{
	mCommandList->SetGraphicsRootShaderResourceView(3, matBuffer->GetGPUVirtualAddress());
	mCommandList->SetGraphicsRootDescriptorTable(5, mSrvDescriptorHeap.GetDescriptorHeap()->GetGPUDescriptorHandleForHeapStart());

	DrawSceneToShadowMap();
}

void DistRenderPipeline::DepthPrePass()
{

}

void DistRenderPipeline::RenderGBuffer()
{

}

/// <summary>
/// 绘制不透明材质
/// </summary>
void DistRenderPipeline::RenderOpaquePass()
{
	mCommandList->SetPipelineState(mPSOs["litOpaque"].Get());
	DrawRenderItems(mCommandList.Get(), mRitemLayer[(int)RenderLayer::Opaque]);
}

void DistRenderPipeline::DeferredRenderPass()
{

}

/// <summary>
/// 绘制天空球
/// </summary>
void DistRenderPipeline::RenderSkyBoxPass()
{
	if (mSceneManager.getInstance().mCameraSetting.renderSkyBox)
	{
		mCommandList->SetPipelineState(mPSOs["sky"].Get());
		DrawRenderItems(mCommandList.Get(), mRitemLayer[(int)RenderLayer::Sky]);
	}
}

void DistRenderPipeline::RenderTransparentPass()
{

}

void DistRenderPipeline::RenderAxisPass()
{
	mCommandList->SetPipelineState(mPSOs["Axis"].Get());
	DrawRenderItems(mCommandList.Get(), mRitemLayer[(int)RenderLayer::Axis]);
}

void DistRenderPipeline::RenderGizmosPass()
{
	if (ShowGizmos)
	{
		mCommandList->SetPipelineState(mPSOs["Gizmo"].Get());
		DrawRenderItems(mCommandList.Get(), mRitemLayer[(int)RenderLayer::Gizmo]);
	}

	if (ShowWire)
	{
		mCommandList->SetPipelineState(mPSOs["Line"].Get());
		DrawRenderItems(mCommandList.Get(), mRitemLayer[(int)RenderLayer::Line]);
	}
}

void DistRenderPipeline::RenderPostProcessPass(ID3D12Resource* matBuffer)
{
	if (mPostProcessSwitch.ShowVolumeFog)
		DrawVolumeFog(matBuffer);

}

//	DrawVolumeFog
void DistRenderPipeline::DrawVolumeFog(ID3D12Resource* matBuffer)
{
	mCommandList->SetGraphicsRootDescriptorTable(4, mRenderTarget->GpuSrv());
	matBuffer = mCurrFrameResource->PostMaterialBuffer->Resource();
	mCommandList->SetGraphicsRootShaderResourceView(3, matBuffer->GetGPUVirtualAddress());

	mCommandList->SetPipelineState(mPSOs["VolumeFog"].Get());
	DrawRenderItems(mCommandList.Get(), mRitemLayer[(int)RenderLayer::PostProcess]);

	mCommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(CurrentBackBuffer(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT));

	mCommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(mRenderTarget->Resource(), D3D12_RESOURCE_STATE_GENERIC_READ, D3D12_RESOURCE_STATE_COPY_DEST));

	mCommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(CurrentBackBuffer(), D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_COPY_SOURCE));

	mCommandList->CopyResource(mRenderTarget->Resource(), CurrentBackBuffer());

	mCommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(CurrentBackBuffer(), D3D12_RESOURCE_STATE_COPY_SOURCE, D3D12_RESOURCE_STATE_PRESENT));

	mCommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(mRenderTarget->Resource(),
		D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_GENERIC_READ));
}


void DistRenderPipeline::CopyColorPass()
{
	mCommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(mRenderTarget->Resource(), D3D12_RESOURCE_STATE_GENERIC_READ, D3D12_RESOURCE_STATE_COPY_DEST));

	mCommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(CurrentBackBuffer(), D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_COPY_SOURCE));

	mCommandList->CopyResource(mRenderTarget->Resource(), CurrentBackBuffer());

	mCommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(CurrentBackBuffer(), D3D12_RESOURCE_STATE_COPY_SOURCE, D3D12_RESOURCE_STATE_PRESENT));

	mCommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(mRenderTarget->Resource(),
		D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_GENERIC_READ));
}