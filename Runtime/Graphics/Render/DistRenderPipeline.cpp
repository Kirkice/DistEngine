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
/// 渲染  
/// </summary>
void DistRenderPipeline::Render() 
{
	//	设置描述堆
	SetDescriptorHeap();

	//	设置根签名
	SetRootSignature();

	//	绘制阴影Pass
	RenderShadowPass(); 

	//	设置View和Rect
	SetViewAndRect();

	//	资源屏障 - 展示到目标
	SetPresentToTarget(CurrentBackBuffer());

	//	设置为渲染目标
	SetRenderTarget();

	//	设置CBuffer
	SetCBuffer();

	//	设置环境贴图到跟描述符表
	SetCubeMapRootDescriptorTable();

	//SetGBufferTarget();

	//RenderGBuffer();

	//ClearGBufferTarget();

	//	渲染不透明物体
	RenderOpaquePass();

	//	渲染坐标轴
	RenderAxisPass();

	//	渲染天空球
	RenderSkyBoxPass();

	//	渲染图标
	RenderGizmosPass();

	//	资源屏障 - 设置目标到展示
	SetTargetToPresnet(CurrentBackBuffer());

	//	拷贝Pass
	CopyColorPass();

	//	渲染后处理
	RenderPostProcessPass();
}

//	渲染阴影
void DistRenderPipeline::RenderShadowPass()
{
	SetMatBuffer(MatBufferType::PBR);  
	DrawSceneToShadowMap();
}

//	提前深度
void DistRenderPipeline::DepthPrePass()
{

}

////	渲染GBuffer
//void DistRenderPipeline::RenderGBuffer()
//{
//	float clearValue[] = {0.0f, 0.0f, 1.0f, 0.0f};
//
//	for (size_t i = 0; i < 4; i++)
//	{
//		_forwardPassCmdList->GetInternal()->ClearRenderTargetView(mGBufferPass->GET_GBUFFER_CPU_RTV(i), clearValue, 0, nullptr);
//	}
//
//	_forwardPassCmdList->GetInternal()->ClearDepthStencilView(DepthStencilView(), D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.0f, 0, 0, nullptr);
//
//
//	D3D12_CPU_DESCRIPTOR_HANDLE rtCPUDescriptors[4] = {};
//	UINT descriptorsCount = 0;
//	for (UINT i = 0; i < 4; ++i)
//	{
//		descriptorsCount++;
//		rtCPUDescriptors[i] = mGBufferPass->GET_GBUFFER_CPU_SRV(i);
//	}
//
//	_forwardPassCmdList->GetInternal()->OMSetRenderTargets(descriptorsCount, rtCPUDescriptors, true, nullptr);
//
//	SetMatBuffer(MatBufferType::PBR);
//	DrawRenderItemFormLayer("GBuffer", (int)RenderLayer::Opaque);
//}

//	渲染不透明物体
void DistRenderPipeline::RenderOpaquePass()
{
	SetMatBuffer(MatBufferType::PBR);
	DrawRenderItemFormLayer("litOpaque", (int)RenderLayer::Opaque);
}

//	延迟渲染
void DistRenderPipeline::DeferredRenderPass()
{

}

//	渲染天空球
void DistRenderPipeline::RenderSkyBoxPass()
{
	if (mSceneManager.getInstance().mCameraSetting.renderSkyBox)
	{
		SetMatBuffer(MatBufferType::SkyBox);
		DrawRenderItemFormLayer("sky", (int)RenderLayer::Sky);
	}
}

//	渲染透明物体
void DistRenderPipeline::RenderTransparentPass()
{

}

//	渲染坐标轴
void DistRenderPipeline::RenderAxisPass()
{
	DrawRenderItemFormLayer("Axis", (int)RenderLayer::Axis);
}

//	渲染图标
void DistRenderPipeline::RenderGizmosPass()
{
	if (ShowGizmos || ShowWire)
		SetMatBuffer(MatBufferType::PBR);

	if (ShowGizmos)
		DrawRenderItemFormLayer("Gizmo", (int)RenderLayer::Gizmo);

	if (ShowWire)
		DrawRenderItemFormLayer("Line", (int)RenderLayer::Line);
}

//	渲染后处理
void DistRenderPipeline::RenderPostProcessPass()
{
	//_forwardPassCmdList->GetInternal()->SetGraphicsRootDescriptorTable(4, mRenderTexture->GpuSrv());
	CD3DX12_GPU_DESCRIPTOR_HANDLE render_target_descriptor(mSrvDescriptorHeap.GetDescriptorHeap()->GetGPUDescriptorHandleForHeapStart());
	render_target_descriptor.Offset(UINT(25), mCbvSrvUavDescriptorSize);
	_forwardPassCmdList->GetInternal()->SetGraphicsRootDescriptorTable(4, render_target_descriptor);

	SetMatBuffer(MatBufferType::PostProcess);

	if (mPostProcessSwitch.ShowVolumeFog)
		DrawVolumeFog();

	if (mPostProcessSwitch.ShowFxAA)
		DrawFxAA();
}

//	渲染体积雾
void DistRenderPipeline::DrawVolumeFog()
{
	DrawRenderItemFormLayer("VolumeFog", (int)RenderLayer::PostProcess);

	SetTargetToPresnet(CurrentBackBuffer());

	SetReadToDest(mRenderTexture->Resource());

	SetPresentToSource(CurrentBackBuffer());

	CopyBlit(mRenderTexture->Resource(), CurrentBackBuffer());

	SetSourceToPresent(CurrentBackBuffer());

	SetDestToRead(mRenderTexture->Resource());
}

//	渲染快速近似抗锯齿
void DistRenderPipeline::DrawFxAA()
{
	DrawRenderItemFormLayer("FxAA", (int)RenderLayer::PostProcess);

	SetTargetToPresnet(CurrentBackBuffer());

	SetReadToDest(mRenderTexture->Resource());

	SetPresentToSource(CurrentBackBuffer());

	CopyBlit(mRenderTexture->Resource(), CurrentBackBuffer());

	SetSourceToPresent(CurrentBackBuffer());

	SetDestToRead(mRenderTexture->Resource());
}


/// <summary>
//	RenderUtils
/// </summary>

//	设置描述符堆
void DistRenderPipeline::SetDescriptorHeap()
{
	ID3D12DescriptorHeap* descriptorHeaps[] = { mSrvDescriptorHeap.GetDescriptorHeap().Get() };
	_forwardPassCmdList->GetInternal()->SetDescriptorHeaps(_countof(descriptorHeaps), descriptorHeaps);
}

//	设置根签名
void DistRenderPipeline::SetRootSignature()
{
	_forwardPassCmdList->GetInternal()->SetGraphicsRootSignature(mRootSignature.GetSignature());
}


//	设置材质Buffer
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
	_forwardPassCmdList->GetInternal()->SetGraphicsRootShaderResourceView(3, mMatBuffer->GetGPUVirtualAddress());
}

//	设置ViewheRect
void DistRenderPipeline::SetViewAndRect()
{
	_forwardPassCmdList->GetInternal()->RSSetViewports(1, &mScreenViewport);
	_forwardPassCmdList->GetInternal()->RSSetScissorRects(1, &mScissorRect);
}

//	资源屏障 - 展示到目标
void DistRenderPipeline::SetPresentToTarget(ID3D12Resource* Resource)
{
	_forwardPassCmdList->GetInternal()->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(Resource, D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET));
}

//	设置为渲染目标
void DistRenderPipeline::SetRenderTarget()
{
	_forwardPassCmdList->GetInternal()->ClearRenderTargetView(CurrentBackBufferView(), mSceneManager.getInstance().mCameraSetting.SolidColor, 0, nullptr);
	_forwardPassCmdList->GetInternal()->ClearDepthStencilView(DepthStencilView(), D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.0f, 0, 0, nullptr);

	_forwardPassCmdList->GetInternal()->OMSetRenderTargets(1, &CurrentBackBufferView(), true, &DepthStencilView());
}

////	设置GBuffer的渲染目标
//void DistRenderPipeline::SetGBufferTarget()
//{
//	std::array<D3D12_RESOURCE_BARRIER, 4> barriers = {
//		CD3DX12_RESOURCE_BARRIER::Transition(mGBufferPass->GetGBuffer(0), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE),
//		CD3DX12_RESOURCE_BARRIER::Transition(mGBufferPass->GetGBuffer(1), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE),
//		CD3DX12_RESOURCE_BARRIER::Transition(mGBufferPass->GetGBuffer(2), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE),
//		CD3DX12_RESOURCE_BARRIER::Transition(mGBufferPass->GetGBuffer(3), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE)
//	};
//
//	_forwardPassCmdList->GetInternal()->ResourceBarrier((UINT)barriers.size(), barriers.data());
//}
//
////	清除GBuffer的渲染目标
//void DistRenderPipeline::ClearGBufferTarget()
//{
//	std::array<D3D12_RESOURCE_BARRIER, 4> barriers = {
//	CD3DX12_RESOURCE_BARRIER::Transition(mGBufferPass->GetGBuffer(0), D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, D3D12_RESOURCE_STATE_RENDER_TARGET),
//	CD3DX12_RESOURCE_BARRIER::Transition(mGBufferPass->GetGBuffer(1), D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, D3D12_RESOURCE_STATE_RENDER_TARGET),
//	CD3DX12_RESOURCE_BARRIER::Transition(mGBufferPass->GetGBuffer(2), D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, D3D12_RESOURCE_STATE_RENDER_TARGET),
//	CD3DX12_RESOURCE_BARRIER::Transition(mGBufferPass->GetGBuffer(3), D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, D3D12_RESOURCE_STATE_RENDER_TARGET)
//	};
//
//	_forwardPassCmdList->GetInternal()->ResourceBarrier((UINT)barriers.size(), barriers.data());
//}

//	拷贝Pass
void DistRenderPipeline::CopyColorPass()
{
	_forwardPassCmdList->GetInternal()->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(mRenderTexture->Resource(), D3D12_RESOURCE_STATE_GENERIC_READ, D3D12_RESOURCE_STATE_COPY_DEST));

	_forwardPassCmdList->GetInternal()->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(CurrentBackBuffer(), D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_COPY_SOURCE));

	_forwardPassCmdList->GetInternal()->CopyResource(mRenderTexture->Resource(), CurrentBackBuffer());

	_forwardPassCmdList->GetInternal()->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(CurrentBackBuffer(), D3D12_RESOURCE_STATE_COPY_SOURCE, D3D12_RESOURCE_STATE_PRESENT));

	_forwardPassCmdList->GetInternal()->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(mRenderTexture->Resource(),
		D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_GENERIC_READ));
}

//	设置CBuffer
void DistRenderPipeline::SetCBuffer()
{
	auto passCB = mCurrFrameResource->PassCB->Resource();
	_forwardPassCmdList->GetInternal()->SetGraphicsRootConstantBufferView(2, passCB->GetGPUVirtualAddress());
}

//	设置环境贴图到跟描述符表
void DistRenderPipeline::SetCubeMapRootDescriptorTable()
{
	_forwardPassCmdList->GetInternal()->SetGraphicsRootDescriptorTable(4, mCubeMapTextures["Sky_specularIBL"]->GpuHandle);
	_forwardPassCmdList->GetInternal()->SetGraphicsRootDescriptorTable(4, mCubeMapTextures["Sky_diffuseIBL"]->GpuHandle);
}

//	资源屏障 - 设置目标到展示
void DistRenderPipeline::SetTargetToPresnet(ID3D12Resource* Resource)
{
	_forwardPassCmdList->GetInternal()->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(Resource, D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT));
}

//	资源屏障 - 设置读取到目标
void DistRenderPipeline::SetReadToDest(ID3D12Resource* Resource)
{
	_forwardPassCmdList->GetInternal()->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(Resource, D3D12_RESOURCE_STATE_GENERIC_READ, D3D12_RESOURCE_STATE_COPY_DEST));
}

//	绘制渲染项目 Layer
void DistRenderPipeline::DrawRenderItemFormLayer(std::string PSO, int Layer)
{
	_forwardPassCmdList->GetInternal()->SetPipelineState(mPSOs[PSO].Get());
	DrawRenderItems(_forwardPassCmdList->GetInternal().Get(), mRitemLayer[Layer]);
}

//	资源屏障 - 设置展示到源
void DistRenderPipeline::SetPresentToSource(ID3D12Resource* Resource)
{
	_forwardPassCmdList->GetInternal()->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(Resource, D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_COPY_SOURCE));
}

//	资源屏障 - 设置源到展示
void DistRenderPipeline::SetSourceToPresent(ID3D12Resource* Resource)
{
	_forwardPassCmdList->GetInternal()->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(Resource, D3D12_RESOURCE_STATE_COPY_SOURCE, D3D12_RESOURCE_STATE_PRESENT));
}

//	资源屏障 - 设置目标到读取
void DistRenderPipeline::SetDestToRead(ID3D12Resource* Resource)
{
	_forwardPassCmdList->GetInternal()->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(Resource,
		D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_GENERIC_READ));
}

//	拷贝复制
void DistRenderPipeline::CopyBlit(ID3D12Resource* dest, ID3D12Resource* source)
{
	_forwardPassCmdList->GetInternal()->CopyResource(dest, source);
}