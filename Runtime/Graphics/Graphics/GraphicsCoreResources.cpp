#include "GraphicsCore.h"

using Microsoft::WRL::ComPtr;
using namespace DirectX;
using namespace DirectX::PackedVector;

void GraphicsCore::BuildRootSignature()
{
    mRootSignature.Build(RootSignature::RootSignatureType::Default, md3dDevice, 9, 0, 0, 20, 9, 0);
}

//swap chain  BuildDescriptorHeaps
void GraphicsCore::BuildDescriptorHeaps()
{
    mSrvDescriptorHeap.Build(md3dDevice, 128);

    CD3DX12_CPU_DESCRIPTOR_HANDLE CPUDescriptor(mSrvDescriptorHeap.GetDescriptorHeap()->GetCPUDescriptorHandleForHeapStart());
    CD3DX12_GPU_DESCRIPTOR_HANDLE GPUDescriptor(mSrvDescriptorHeap.GetDescriptorHeap()->GetGPUDescriptorHandleForHeapStart());

    UINT mCbvSrvUavDescriptorSize = md3dDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
    UINT RenderTargetStartIndex = 0;
    UINT TextureIndex = 0;
    //Load Icon
    for (size_t i = 0; i < mProjectIconTextures.size(); i++)
    {
        int image_width = 0;
        int image_height = 0;

        ID3D12Resource* texture = NULL;

        // Load the texture from a file
        bool ret = ResourcesManager::LoadTextureFromFile(mProjectIconTextures[i].c_str(), md3dDevice, CPUDescriptor, &texture, &image_width, &image_height);

        if (ret)
        {
            auto tex = std::make_unique<Texture2D>();
            tex->Width = image_width;
            tex->Height = image_height;
            tex->Bit = 32;
            tex->Size = (float)((image_width * image_height * 32) / 8388608);
            tex->type = TexturesType::Icon;
            tex->TexIndex = TextureIndex;
            TextureIndex++;
            tex->CpuHandle = CPUDescriptor;
            tex->GpuHandle = GPUDescriptor;
            tex->Name = mProjectIconName[i];
            tex->Path = mProjectIconTextures[i];
            mIconTextures[tex->Name] = std::move(tex);
        }
        CPUDescriptor.Offset(1, mCbvSrvUavDescriptorSize);
        GPUDescriptor.Offset(1, mCbvSrvUavDescriptorSize);
    }

    //Load Gizmo
    for (size_t i = 0; i < mProjectGizmoTextures.size(); i++)
    {
        int image_width = 0;
        int image_height = 0;

        ID3D12Resource* texture = NULL;

        // Load the texture from a file
        bool ret = ResourcesManager::LoadTextureFromFile(mProjectGizmoTextures[i].c_str(), md3dDevice, CPUDescriptor, &texture, &image_width, &image_height);
        IM_ASSERT(ret);

        auto tex = std::make_unique<Texture2D>();
        tex->Width = image_width;
        tex->Height = image_height;
        tex->Bit = 32;
        tex->Size = (float)((image_width * image_height * 32) / 8388608);
        tex->type = TexturesType::Gizom;
        tex->TexIndex = TextureIndex;
        TextureIndex++;
        tex->CpuHandle = CPUDescriptor;
        tex->GpuHandle = GPUDescriptor;
        tex->Name = mProjectGizmoName[i];
        tex->Path = mProjectGizmoTextures[i];
        mGizmosTextures[tex->Name] = std::move(tex);

        CPUDescriptor.Offset(1, mCbvSrvUavDescriptorSize);
        GPUDescriptor.Offset(1, mCbvSrvUavDescriptorSize);
    }

    //Load Project
    for (size_t i = 0; i < mProjectResourceTextures.size(); i++)
    {
        int image_width = 0;
        int image_height = 0;

        ID3D12Resource* texture = NULL;

        // Load the texture from a file
        bool ret = ResourcesManager::LoadTextureFromFile(mProjectResourceTextures[i].c_str(), md3dDevice, CPUDescriptor, &texture, &image_width, &image_height);
        IM_ASSERT(ret);

        auto tex = std::make_unique<Texture2D>();
        tex->Width = image_width;
        tex->Height = image_height;
        tex->Bit = 32;
        tex->Size = (float)((image_width * image_height * 32) / 8388608);
        tex->type = TexturesType::Project;
        tex->TexIndex = TextureIndex;
        TextureIndex++;
        tex->CpuHandle = CPUDescriptor;
        tex->GpuHandle = GPUDescriptor;
        tex->Name = mProjectResourceName[i];
        tex->Path = mProjectResourceTextures[i];
        mResourcesTextures[tex->Name] = std::move(tex);

        CPUDescriptor.Offset(1, mCbvSrvUavDescriptorSize);
        GPUDescriptor.Offset(1, mCbvSrvUavDescriptorSize);
    }

    RenderTargetStartIndex = TextureIndex;

    //Load CubeMap
    for (size_t i = 0; i < mProjectCubeMapTextures.size(); i++)
    {
        if (mCubeMapTextures.find(mProjectCubeMapName[i]) == std::end(mCubeMapTextures))
        {
            auto tex = std::make_unique<TextureCube>();
            tex->Name = mProjectCubeMapName[i];
            tex->Path = mProjectCubeMapTextures[i];
            tex->CpuHandle = CPUDescriptor;
            tex->GpuHandle = GPUDescriptor;
            tex->type = TexturesType::CubeMap;
            tex->TexIndex = TextureIndex;
            TextureIndex++;
            ThrowIfFailed(DirectX::CreateDDSTextureFromFile12(md3dDevice.Get(),
                _forwardPassCmdList->GetInternal().Get(), tex->Path.c_str(),
                tex->Resource, tex->UploadHeap));

            mCubeMapTextures[tex->Name] = std::move(tex);
        }
    }

    GraphicsUtils::BuildTextureCubeSrvDesc(md3dDevice, mCbvSrvUavDescriptorSize, CPUDescriptor, GPUDescriptor, mCubeMapTextures, "Sky_diffuseIBL");
    GraphicsUtils::BuildTextureCubeSrvDesc(md3dDevice, mCbvSrvUavDescriptorSize, CPUDescriptor, GPUDescriptor, mCubeMapTextures, "Sky_specularIBL");

    mRenderTexture->BuildDescriptors(
        CPUDescriptor,
        GPUDescriptor,
        mCbvSrvUavDescriptorSize
    );

    mShadowMap->BuildDescriptors(
        CPUDescriptor, 
        GPUDescriptor,
        GetDsv(1), 
        mCbvSrvUavDescriptorSize
    );

    mDepthPass->BuildDescriptors(
        mDepthStencilBuffer.Get(),
        CPUDescriptor,
        GPUDescriptor,
        mCbvSrvUavDescriptorSize
    );

    //mGBufferPass->BuildDescriptors(
    //	CPUDescriptor,
    //	GPUDescriptor,
    //	GetRtv(SwapChainBufferCount),
    //	mCbvSrvUavDescriptorSize,
    //	mRtvDescriptorSize
    //);
}


void GraphicsCore::BuildShadersAndInputLayout()
{
    mShaderManager.Build();
}

void GraphicsCore::BuildPSOs()
{
    // 	PSO for Line.
    PipelineStateObject LineObject = PipelineStateObject();
    LineObject.BuildDefault(mShaderManager,mRootSignature);
    LineObject.SetShader(mShaderManager, mRootSignature, "lineVS", "linePS");
    LineObject.SetRasterizerState(D3D12_FILL_MODE_SOLID, D3D12_CULL_MODE_NONE);
    LineObject.SetPrimitiveTopologyType(D3D12_PRIMITIVE_TOPOLOGY_TYPE_LINE);
    ThrowIfFailed(md3dDevice->CreateGraphicsPipelineState(LineObject.GetPSODesc(), IID_PPV_ARGS(&mPSOs["Line"])));

    // 	PSO for Gizmo.
    PipelineStateObject GizmoObject = PipelineStateObject();
    GizmoObject.BuildDefault(mShaderManager, mRootSignature);
    GizmoObject.SetShader(mShaderManager, mRootSignature, "gizmoVS", "gizmoPS");
    GizmoObject.SetDefaultBlend();
    ThrowIfFailed(md3dDevice->CreateGraphicsPipelineState(GizmoObject.GetPSODesc(), IID_PPV_ARGS(&mPSOs["Gizmo"])));

    //PSO for Unlit.
    PipelineStateObject AxisObject = PipelineStateObject();
    AxisObject.BuildDefault(mShaderManager, mRootSignature);
    AxisObject.SetShader(mShaderManager, mRootSignature, "unlitVS", "unlitPS");
    AxisObject.SetDefaultBlend();
    AxisObject.SetDepthStencilState(true, D3D12_DEPTH_WRITE_MASK_ALL, D3D12_COMPARISON_FUNC_ALWAYS);
    ThrowIfFailed(md3dDevice->CreateGraphicsPipelineState(AxisObject.GetPSODesc(), IID_PPV_ARGS(&mPSOs["Axis"])));

    //	PSO for GBuffer
    PipelineStateObject GBufferObject = PipelineStateObject();
    GBufferObject.BuildDefault(mShaderManager, mRootSignature);
    GBufferObject.SetShader(mShaderManager, mRootSignature, "gbufferVS", "gbufferPS");
    ThrowIfFailed(md3dDevice->CreateGraphicsPipelineState(GBufferObject.GetPSODesc(), IID_PPV_ARGS(&mPSOs["GBuffer"])));

    // PSO for Lit.
    PipelineStateObject LitObject = PipelineStateObject();
    LitObject.BuildDefault(mShaderManager, mRootSignature);
    ThrowIfFailed(md3dDevice->CreateGraphicsPipelineState(LitObject.GetPSODesc(), IID_PPV_ARGS(&mPSOs["litOpaque"])));

    // 	PSO for Lit. (Transparent)
    PipelineStateObject LitTransparentObject = PipelineStateObject();
    LitTransparentObject.BuildDefault(mShaderManager, mRootSignature);
    LitTransparentObject.SetDefaultBlend();
    ThrowIfFailed(md3dDevice->CreateGraphicsPipelineState(LitTransparentObject.GetPSODesc(), IID_PPV_ARGS(&mPSOs["transparent"])));

    // PSO for shadow map pass.
    PipelineStateObject SmapObject = PipelineStateObject();
    SmapObject.Build(mShaderManager,mRootSignature,"shadowVS","shadowOpaquePS",0,DXGI_FORMAT_UNKNOWN);
    SmapObject.SetRasterizerDepth(100000, 0.0f, 1.0f);
    ThrowIfFailed(md3dDevice->CreateGraphicsPipelineState(SmapObject.GetPSODesc(), IID_PPV_ARGS(&mPSOs["shadow_opaque"])));

    // PSO for debug layer.
    PipelineStateObject DebugObject = PipelineStateObject();
    DebugObject.BuildDefault(mShaderManager, mRootSignature);
    DebugObject.SetShader(mShaderManager, mRootSignature, "debugVS", "debugPS");
    ThrowIfFailed(md3dDevice->CreateGraphicsPipelineState(DebugObject.GetPSODesc(), IID_PPV_ARGS(&mPSOs["debug"])));

    // 	PSO for FinalBlit
    PipelineStateObject FinalBlitObject = PipelineStateObject();
    FinalBlitObject.BuildDefault(mShaderManager, mRootSignature);
    FinalBlitObject.SetShader(mShaderManager, mRootSignature, "finalBlitVS", "finalBlitPS");
    ThrowIfFailed(md3dDevice->CreateGraphicsPipelineState(FinalBlitObject.GetPSODesc(), IID_PPV_ARGS(&mPSOs["FinalBlit"])));


    // 	PSO for CopyColor
    PipelineStateObject CopyColorObject = PipelineStateObject();
    CopyColorObject.BuildDefault(mShaderManager, mRootSignature);
    CopyColorObject.SetShader(mShaderManager, mRootSignature, "copyColorVS", "copyColorPS");
    ThrowIfFailed(md3dDevice->CreateGraphicsPipelineState(CopyColorObject.GetPSODesc(), IID_PPV_ARGS(&mPSOs["CopyColor"])));

    // 	PSO for RGBSplit
    PipelineStateObject RGBSplitObject = PipelineStateObject();
    RGBSplitObject.BuildDefault(mShaderManager, mRootSignature);
    RGBSplitObject.SetShader(mShaderManager, mRootSignature, "rgbSplitVS", "rgbSplitPS");
    ThrowIfFailed(md3dDevice->CreateGraphicsPipelineState(RGBSplitObject.GetPSODesc(), IID_PPV_ARGS(&mPSOs["RGBSplit"])));

    // 	PSO for RadialBlur
    PipelineStateObject RadialBlurObject = PipelineStateObject();
    RadialBlurObject.BuildDefault(mShaderManager, mRootSignature);
    RadialBlurObject.SetShader(mShaderManager, mRootSignature, "radialBlurVS", "radialBlurPS");
    ThrowIfFailed(md3dDevice->CreateGraphicsPipelineState(RadialBlurObject.GetPSODesc(), IID_PPV_ARGS(&mPSOs["RadialBlur"])));

    // 	PSO for Vignette
    PipelineStateObject VignetteObject = PipelineStateObject();
    VignetteObject.BuildDefault(mShaderManager, mRootSignature);
    VignetteObject.SetShader(mShaderManager, mRootSignature, "vignetteVS", "vignettePS");
    ThrowIfFailed(md3dDevice->CreateGraphicsPipelineState(VignetteObject.GetPSODesc(), IID_PPV_ARGS(&mPSOs["Vignette"])));

    // 	PSO for Decolor
    PipelineStateObject DecolorObject = PipelineStateObject();
    DecolorObject.BuildDefault(mShaderManager, mRootSignature);
    DecolorObject.SetShader(mShaderManager, mRootSignature, "decolorVS", "decolorPS");
    ThrowIfFailed(md3dDevice->CreateGraphicsPipelineState(DecolorObject.GetPSODesc(), IID_PPV_ARGS(&mPSOs["Decolor"])));

    // 	PSO for Brightness
    PipelineStateObject BrightnessObject = PipelineStateObject();
    BrightnessObject.BuildDefault(mShaderManager, mRootSignature);
    BrightnessObject.SetShader(mShaderManager, mRootSignature, "brightnessVS", "brightnessPS");
    ThrowIfFailed(md3dDevice->CreateGraphicsPipelineState(BrightnessObject.GetPSODesc(), IID_PPV_ARGS(&mPSOs["Brightness"])));

    // 	PSO for HSV
    PipelineStateObject HsvObject = PipelineStateObject();
    HsvObject.BuildDefault(mShaderManager, mRootSignature);
    HsvObject.SetShader(mShaderManager, mRootSignature, "hsvVS", "hsvPS");
    ThrowIfFailed(md3dDevice->CreateGraphicsPipelineState(HsvObject.GetPSODesc(), IID_PPV_ARGS(&mPSOs["HSV"])));

    // 	PSO for Mosaic
    PipelineStateObject MosaicObject = PipelineStateObject();
    MosaicObject.BuildDefault(mShaderManager, mRootSignature);
    MosaicObject.SetShader(mShaderManager, mRootSignature, "mosaicVS", "mosaicPS");
    ThrowIfFailed(md3dDevice->CreateGraphicsPipelineState(MosaicObject.GetPSODesc(), IID_PPV_ARGS(&mPSOs["Mosaic"])));

    // 	PSO for Sharpen
    PipelineStateObject SharpenObject = PipelineStateObject();
    SharpenObject.BuildDefault(mShaderManager, mRootSignature);
    SharpenObject.SetShader(mShaderManager, mRootSignature, "sharpenVS", "sharpenPS");
    ThrowIfFailed(md3dDevice->CreateGraphicsPipelineState(SharpenObject.GetPSODesc(), IID_PPV_ARGS(&mPSOs["Sharpen"])));

    // 	PSO for Spherize
    PipelineStateObject SpherizeObject = PipelineStateObject();
    SpherizeObject.BuildDefault(mShaderManager, mRootSignature);
    SpherizeObject.SetShader(mShaderManager, mRootSignature, "spherizeVS", "spherizePS");
    ThrowIfFailed(md3dDevice->CreateGraphicsPipelineState(SpherizeObject.GetPSODesc(), IID_PPV_ARGS(&mPSOs["Spherize"])));

    // 	PSO for WhiteBalance
    PipelineStateObject WhiteBalanceObject = PipelineStateObject();
    WhiteBalanceObject.BuildDefault(mShaderManager, mRootSignature);
    WhiteBalanceObject.SetShader(mShaderManager, mRootSignature, "whiteBalanceVS", "whiteBalancePS");
    ThrowIfFailed(md3dDevice->CreateGraphicsPipelineState(WhiteBalanceObject.GetPSODesc(), IID_PPV_ARGS(&mPSOs["WhiteBalance"])));

    // 	PSO for OilPaint
    PipelineStateObject OilPaintObject = PipelineStateObject();
    OilPaintObject.BuildDefault(mShaderManager, mRootSignature);
    OilPaintObject.SetShader(mShaderManager, mRootSignature, "oilPaintVS", "oilPaintPS");
    ThrowIfFailed(md3dDevice->CreateGraphicsPipelineState(OilPaintObject.GetPSODesc(), IID_PPV_ARGS(&mPSOs["OilPaint"])));

    // 	PSO for Relief
    PipelineStateObject ReliefObject = PipelineStateObject();
    ReliefObject.BuildDefault(mShaderManager, mRootSignature);
    ReliefObject.SetShader(mShaderManager, mRootSignature, "reliefVS", "reliefPS");
    ThrowIfFailed(md3dDevice->CreateGraphicsPipelineState(ReliefObject.GetPSODesc(), IID_PPV_ARGS(&mPSOs["Relief"])));

    // 	PSO for EdgeDetection
    PipelineStateObject EdgeDetectionObject = PipelineStateObject();
    EdgeDetectionObject.BuildDefault(mShaderManager, mRootSignature);
    EdgeDetectionObject.SetShader(mShaderManager, mRootSignature, "edgeDetectionVS", "edgeDetectionPS");
    ThrowIfFailed(md3dDevice->CreateGraphicsPipelineState(EdgeDetectionObject.GetPSODesc(), IID_PPV_ARGS(&mPSOs["EdgeDetection"])));

    // 	PSO for VolumeFog
    PipelineStateObject VolumeFogObject = PipelineStateObject();
    VolumeFogObject.BuildDefault(mShaderManager, mRootSignature);
    VolumeFogObject.SetShader(mShaderManager, mRootSignature, "volumeFogVS", "volumeFogPS");
    ThrowIfFailed(md3dDevice->CreateGraphicsPipelineState(VolumeFogObject.GetPSODesc(), IID_PPV_ARGS(&mPSOs["VolumeFog"]))); 

    // 	PSO for FxAA
    PipelineStateObject FxAAObject = PipelineStateObject();
    FxAAObject.BuildDefault(mShaderManager, mRootSignature);
    FxAAObject.SetShader(mShaderManager, mRootSignature, "FastApproximateAntialiasingVS", "FastApproximateAntialiasingPS");
    ThrowIfFailed(md3dDevice->CreateGraphicsPipelineState(FxAAObject.GetPSODesc(), IID_PPV_ARGS(&mPSOs["FxAA"])));

    // PSO for sky.
    PipelineStateObject SkyBoxObject = PipelineStateObject();
    SkyBoxObject.BuildDefault(mShaderManager, mRootSignature);
    SkyBoxObject.SetShader(mShaderManager, mRootSignature, "skyVS", "skyPS");
    SkyBoxObject.SetDepthStencilState(true, D3D12_DEPTH_WRITE_MASK_ALL, D3D12_COMPARISON_FUNC_LESS_EQUAL);
    SkyBoxObject.SetRasterizerState(D3D12_FILL_MODE_SOLID, D3D12_CULL_MODE_NONE);
    ThrowIfFailed(md3dDevice->CreateGraphicsPipelineState(SkyBoxObject.GetPSODesc(), IID_PPV_ARGS(&mPSOs["sky"])));
}

void GraphicsCore::BuildFrameResources()
{
    for (int i = 0; i < gNumFrameResources; ++i)
    {
        mFrameResources.push_back(
            std::make_unique<FrameResource>(md3dDevice.Get(),
            2, 
            (UINT)mAllRitems.size(),
            (UINT)(mGizmoManager.getInstance().mRenderObjects.size()) +
            (UINT)(mSceneManager.getInstance().mRenderObjects.size()) +
            (UINT)(mPostProcessManager.getInstance().mRenderObjects.size())
            )
        );
    }
}

void GraphicsCore::BuildRenderItems()
{
    mGizmoManager.getInstance().BuildRenderItem(mRitemLayer, mAllRitems, md3dDevice, _forwardPassCmdList->GetInternal());
    mSceneManager.getInstance().BuildRenderItem(mRitemLayer, mAllRitems, md3dDevice, _forwardPassCmdList->GetInternal(), mGizmoManager.getInstance().mRenderObjects.size(), matCBIndexUtils);
    mPostProcessManager.getInstance().BuildRenderItem(mRitemLayer, mAllRitems, md3dDevice, _forwardPassCmdList->GetInternal(), mGizmoManager.getInstance().mRenderObjects.size() + mSceneManager.getInstance().mRenderObjects.size(), matCBIndexUtils);
}
