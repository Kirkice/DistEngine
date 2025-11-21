#include "GraphicsCore.h"

using Microsoft::WRL::ComPtr;
using namespace DirectX;
using namespace DirectX::PackedVector;

GraphicsCore::GraphicsCore(HINSTANCE hInstance) : InputSystem(hInstance)
{
}

GraphicsCore::~GraphicsCore()
{
    if (md3dDevice != nullptr)
        FlushCommandQueue();
}

bool GraphicsCore::Initialize()
{
    if (!DX12GameApp::Initialize())
        return false;

    // Reset the command list to prep for initialization commands.
    _forwardPassCmdList->Reset();

    mCamera.getInstance().SetPosition(0.0f, 2.0f, -15.0f);

    //ShadowMap Init
    mShadowMap = std::make_unique<DistRender::ShadowMap>(md3dDevice.Get(), 2048, 2048);

    //DepthPass Init
    mDepthPass = std::make_unique<DistRender::DepthPass>(md3dDevice.Get());

    //RenderTarget Init
    mRenderTexture = std::make_unique<DistRender::RenderTexture>(md3dDevice.Get(), mClientWidth, mClientHeight);

    //GBuffer Init
    //mGBufferPass = std::make_unique<GBuffer>(md3dDevice.Get(), mClientWidth, mClientHeight);

    BuildRootSignature();
    BuildDescriptorHeaps();
    GraphicsUtils::IMGUIInit(mhMainWnd, md3dDevice, mSrvDescriptorHeap.GetDescriptorHeap());
    BuildShadersAndInputLayout();

    matCBIndexUtils.getInstance().Init();
    mGizmoManager.getInstance().BuildScene(mGizmosTextures, matCBIndexUtils);
    mSceneManager.getInstance().BuildScene(mResourcesTextures, matCBIndexUtils);
    mPostProcessManager.getInstance().Build(matCBIndexUtils);

    BuildRenderItems();
    BuildFrameResources();
    BuildPSOs();

    // Execute the initialization commands.
    _forwardPassCmdList->Close();
    ID3D12CommandList* cmdsLists[] = { _forwardPassCmdList->GetInternal().Get() };
    mCommandQueue->ExecuteCommandLists(_countof(cmdsLists), cmdsLists);

    // Wait until initialization is complete.
    FlushCommandQueue();

    return true;
}

void GraphicsCore::CreateRtvAndDsvDescriptorHeaps()
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

void GraphicsCore::OnResize()
{
    DX12GameApp::OnResize();

    mCamera.getInstance().SetLens((mSceneManager.getInstance().mCameraSetting.mCamFov / 180) * Mathf::Pi, AspectRatio(), mSceneManager.getInstance().mCameraSetting.mCamClipN, mSceneManager.getInstance().mCameraSetting.mCamClipF);

    ////	GBuffer OnResize
    //if (mGBufferPass != nullptr)
    //{
    //	mGBufferPass->OnResize(mClientWidth, mClientHeight);
    //	mGBufferPass->RebuildDescriptors();
    //}
}
