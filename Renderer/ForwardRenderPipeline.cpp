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
	ForwardRender();
}

 
void ForwardRenderer::ForwardRender()
{
	//Init    PSO / RootSignature / Material  /  descriptorHeaps
	auto cmdListAlloc = mCurrFrameResource->CmdListAlloc;

	ThrowIfFailed(cmdListAlloc->Reset());
	ThrowIfFailed(mCommandList->Reset(cmdListAlloc.Get(), mPSOs["opaque"].Get()));

	ID3D12DescriptorHeap* descriptorHeaps[] = { mSrvDescriptorHeap.Get() };
	mCommandList->SetDescriptorHeaps(_countof(descriptorHeaps), descriptorHeaps);
	mCommandList->SetGraphicsRootSignature(mRootSignature.Get());
	auto matBuffer = mCurrFrameResource->PBRMaterialBuffer->Resource();

	//Draw ShadowMap
	DrawShadowMap(matBuffer);

	//SSAO
	DrawSSAO(matBuffer);

	//Draw Color To Target
	DrawColorToTarget(matBuffer);
	
	//CopyColor
	CopyColorPass();

	//RGBSplit
	DrawRGBSplit(matBuffer);

	//Radial Blur
	DrawRadialBlur(matBuffer);

	//Vignette
	DrawVignette(matBuffer);

	//Decolor
	DrawDecolor(matBuffer);

	//Brightness
	DrawBrightness(matBuffer);

	//HSV
	DrawHSV(matBuffer);

	//Mosaic
	DrawMosaic(matBuffer);

	//Sharpen
	DrawSharpen(matBuffer);

	//Spherize
	DrawSpherize(matBuffer);

	//WhiteBalance
	DrawWhiteBalance(matBuffer);

	//OilPaint
	DrawOilPaint(matBuffer);

	//Relief
	DrawRelief(matBuffer);

	//EdgeDetection
	DrawEdgeDetection(matBuffer);

	//Final Blit
	DrawFinalBlit();

	ThrowIfFailed(mCommandList->Close());
	ID3D12CommandList* cmdsLists[] = { mCommandList.Get()};
	mCommandQueue->ExecuteCommandLists(_countof(cmdsLists), cmdsLists);

	ThrowIfFailed(mSwapChain->Present(0, 0));
	mCurrBackBuffer = (mCurrBackBuffer + 1) % SwapChainBufferCount;

	mCurrFrameResource->Fence = ++mCurrentFence;

	mCommandQueue->Signal(mFence.Get(), mCurrentFence);
}


// DrawColorToTarget
void ForwardRenderer::DrawColorToTarget(ID3D12Resource* matBuffer)
{
	mCommandList->SetGraphicsRootSignature(mRootSignature.Get());

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


	CD3DX12_GPU_DESCRIPTOR_HANDLE skyTexDescriptor(mSrvDescriptorHeap->GetGPUDescriptorHandleForHeapStart());
	skyTexDescriptor.Offset(mSkyTexHeapIndex, mCbvSrvUavDescriptorSize);
	mCommandList->SetGraphicsRootDescriptorTable(4, skyTexDescriptor);

	CD3DX12_GPU_DESCRIPTOR_HANDLE iblTexDescriptor(mSrvDescriptorHeap->GetGPUDescriptorHandleForHeapStart());
	iblTexDescriptor.Offset(mIBLTexHeapIndex, mCbvSrvUavDescriptorSize);
	mCommandList->SetGraphicsRootDescriptorTable(4, iblTexDescriptor);

	ForwardRenderer::DrawOpaque();

	//SkyBox
	auto skyMatBuufer = mCurrFrameResource->SkyBoxMaterialBuffer->Resource();
	mCommandList->SetGraphicsRootShaderResourceView(3, skyMatBuufer->GetGPUVirtualAddress());
	ForwardRenderer::DrawSkyBox();

	//Draw Transparent
	matBuffer = mCurrFrameResource->PBRMaterialBuffer->Resource();
	mCommandList->SetGraphicsRootShaderResourceView(3, matBuffer->GetGPUVirtualAddress());
	DrawTransparent();

	ForwardRenderer::DrawBounding();
	ForwardRenderer::DrawGizmo();

	mCommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(CurrentBackBuffer(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT));

}

//	CopyColorPass
void ForwardRenderer::CopyColorPass()
{
	mCommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(mRenderTarget->Resource(), D3D12_RESOURCE_STATE_GENERIC_READ, D3D12_RESOURCE_STATE_COPY_DEST));

	mCommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(CurrentBackBuffer(), D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_COPY_SOURCE));

	mCommandList->CopyResource(mRenderTarget->Resource(), CurrentBackBuffer());

	mCommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(CurrentBackBuffer(), D3D12_RESOURCE_STATE_COPY_SOURCE, D3D12_RESOURCE_STATE_PRESENT));

	mCommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(mRenderTarget->Resource(),
		D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_GENERIC_READ));
}

void ForwardRenderer::DrawFinalBlit()
{
	mCommandList->RSSetViewports(1, &mScreenViewport);
	mCommandList->RSSetScissorRects(1, &mScissorRect);

	mCommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(CurrentBackBuffer(), D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET));

	mCommandList->ClearRenderTargetView(CurrentBackBufferView(), SolidColor, 0, nullptr);
	mCommandList->ClearDepthStencilView(DepthStencilView(), D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.0f, 0, 0, nullptr);

	mCommandList->OMSetRenderTargets(1, &CurrentBackBufferView(), true, &DepthStencilView());

	CD3DX12_GPU_DESCRIPTOR_HANDLE render_target_descriptor(mSrvDescriptorHeap->GetGPUDescriptorHandleForHeapStart());
	render_target_descriptor.Offset(UINT(24), mCbvSrvUavDescriptorSize);
	mCommandList->SetGraphicsRootDescriptorTable(4, render_target_descriptor);

	mCommandList->SetPipelineState(mPSOs["FinalBlit"].Get());
	DrawRenderItems(mCommandList.Get(), mRitemLayer[(int)RenderLayer::PostProcessing]);

	ForwardRenderer::DrawImgui();

	mCommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(CurrentBackBuffer(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT));
}

//	DrawShadowMap
void ForwardRenderer::DrawShadowMap(ID3D12Resource* matBuffer)
{
	mCommandList->SetGraphicsRootShaderResourceView(3, matBuffer->GetGPUVirtualAddress());

	mCommandList->SetGraphicsRootDescriptorTable(4, mNullSrv);

	mCommandList->SetGraphicsRootDescriptorTable(5, mSrvDescriptorHeap->GetGPUDescriptorHandleForHeapStart());
	DrawSceneToShadowMap();
}

void ForwardRenderer::DrawSSAO(ID3D12Resource* matBuffer)
{
	ForwardRenderer::DrawDepthNormal();
	mCommandList->SetGraphicsRootSignature(mSsaoRootSignature.Get());
	mSsao->ComputeSsao(mCommandList.Get(), mCurrFrameResource, 2);
}

//	DrawDepthNormal
void ForwardRenderer::DrawDepthNormal()
{
	DrawNormalsAndDepth();
}

///	DrawOutLine
void ForwardRenderer::DrawOutLine()
{
	mCommandList->SetPipelineState(mPSOs["outline"].Get());
	DrawRenderItems(mCommandList.Get(), mRitemLayer[(int)RenderLayer::Opaque]);
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

//	Draw Transparent
void ForwardRenderer::DrawTransparent()
{
	mCommandList->SetPipelineState(mPSOs["transparent"].Get());
	DrawRenderItems(mCommandList.Get(), mRitemLayer[(int)RenderLayer::Transparent]);

	mCommandList->SetPipelineState(mPSOs["Unlit"].Get());
	DrawRenderItems(mCommandList.Get(), mRitemLayer[(int)RenderLayer::Unlit]);
}

//	Draw Bounding
void ForwardRenderer::DrawBounding()
{
	if (ShowBounding)
	{
		mCommandList->SetPipelineState(mPSOs["bounding"].Get());
		DrawRenderItems(mCommandList.Get(), mRitemLayer[(int)RenderLayer::Bounding]);
	}
}


//	Draw RGBSplit
void ForwardRenderer::DrawRGBSplit(ID3D12Resource* matBuffer)
{
	//// RGB Split
	if (UseRGBSplit)
	{
		mCommandList->RSSetViewports(1, &mScreenViewport);
		mCommandList->RSSetScissorRects(1, &mScissorRect);

		mCommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(CurrentBackBuffer(), D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET));

		mCommandList->ClearRenderTargetView(CurrentBackBufferView(), SolidColor, 0, nullptr);
		mCommandList->ClearDepthStencilView(DepthStencilView(), D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.0f, 0, 0, nullptr);

		mCommandList->OMSetRenderTargets(1, &CurrentBackBufferView(), true, &DepthStencilView());

		CD3DX12_GPU_DESCRIPTOR_HANDLE render_target_descriptor(mSrvDescriptorHeap->GetGPUDescriptorHandleForHeapStart());
		render_target_descriptor.Offset(UINT(24), mCbvSrvUavDescriptorSize);
		mCommandList->SetGraphicsRootDescriptorTable(4, render_target_descriptor);

		matBuffer = mCurrFrameResource->PostMaterialBuffer->Resource();
		mCommandList->SetGraphicsRootShaderResourceView(3, matBuffer->GetGPUVirtualAddress());

		mCommandList->SetPipelineState(mPSOs["RGBSplit"].Get());
		DrawRenderItems(mCommandList.Get(), mRitemLayer[(int)RenderLayer::PostProcessing]);

		mCommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(CurrentBackBuffer(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT));

		mCommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(mRenderTarget->Resource(), D3D12_RESOURCE_STATE_GENERIC_READ, D3D12_RESOURCE_STATE_COPY_DEST));

		mCommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(CurrentBackBuffer(), D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_COPY_SOURCE));

		mCommandList->CopyResource(mRenderTarget->Resource(), CurrentBackBuffer());

		mCommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(CurrentBackBuffer(), D3D12_RESOURCE_STATE_COPY_SOURCE, D3D12_RESOURCE_STATE_PRESENT));

		mCommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(mRenderTarget->Resource(),
			D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_GENERIC_READ));
	}
}

//	Draw Radial Blur
void ForwardRenderer::DrawRadialBlur(ID3D12Resource* matBuffer)
{
	//// Radial Blur
	if (UseRadialBlur)
	{
		mCommandList->RSSetViewports(1, &mScreenViewport);
		mCommandList->RSSetScissorRects(1, &mScissorRect);

		mCommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(CurrentBackBuffer(), D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET));

		mCommandList->ClearRenderTargetView(CurrentBackBufferView(), SolidColor, 0, nullptr);
		mCommandList->ClearDepthStencilView(DepthStencilView(), D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.0f, 0, 0, nullptr);

		mCommandList->OMSetRenderTargets(1, &CurrentBackBufferView(), true, &DepthStencilView());

		CD3DX12_GPU_DESCRIPTOR_HANDLE render_target_descriptor(mSrvDescriptorHeap->GetGPUDescriptorHandleForHeapStart());
		render_target_descriptor.Offset(UINT(24), mCbvSrvUavDescriptorSize);
		mCommandList->SetGraphicsRootDescriptorTable(4, render_target_descriptor);


		matBuffer = mCurrFrameResource->PostMaterialBuffer->Resource();
		mCommandList->SetGraphicsRootShaderResourceView(3, matBuffer->GetGPUVirtualAddress());

		mCommandList->SetPipelineState(mPSOs["RadialBlur"].Get());
		DrawRenderItems(mCommandList.Get(), mRitemLayer[(int)RenderLayer::PostProcessing]);

		mCommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(CurrentBackBuffer(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT));

		mCommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(mRenderTarget->Resource(), D3D12_RESOURCE_STATE_GENERIC_READ, D3D12_RESOURCE_STATE_COPY_DEST));

		mCommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(CurrentBackBuffer(), D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_COPY_SOURCE));

		mCommandList->CopyResource(mRenderTarget->Resource(), CurrentBackBuffer());

		mCommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(CurrentBackBuffer(), D3D12_RESOURCE_STATE_COPY_SOURCE, D3D12_RESOURCE_STATE_PRESENT));

		mCommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(mRenderTarget->Resource(),
			D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_GENERIC_READ));
	}
}


//	Draw Vignette
void ForwardRenderer::DrawVignette(ID3D12Resource* matBuffer)
{
	//// Vignette
	if (UseVignette)
	{
		mCommandList->RSSetViewports(1, &mScreenViewport);
		mCommandList->RSSetScissorRects(1, &mScissorRect);

		mCommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(CurrentBackBuffer(), D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET));

		mCommandList->ClearRenderTargetView(CurrentBackBufferView(), SolidColor, 0, nullptr);
		mCommandList->ClearDepthStencilView(DepthStencilView(), D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.0f, 0, 0, nullptr);

		mCommandList->OMSetRenderTargets(1, &CurrentBackBufferView(), true, &DepthStencilView());

		CD3DX12_GPU_DESCRIPTOR_HANDLE render_target_descriptor(mSrvDescriptorHeap->GetGPUDescriptorHandleForHeapStart());
		render_target_descriptor.Offset(UINT(24), mCbvSrvUavDescriptorSize);
		mCommandList->SetGraphicsRootDescriptorTable(4, render_target_descriptor);

		matBuffer = mCurrFrameResource->PostMaterialBuffer->Resource();
		mCommandList->SetGraphicsRootShaderResourceView(3, matBuffer->GetGPUVirtualAddress());

		mCommandList->SetPipelineState(mPSOs["Vignette"].Get());
		DrawRenderItems(mCommandList.Get(), mRitemLayer[(int)RenderLayer::PostProcessing]);

		mCommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(CurrentBackBuffer(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT));

		mCommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(mRenderTarget->Resource(), D3D12_RESOURCE_STATE_GENERIC_READ, D3D12_RESOURCE_STATE_COPY_DEST));

		mCommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(CurrentBackBuffer(), D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_COPY_SOURCE));

		mCommandList->CopyResource(mRenderTarget->Resource(), CurrentBackBuffer());

		mCommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(CurrentBackBuffer(), D3D12_RESOURCE_STATE_COPY_SOURCE, D3D12_RESOURCE_STATE_PRESENT));

		mCommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(mRenderTarget->Resource(),
			D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_GENERIC_READ));
	}
}



//	Draw Decolor
void ForwardRenderer::DrawDecolor(ID3D12Resource* matBuffer)
{
	//// Decolor
	if (UseDecolor)
	{
		mCommandList->RSSetViewports(1, &mScreenViewport);
		mCommandList->RSSetScissorRects(1, &mScissorRect);

		mCommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(CurrentBackBuffer(), D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET));

		mCommandList->ClearRenderTargetView(CurrentBackBufferView(), SolidColor, 0, nullptr);
		mCommandList->ClearDepthStencilView(DepthStencilView(), D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.0f, 0, 0, nullptr);

		mCommandList->OMSetRenderTargets(1, &CurrentBackBufferView(), true, &DepthStencilView());

		CD3DX12_GPU_DESCRIPTOR_HANDLE render_target_descriptor(mSrvDescriptorHeap->GetGPUDescriptorHandleForHeapStart());
		render_target_descriptor.Offset(UINT(24), mCbvSrvUavDescriptorSize);
		mCommandList->SetGraphicsRootDescriptorTable(4, render_target_descriptor);


		matBuffer = mCurrFrameResource->PostMaterialBuffer->Resource();
		mCommandList->SetGraphicsRootShaderResourceView(3, matBuffer->GetGPUVirtualAddress());

		mCommandList->SetPipelineState(mPSOs["Decolor"].Get());
		DrawRenderItems(mCommandList.Get(), mRitemLayer[(int)RenderLayer::PostProcessing]);

		mCommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(CurrentBackBuffer(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT));


		mCommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(mRenderTarget->Resource(), D3D12_RESOURCE_STATE_GENERIC_READ, D3D12_RESOURCE_STATE_COPY_DEST));

		mCommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(CurrentBackBuffer(), D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_COPY_SOURCE));

		mCommandList->CopyResource(mRenderTarget->Resource(), CurrentBackBuffer());

		mCommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(CurrentBackBuffer(), D3D12_RESOURCE_STATE_COPY_SOURCE, D3D12_RESOURCE_STATE_PRESENT));

		mCommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(mRenderTarget->Resource(),
			D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_GENERIC_READ));
	}
}


//	Draw Brightness
void ForwardRenderer::DrawBrightness(ID3D12Resource* matBuffer)
{
	if (UseBrightness)
	{
		mCommandList->RSSetViewports(1, &mScreenViewport);
		mCommandList->RSSetScissorRects(1, &mScissorRect);

		mCommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(CurrentBackBuffer(), D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET));

		mCommandList->ClearRenderTargetView(CurrentBackBufferView(), SolidColor, 0, nullptr);
		mCommandList->ClearDepthStencilView(DepthStencilView(), D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.0f, 0, 0, nullptr);

		mCommandList->OMSetRenderTargets(1, &CurrentBackBufferView(), true, &DepthStencilView());

		CD3DX12_GPU_DESCRIPTOR_HANDLE render_target_descriptor(mSrvDescriptorHeap->GetGPUDescriptorHandleForHeapStart());
		render_target_descriptor.Offset(UINT(24), mCbvSrvUavDescriptorSize);
		mCommandList->SetGraphicsRootDescriptorTable(4, render_target_descriptor);


		matBuffer = mCurrFrameResource->PostMaterialBuffer->Resource();
		mCommandList->SetGraphicsRootShaderResourceView(3, matBuffer->GetGPUVirtualAddress());

		mCommandList->SetPipelineState(mPSOs["Brightness"].Get());
		DrawRenderItems(mCommandList.Get(), mRitemLayer[(int)RenderLayer::PostProcessing]);

		mCommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(CurrentBackBuffer(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT));


		mCommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(mRenderTarget->Resource(), D3D12_RESOURCE_STATE_GENERIC_READ, D3D12_RESOURCE_STATE_COPY_DEST));

		mCommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(CurrentBackBuffer(), D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_COPY_SOURCE));

		mCommandList->CopyResource(mRenderTarget->Resource(), CurrentBackBuffer());

		mCommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(CurrentBackBuffer(), D3D12_RESOURCE_STATE_COPY_SOURCE, D3D12_RESOURCE_STATE_PRESENT));

		mCommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(mRenderTarget->Resource(),
			D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_GENERIC_READ));
	}
}


//	Draw HSV
void ForwardRenderer::DrawHSV(ID3D12Resource* matBuffer)
{
	if (UseHSV)
	{
		mCommandList->RSSetViewports(1, &mScreenViewport);
		mCommandList->RSSetScissorRects(1, &mScissorRect);

		mCommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(CurrentBackBuffer(), D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET));

		mCommandList->ClearRenderTargetView(CurrentBackBufferView(), SolidColor, 0, nullptr);
		mCommandList->ClearDepthStencilView(DepthStencilView(), D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.0f, 0, 0, nullptr);

		mCommandList->OMSetRenderTargets(1, &CurrentBackBufferView(), true, &DepthStencilView());

		CD3DX12_GPU_DESCRIPTOR_HANDLE render_target_descriptor(mSrvDescriptorHeap->GetGPUDescriptorHandleForHeapStart());
		render_target_descriptor.Offset(UINT(24), mCbvSrvUavDescriptorSize);
		mCommandList->SetGraphicsRootDescriptorTable(4, render_target_descriptor);


		matBuffer = mCurrFrameResource->PostMaterialBuffer->Resource();
		mCommandList->SetGraphicsRootShaderResourceView(3, matBuffer->GetGPUVirtualAddress());

		mCommandList->SetPipelineState(mPSOs["HSV"].Get());
		DrawRenderItems(mCommandList.Get(), mRitemLayer[(int)RenderLayer::PostProcessing]);

		mCommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(CurrentBackBuffer(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT));


		mCommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(mRenderTarget->Resource(), D3D12_RESOURCE_STATE_GENERIC_READ, D3D12_RESOURCE_STATE_COPY_DEST));

		mCommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(CurrentBackBuffer(), D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_COPY_SOURCE));

		mCommandList->CopyResource(mRenderTarget->Resource(), CurrentBackBuffer());

		mCommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(CurrentBackBuffer(), D3D12_RESOURCE_STATE_COPY_SOURCE, D3D12_RESOURCE_STATE_PRESENT));

		mCommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(mRenderTarget->Resource(),
			D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_GENERIC_READ));
	}
}

//	Draw Mosaic
void ForwardRenderer::DrawMosaic(ID3D12Resource* matBuffer)
{
	if (UseMosaic)
	{
		mCommandList->RSSetViewports(1, &mScreenViewport);
		mCommandList->RSSetScissorRects(1, &mScissorRect);

		mCommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(CurrentBackBuffer(), D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET));

		mCommandList->ClearRenderTargetView(CurrentBackBufferView(), SolidColor, 0, nullptr);
		mCommandList->ClearDepthStencilView(DepthStencilView(), D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.0f, 0, 0, nullptr);

		mCommandList->OMSetRenderTargets(1, &CurrentBackBufferView(), true, &DepthStencilView());

		CD3DX12_GPU_DESCRIPTOR_HANDLE render_target_descriptor(mSrvDescriptorHeap->GetGPUDescriptorHandleForHeapStart());
		render_target_descriptor.Offset(UINT(24), mCbvSrvUavDescriptorSize);
		mCommandList->SetGraphicsRootDescriptorTable(4, render_target_descriptor);


		matBuffer = mCurrFrameResource->PostMaterialBuffer->Resource();
		mCommandList->SetGraphicsRootShaderResourceView(3, matBuffer->GetGPUVirtualAddress());

		mCommandList->SetPipelineState(mPSOs["Mosaic"].Get());
		DrawRenderItems(mCommandList.Get(), mRitemLayer[(int)RenderLayer::PostProcessing]);

		mCommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(CurrentBackBuffer(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT));


		mCommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(mRenderTarget->Resource(), D3D12_RESOURCE_STATE_GENERIC_READ, D3D12_RESOURCE_STATE_COPY_DEST));

		mCommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(CurrentBackBuffer(), D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_COPY_SOURCE));

		mCommandList->CopyResource(mRenderTarget->Resource(), CurrentBackBuffer());

		mCommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(CurrentBackBuffer(), D3D12_RESOURCE_STATE_COPY_SOURCE, D3D12_RESOURCE_STATE_PRESENT));

		mCommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(mRenderTarget->Resource(),
			D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_GENERIC_READ));
	}
}

//	Draw Sharpen
void ForwardRenderer::DrawSharpen(ID3D12Resource* matBuffer)
{
	if (UseSharpen)
	{
		mCommandList->RSSetViewports(1, &mScreenViewport);
		mCommandList->RSSetScissorRects(1, &mScissorRect);

		mCommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(CurrentBackBuffer(), D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET));

		mCommandList->ClearRenderTargetView(CurrentBackBufferView(), SolidColor, 0, nullptr);
		mCommandList->ClearDepthStencilView(DepthStencilView(), D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.0f, 0, 0, nullptr);

		mCommandList->OMSetRenderTargets(1, &CurrentBackBufferView(), true, &DepthStencilView());

		CD3DX12_GPU_DESCRIPTOR_HANDLE render_target_descriptor(mSrvDescriptorHeap->GetGPUDescriptorHandleForHeapStart());
		render_target_descriptor.Offset(UINT(24), mCbvSrvUavDescriptorSize);
		mCommandList->SetGraphicsRootDescriptorTable(4, render_target_descriptor);


		matBuffer = mCurrFrameResource->PostMaterialBuffer->Resource();
		mCommandList->SetGraphicsRootShaderResourceView(3, matBuffer->GetGPUVirtualAddress());

		mCommandList->SetPipelineState(mPSOs["Sharpen"].Get());
		DrawRenderItems(mCommandList.Get(), mRitemLayer[(int)RenderLayer::PostProcessing]);

		mCommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(CurrentBackBuffer(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT));


		mCommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(mRenderTarget->Resource(), D3D12_RESOURCE_STATE_GENERIC_READ, D3D12_RESOURCE_STATE_COPY_DEST));

		mCommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(CurrentBackBuffer(), D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_COPY_SOURCE));

		mCommandList->CopyResource(mRenderTarget->Resource(), CurrentBackBuffer());

		mCommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(CurrentBackBuffer(), D3D12_RESOURCE_STATE_COPY_SOURCE, D3D12_RESOURCE_STATE_PRESENT));

		mCommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(mRenderTarget->Resource(),
			D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_GENERIC_READ));
	}
}

//	Draw Spherize
void ForwardRenderer::DrawSpherize(ID3D12Resource* matBuffer)
{
	if (UseSpherize)
	{
		mCommandList->RSSetViewports(1, &mScreenViewport);
		mCommandList->RSSetScissorRects(1, &mScissorRect);

		mCommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(CurrentBackBuffer(), D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET));

		mCommandList->ClearRenderTargetView(CurrentBackBufferView(), SolidColor, 0, nullptr);
		mCommandList->ClearDepthStencilView(DepthStencilView(), D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.0f, 0, 0, nullptr);

		mCommandList->OMSetRenderTargets(1, &CurrentBackBufferView(), true, &DepthStencilView());

		CD3DX12_GPU_DESCRIPTOR_HANDLE render_target_descriptor(mSrvDescriptorHeap->GetGPUDescriptorHandleForHeapStart());
		render_target_descriptor.Offset(UINT(24), mCbvSrvUavDescriptorSize);
		mCommandList->SetGraphicsRootDescriptorTable(4, render_target_descriptor);


		matBuffer = mCurrFrameResource->PostMaterialBuffer->Resource();
		mCommandList->SetGraphicsRootShaderResourceView(3, matBuffer->GetGPUVirtualAddress());

		mCommandList->SetPipelineState(mPSOs["Spherize"].Get());
		DrawRenderItems(mCommandList.Get(), mRitemLayer[(int)RenderLayer::PostProcessing]);

		mCommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(CurrentBackBuffer(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT));


		mCommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(mRenderTarget->Resource(), D3D12_RESOURCE_STATE_GENERIC_READ, D3D12_RESOURCE_STATE_COPY_DEST));

		mCommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(CurrentBackBuffer(), D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_COPY_SOURCE));

		mCommandList->CopyResource(mRenderTarget->Resource(), CurrentBackBuffer());

		mCommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(CurrentBackBuffer(), D3D12_RESOURCE_STATE_COPY_SOURCE, D3D12_RESOURCE_STATE_PRESENT));

		mCommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(mRenderTarget->Resource(),
			D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_GENERIC_READ));
	}
}

//	Draw WhiteBalance
void ForwardRenderer::DrawWhiteBalance(ID3D12Resource* matBuffer)
{
	if (UseWhiteBalance)
	{
		mCommandList->RSSetViewports(1, &mScreenViewport);
		mCommandList->RSSetScissorRects(1, &mScissorRect);

		mCommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(CurrentBackBuffer(), D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET));

		mCommandList->ClearRenderTargetView(CurrentBackBufferView(), SolidColor, 0, nullptr);
		mCommandList->ClearDepthStencilView(DepthStencilView(), D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.0f, 0, 0, nullptr);

		mCommandList->OMSetRenderTargets(1, &CurrentBackBufferView(), true, &DepthStencilView());

		CD3DX12_GPU_DESCRIPTOR_HANDLE render_target_descriptor(mSrvDescriptorHeap->GetGPUDescriptorHandleForHeapStart());
		render_target_descriptor.Offset(UINT(24), mCbvSrvUavDescriptorSize);
		mCommandList->SetGraphicsRootDescriptorTable(4, render_target_descriptor);


		matBuffer = mCurrFrameResource->PostMaterialBuffer->Resource();
		mCommandList->SetGraphicsRootShaderResourceView(3, matBuffer->GetGPUVirtualAddress());

		mCommandList->SetPipelineState(mPSOs["WhiteBalance"].Get());
		DrawRenderItems(mCommandList.Get(), mRitemLayer[(int)RenderLayer::PostProcessing]);

		mCommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(CurrentBackBuffer(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT));


		mCommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(mRenderTarget->Resource(), D3D12_RESOURCE_STATE_GENERIC_READ, D3D12_RESOURCE_STATE_COPY_DEST));

		mCommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(CurrentBackBuffer(), D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_COPY_SOURCE));

		mCommandList->CopyResource(mRenderTarget->Resource(), CurrentBackBuffer());

		mCommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(CurrentBackBuffer(), D3D12_RESOURCE_STATE_COPY_SOURCE, D3D12_RESOURCE_STATE_PRESENT));

		mCommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(mRenderTarget->Resource(),
			D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_GENERIC_READ));
	}
}


//	Draw OilPaint
void ForwardRenderer::DrawOilPaint(ID3D12Resource* matBuffer)
{
	if (UseOilPaint)
	{
		mCommandList->RSSetViewports(1, &mScreenViewport);
		mCommandList->RSSetScissorRects(1, &mScissorRect);

		mCommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(CurrentBackBuffer(), D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET));

		mCommandList->ClearRenderTargetView(CurrentBackBufferView(), SolidColor, 0, nullptr);
		mCommandList->ClearDepthStencilView(DepthStencilView(), D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.0f, 0, 0, nullptr);

		mCommandList->OMSetRenderTargets(1, &CurrentBackBufferView(), true, &DepthStencilView());

		CD3DX12_GPU_DESCRIPTOR_HANDLE render_target_descriptor(mSrvDescriptorHeap->GetGPUDescriptorHandleForHeapStart());
		render_target_descriptor.Offset(UINT(24), mCbvSrvUavDescriptorSize);
		mCommandList->SetGraphicsRootDescriptorTable(4, render_target_descriptor);


		matBuffer = mCurrFrameResource->PostMaterialBuffer->Resource();
		mCommandList->SetGraphicsRootShaderResourceView(3, matBuffer->GetGPUVirtualAddress());

		mCommandList->SetPipelineState(mPSOs["OilPaint"].Get());
		DrawRenderItems(mCommandList.Get(), mRitemLayer[(int)RenderLayer::PostProcessing]);

		mCommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(CurrentBackBuffer(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT));


		mCommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(mRenderTarget->Resource(), D3D12_RESOURCE_STATE_GENERIC_READ, D3D12_RESOURCE_STATE_COPY_DEST));

		mCommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(CurrentBackBuffer(), D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_COPY_SOURCE));

		mCommandList->CopyResource(mRenderTarget->Resource(), CurrentBackBuffer());

		mCommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(CurrentBackBuffer(), D3D12_RESOURCE_STATE_COPY_SOURCE, D3D12_RESOURCE_STATE_PRESENT));

		mCommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(mRenderTarget->Resource(),
			D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_GENERIC_READ));
	}
}


//	Draw Relief
void ForwardRenderer::DrawRelief(ID3D12Resource* matBuffer)
{
	if (UseRelief)
	{
		mCommandList->RSSetViewports(1, &mScreenViewport);
		mCommandList->RSSetScissorRects(1, &mScissorRect);

		mCommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(CurrentBackBuffer(), D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET));

		mCommandList->ClearRenderTargetView(CurrentBackBufferView(), SolidColor, 0, nullptr);
		mCommandList->ClearDepthStencilView(DepthStencilView(), D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.0f, 0, 0, nullptr);

		mCommandList->OMSetRenderTargets(1, &CurrentBackBufferView(), true, &DepthStencilView());

		CD3DX12_GPU_DESCRIPTOR_HANDLE render_target_descriptor(mSrvDescriptorHeap->GetGPUDescriptorHandleForHeapStart());
		render_target_descriptor.Offset(UINT(24), mCbvSrvUavDescriptorSize);
		mCommandList->SetGraphicsRootDescriptorTable(4, render_target_descriptor);


		matBuffer = mCurrFrameResource->PostMaterialBuffer->Resource();
		mCommandList->SetGraphicsRootShaderResourceView(3, matBuffer->GetGPUVirtualAddress());

		mCommandList->SetPipelineState(mPSOs["Relief"].Get());
		DrawRenderItems(mCommandList.Get(), mRitemLayer[(int)RenderLayer::PostProcessing]);

		mCommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(CurrentBackBuffer(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT));


		mCommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(mRenderTarget->Resource(), D3D12_RESOURCE_STATE_GENERIC_READ, D3D12_RESOURCE_STATE_COPY_DEST));

		mCommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(CurrentBackBuffer(), D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_COPY_SOURCE));

		mCommandList->CopyResource(mRenderTarget->Resource(), CurrentBackBuffer());

		mCommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(CurrentBackBuffer(), D3D12_RESOURCE_STATE_COPY_SOURCE, D3D12_RESOURCE_STATE_PRESENT));

		mCommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(mRenderTarget->Resource(),
			D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_GENERIC_READ));
	}
}

//	Draw EdgeDetection
void ForwardRenderer::DrawEdgeDetection(ID3D12Resource* matBuffer)
{
	if (UseEdgeDetection)
	{
		mCommandList->RSSetViewports(1, &mScreenViewport);
		mCommandList->RSSetScissorRects(1, &mScissorRect);

		mCommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(CurrentBackBuffer(), D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET));

		mCommandList->ClearRenderTargetView(CurrentBackBufferView(), SolidColor, 0, nullptr);
		mCommandList->ClearDepthStencilView(DepthStencilView(), D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.0f, 0, 0, nullptr);

		mCommandList->OMSetRenderTargets(1, &CurrentBackBufferView(), true, &DepthStencilView());

		CD3DX12_GPU_DESCRIPTOR_HANDLE render_target_descriptor(mSrvDescriptorHeap->GetGPUDescriptorHandleForHeapStart());
		render_target_descriptor.Offset(UINT(24), mCbvSrvUavDescriptorSize);
		mCommandList->SetGraphicsRootDescriptorTable(4, render_target_descriptor);


		matBuffer = mCurrFrameResource->PostMaterialBuffer->Resource();
		mCommandList->SetGraphicsRootShaderResourceView(3, matBuffer->GetGPUVirtualAddress());

		mCommandList->SetPipelineState(mPSOs["EdgeDetection"].Get());
		DrawRenderItems(mCommandList.Get(), mRitemLayer[(int)RenderLayer::PostProcessing]);

		mCommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(CurrentBackBuffer(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT));


		mCommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(mRenderTarget->Resource(), D3D12_RESOURCE_STATE_GENERIC_READ, D3D12_RESOURCE_STATE_COPY_DEST));

		mCommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(CurrentBackBuffer(), D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_COPY_SOURCE));

		mCommandList->CopyResource(mRenderTarget->Resource(), CurrentBackBuffer());

		mCommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(CurrentBackBuffer(), D3D12_RESOURCE_STATE_COPY_SOURCE, D3D12_RESOURCE_STATE_PRESENT));

		mCommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(mRenderTarget->Resource(),
			D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_GENERIC_READ));
	}
}




//	Draw Gizmo
void ForwardRenderer::DrawGizmo()
{
	mCommandList->SetPipelineState(mPSOs["Gizmo"].Get());
	DrawRenderItems(mCommandList.Get(), mRitemLayer[(int)RenderLayer::Gizmo]);
}

//	Draw Imgui
void ForwardRenderer::DrawImgui()
{
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;       // Enable Keyboard Controls
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;           // Enable Docking
	DrawEditor();
}


//	DrawEditor
void ForwardRenderer::DrawEditor()
{
	// Start the Dear ImGui frame
	ImGui_ImplDX12_NewFrame();
	ImGui_ImplWin32_NewFrame();

	ImGui::NewFrame();

	//Draw Stylize
	SetDefaultStyle();
	SetDockSpace(&show_app_dockspace);
	DrawSceneGameView();
	DrawRenderSetting();
	DrawProfileView();
	//Draw Light Setting
	if (show_lightSetting_panel)
			ForwardRenderer::DrawLightSettings();
	if (show_physicsSetting_panel)
		ForwardRenderer::DrawPhysicsSettings();
	if (show_postprocessingSetting_panel)
		ForwardRenderer::DrawPostProcessingSettings();

	//Draw Graphics Item
	ForwardRenderer::DrawGraphicsItemEditor();
	////Render Item
	ForwardRenderer::DrawRenderItemEditor();
	////Console
	ForwardRenderer::DrawConsoleEditor();

	mCommandList->SetDescriptorHeaps(1, mSrvHeap.GetAddressOf());
	ImGui::Render();
	ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), mCommandList.Get());
}

//	设置DockSpace
void ForwardRenderer::SetDockSpace(bool* p_open)
{
	static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;

	ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;

	const ImGuiViewport* viewport = ImGui::GetMainViewport();
	ImGui::SetNextWindowPos(viewport->WorkPos);
	ImGui::SetNextWindowSize(viewport->WorkSize);
	ImGui::SetNextWindowViewport(viewport->ID);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
	window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
	window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;

	if (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode)
		window_flags |= ImGuiWindowFlags_NoBackground;

	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
	ImGui::Begin("DockSpace Demo", p_open, window_flags);
	ImGui::PopStyleVar();

	ImGui::PopStyleVar(2);

	ImGuiIO& io = ImGui::GetIO();
	if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
	{
		ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
		ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);
	}

	//Draw Menu
	ForwardRenderer::DrawMenuEditor();

	ImGui::End();
}

//	DrawMenuEditor
void ForwardRenderer::DrawMenuEditor()
{
	if (ImGui::BeginMenuBar())
	{
		if (ImGui::BeginMenu("File"))
		{
			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("Edit"))
		{
			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("GameObject"))
		{
			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("Tools"))
		{
			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("Window"))
		{
			ImGui::MenuItem("Light Settings", NULL, &show_lightSetting_panel);
			ImGui::MenuItem("Physics Settings", NULL, &show_physicsSetting_panel);
			ImGui::MenuItem("PostProcessing Settings", NULL, &show_postprocessingSetting_panel);
			ImGui::EndMenu();
		}
		ImGui::EndMenuBar();
	}
}

//	DrawGraphicsItemEditor
void ForwardRenderer::DrawGraphicsItemEditor()
{
	//Graphics Item
	const ImGuiViewport* main_viewport = ImGui::GetMainViewport();
	ImGui::SetWindowPos(ImVec2(main_viewport->WorkPos.x, main_viewport->WorkPos.y), ImGuiCond_FirstUseEver);
	ImGui::SetWindowSize(ImVec2(320, mClientHeight), ImGuiCond_FirstUseEver);

	//Main Camera
	ImGui::Begin("Main Camera");
	mCamera.UpdateCamPosArray();
	mCamera.UpdateCamRotArray();
	ImGui::Separator();
	if (ImGui::CollapsingHeader("Transform"))
	{
		ImGui::Text("Transform");
		ImGui::InputFloat3("Position", mCamera.mPosArray);
		ImGui::InputFloat3("Rotation", mCamera.mRotArray);
		ImGui::InputFloat3("Scale", mCamera.mScaleArray);
	}
	ImGui::Separator();
	if (ImGui::CollapsingHeader("Camera"))
	{
		ImGui::SliderFloat("FOV", &mCamFov, 0, 179);
		ImGui::InputFloat("Near Clip", &mCamClipN);
		ImGui::InputFloat("Far Clip", &mCamClipF);
		ImGui::Checkbox("RenderSkyBox", &renderSkyBox);
		ImGui::ColorEdit3("SolidColor", SolidColor);
		ImGui::InputFloat2("ViewPort", RectXY);
		ImGui::InputFloat2("Rect", RectWH);
	}
	ImGui::End();



	ImGui::Begin("Light Settings");
	if (ImGui::CollapsingHeader("Direction Light"))
	{
		//Direction Light
		ImGui::Text("Transform");
		float dirLightPos[3] = { mDirectionLightsPos.x,mDirectionLightsPos.y,mDirectionLightsPos.z };
		ImGui::InputFloat3("(D)Position", dirLightPos);
		mDirectionLightsPos = XMFLOAT3(dirLightPos[0], dirLightPos[1], dirLightPos[2]);
		ImGui::InputFloat3("(D)Rotation", mDirectionLightsAngle);
		ImGui::InputFloat3("(D)Scale", mDirectionLightsScale);
		ImGui::Text("Porperties");
		ImGui::Checkbox("(D)Enable", &mDirectionLightsActive);
		ImGui::ColorEdit3("(D)Color", mDirectionLightsColor);
		ImGui::SliderFloat("(D)Intensity", &mDirectionLightsStrength, 0.0f, 10);
		ImGui::Checkbox("Cast Shadow", &mDirectionLightsCastShadow);
	}
	ImGui::Separator();
	
	if (ImGui::CollapsingHeader("Point Light"))
	{
		//Point Light
		ImGui::Text("Transform");
		float pointLightPos[3] = { mPointLightsPos.x,mPointLightsPos.y,mPointLightsPos.z };
		ImGui::InputFloat3("(P)Position", pointLightPos);
		mPointLightsPos = XMFLOAT3(pointLightPos[0], pointLightPos[1], pointLightPos[2]);
		ImGui::InputFloat3("(P)Rotation", mPointLightsRot);
		ImGui::InputFloat3("(P)Scale", mPointLightsScale);
		ImGui::Text("Porperties");
		ImGui::Checkbox("(P)Enable", &mPointLightsActive);
		ImGui::ColorEdit3("(P)Color", mPointLightsColor);
		ImGui::SliderFloat("(P)Intensity", &mPointLightsStrength, 0, 10);
		ImGui::InputFloat("(P)Range", &mPointLightsRange);
	}
	ImGui::Separator();

	if (ImGui::CollapsingHeader("Spot Light"))
	{
		//Spot Light
		ImGui::Text("Transform");
		float spotLightPos[3] = { mSpotLightsPos.x,mSpotLightsPos.y,mSpotLightsPos.z };
		ImGui::InputFloat3("(S)Position", spotLightPos);
		ImGui::InputFloat3("(S)Rotation", mSpotLightsAngle);
		ImGui::InputFloat3("(S)Scale", mSpotLightsScale);
		ImGui::Text("Porperties");
		ImGui::Checkbox("(S)Enable", &mSpotLightsActive);
		ImGui::ColorEdit3("(S)Color", mSpotLightsColor);
		ImGui::DragFloat("(S)Intensity", &mSpotLightsStrength, 0.003f, 0, 3);
		ImGui::InputFloat("(S)Range", &mSpotLightsRange);
		ImGui::InputFloat("Cut Angle", &mSpotLightsCutAngle);
	}

	ImGui::End();
}

//	DrawRenderItemEditor
void ForwardRenderer::DrawRenderItemEditor()
{
	const ImGuiViewport* main_viewport = ImGui::GetMainViewport();
	ImGui::SetWindowPos(ImVec2(main_viewport->WorkPos.x + mClientWidth - 300, main_viewport->WorkPos.y), ImGuiCond_FirstUseEver);
	ImGui::SetWindowSize(ImVec2(300, mClientHeight), ImGuiCond_FirstUseEver);

	//Render Item
	ImGui::Begin("Inspector");

	if (ImGui::CollapsingHeader("Transform"))
	{
		ImGui::Separator();
		ImGui::Text("Transform");
		ImGui::InputFloat3("Position", TargetTransform);
		ImGui::InputFloat3("Rotation", TargetRotationAngle);
		ImGui::InputFloat3("Scale", TargetScale);
		ImGui::Separator();
	}
	if (ImGui::CollapsingHeader("Material"))
	{
		Material* currentMat = mMaterials["sphere"].get();

		if (ImGui::TreeNode("PBR Settings"))
		{
			float colorMain[4] = { currentMat->BaseColor.x,currentMat->BaseColor.y,currentMat->BaseColor.z,1 };
			ImGui::ColorEdit3("MainColor", colorMain);
			currentMat->BaseColor = Vector4(colorMain[0], colorMain[1], colorMain[2], colorMain[3]);

			ImGui::SliderFloat("Metallic", &currentMat->Metallic, 0, 1);
			ImGui::SliderFloat("Smoothness", &currentMat->Smoothness, 0, 1);
			ImGui::SliderFloat("Occlusion", &currentMat->Occlusion, 0, 1);
			ImGui::TreePop();
		}

		if (ImGui::TreeNode("Addition Settings"))
		{
			float colorEmission[4] = { currentMat->EmissionColor.x,currentMat->EmissionColor.y,currentMat->EmissionColor.z,1 };
			ImGui::ColorEdit3("Emission", colorEmission);
			currentMat->EmissionColor = Vector4(colorEmission[0], colorEmission[1], colorEmission[2], colorEmission[3]);
			ImGui::SliderFloat("Strength", &currentMat->EmissionStrength, 0, 1);
			ImGui::TreePop();
		}

		currentMat->NumFramesDirty++;
	}
	ImGui::End();
}

//	DrawConsoleEditor
void ForwardRenderer::DrawConsoleEditor()
{
	const ImGuiViewport* main_viewport = ImGui::GetMainViewport();
	ImGui::SetWindowPos(ImVec2(main_viewport->WorkPos.x + 320, main_viewport->WorkPos.y + mClientHeight - 320), ImGuiCond_FirstUseEver);
	ImGui::SetWindowSize(ImVec2(mClientWidth - 620, 320), ImGuiCond_FirstUseEver);

	ImGui::Begin("Project");


	if (ImGui::BeginTabBar("##tabs", ImGuiTabBarFlags_None))
	{
		if (ImGui::BeginTabItem("Image"))
		{
			ImGui::EndTabItem();
		}

		if (ImGui::BeginTabItem("Mesh"))
		{
			ImGui::EndTabItem();
		}

		if (ImGui::BeginTabItem("Scene"))
		{
			ImGui::EndTabItem();
		}

		if (ImGui::BeginTabItem("Material"))
		{
			ImGui::EndTabItem();
		}

		ImGui::EndTabBar();
	}

	ImGui::End();

	ImGui::Begin("Console");
	ImGui::End();
}

//	DrawOverLayProfile
void ForwardRenderer::DrawOverLayProfile()
{
	//static int corner = 0;
	//ImGui::SetNextWindowBgAlpha(0.35f); // Transparent background
	//if (ImGui::Begin("Example: Simple overlay"))
	//{
	//	ImGui::Text("Simple overlay\n" "in the corner of the screen.\n" "(right-click to change position)");
	//	ImGui::Separator();
	//	if (ImGui::IsMousePosValid())
	//		ImGui::Text("Mouse Position: (%.1f,%.1f)", 0, 0);
	//	else
	//		ImGui::Text("Mouse Position: <invalid>");
	//	if (ImGui::BeginPopupContextWindow())
	//	{
	//		if (ImGui::MenuItem("Custom", NULL, corner == -1)) corner = -1;
	//		if (ImGui::MenuItem("Top-left", NULL, corner == 0)) corner = 0;
	//		if (ImGui::MenuItem("Top-right", NULL, corner == 1)) corner = 1;
	//		if (ImGui::MenuItem("Bottom-left", NULL, corner == 2)) corner = 2;
	//		if (ImGui::MenuItem("Bottom-right", NULL, corner == 3)) corner = 3;
	//		ImGui::EndPopup();
	//	}
	//}
	//ImGui::End();
}

//	DrawPhysicsSettings
void ForwardRenderer::DrawPhysicsSettings()
{
	//Render Item
	ImGui::SetWindowSize(ImVec2(180, 500));
	ImGui::Begin("Physics Settings", &show_physicsSetting_panel);

	ImGui::Text("Bounce Range");
	ImGui::Checkbox("Bounce Show", &ShowBounding);
	ImGui::End();
}

void ForwardRenderer::DrawSceneGameView()
{
	ImGui::Begin("Scene");
	CD3DX12_GPU_DESCRIPTOR_HANDLE render_target_descriptor(mSrvDescriptorHeap->GetGPUDescriptorHandleForHeapStart());
	render_target_descriptor.Offset(UINT(15), mCbvSrvUavDescriptorSize);

	ImGui::Image((ImTextureID)render_target_descriptor.ptr, ImVec2((float)300, (float)300));
	ImGui::End();

	ImGui::Begin("Game");
	ImGui::End();
}

void ForwardRenderer::DrawRenderSetting()
{
	ImGui::Begin("Render Setting");
	ImGui::End();
}

//	Light Settings
void ForwardRenderer::DrawLightSettings()
{
	//Render Item
	ImGui::SetWindowSize(ImVec2(180, 500));
	ImGui::Begin("Light Settings", &show_lightSetting_panel);

	Material* currentMat = mMaterials["sky"].get();

	//ImGui::Text("Environment");

	//ImGui::ColorEdit3("Shadow Color", EnvironmentShadowColor);

	ImGui::Text("SkyBox");

	float colorMain[4] = { currentMat->SkyBoxTint.x,currentMat->SkyBoxTint.y,currentMat->SkyBoxTint.z,1 };
	ImGui::ColorEdit3("Tint", colorMain);
	currentMat->SkyBoxTint = Vector4(colorMain[0], colorMain[1], colorMain[2], colorMain[3]);

	ImGui::SliderFloat("Exposure", &currentMat->SkyBoxExposure, 0, 8);
	ImGui::SliderFloat("Rotation", &currentMat->SkyBoxRotation, 0, 360);

	bool UseAces = (bool)currentMat->ACES;
	ImGui::Checkbox("Aces Enable", &UseAces);
	currentMat->ACES = (int)UseAces;

	currentMat->NumFramesDirty++;
	ImGui::End();
}

//	PostProcessing
void ForwardRenderer::DrawPostProcessingSettings()
{
	//Render Item
	ImGui::SetWindowSize(ImVec2(400, 900));
	ImGui::Begin("PostProcessing Settings", &show_postprocessingSetting_panel);


	/// <summary>
	/// RGBSplit
	/// </summary>
	if (ImGui::CollapsingHeader("RGB Split"))
	{
		Material* currentMat = mMaterials["RGBSplit"].get();
		ImGui::Checkbox("Enable RGB Split", &UseRGBSplit);
		if (UseRGBSplit)
		{
			ImGui::SliderFloat("RgbSplit Strength", &currentMat->RGBSplitStrength, 0, 0.3f);
		}
		currentMat->NumFramesDirty++;
	}


	/// <summary>
	/// RadialBlur
	/// </summary>
	if (ImGui::CollapsingHeader("Radial Blur"))
	{
		Material* currentMat = mMaterials["RadialBlur"].get();
		ImGui::Checkbox("Enable Radial Blur", &UseRadialBlur);
		if (UseRadialBlur)
		{
			ImGui::SliderFloat("Blur Factory", &currentMat->BlurFactory, 0, 0.1f);
		}
		currentMat->NumFramesDirty++;
	}



	/// <summary>
	/// Vignette
	/// </summary>
	if (ImGui::CollapsingHeader("Vignette"))
	{
		Material* currentMat = mMaterials["Vignette"].get();
		ImGui::Checkbox("Enable Vignette", &UseVignette);
		if (UseVignette)
		{
			ImGui::SliderFloat("Width", &currentMat->Width, 0, 2.0f);
			ImGui::SliderFloat("Height", &currentMat->Height, 0, 1.0f);
			ImGui::SliderFloat("Smooth", &currentMat->Smooth, 1.0, 10.0f);
			ImGui::SliderFloat("Alpha", &currentMat->Alpha, 0.0, 1.0f);
			float colorMain[4] = { currentMat->VignetteColor.x,currentMat->VignetteColor.y,currentMat->VignetteColor.z,1 };
			ImGui::ColorEdit3("Vignette Color", colorMain);
			currentMat->VignetteColor = XMFLOAT4(colorMain[0], colorMain[1], colorMain[2], colorMain[3]);
		}
		currentMat->NumFramesDirty++;
	}


	/// <summary>
	/// Decolor
	/// </summary>
	if (ImGui::CollapsingHeader("Decolor"))
	{
		Material* currentMat = mMaterials["Decolor"].get();
		ImGui::Checkbox("Enable Decolor", &UseDecolor);
		if (UseDecolor)
		{
			ImGui::SliderFloat("Decolor Strength", &currentMat->DecolorStrength, 0, 1.0f);
		}
		currentMat->NumFramesDirty++;
	}


	/// <summary>
	/// Brightness
	/// </summary>
	if (ImGui::CollapsingHeader("Brightness And Contrast"))
	{
		Material* currentMat = mMaterials["Brightness"].get();
		ImGui::Checkbox("Enable Brightness", &UseBrightness);
		if (UseBrightness)
		{
			ImGui::SliderFloat("Bright-ness", &currentMat->Brightness, 0.0f, 3.0f);

			ImGui::SliderFloat("Contrast", &currentMat->Contrast, 0.0f, 3.0f);
		}
		currentMat->NumFramesDirty++;
	}


	/// <summary>
	/// HSV
	/// </summary>
	if (ImGui::CollapsingHeader("HSV"))
	{
		Material* currentMat = mMaterials["HSV"].get();
		ImGui::Checkbox("Enable HSV", &UseHSV);
		if (UseHSV)
		{
			ImGui::SliderFloat("Hue", &currentMat->Hue, 0, 1.0f);
			ImGui::SliderFloat("Saturation", &currentMat->Saturation, -1, 1.0f);
			ImGui::SliderFloat("Value", &currentMat->Value, -1, 1.0f);
		}
		currentMat->NumFramesDirty++;
	}

	/// <summary>
	/// Mosaic
	/// </summary>
	if (ImGui::CollapsingHeader("Mosaic"))
	{
		Material* currentMat = mMaterials["Mosaic"].get();
		ImGui::Checkbox("Enable Mosaic", &UseMosaic);
		if (UseMosaic)
		{
			ImGui::InputFloat("PixelSize", &currentMat->PixelSize);
		}
		currentMat->NumFramesDirty++;
	}


	/// <summary>
	/// Sharpen
	/// </summary>
	if (ImGui::CollapsingHeader("Sharpen"))
	{
		Material* currentMat = mMaterials["Sharpen"].get();
		ImGui::Checkbox("Enable Sharpen", &UseSharpen);
		if (UseSharpen)
		{
			ImGui::SliderFloat("SharpenStrength", &currentMat->SharpenStrength, 0, 5.0f);
			ImGui::SliderFloat("SharpenThreshold", &currentMat->SharpenThreshold, 0, 1.0f);
		}
		currentMat->NumFramesDirty++;
	}


	/// <summary>
	/// Spherize
	/// </summary>
	if (ImGui::CollapsingHeader("Spherize"))
	{
		Material* currentMat = mMaterials["Spherize"].get();
		ImGui::Checkbox("Enable Spherize", &UseSpherize);
		if (UseSpherize)
		{
			ImGui::SliderFloat("Spherify", &currentMat->Spherify, 0, 1.0f);
		}
		currentMat->NumFramesDirty++;
	}


	/// <summary>
	/// WhiteBalance
	/// </summary>
	if (ImGui::CollapsingHeader("White Balance"))
	{
		Material* currentMat = mMaterials["WhiteBalance"].get();
		ImGui::Checkbox("Enable WhiteBalance", &UseWhiteBalance);
		if (UseWhiteBalance)
		{
			ImGui::SliderFloat("Temperature", &currentMat->Temperature, -1.0, 1.0f);
			ImGui::SliderFloat("Tint", &currentMat->Tint, -1.0, 1.0f);
		}
		currentMat->NumFramesDirty++;
	}


	/// <summary>
	/// OilPaint
	/// </summary>
	if (ImGui::CollapsingHeader("Oil Paint"))
	{
		Material* currentMat = mMaterials["OilPaint"].get();
		ImGui::Checkbox("Enable OilPaint", &UseOilPaint);
		if (UseOilPaint)
		{
			ImGui::SliderFloat("OilPaint Radius", &currentMat->OilPaintRadius, 0.0, 5.0f);
			ImGui::SliderFloat("ResoultionValue", &currentMat->ResoultionValue, 0.0, 5.0f);
		}
		currentMat->NumFramesDirty++;
	}


	/// <summary>
	/// Relief
	/// </summary>
	if (ImGui::CollapsingHeader("Relief"))
	{
		ImGui::Checkbox("Enable Relief", &UseRelief);
	}

	/// <summary>
	/// EdgeDetection
	/// </summary>
	if (ImGui::CollapsingHeader("EdgeDetection"))
	{
		Material* currentMat = mMaterials["EdgeDetection"].get();
		ImGui::Checkbox("Enable EdgeDetection", &UseEdgeDetection);
		if (UseEdgeDetection)
		{
			ImGui::SliderFloat("Edge Width", &currentMat->EdgeWdith, 0.0, 5.0f);
			ImGui::SliderFloat("Edge Neon Fade", &currentMat->EdgeNeonFade, 0.0, 1.0f);

			float colorOutLine[4] = { currentMat->OutLineColor.x,currentMat->OutLineColor.y,currentMat->OutLineColor.z,1 };
			ImGui::ColorEdit3("OutLine Color", colorOutLine);
			currentMat->OutLineColor = XMFLOAT4(colorOutLine[0], colorOutLine[1], colorOutLine[2], colorOutLine[3]);

			ImGui::SliderFloat("Background Fade", &currentMat->BackgroundFade, 0.0, 1.0f);

			float colorMain[4] = { currentMat->BackgroundColor.x,currentMat->BackgroundColor.y,currentMat->BackgroundColor.z,1 };
			ImGui::ColorEdit3("Background Color", colorMain);
			currentMat->BackgroundColor = XMFLOAT4(colorMain[0], colorMain[1], colorMain[2], colorMain[3]);

		}
		currentMat->NumFramesDirty++;
	}

	/// <summary>
	/// SSAO
	/// </summary>
	if (ImGui::CollapsingHeader("SSAO"))
	{
		ImGui::Checkbox("Enable SSAO", &UseSSAO);
	}

	/// <summary>
	/// MotionBlur
	/// </summary>
	if (ImGui::CollapsingHeader("Motion Blur"))
	{
		Material* currentMat = mMaterials["OilPaint"].get();
		ImGui::Checkbox("Enable MotionBlur", &UseMotionBlur);
		if (UseMotionBlur)
		{
			ImGui::SliderFloat("Blur Amount", &currentMat->MotionBlurAmount, 0.0, 5.0f);
		}
		currentMat->NumFramesDirty++;
	}

	/// <summary>
	/// DOF
	/// </summary>
	if (ImGui::CollapsingHeader("Depth Of Field"))
	{
		Material* currentMat = mMaterials["OilPaint"].get();
		ImGui::Checkbox("Enable Depth Of Field", &UseDOF);
		if (UseDOF)
		{
			ImGui::InputFloat("Distance", &currentMat->Distance);
			ImGui::InputFloat("Aperture", &currentMat->Aperture);
			ImGui::SliderFloat("FocalLength", &currentMat->FocalLength, 0.0, 300.0f);
		}
		currentMat->NumFramesDirty++;
	}

	/// <summary>
	/// FidelityFX Super Resolution
	/// </summary>
	if (ImGui::CollapsingHeader("FidelityFX Super Resolution"))
	{
		Material* currentMat = mMaterials["OilPaint"].get();
		ImGui::Checkbox("Enable FSR", &UseFSR);
		if (UseFSR)
		{
			ImGui::InputFloat("ResoultionLevel", &currentMat->ResoultionLevel);
			ImGui::SliderFloat("FSRSharpen", &currentMat->FSRSharpen, 0.0, 1.0f);
		}
		currentMat->NumFramesDirty++;
	}

	/// <summary>
	/// FXAA
	/// </summary>
	if (ImGui::CollapsingHeader("FXAA"))
	{
		ImGui::Checkbox("Enable FXAA", &UseFXAA);
	}

	/// <summary>
	/// ToneMapping
	/// </summary>
	if (ImGui::CollapsingHeader("ToneMapping(ACES)"))
	{
		ImGui::Checkbox("Enable ToneMapping", &UseToneMapping);
	}

	/// <summary>
	/// Bloom
	/// </summary>
	if (ImGui::CollapsingHeader("Bloom"))
	{
		Material* currentMat = mMaterials["Bloom"].get();
		ImGui::Checkbox("Enable Bloom", &UseBloom);
		if (UseBloom)
		{
			ImGui::SliderFloat("Threshold", &currentMat->Threshold, 0.0, 10.0f);
			ImGui::SliderFloat("SoftKnee", &currentMat->SoftKnee,0.0, 1.0f);
			ImGui::SliderFloat("Radius", &currentMat->Radius, 0.0, 7.0f);
			ImGui::InputFloat("Intensity", &currentMat->Intensity);
		}
		currentMat->NumFramesDirty++;
	}

	ImGui::End();
}

void ForwardRenderer::SetDefaultStyle()
{
	ImGuiStyle* style = &ImGui::GetStyle();
	ImVec4* colors = style->Colors;

	//	文字颜色
	colors[ImGuiCol_Text] = ImVec4(0.89f, 0.89f, 0.89f, 1.000f);
	//	文字不显示
	colors[ImGuiCol_TextDisabled] = ImVec4(0.500f, 0.500f, 0.500f, 1.000f);
	//	窗口背景色
	colors[ImGuiCol_WindowBg] = ImVec4(0.1843f, 0.1882f, 0.2078f, 1.000f);
	//	子窗口颜色
	colors[ImGuiCol_ChildBg] = ImVec4(0.2431f, 0.2470f, 0.2745f, 0.000f);
	//	弹出框
	colors[ImGuiCol_PopupBg] = ImVec4(0.08f, 0.08f, 0.08f, 0.8f);
	//	边界线
	colors[ImGuiCol_Border] = ImVec4(0.1098f, 0.1137f, 0.1254f, 1.000f);
	//	子结点分界框
	colors[ImGuiCol_BorderShadow] = ImVec4(0.000f, 0.000f, 0.000f, 0.000f);

	colors[ImGuiCol_FrameBg] = ImVec4(0.3137f, 0.3137f, 0.3456f, 1.000f);
	colors[ImGuiCol_FrameBgHovered] = ImVec4(0.4431f, 0.4549f, 0.5019f, 1.000f);
	colors[ImGuiCol_FrameBgActive] = ImVec4(0.4431f, 0.4549f, 0.5019f, 1.000f);

	//	标题背景
	colors[ImGuiCol_TitleBg] = ImVec4(0.1215f, 0.1176f, 0.1411f, 1.000f);
	colors[ImGuiCol_TitleBgActive] = ImVec4(0.1215f, 0.1176f, 0.1411f, 1.000f);
	colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.1215f, 0.1176f, 0.1411f, 1.000f);

	//	主菜单
	colors[ImGuiCol_MenuBarBg] = ImVec4(0.1607f, 0.1568f, 0.1803f, 1.000f);

	//	滚动条
	colors[ImGuiCol_ScrollbarBg] = ImVec4(0.1843f, 0.1882f, 0.2078f, 1.000f);
	colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.2470f, 0.2470f, 0.2745f, 1.000f);
	colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.2470f, 0.2470f, 0.2745f, 1.000f);
	colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.8f, 0.1568f, 0.1803f, 1.000f);

	//	复选框
	colors[ImGuiCol_CheckMark] = ImVec4(1.000f, 1.000f, 1.000f, 1.000f);

	//	滑动条
	colors[ImGuiCol_SliderGrab] = ImVec4(0.3137f, 0.3137f, 0.3456f, 1.000f);
	colors[ImGuiCol_SliderGrabActive] = ImVec4(0.8f, 0.1568f, 0.1803f, 1.000f);

	//	按钮
	colors[ImGuiCol_Button] = ImVec4(1.000f, 1.000f, 1.000f, 0.000f);
	colors[ImGuiCol_ButtonHovered] = ImVec4(1.000f, 1.000f, 1.000f, 0.156f);
	colors[ImGuiCol_ButtonActive] = ImVec4(1.000f, 1.000f, 1.000f, 0.391f);

	//	标题
	colors[ImGuiCol_Header] = ImVec4(0.1215f, 0.1176f, 0.1411f, 1.000f);
	colors[ImGuiCol_HeaderHovered] = ImVec4(0.8f, 0.1568f, 0.1803f, 1.000f);
	colors[ImGuiCol_HeaderActive] = ImVec4(0.8f, 0.1568f, 0.1803f, 1.000f);

	//	分割线
	colors[ImGuiCol_Separator] = colors[ImGuiCol_Border];
	colors[ImGuiCol_SeparatorHovered] = ImVec4(0.0862f, 0.0901f, 0.1058f, 1.000f);
	colors[ImGuiCol_SeparatorActive] = ImVec4(0.0862f, 0.0901f, 0.1058f, 1.000f);

	//	调整大小
	colors[ImGuiCol_ResizeGrip] = ImVec4(1.000f, 1.000f, 1.000f, 0.250f);
	colors[ImGuiCol_ResizeGripHovered] = ImVec4(1.000f, 1.000f, 1.000f, 0.670f);
	colors[ImGuiCol_ResizeGripActive] = ImVec4(1.000f, 0.391f, 0.000f, 1.000f);

	//	Tap
	colors[ImGuiCol_Tab] = ImVec4(0.1215f, 0.1176f, 0.1411f, 1.000f);
	colors[ImGuiCol_TabHovered] = ImVec4(0.8f, 0.1568f, 0.1803f, 1.000f);
	colors[ImGuiCol_TabActive] = ImVec4(0.1098f, 0.1137f, 0.1254f, 1.000f);
	colors[ImGuiCol_TabUnfocused] = ImVec4(0.098f, 0.098f, 0.098f, 1.000f);
	colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.195f, 0.195f, 0.195f, 1.000f);

	//	
	colors[ImGuiCol_PlotLines] = ImVec4(0.469f, 0.469f, 0.469f, 1.000f);
	colors[ImGuiCol_PlotLinesHovered] = ImVec4(1.000f, 0.391f, 0.000f, 1.000f);
	colors[ImGuiCol_PlotHistogram] = ImVec4(0.586f, 0.586f, 0.586f, 1.000f);
	colors[ImGuiCol_PlotHistogramHovered] = ImVec4(1.000f, 0.391f, 0.000f, 1.000f);

	//	文字选中
	colors[ImGuiCol_TextSelectedBg] = ImVec4(1.000f, 1.000f, 1.000f, 0.156f);
	colors[ImGuiCol_DragDropTarget] = ImVec4(1.000f, 0.391f, 0.000f, 1.000f);
	colors[ImGuiCol_NavHighlight] = ImVec4(1.000f, 0.391f, 0.000f, 1.000f);
	colors[ImGuiCol_NavWindowingHighlight] = ImVec4(1.000f, 0.391f, 0.000f, 1.000f);
	colors[ImGuiCol_NavWindowingDimBg] = ImVec4(0.000f, 0.000f, 0.000f, 0.586f);
	colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0.000f, 0.000f, 0.000f, 0.586f);

	style->ChildRounding = 4.0f;
	style->FrameBorderSize = 1.0f;
	style->FrameRounding = 3.0f;
	style->GrabMinSize = 7.0f;
	style->PopupRounding = 2.0f;
	style->ScrollbarRounding = 12.0f;
	style->ScrollbarSize = 13.0f;
	style->TabBorderSize = 1.0f;
	style->TabRounding = 0.0f;
	style->WindowRounding = 0.0f;
}

void ForwardRenderer::DrawProfileView()
{
	ImGui::Begin("Profile");
	static bool animate = true;

	//static float arr[] = { 0.6f, 0.1f, 1.0f, 0.5f, 0.92f, 0.1f, 0.2f };
	//ImGui::PlotLines("Frame Times", arr, IM_ARRAYSIZE(arr));
	//ImGui::PlotHistogram("Histogram", arr, IM_ARRAYSIZE(arr), 0, NULL, 0.0f, 1.0f, ImVec2(0, 80.0f));

	static float values[90] = {};
	static int values_offset = 0;
	static double refresh_time = 0.0;
	if (!animate || refresh_time == 0.0)
		refresh_time = ImGui::GetTime();
	while (refresh_time < ImGui::GetTime()) // Create data at fixed 60 Hz rate for the demo
	{
		static float phase = 0.0f;
		values[values_offset] = cosf(phase);
		values_offset = (values_offset + 1) % IM_ARRAYSIZE(values);
		phase += 0.10f * values_offset;
		refresh_time += 1.0f / 60.0f;
	}

	// Plots can display overlay texts
	// (in this example, we will display an average value)
	{
		float average = 0.0f;
		for (int n = 0; n < IM_ARRAYSIZE(values); n++)
			average += values[n];
		average /= (float)IM_ARRAYSIZE(values);
		char overlay[32];
		sprintf(overlay, "avg %f", average);
		ImGui::PlotLines(" ", values, IM_ARRAYSIZE(values), values_offset, overlay, -1.0f, 1.0f, ImVec2(300, 300));
	}
	ImGui::End();
}

void DrawSceneHelper()
{
	//ImGui::ImageButton()
}