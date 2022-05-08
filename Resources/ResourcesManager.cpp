#include "ResourcesManager.h"

using Microsoft::WRL::ComPtr;
using namespace DirectX;
using namespace DirectX::PackedVector;

namespace Dist
{

	UINT ResourceManager::GetResourcesTextureEndIndex()
	{
		UINT CurrentTextureEndIndex = ResourcesTextureEndIndex;
		ResourcesTextureEndIndex = ResourcesTextureEndIndex + 1;
		return CurrentTextureEndIndex;
	}

	//	º”‘ÿŒ∆¿Ì
	bool ResourceManager::LoadTextureFromFile(const char* filename, Microsoft::WRL::ComPtr<ID3D12Device> d3d_device, D3D12_CPU_DESCRIPTOR_HANDLE srv_cpu_handle, ID3D12Resource** out_tex_resource, int* out_width, int* out_height)
	{
		// Load from disk into a raw RGBA buffer
		int image_width = 0;
		int image_height = 0;
		unsigned char* image_data = stbi_load(filename, &image_width, &image_height, NULL, 4);
		if (image_data == NULL)
			return false;

		// Create texture resource
		D3D12_HEAP_PROPERTIES props;
		memset(&props, 0, sizeof(D3D12_HEAP_PROPERTIES));
		props.Type = D3D12_HEAP_TYPE_DEFAULT;
		props.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
		props.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;

		D3D12_RESOURCE_DESC desc;
		ZeroMemory(&desc, sizeof(desc));
		desc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
		desc.Alignment = 0;
		desc.Width = image_width;
		desc.Height = image_height;
		desc.DepthOrArraySize = 1;
		desc.MipLevels = 1;
		desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		desc.SampleDesc.Count = 1;
		desc.SampleDesc.Quality = 0;
		desc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
		desc.Flags = D3D12_RESOURCE_FLAG_NONE;

		ID3D12Resource* pTexture = NULL;
		d3d_device->CreateCommittedResource(&props, D3D12_HEAP_FLAG_NONE, &desc,
			D3D12_RESOURCE_STATE_COPY_DEST, NULL, IID_PPV_ARGS(&pTexture));

		// Create a temporary upload resource to move the data in
		UINT uploadPitch = (image_width * 4 + D3D12_TEXTURE_DATA_PITCH_ALIGNMENT - 1u) & ~(D3D12_TEXTURE_DATA_PITCH_ALIGNMENT - 1u);
		UINT uploadSize = image_height * uploadPitch;
		desc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
		desc.Alignment = 0;
		desc.Width = uploadSize;
		desc.Height = 1;
		desc.DepthOrArraySize = 1;
		desc.MipLevels = 1;
		desc.Format = DXGI_FORMAT_UNKNOWN;
		desc.SampleDesc.Count = 1;
		desc.SampleDesc.Quality = 0;
		desc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
		desc.Flags = D3D12_RESOURCE_FLAG_NONE;

		props.Type = D3D12_HEAP_TYPE_UPLOAD;
		props.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
		props.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;

		ID3D12Resource* uploadBuffer = NULL;
		HRESULT hr = d3d_device->CreateCommittedResource(&props, D3D12_HEAP_FLAG_NONE, &desc,
			D3D12_RESOURCE_STATE_GENERIC_READ, NULL, IID_PPV_ARGS(&uploadBuffer));
		IM_ASSERT(SUCCEEDED(hr));

		// Write pixels into the upload resource
		void* mapped = NULL;
		D3D12_RANGE range = { 0, uploadSize };
		hr = uploadBuffer->Map(0, &range, &mapped);
		IM_ASSERT(SUCCEEDED(hr));
		for (int y = 0; y < image_height; y++)
			memcpy((void*)((uintptr_t)mapped + y * uploadPitch), image_data + y * image_width * 4, image_width * 4);
		uploadBuffer->Unmap(0, &range);

		// Copy the upload resource content into the real resource
		D3D12_TEXTURE_COPY_LOCATION srcLocation = {};
		srcLocation.pResource = uploadBuffer;
		srcLocation.Type = D3D12_TEXTURE_COPY_TYPE_PLACED_FOOTPRINT;
		srcLocation.PlacedFootprint.Footprint.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		srcLocation.PlacedFootprint.Footprint.Width = image_width;
		srcLocation.PlacedFootprint.Footprint.Height = image_height;
		srcLocation.PlacedFootprint.Footprint.Depth = 1;
		srcLocation.PlacedFootprint.Footprint.RowPitch = uploadPitch;

		D3D12_TEXTURE_COPY_LOCATION dstLocation = {};
		dstLocation.pResource = pTexture;
		dstLocation.Type = D3D12_TEXTURE_COPY_TYPE_SUBRESOURCE_INDEX;
		dstLocation.SubresourceIndex = 0;

		D3D12_RESOURCE_BARRIER barrier = {};
		barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
		barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
		barrier.Transition.pResource = pTexture;
		barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
		barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_COPY_DEST;
		barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;

		// Create a temporary command queue to do the copy with
		ID3D12Fence* fence = NULL;
		hr = d3d_device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&fence));
		IM_ASSERT(SUCCEEDED(hr));

		HANDLE event = CreateEvent(0, 0, 0, 0);
		IM_ASSERT(event != NULL);

		D3D12_COMMAND_QUEUE_DESC queueDesc = {};
		queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
		queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
		queueDesc.NodeMask = 1;

		ID3D12CommandQueue* cmdQueue = NULL;
		hr = d3d_device->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&cmdQueue));
		IM_ASSERT(SUCCEEDED(hr));

		ID3D12CommandAllocator* cmdAlloc = NULL;
		hr = d3d_device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&cmdAlloc));
		IM_ASSERT(SUCCEEDED(hr));

		ID3D12GraphicsCommandList* cmdList = NULL;
		hr = d3d_device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, cmdAlloc, NULL, IID_PPV_ARGS(&cmdList));
		IM_ASSERT(SUCCEEDED(hr));

		cmdList->CopyTextureRegion(&dstLocation, 0, 0, 0, &srcLocation, NULL);
		cmdList->ResourceBarrier(1, &barrier);

		hr = cmdList->Close();
		IM_ASSERT(SUCCEEDED(hr));

		// Execute the copy
		cmdQueue->ExecuteCommandLists(1, (ID3D12CommandList* const*)&cmdList);
		hr = cmdQueue->Signal(fence, 1);
		IM_ASSERT(SUCCEEDED(hr));

		// Wait for everything to complete
		fence->SetEventOnCompletion(1, event);
		WaitForSingleObject(event, INFINITE);

		// Tear down our temporary command queue and release the upload resource
		cmdList->Release();
		cmdAlloc->Release();
		cmdQueue->Release();
		CloseHandle(event);
		fence->Release();
		uploadBuffer->Release();

		// Create a shader resource view for the texture
		D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc;
		ZeroMemory(&srvDesc, sizeof(srvDesc));
		srvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
		srvDesc.Texture2D.MipLevels = desc.MipLevels;
		srvDesc.Texture2D.MostDetailedMip = 0;
		srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
		d3d_device->CreateShaderResourceView(pTexture, &srvDesc, srv_cpu_handle);

		// Return results
		*out_tex_resource = pTexture;
		*out_width = image_width;
		*out_height = image_height;
		stbi_image_free(image_data);

		return true;
	}

	//	º”‘ÿTexture2D
	void ResourceManager::LoadTexture2D(Microsoft::WRL::ComPtr<ID3D12Device> d3d_device, Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> mSrvHeap, Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> mCommandList)
	{
		//	√Ë ˆ∑˚æ‰±˙
		D3D12_CPU_DESCRIPTOR_HANDLE texture_srv_cpu_handle = mSrvHeap->GetCPUDescriptorHandleForHeapStart();
		D3D12_GPU_DESCRIPTOR_HANDLE texture_srv_gpu_handle = mSrvHeap->GetGPUDescriptorHandleForHeapStart();
		UINT handle_increment = d3d_device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

		UINT TextureIndex = 1;
		//Load Icon
		for (size_t i = 0; i < mProjectIconTextures.size(); i++)
		{
			//	øÌ∏ﬂ
			int image_width = 0;
			int image_height = 0;
			
			ID3D12Resource* texture = NULL;

			texture_srv_cpu_handle.ptr += handle_increment;
			texture_srv_gpu_handle.ptr += handle_increment;

			// Load the texture from a file
			bool ret = LoadTextureFromFile(mProjectIconTextures[i].c_str(), d3d_device, texture_srv_cpu_handle, &texture, &image_width, &image_height);

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
				tex->CpuHandle = texture_srv_cpu_handle;
				tex->GpuHandle = texture_srv_gpu_handle;
				tex->Name = mProjectIconName[i];
				tex->Path = mProjectIconTextures[i];
				mIconTextures[tex->Name] = std::move(tex);
			}
		}

		//Load Project
		for (size_t i = 0; i < mProjectResourceTextures.size(); i++)
		{
			//	øÌ∏ﬂ
			int image_width = 0;
			int image_height = 0;

			ID3D12Resource* texture = NULL;

			texture_srv_cpu_handle.ptr += handle_increment;
			texture_srv_gpu_handle.ptr += handle_increment;

			// Load the texture from a file
			bool ret = LoadTextureFromFile(mProjectResourceTextures[i].c_str(), d3d_device, texture_srv_cpu_handle, &texture, &image_width, &image_height);
			IM_ASSERT(ret);

			auto tex = std::make_unique<Texture2D>();
			tex->Width = image_width;
			tex->Height = image_height;
			tex->Bit = 32;
			tex->Size = (float)((image_width * image_height * 32) / 8388608);
			tex->type = TexturesType::Project;
			tex->TexIndex = TextureIndex;
			TextureIndex++;
			tex->CpuHandle = texture_srv_cpu_handle;
			tex->GpuHandle = texture_srv_gpu_handle;
			tex->Name = mProjectResourceName[i];
			tex->Path = mProjectResourceTextures[i];
			mResourcesTextures[tex->Name] = std::move(tex);
		}

		//Load Gizmo
		for (size_t i = 0; i < mProjectGizmoTextures.size(); i++)
		{
			//	øÌ∏ﬂ
			int image_width = 0;
			int image_height = 0;

			ID3D12Resource* texture = NULL;

			texture_srv_cpu_handle.ptr += handle_increment;
			texture_srv_gpu_handle.ptr += handle_increment;

			// Load the texture from a file
			bool ret = LoadTextureFromFile(mProjectGizmoTextures[i].c_str(), d3d_device, texture_srv_cpu_handle, &texture, &image_width, &image_height);
			IM_ASSERT(ret);

			auto tex = std::make_unique<Texture2D>();
			tex->Width = image_width;
			tex->Height = image_height;
			tex->Bit = 32;
			tex->Size = (float)((image_width * image_height * 32) / 8388608);
			tex->type = TexturesType::Gizom;
			tex->TexIndex = TextureIndex;
			TextureIndex++;
			tex->CpuHandle = texture_srv_cpu_handle;
			tex->GpuHandle = texture_srv_gpu_handle;
			tex->Name = mProjectGizmoName[i];
			tex->Path = mProjectGizmoTextures[i];
			mGizmosTextures[tex->Name] = std::move(tex);
		}

		//Load CubeMap
		for (size_t i = 0; i < mProjectCubeMapTextures.size(); i++)
		{
			if (mCubeMapTextures.find(mProjectCubeMapName[i]) == std::end(mCubeMapTextures))
			{
				auto tex = std::make_unique<TextureCube>();
				tex->Name = mProjectCubeMapName[i];
				tex->Path = mProjectCubeMapTextures[i];
				tex->CpuHandle = texture_srv_cpu_handle;
				tex->GpuHandle = texture_srv_gpu_handle;
				tex->type = TexturesType::CubeMap;
				tex->TexIndex = TextureIndex;
				TextureIndex++;
				ThrowIfFailed(DirectX::CreateDDSTextureFromFile12(d3d_device.Get(),
					mCommandList.Get(), tex->Path.c_str(),
					tex->Resource, tex->UploadHeap));

				mCubeMapTextures[tex->Name] = std::move(tex);
			}
		}


		texture_srv_cpu_handle.ptr += handle_increment;
		texture_srv_gpu_handle.ptr += handle_increment;

		ComPtr<ID3D12Resource> diffuseIBL = mCubeMapTextures["DGarden_diffuseIBL"]->Resource;

		D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
		srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
		srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
		srvDesc.Texture2D.MostDetailedMip = 0;
		srvDesc.Texture2D.ResourceMinLODClamp = 0.0f;

		srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURECUBE;
		srvDesc.TextureCube.MostDetailedMip = 0;
		srvDesc.TextureCube.MipLevels = diffuseIBL->GetDesc().MipLevels;
		srvDesc.TextureCube.ResourceMinLODClamp = 0.0f;
		srvDesc.Format = diffuseIBL->GetDesc().Format;
		d3d_device->CreateShaderResourceView(diffuseIBL.Get(), &srvDesc, texture_srv_cpu_handle);
		mCubeMapTextures["DGarden_diffuseIBL"]->CpuHandle = texture_srv_cpu_handle;
		mCubeMapTextures["DGarden_diffuseIBL"]->GpuHandle = texture_srv_gpu_handle;


		texture_srv_cpu_handle.ptr += handle_increment;
		texture_srv_gpu_handle.ptr += handle_increment;

		ComPtr<ID3D12Resource> specularIBL = mCubeMapTextures["DGarden_specularIBL"]->Resource;

		texture_srv_cpu_handle.ptr += handle_increment;
		texture_srv_gpu_handle.ptr += handle_increment;
		srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURECUBE;
		srvDesc.TextureCube.MostDetailedMip = 0;
		srvDesc.TextureCube.MipLevels = specularIBL->GetDesc().MipLevels;
		srvDesc.TextureCube.ResourceMinLODClamp = 0.0f;
		srvDesc.Format = specularIBL->GetDesc().Format;
		d3d_device->CreateShaderResourceView(specularIBL.Get(), &srvDesc, texture_srv_cpu_handle);
		mCubeMapTextures["DGarden_specularIBL"]->CpuHandle = texture_srv_cpu_handle;
		mCubeMapTextures["DGarden_specularIBL"]->GpuHandle = texture_srv_gpu_handle;

		ResourcesTextureEndIndex = TextureIndex;
	}
}