#include "UploadBuffer.h"
#include "../Geometry/GeometryGenerator.h"
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

/// <summary>
/// ��Ⱦ
/// </summary>
void DistRenderPipeline::Render() 
{
	//	����������
	SetDescriptorHeap();

	//	���ø�ǩ��
	SetRootSignature();

	//	������ӰPass
	RenderShadowPass();

	//	����View��Rect
	SetViewAndRect();

	//	��Դ���� - չʾ��Ŀ��
	//SetPresentToTarget(CurrentBackBuffer());

	//	����Ϊ��ȾĿ��
	//SetRenderTarget();

	//	����CBuffer
	//SetCBuffer();

	//	���û�����ͼ������������
	//SetCubeMapRootDescriptorTable();

	//SetGBufferTarget();

	//RenderGBuffer();

	//ClearGBufferTarget();

	////	��Ⱦ��͸������
	//RenderOpaquePass();

	////	��Ⱦ������
	//RenderAxisPass();

	////	��Ⱦ�����
	//RenderSkyBoxPass();

	////	��Ⱦͼ��
	//RenderGizmosPass();

	//	��Դ���� - ����Ŀ�굽չʾ
	//SetTargetToPresnet(CurrentBackBuffer());

	//	����Pass
	CopyColorPass();

	//	��Ⱦ����
	RenderPostProcessPass();
}

//	��Ⱦ��Ӱ
void DistRenderPipeline::RenderShadowPass()
{
	SetMatBuffer(MatBufferType::PBR);
	DrawSceneToShadowMap();
}

//	��ǰ���
void DistRenderPipeline::DepthPrePass()
{

}

//	��ȾGBuffer
void DistRenderPipeline::RenderGBuffer()
{
	float clearValue[] = {0.0f, 0.0f, 1.0f, 0.0f};

	for (size_t i = 0; i < 4; i++)
	{
		mCommandList->ClearRenderTargetView(mGBufferPass->GET_GBUFFER_CPU_RTV(i), clearValue, 0, nullptr);
	}

	mCommandList->ClearDepthStencilView(DepthStencilView(), D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.0f, 0, 0, nullptr);


	D3D12_CPU_DESCRIPTOR_HANDLE rtCPUDescriptors[4] = {};
	UINT descriptorsCount = 0;
	for (UINT i = 0; i < 4; ++i)
	{
		descriptorsCount++;
		rtCPUDescriptors[i] = mGBufferPass->GET_GBUFFER_CPU_SRV(i);
	}

	mCommandList->OMSetRenderTargets(descriptorsCount, rtCPUDescriptors, true, nullptr);

	SetMatBuffer(MatBufferType::PBR);
	DrawRenderItemFormLayer("GBuffer", (int)RenderLayer::Opaque);
}

//	��Ⱦ��͸������
void DistRenderPipeline::RenderOpaquePass()
{
	SetMatBuffer(MatBufferType::PBR);
	DrawRenderItemFormLayer("litOpaque", (int)RenderLayer::Opaque);
}

//	�ӳ���Ⱦ
void DistRenderPipeline::DeferredRenderPass()
{

}

//	��Ⱦ�����
void DistRenderPipeline::RenderSkyBoxPass()
{
	if (mSceneManager.getInstance().mCameraSetting.renderSkyBox)
	{
		SetMatBuffer(MatBufferType::SkyBox);
		DrawRenderItemFormLayer("sky", (int)RenderLayer::Sky);
	}
}

//	��Ⱦ͸������
void DistRenderPipeline::RenderTransparentPass()
{

}

//	��Ⱦ������
void DistRenderPipeline::RenderAxisPass()
{
	DrawRenderItemFormLayer("Axis", (int)RenderLayer::Axis);
}

//	��Ⱦͼ��
void DistRenderPipeline::RenderGizmosPass()
{
	if (ShowGizmos || ShowWire)
		SetMatBuffer(MatBufferType::PBR);

	if (ShowGizmos)
		DrawRenderItemFormLayer("Gizmo", (int)RenderLayer::Gizmo);

	if (ShowWire)
		DrawRenderItemFormLayer("Line", (int)RenderLayer::Line);
}

//	��Ⱦ����
void DistRenderPipeline::RenderPostProcessPass()
{
	//mCommandList->SetGraphicsRootDescriptorTable(4, mRenderTarget->GpuSrv());
	CD3DX12_GPU_DESCRIPTOR_HANDLE render_target_descriptor(mSrvDescriptorHeap.GetDescriptorHeap()->GetGPUDescriptorHandleForHeapStart());
	render_target_descriptor.Offset(UINT(25), mCbvSrvUavDescriptorSize);
	mCommandList->SetGraphicsRootDescriptorTable(4, render_target_descriptor);

	SetMatBuffer(MatBufferType::PostProcess);

	if (mPostProcessSwitch.ShowVolumeFog)
		DrawVolumeFog();

	if (mPostProcessSwitch.ShowFxAA)
		DrawFxAA();
}

//	��Ⱦ�����
void DistRenderPipeline::DrawVolumeFog()
{
	DrawRenderItemFormLayer("VolumeFog", (int)RenderLayer::PostProcess);

	SetTargetToPresnet(CurrentBackBuffer());

	SetReadToDest(mRenderTarget->Resource());

	SetPresentToSource(CurrentBackBuffer());

	CopyBlit(mRenderTarget->Resource(), CurrentBackBuffer());

	SetSourceToPresent(CurrentBackBuffer());

	SetDestToRead(mRenderTarget->Resource());
}

//	��Ⱦ���ٽ��ƿ����
void DistRenderPipeline::DrawFxAA()
{
	DrawRenderItemFormLayer("FxAA", (int)RenderLayer::PostProcess);

	SetTargetToPresnet(CurrentBackBuffer());

	SetReadToDest(mRenderTarget->Resource());

	SetPresentToSource(CurrentBackBuffer());

	CopyBlit(mRenderTarget->Resource(), CurrentBackBuffer());

	SetSourceToPresent(CurrentBackBuffer());

	SetDestToRead(mRenderTarget->Resource());
}


/// <summary>
//	RenderUtils
/// </summary>

//	������������
void DistRenderPipeline::SetDescriptorHeap()
{
	ID3D12DescriptorHeap* descriptorHeaps[] = { mSrvDescriptorHeap.GetDescriptorHeap().Get() };
	mCommandList->SetDescriptorHeaps(_countof(descriptorHeaps), descriptorHeaps);
}

//	���ø�ǩ��
void DistRenderPipeline::SetRootSignature()
{
	mCommandList->SetGraphicsRootSignature(mRootSignature.GetSignature());
}


//	���ò���Buffer
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
	mCommandList->SetGraphicsRootShaderResourceView(3, mMatBuffer->GetGPUVirtualAddress());
}

//	����ViewheRect
void DistRenderPipeline::SetViewAndRect()
{
	mCommandList->RSSetViewports(1, &mScreenViewport);
	mCommandList->RSSetScissorRects(1, &mScissorRect);
}

//	��Դ���� - չʾ��Ŀ��
void DistRenderPipeline::SetPresentToTarget(ID3D12Resource* Resource)
{
	mCommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(Resource, D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET));
}

//	����Ϊ��ȾĿ��
void DistRenderPipeline::SetRenderTarget()
{
	mCommandList->ClearRenderTargetView(CurrentBackBufferView(), mSceneManager.getInstance().mCameraSetting.SolidColor, 0, nullptr);
	mCommandList->ClearDepthStencilView(DepthStencilView(), D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.0f, 0, 0, nullptr);

	mCommandList->OMSetRenderTargets(1, &CurrentBackBufferView(), true, &DepthStencilView());
}

//	����GBuffer����ȾĿ��
void DistRenderPipeline::SetGBufferTarget()
{
	std::array<D3D12_RESOURCE_BARRIER, 4> barriers = {
		CD3DX12_RESOURCE_BARRIER::Transition(mGBufferPass->GetGBuffer(0), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE),
		CD3DX12_RESOURCE_BARRIER::Transition(mGBufferPass->GetGBuffer(1), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE),
		CD3DX12_RESOURCE_BARRIER::Transition(mGBufferPass->GetGBuffer(2), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE),
		CD3DX12_RESOURCE_BARRIER::Transition(mGBufferPass->GetGBuffer(3), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE)
	};

	mCommandList->ResourceBarrier((UINT)barriers.size(), barriers.data());
}

//	���GBuffer����ȾĿ��
void DistRenderPipeline::ClearGBufferTarget()
{
	std::array<D3D12_RESOURCE_BARRIER, 4> barriers = {
	CD3DX12_RESOURCE_BARRIER::Transition(mGBufferPass->GetGBuffer(0), D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, D3D12_RESOURCE_STATE_RENDER_TARGET),
	CD3DX12_RESOURCE_BARRIER::Transition(mGBufferPass->GetGBuffer(1), D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, D3D12_RESOURCE_STATE_RENDER_TARGET),
	CD3DX12_RESOURCE_BARRIER::Transition(mGBufferPass->GetGBuffer(2), D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, D3D12_RESOURCE_STATE_RENDER_TARGET),
	CD3DX12_RESOURCE_BARRIER::Transition(mGBufferPass->GetGBuffer(3), D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, D3D12_RESOURCE_STATE_RENDER_TARGET)
	};

	mCommandList->ResourceBarrier((UINT)barriers.size(), barriers.data());
}

//	����Pass
void DistRenderPipeline::CopyColorPass()
{
	mCommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(mRenderTarget->Resource(), D3D12_RESOURCE_STATE_GENERIC_READ, D3D12_RESOURCE_STATE_COPY_DEST));

	mCommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(CurrentBackBuffer(), D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_COPY_SOURCE));

	mCommandList->CopyResource(mRenderTarget->Resource(), CurrentBackBuffer());

	mCommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(CurrentBackBuffer(), D3D12_RESOURCE_STATE_COPY_SOURCE, D3D12_RESOURCE_STATE_PRESENT));

	mCommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(mRenderTarget->Resource(),
		D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_GENERIC_READ));
}

//	����CBuffer
void DistRenderPipeline::SetCBuffer()
{
	auto passCB = mCurrFrameResource->PassCB->Resource();
	mCommandList->SetGraphicsRootConstantBufferView(2, passCB->GetGPUVirtualAddress());
}

//	���û�����ͼ������������
void DistRenderPipeline::SetCubeMapRootDescriptorTable()
{
	mCommandList->SetGraphicsRootDescriptorTable(4, mCubeMapTextures["Sky_specularIBL"]->GpuHandle);
	mCommandList->SetGraphicsRootDescriptorTable(4, mCubeMapTextures["Sky_diffuseIBL"]->GpuHandle);
}

//	��Դ���� - ����Ŀ�굽չʾ
void DistRenderPipeline::SetTargetToPresnet(ID3D12Resource* Resource)
{
	mCommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(Resource, D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT));
}

//	��Դ���� - ���ö�ȡ��Ŀ��
void DistRenderPipeline::SetReadToDest(ID3D12Resource* Resource)
{
	mCommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(Resource, D3D12_RESOURCE_STATE_GENERIC_READ, D3D12_RESOURCE_STATE_COPY_DEST));
}

//	������Ⱦ��Ŀ Layer
void DistRenderPipeline::DrawRenderItemFormLayer(std::string PSO, int Layer)
{
	mCommandList->SetPipelineState(mPSOs[PSO].Get());
	DrawRenderItems(mCommandList.Get(), mRitemLayer[Layer]);
}

//	��Դ���� - ����չʾ��Դ
void DistRenderPipeline::SetPresentToSource(ID3D12Resource* Resource)
{
	mCommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(Resource, D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_COPY_SOURCE));
}

//	��Դ���� - ����Դ��չʾ
void DistRenderPipeline::SetSourceToPresent(ID3D12Resource* Resource)
{
	mCommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(Resource, D3D12_RESOURCE_STATE_COPY_SOURCE, D3D12_RESOURCE_STATE_PRESENT));
}

//	��Դ���� - ����Ŀ�굽��ȡ
void DistRenderPipeline::SetDestToRead(ID3D12Resource* Resource)
{
	mCommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(Resource,
		D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_GENERIC_READ));
}

//	��������
void DistRenderPipeline::CopyBlit(ID3D12Resource* dest, ID3D12Resource* source)
{
	mCommandList->CopyResource(dest, source);
}