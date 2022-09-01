#include "FrameResource.h"

FrameResource::FrameResource(ID3D12Device* device, UINT passCount, UINT objectCount, UINT materialCount)
{
    ThrowIfFailed(device->CreateCommandAllocator(
        D3D12_COMMAND_LIST_TYPE_DIRECT,
		IID_PPV_ARGS(CmdListAlloc.GetAddressOf()))); 

    PassCB = std::make_unique<UploadBuffer<PassConstants>>(device, passCount, true);
	PBRMaterialBuffer = std::make_unique<UploadBuffer<PBRMaterialData>>(device, materialCount, false);
    SkyBoxMaterialBuffer = std::make_unique<UploadBuffer<SkyBoxMaterialData>>(device, materialCount, false);
    PostMaterialBuffer = std::make_unique<UploadBuffer<PostprocessingData>>(device, materialCount, false);
    ObjectCB = std::make_unique<UploadBuffer<ObjectConstants>>(device, objectCount, true);
}

FrameResource::~FrameResource()
{

}