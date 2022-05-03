#include "EditorUtils.h"

//	显示图片纹理大小
const int size = 84;

//	纹理总数
const int SUM_COUNT = 18;
//	图标
const int ICON_START_INDEX = 0;
const int ICON_COUNT = 5;

//	场景纹理
const int TEXTURES_START_INDEX = 5;
const int TEXTURES_COUNT = 18;

//	纹理路径集合
const std::vector<std::string> mProjectTextures =
{
	//	ICON
		"Asset/Textures/ICON/Folder.png",
		"Asset/Textures/ICON/Mat.png",
		"Asset/Textures/ICON/Mesh.png",
		"Asset/Textures/ICON/Scene.png",
		"Asset/Textures/ICON/Back.png",

	//	TEXTURES
		"Asset/Textures/DefaultTextures/OakDiffuse.png",
		"Asset/Textures/DefaultTextures/OakNormal.png",

		"Asset/Textures/DefaultTextures/PlaneDiffuse.png",
		"Asset/Textures/DefaultTextures/PlaneNormal.png",

		"Asset/Textures/DefaultTextures/WoodenDiffuse.png",
		"Asset/Textures/DefaultTextures/WoodenNormal.png",

		"Asset/Textures/DefaultTextures/WoodenDiffuse2.png",
		"Asset/Textures/DefaultTextures/WoodenNormal2.png",

		"Asset/Textures/DefaultTextures/WoodenDiffuse3.png",
		"Asset/Textures/DefaultTextures/WoodenNormal3.png",

		"Asset/Textures/DefaultTextures/WoodenDiffuse4.png",
		"Asset/Textures/DefaultTextures/WoodenNormal4.png",

		"Asset/Textures/DefaultTextures/sampleLUT.png"
};

//	加载纹理
bool EditorUtils::LoadTextureFromFile(const char* filename, Microsoft::WRL::ComPtr<ID3D12Device> d3d_device, D3D12_CPU_DESCRIPTOR_HANDLE srv_cpu_handle, ID3D12Resource** out_tex_resource, int* out_width, int* out_height)
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

//	初始化Resource
void EditorUtils::InitProjectResources(Microsoft::WRL::ComPtr<ID3D12Device> md3dDevice, Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> mSrvHeap)
{
	D3D12_CPU_DESCRIPTOR_HANDLE my_texture_srv_cpu_handle = mSrvHeap->GetCPUDescriptorHandleForHeapStart();
	D3D12_GPU_DESCRIPTOR_HANDLE my_texture_srv_gpu_handle = mSrvHeap->GetGPUDescriptorHandleForHeapStart();

	for (size_t i = 0; i < SUM_COUNT; i++)
	{
		int my_image_width = 0;
		int my_image_height = 0;
		ID3D12Resource* my_texture = NULL;

		UINT handle_increment = md3dDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
		int descriptor_index = 1;
		my_texture_srv_cpu_handle.ptr += (handle_increment * descriptor_index);
		my_texture_srv_gpu_handle.ptr += (handle_increment * descriptor_index);

		// Load the texture from a file
		bool ret = EditorUtils::LoadTextureFromFile(mProjectTextures[i].c_str(), md3dDevice, my_texture_srv_cpu_handle, &my_texture, &my_image_width, &my_image_height);
		IM_ASSERT(ret);
	}
}

//	绘制资源文件夹
void EditorUtils::DrawProjetcFolder(Microsoft::WRL::ComPtr<ID3D12Device> md3dDevice, Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> mSrvHeap, bool* show_folder_panel, bool* show_Textures_panel, bool* show_Mesh_panel, bool* show_Materials_panel, bool* show_Scene_panel)
{
	UINT handle_increment = md3dDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	D3D12_CPU_DESCRIPTOR_HANDLE folder_srv_cpu_handle = mSrvHeap->GetCPUDescriptorHandleForHeapStart();
	D3D12_GPU_DESCRIPTOR_HANDLE folder_srv_gpu_handle = mSrvHeap->GetGPUDescriptorHandleForHeapStart();

	ImGui::Text("../Asset/");
	for (size_t i = ICON_START_INDEX; i < ICON_COUNT - 1; i++)
	{
		int descriptor_index = 1;
		folder_srv_cpu_handle.ptr += (handle_increment * descriptor_index);
		folder_srv_gpu_handle.ptr += (handle_increment * descriptor_index);

		if(ImGui::ImageButton((ImTextureID)folder_srv_gpu_handle.ptr, ImVec2(size, size)))
		{
			*show_folder_panel = false;
			*show_Textures_panel = true;
			*show_Mesh_panel = false;
			*show_Materials_panel = false;
			*show_Scene_panel = false;
		}
		ImGui::SameLine();
	}
}

//	绘制资源纹理
void EditorUtils::DrawProjectTextures(Microsoft::WRL::ComPtr<ID3D12Device> md3dDevice, Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> mSrvHeap, int width, bool* show_folder_panel, bool* show_Textures_panel, bool* show_Mesh_panel, bool* show_Materials_panel, bool* show_Scene_panel)
{
	UINT handle_increment = md3dDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

	//	从Back ICON开始
	D3D12_CPU_DESCRIPTOR_HANDLE texture_srv_cpu_handle = mSrvHeap->GetCPUDescriptorHandleForHeapStart();
	texture_srv_cpu_handle.ptr += handle_increment * (ICON_COUNT - 1);
	D3D12_GPU_DESCRIPTOR_HANDLE texture_srv_gpu_handle = mSrvHeap->GetGPUDescriptorHandleForHeapStart();
	texture_srv_gpu_handle.ptr += handle_increment * (ICON_COUNT - 1);


	ImGui::Text("../Asset/Textures");
	//	BACK ICON
	texture_srv_cpu_handle.ptr += (handle_increment);
	texture_srv_gpu_handle.ptr += (handle_increment);
	if (ImGui::ImageButton((ImTextureID)texture_srv_gpu_handle.ptr, ImVec2(size, size)))
	{
		*show_folder_panel = true;
		*show_Textures_panel = false;
		*show_Mesh_panel = false;
		*show_Materials_panel = false;
		*show_Scene_panel = false;
	}
	ImGui::SameLine();

	//	TEXTURES
	int frame_padding = 1;
	for (size_t i = TEXTURES_START_INDEX; i < TEXTURES_COUNT; i++)
	{
		int descriptor_index = 1; 
		texture_srv_cpu_handle.ptr += (handle_increment * descriptor_index);
		texture_srv_gpu_handle.ptr += (handle_increment * descriptor_index);

		ImGui::Image((ImTextureID)texture_srv_gpu_handle.ptr, ImVec2(size, size));

		if ((frame_padding * size) < width)
		{
			ImGui::SameLine();
			frame_padding++;
		}
		else
		{
			frame_padding = 0;
		}
	}
}



