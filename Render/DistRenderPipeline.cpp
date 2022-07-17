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
	//	����������
	SetDescriptorHeap();

	//	���ø�ǩ��
	SetRootSignature();

	//	���ò���Buffer
	SetMatBuffer(MatBufferType::PBR);

	//	������ӰPass
	RenderShadowPass(mMatBuffer);

	SetRootSignature();

	mCommandList->SetGraphicsRootShaderResourceView(3, mMatBuffer->GetGPUVirtualAddress());

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

	SetMatBuffer(MatBufferType::PBR);
	mCommandList->SetGraphicsRootShaderResourceView(3, mMatBuffer->GetGPUVirtualAddress());

	RenderOpaquePass();

	RenderAxisPass();

	//SkyBox
	SetMatBuffer(MatBufferType::SkyBox);
	mCommandList->SetGraphicsRootShaderResourceView(3, mMatBuffer->GetGPUVirtualAddress());
	RenderSkyBoxPass();

	//DrawGizmos
	SetMatBuffer(MatBufferType::PBR);
	mCommandList->SetGraphicsRootShaderResourceView(3, mMatBuffer->GetGPUVirtualAddress());
	RenderGizmosPass();

	mCommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(CurrentBackBuffer(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT));

	CopyColorPass();

	RenderPostProcessPass();

}

/// <summary>
/// ������ӰPass
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
/// ���Ʋ�͸������
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
/// ���������
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

void DistRenderPipeline::RenderPostProcessPass()
{
	if (mPostProcessSwitch.ShowVolumeFog)
		DrawVolumeFog();

}

//	DrawVolumeFog
void DistRenderPipeline::DrawVolumeFog()
{
	mCommandList->SetGraphicsRootDescriptorTable(4, mRenderTarget->GpuSrv());
	SetMatBuffer(MatBufferType::PostProcess);
	mCommandList->SetGraphicsRootShaderResourceView(3, mMatBuffer->GetGPUVirtualAddress());

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

/// <summary>
/// ����������
/// </summary>
/// <returns></returns>
void DistRenderPipeline::SetDescriptorHeap()
{
	ID3D12DescriptorHeap* descriptorHeaps[] = { mSrvDescriptorHeap.GetDescriptorHeap().Get() };
	mCommandList->SetDescriptorHeaps(_countof(descriptorHeaps), descriptorHeaps);
}

/// <summary>
/// ���ø�ǩ��
/// </summary>
void DistRenderPipeline::SetRootSignature()
{
	mCommandList->SetGraphicsRootSignature(mRootSignature.GetSignature());
}

/// <summary>
/// ���ò���Buffer
/// </summary>
/// <param name="type"></param>
void DistRenderPipeline::SetMatBuffer(MatBufferType type)
{
	switch (type)
	{
	case MatBufferType::PBR: mMatBuffer = mCurrFrameResource->PBRMaterialBuffer->Resource();
		break;
	case MatBufferType::SkyBox:	mMatBuffer = mCurrFrameResource->SkyBoxMaterialBuffer->Resource();
		break;
	case MatBufferType::PostProcess: mMatBuffer = mCurrFrameResource->PostMaterialBuffer->Resource();
		break;

	default: mMatBuffer = mCurrFrameResource->PBRMaterialBuffer->Resource();
		break;
	}
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