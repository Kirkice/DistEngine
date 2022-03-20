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
	ImGui::StyleColorsClassic();

	//Draw Menu
	ForwardRenderer::DrawMenuEditor();

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

//	DrawMenuEditor
void ForwardRenderer::DrawMenuEditor()
{
	ImGui::SetNextWindowBgAlpha(0.8f);
	if (ImGui::BeginMainMenuBar())
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
		ImGui::EndMainMenuBar();
	}
}

//	DrawGraphicsItemEditor
void ForwardRenderer::DrawGraphicsItemEditor()
{
	//Graphics Item
	ImGui::SetNextWindowBgAlpha(0.8f);
	ImGui::Begin("Graphics Item");
	const ImGuiViewport* main_viewport = ImGui::GetMainViewport();
	ImGui::SetWindowPos(ImVec2(main_viewport->WorkPos.x, main_viewport->WorkPos.y), ImGuiCond_Always);
	ImGui::SetWindowSize(ImVec2(320, mClientHeight), ImGuiCond_Always);

	//Camera
	if (ImGui::CollapsingHeader("MainCamera"))
	{
		mCamera.UpdateCamPosArray();
		mCamera.UpdateCamRotArray();
		ImGui::Separator();
		ImGui::Text("Transform");
		ImGui::InputFloat3("Position", mCamera.mPosArray);
		ImGui::InputFloat3("Rotation", mCamera.mRotArray);
		ImGui::InputFloat3("Scale", mCamera.mScaleArray);
		ImGui::Separator();
		ImGui::Text("Camera");
		ImGui::SliderFloat("FOV(Y)", &mCamFov, 0, 179);
		ImGui::InputFloat("Near Clip", &mCamClipN);
		ImGui::InputFloat("Far Clip", &mCamClipF);
		ImGui::Checkbox("RenderSkyBox", &renderSkyBox);
		ImGui::ColorEdit3("SolidColor", SolidColor);
		ImGui::InputFloat2("ViewPort", RectXY);
		ImGui::InputFloat2("Rect", RectWH); 
		ImGui::Separator();
	}

	//Light
	if (ImGui::CollapsingHeader("Light"))
	{
		if (ImGui::TreeNode("Direction Light"))
		{
			//Direction Light
			ImGui::Text("Transform");
			float dirLightPos[3] = { mDirectionLightsPos.x,mDirectionLightsPos.y,mDirectionLightsPos.z };
			ImGui::InputFloat3("(D)Position", dirLightPos);
			mDirectionLightsPos = XMFLOAT3(dirLightPos[0], dirLightPos[1], dirLightPos[2]);
			ImGui::InputFloat3("(D)Rotation", mDirectionLightsAngle);
			ImGui::InputFloat3("(D)Scale", mDirectionLightsScale);
			ImGui::Text("Settings");
			ImGui::Checkbox("Direction Enable", &mDirectionLightsActive);
			ImGui::ColorEdit3("(D)Color", mDirectionLightsColor);
			ImGui::SliderFloat("(D)Intensity", &mDirectionLightsStrength, 0.0f, 10);
			ImGui::Checkbox("Cast Shadow", &mDirectionLightsCastShadow);
			ImGui::TreePop();
		}

		if (ImGui::TreeNode("Point Light"))
		{
			//Point Light
			ImGui::Text("Transform");
			float pointLightPos[3] = { mPointLightsPos.x,mPointLightsPos.y,mPointLightsPos.z };
			ImGui::InputFloat3("(P)Position", pointLightPos);
			mPointLightsPos = XMFLOAT3(pointLightPos[0], pointLightPos[1], pointLightPos[2]);
			ImGui::InputFloat3("(P)Rotation", mPointLightsRot);
			ImGui::InputFloat3("(P)Scale", mPointLightsScale);
			ImGui::Text("Settings");
			ImGui::Checkbox("Point Enable", &mPointLightsActive);
			ImGui::ColorEdit3("(P)Color", mPointLightsColor);
			ImGui::SliderFloat("(P)Intensity", &mPointLightsStrength, 0, 10);
			ImGui::InputFloat("(P)Range", &mPointLightsRange);
			ImGui::TreePop();
		}

		if (ImGui::TreeNode("Spot Light"))
		{
			//Spot Light
			ImGui::Text("Transform");
			float spotLightPos[3] = { mSpotLightsPos.x,mSpotLightsPos.y,mSpotLightsPos.z };
			ImGui::InputFloat3("(S)Position", spotLightPos);
			ImGui::InputFloat3("(S)Rotation", mSpotLightsAngle);
			ImGui::InputFloat3("(S)Scale", mSpotLightsScale);
			ImGui::Text("Settings");
			ImGui::Checkbox("Spot Enable", &mSpotLightsActive);
			ImGui::ColorEdit3("(S)Color", mSpotLightsColor);
			ImGui::DragFloat("(S)Intensity", &mSpotLightsStrength, 0.003f, 0, 3);
			ImGui::InputFloat("(S)Range", &mSpotLightsRange);
			ImGui::InputFloat("Cut Angle", &mSpotLightsCutAngle);
			ImGui::TreePop();
		}
	}
	ImGui::End();
}

//	DrawRenderItemEditor
void ForwardRenderer::DrawRenderItemEditor()
{
	//Render Item
	ImGui::SetNextWindowBgAlpha(0.8f);
	ImGui::Begin("RenderItem Properties");
	const ImGuiViewport* main_viewport = ImGui::GetMainViewport();
	ImGui::SetWindowPos(ImVec2(main_viewport->WorkPos.x + mClientWidth - 300, main_viewport->WorkPos.y), ImGuiCond_Always);
	ImGui::SetWindowSize(ImVec2(300, mClientHeight), ImGuiCond_Always);

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
	ImGui::SetNextWindowBgAlpha(0.8f);
	ImGui::Begin("Console");
	const ImGuiViewport* main_viewport = ImGui::GetMainViewport();
	ImGui::SetWindowPos(ImVec2(main_viewport->WorkPos.x + 320, main_viewport->WorkPos.y + mClientHeight - 180), ImGuiCond_Always);
	ImGui::SetWindowSize(ImVec2(mClientWidth - 620, 180), ImGuiCond_Always);
	if (ImGui::SmallButton("[Debug] Add 5 entries"))
	{
		static int counter = 0;
		const char* categories[3] = { "info", "warn", "error" };
		const char* words[] = { "Bumfuzzled", "Cattywampus", "Snickersnee", "Abibliophobia", "Absquatulate", "Nincompoop", "Pauciloquent" };
		for (int n = 0; n < 5; n++)
		{
			const char* category = categories[counter % IM_ARRAYSIZE(categories)];
			const char* word = words[counter % IM_ARRAYSIZE(words)];
			counter++;
		}
	}
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
	ImGui::SetNextWindowBgAlpha(0.8f);
	ImGui::SetWindowSize(ImVec2(180, 500), ImGuiCond_Always);
	ImGui::Begin("Physics Settings", &show_physicsSetting_panel);

	ImGui::Text("Bounce Range");
	ImGui::Checkbox("Bounce Show", &ShowBounding);
	ImGui::End();
}

//	Light Settings
void ForwardRenderer::DrawLightSettings()
{
	//Render Item
	ImGui::SetNextWindowBgAlpha(0.8f);
	ImGui::SetWindowSize(ImVec2(180, 500), ImGuiCond_Always);
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
	ImGui::SetNextWindowBgAlpha(0.8f);
	ImGui::SetWindowSize(ImVec2(400, 900), ImGuiCond_Always);
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

	ImGui::End();
}