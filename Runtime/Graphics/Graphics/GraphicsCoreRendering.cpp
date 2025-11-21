#include "GraphicsCore.h"

using Microsoft::WRL::ComPtr;
using namespace DirectX;
using namespace DirectX::PackedVector;

void GraphicsCore::OnMouseMoveAndSelect(WPARAM btnState, int x, int y)
{
    if ((btnState & MK_LBUTTON) != 0)
    {
        XMFLOAT4X4 P = mCamera.getInstance().GetProj4x4f();

        // Compute picking ray in view space.
        float vx = (+2.0f * x / mClientWidth - 1.0f) / P(0, 0);
        float vy = (-2.0f * y / mClientHeight + 1.0f) / P(1, 1);

        // Ray definition in view space.
        XMVECTOR rayOrigin = XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);
        XMVECTOR rayDir = XMVectorSet(vx, vy, 1.0f, 0.0f);

        XMMATRIX V = mCamera.getInstance().GetView();
        XMMATRIX invView = XMMatrixInverse(&XMMatrixDeterminant(V), V);

        for (auto ri : mRitemLayer[(int)RenderLayer::Axis])
        {
            auto geo = ri->Geo;

            XMMATRIX W = XMLoadFloat4x4(&ri->World);
            XMMATRIX invWorld = XMMatrixInverse(&XMMatrixDeterminant(W), W);

            // Tranform ray to vi space of Mesh.
            XMMATRIX toLocal = XMMatrixMultiply(invView, invWorld);

            rayOrigin = XMVector3TransformCoord(rayOrigin, toLocal);
            rayDir = XMVector3TransformNormal(rayDir, toLocal);

            // Make the ray direction unit length for the intersection tests.
            rayDir = XMVector3Normalize(rayDir);

            float tmin = 1000.0f;
            if (ri->Bound.Intersects(rayOrigin, rayDir, tmin))
            {
                //float dx = XMConvertToRadians(0.25f * static_cast<float>(x - mLastMousePos.x));
                //float dy = XMConvertToRadians(0.25f * static_cast<float>(y - mLastMousePos.y));
            }
        }
    }
}

void GraphicsCore::DrawRenderItems(ID3D12GraphicsCommandList* cmdList, const std::vector<RenderItem*>& ritems)
{
    UINT objCBByteSize = DX12Utils::CalcConstantBufferByteSize(sizeof(ObjectConstants));

    auto objectCB = mCurrFrameResource->ObjectCB->Resource();

    for (size_t i = 0; i < ritems.size(); ++i)
    {
        auto ri = ritems[i];

        if (ri->Enable)
        {
            cmdList->IASetVertexBuffers(0, 1, &ri->Geo->VertexBufferView());
            cmdList->IASetIndexBuffer(&ri->Geo->IndexBufferView());
            cmdList->IASetPrimitiveTopology(ri->PrimitiveType);

            D3D12_GPU_VIRTUAL_ADDRESS objCBAddress = objectCB->GetGPUVirtualAddress() + ri->ObjCBIndex * objCBByteSize;

            cmdList->SetGraphicsRootConstantBufferView(0, objCBAddress);

            cmdList->DrawIndexedInstanced(ri->IndexCount, 1, ri->StartIndexLocation, ri->BaseVertexLocation, 0);
        }
    }
}

void GraphicsCore::DrawSceneToShadowMap()
{
    _forwardPassCmdList->GetInternal()->SetGraphicsRootDescriptorTable(5, mSrvDescriptorHeap.GetDescriptorHeap()->GetGPUDescriptorHandleForHeapStart());

    _forwardPassCmdList->GetInternal()->RSSetViewports(1, &mShadowMap->Viewport());
    _forwardPassCmdList->GetInternal()->RSSetScissorRects(1, &mShadowMap->ScissorRect());

    // Change to DEPTH_WRITE.
    _forwardPassCmdList->GetInternal()->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(mShadowMap->Resource(),
        D3D12_RESOURCE_STATE_GENERIC_READ, D3D12_RESOURCE_STATE_DEPTH_WRITE));

    // Clear the back buffer and depth buffer.
    _forwardPassCmdList->GetInternal()->ClearDepthStencilView(mShadowMap->Dsv(),
        D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.0f, 0, 0, nullptr);

    // Specify the buffers we are going to render to.
    _forwardPassCmdList->GetInternal()->OMSetRenderTargets(0, nullptr, false, &mShadowMap->Dsv());

    // Bind the pass constant buffer for the shadow map pass.
    UINT passCBByteSize = DX12Utils::CalcConstantBufferByteSize(sizeof(PassConstants));
    auto passCB = mCurrFrameResource->PassCB->Resource();
    D3D12_GPU_VIRTUAL_ADDRESS passCBAddress = passCB->GetGPUVirtualAddress() + 1 * passCBByteSize;
    _forwardPassCmdList->GetInternal()->SetGraphicsRootConstantBufferView(2, passCBAddress);

    _forwardPassCmdList->GetInternal()->SetPipelineState(mPSOs["shadow_opaque"].Get());
    DrawRenderItems(_forwardPassCmdList->GetInternal().Get(), mRitemLayer[(int)RenderLayer::Opaque]);

    // Change back to GENERIC_READ so we can read the texture in a shader.
    _forwardPassCmdList->GetInternal()->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(mShadowMap->Resource(),
        D3D12_RESOURCE_STATE_DEPTH_WRITE, D3D12_RESOURCE_STATE_GENERIC_READ));
}

CD3DX12_CPU_DESCRIPTOR_HANDLE GraphicsCore::GetDsv(int index)const
{
    auto dsv = CD3DX12_CPU_DESCRIPTOR_HANDLE(mDsvHeap->GetCPUDescriptorHandleForHeapStart());
    dsv.Offset(index, mDsvDescriptorSize);
    return dsv;
}

CD3DX12_CPU_DESCRIPTOR_HANDLE GraphicsCore::GetRtv(int index)const
{
    auto rtv = CD3DX12_CPU_DESCRIPTOR_HANDLE(mRtvHeap->GetCPUDescriptorHandleForHeapStart());
    rtv.Offset(index, mRtvDescriptorSize);
    return rtv;
}
