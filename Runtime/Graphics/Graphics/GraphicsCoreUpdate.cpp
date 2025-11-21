#include "GraphicsCore.h"

using Microsoft::WRL::ComPtr;
using namespace DirectX;
using namespace DirectX::PackedVector;

void GraphicsCore::Update(const GameTimer& gt)
{
    OnKeyboardInput(gt);
    mCamera.getInstance().SetLens((mSceneManager.getInstance().mCameraSetting.mCamFov / 180) * Mathf::Pi, AspectRatio(), mSceneManager.getInstance().mCameraSetting.mCamClipN, mSceneManager.getInstance().mCameraSetting.mCamClipF);

    // Cycle through the circular frame resource array.
    mCurrFrameResourceIndex = (mCurrFrameResourceIndex + 1) % gNumFrameResources;
    mCurrFrameResource = mFrameResources[mCurrFrameResourceIndex].get();

    // Has the GPU finished processing the commands of the current frame resource?
    // If not, wait until the GPU has completed commands up to this fence point.
    if (mCurrFrameResource->Fence != 0 && mFence->GetCompletedValue() < mCurrFrameResource->Fence)
    {
        HANDLE eventHandle = CreateEventEx(nullptr, false, false, EVENT_ALL_ACCESS);
        ThrowIfFailed(mFence->SetEventOnCompletion(mCurrFrameResource->Fence, eventHandle));
        WaitForSingleObject(eventHandle, INFINITE);
        CloseHandle(eventHandle);
    }

    UpdateLights(gt);
    UpdateObjectCBs(gt);
    UpdateMaterialBuffer(gt);
    UpdateShadowTransform(gt);
    UpdateMainPassCB(gt);
    UpdateShadowPassCB(gt);
    //OnTestMove(gt);
}


void GraphicsCore::UpdateLights(const GameTimer& gt)
{
    mSceneManager.getInstance().MainLight->GetComponent<Transform>(1)->Tick(gt);
}

void GraphicsCore::UpdateObjectCBs(const GameTimer& gt)
{
    mSceneManager.getInstance().UpdateObjectBuffer(mAllRitems, mGizmoManager.getInstance().mRenderObjects.size());


    XMMATRIX view = mCamera.getInstance().GetView();			//WorldToView的变换矩阵
    auto viewDeterminant = XMMatrixDeterminant(view);
    XMMATRIX invView = XMMatrixInverse(&viewDeterminant, view);		//ViewToWorld的变换矩阵 

    auto currObjectCB = mCurrFrameResource->ObjectCB.get();

    for (auto& e : mAllRitems)
    { 
        XMFLOAT4X4* eWorldMatrix = &e->World;

        XMMATRIX world = XMLoadFloat4x4(eWorldMatrix);
        XMMATRIX InvWorld = XMMatrixInverse(&XMMatrixDeterminant(world), world);
        XMMATRIX texTransform = XMLoadFloat4x4(&e->TexTransform);
        
        //	视锥剔除
        XMMATRIX viewToLocal = XMMatrixMultiply(invView, InvWorld);
        //	创建视锥体
        BoundingFrustum localSpaceFrustum;
        localSpaceFrustum.CreateFromMatrix(localSpaceFrustum, mCamera.GetProj());
        //	将视锥体从观察空间变换到局部空间
        localSpaceFrustum.Transform(localSpaceFrustum, viewToLocal);

        bool enable = e->Enable;
        if ((localSpaceFrustum.Contains(e->Bound) != DirectX::DISJOINT) && enable)
        {
            ObjectConstants objConstants;
            XMStoreFloat4x4(&objConstants.World, XMMatrixTranspose(world));
            XMStoreFloat4x4(&objConstants.InvWorld, XMMatrixTranspose(InvWorld));
            XMStoreFloat4x4(&objConstants.TexTransform, XMMatrixTranspose(texTransform));
            objConstants.MaterialIndex = e->Mat->MatCBIndex;

            currObjectCB->CopyData(e->ObjCBIndex, objConstants);
        }
    }
}

void GraphicsCore::UpdateMaterialBuffer(const GameTimer& gt)
{
    auto PBRMaterialBuffer = mCurrFrameResource->PBRMaterialBuffer.get();
    auto SkyBoxMaterialBuffer = mCurrFrameResource->SkyBoxMaterialBuffer.get();
    auto PostProcessBuffer = mCurrFrameResource->PostMaterialBuffer.get();

    //	更新GizmosBuffer
    mGizmoManager.getInstance().UpdateSceneMaterialBuffer(PBRMaterialBuffer);

    //	更新场景材质Buffer
    mSceneManager.getInstance().UpdateSceneMaterialBuffer(PBRMaterialBuffer, SkyBoxMaterialBuffer, matCBIndexUtils);

    //	更新后处理材质Buffer
    mPostProcessManager.getInstance().UpdateMaterialBuffer(PostProcessBuffer, matCBIndexUtils);
}

   

void GraphicsCore::UpdateShadowTransform(const GameTimer& gt)
{
    // Only the first "main" light casts a shadow.
    XMVECTOR lightDir = mSceneManager.getInstance().MainLight->GetComponent<Transform>(1)->forward.ToSIMD();
    XMVECTOR lightPos = -2.0f * mSceneBounds.Radius * lightDir;
    XMVECTOR targetPos = XMLoadFloat3(&mSceneBounds.Center);
    XMVECTOR lightUp = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
    XMMATRIX lightView = XMMatrixLookAtLH(lightPos, targetPos, lightUp);

    XMStoreFloat3(&mLightPosW, lightPos);

    // Transform bounding sphere to light space.
    XMFLOAT3 sphereCenterLS;
    XMStoreFloat3(&sphereCenterLS, XMVector3TransformCoord(targetPos, lightView));

    // Ortho frustum in light space encloses scene.  
    float l = sphereCenterLS.x - mSceneBounds.Radius;
    float b = sphereCenterLS.y - mSceneBounds.Radius;
    float n = sphereCenterLS.z - mSceneBounds.Radius;
    float r = sphereCenterLS.x + mSceneBounds.Radius;
    float t = sphereCenterLS.y + mSceneBounds.Radius;
    float f = sphereCenterLS.z + mSceneBounds.Radius;

    mLightNearZ = n;
    mLightFarZ = f;
    XMMATRIX lightProj = XMMatrixOrthographicOffCenterLH(l, r, b, t, n, f);

    // Transform NDC space [-1,+1]^2 to texture space [0,1]^2
    XMMATRIX T(
        0.5f, 0.0f, 0.0f, 0.0f,
        0.0f, -0.5f, 0.0f, 0.0f,
        0.0f, 0.0f, 1.0f, 0.0f,
        0.5f, 0.5f, 0.0f, 1.0f);

    XMMATRIX S = lightView * lightProj * T;
    XMStoreFloat4x4(&mLightView, lightView);
    XMStoreFloat4x4(&mLightProj, lightProj);
    XMStoreFloat4x4(&mShadowTransform, S);
}

void GraphicsCore::UpdateMainPassCB(const GameTimer& gt)
{
    XMMATRIX view = mCamera.getInstance().GetView();
    XMMATRIX proj = mCamera.getInstance().GetProj();

    XMMATRIX viewProj = XMMatrixMultiply(view, proj);
    XMMATRIX invView = XMMatrixInverse(&XMMatrixDeterminant(view), view);
    XMMATRIX invProj = XMMatrixInverse(&XMMatrixDeterminant(proj), proj);
    XMMATRIX invViewProj = XMMatrixInverse(&XMMatrixDeterminant(viewProj), viewProj);

    // Transform NDC space [-1,+1]^2 to texture space [0,1]^2
    XMMATRIX T(
        0.5f, 0.0f, 0.0f, 0.0f, 
        0.0f, -0.5f, 0.0f, 0.0f,
        0.0f, 0.0f, 1.0f, 0.0f,
        0.5f, 0.5f, 0.0f, 1.0f);

    XMMATRIX viewProjTex = XMMatrixMultiply(viewProj, T);
    XMMATRIX shadowTransform = XMLoadFloat4x4(&mShadowTransform);

    XMStoreFloat4x4(&mMainPassCB.View, XMMatrixTranspose(view));
    XMStoreFloat4x4(&mMainPassCB.InvView, XMMatrixTranspose(invView));
    XMStoreFloat4x4(&mMainPassCB.Proj, XMMatrixTranspose(proj));
    XMStoreFloat4x4(&mMainPassCB.InvProj, XMMatrixTranspose(invProj));
    XMStoreFloat4x4(&mMainPassCB.ViewProj, XMMatrixTranspose(viewProj));
    XMStoreFloat4x4(&mMainPassCB.InvViewProj, XMMatrixTranspose(invViewProj));
    XMStoreFloat4x4(&mMainPassCB.ViewProjTex, XMMatrixTranspose(viewProjTex));
    XMStoreFloat4x4(&mMainPassCB.ShadowTransform, XMMatrixTranspose(shadowTransform));
    mCamera.getInstance().tick(gt);
    mMainPassCB.EyePosW = mCamera.getInstance().position;
    mMainPassCB.RenderTargetSize = Vector2((float)mClientWidth, (float)mClientHeight);
    mMainPassCB.InvRenderTargetSize = Vector2(1.0f / mClientWidth, 1.0f / mClientHeight);
    mMainPassCB.NearZ = 1.0f;
    mMainPassCB.FarZ = 1000.0f;
    mMainPassCB.TotalTime = gt.TotalTime();
    mMainPassCB.DeltaTime = gt.DeltaTime();

    //Fog
    mMainPassCB.FogColor = mFogSettings.FogColor;
    mMainPassCB.LinearFogParam = Vector4(mFogSettings.fogStrat, mFogSettings.fogEnd, mFogSettings.FogDensity, mFogSettings.EnableHeightFog);
    mMainPassCB.HeightFogParam = Vector4(mFogSettings.FogFeather,mFogSettings.FogStep,mFogSettings.HeightMin,mFogSettings.HeightMax);
    mMainPassCB.VolumeFogParam0 = Vector4(mFogSettings.CubePos, mFogSettings.NoiseStrength);
    mMainPassCB.VolumeFogParam1 = Vector4(mFogSettings.CubeScale);
    mMainPassCB.VolumeFogParam2 = Vector4(mFogSettings.CameraDir);

    //Scattering
    mMainPassCB.Parames01 = Vector4(mSceneManager.getInstance().mSkyBoxSetting.EnableScatteringSky, mSceneManager.getInstance().mSkyBoxSetting.SunHeight, mSceneManager.getInstance().mSkyBoxSetting.ZenithOffset, mSceneManager.getInstance().mSkyBoxSetting.Density);
    mMainPassCB.Parames02 = Vector4(mSceneManager.getInstance().mSkyBoxSetting.MultiScatterPhase, mSceneManager.getInstance().mSkyBoxSetting.AnisotropicIntensity, 0, 0);
    mMainPassCB.SkyColor = mSceneManager.getInstance().mSkyBoxSetting.SkyColor;

    //AntialiasingSettings
    mMainPassCB.FxaaParames = Vector4(mAntialiasingSettings.AbsoluteLumaThreshold, mAntialiasingSettings.RelativeLumaThreshold, mAntialiasingSettings.ConsoleCharpness, mAntialiasingSettings.DebugMode);

    //Light
    mMainPassCB.DirectionLights.Direction = mSceneManager.getInstance().MainLight->GetComponent<Transform>(1)->forward;
    mMainPassCB.DirectionLights.Strength = mSceneManager.getInstance().MainLight->GetComponent<DirectionLight>(2)->intensity;
    mMainPassCB.DirectionLights.Color = Vector3(mSceneManager.getInstance().MainLight->GetComponent<DirectionLight>(2)->color);
    mMainPassCB.DirectionLights.CastShadow = 1;
    mMainPassCB.DirectionLights.Position = mSceneManager.getInstance().MainLight->GetComponent<Transform>(1)->position;
    bool lightEnable = mSceneManager.getInstance().MainLight->Enable;
    mMainPassCB.DirectionLights.Active = (int)mSceneManager.getInstance().MainLight->Enable;

    auto currPassCB = mCurrFrameResource->PassCB.get();
    currPassCB->CopyData(0, mMainPassCB);
}

void GraphicsCore::UpdateShadowPassCB(const GameTimer& gt)
{
    XMMATRIX view = XMLoadFloat4x4(&mLightView);
    XMMATRIX proj = XMLoadFloat4x4(&mLightProj);

    XMMATRIX viewProj = XMMatrixMultiply(view, proj);
    XMMATRIX invView = XMMatrixInverse(&XMMatrixDeterminant(view), view);
    XMMATRIX invProj = XMMatrixInverse(&XMMatrixDeterminant(proj), proj);
    XMMATRIX invViewProj = XMMatrixInverse(&XMMatrixDeterminant(viewProj), viewProj);

    UINT w = mShadowMap->Width();
    UINT h = mShadowMap->Height();

    XMStoreFloat4x4(&mShadowPassCB.View, XMMatrixTranspose(view));
    XMStoreFloat4x4(&mShadowPassCB.InvView, XMMatrixTranspose(invView));
    XMStoreFloat4x4(&mShadowPassCB.Proj, XMMatrixTranspose(proj));
    XMStoreFloat4x4(&mShadowPassCB.InvProj, XMMatrixTranspose(invProj));
    XMStoreFloat4x4(&mShadowPassCB.ViewProj, XMMatrixTranspose(viewProj));
    XMStoreFloat4x4(&mShadowPassCB.InvViewProj, XMMatrixTranspose(invViewProj));
    mShadowPassCB.EyePosW = Vector3(mLightPosW);
    mShadowPassCB.RenderTargetSize = Vector2((float)w, (float)h);
    mShadowPassCB.InvRenderTargetSize = Vector2(1.0f / w, 1.0f / h);
    mShadowPassCB.NearZ = mLightNearZ;
    mShadowPassCB.FarZ = mLightFarZ;

    auto currPassCB = mCurrFrameResource->PassCB.get();
    currPassCB->CopyData(1, mShadowPassCB); 
}

void GraphicsCore::UpdateRenderItems(const GameTimer& gt)
{

}
