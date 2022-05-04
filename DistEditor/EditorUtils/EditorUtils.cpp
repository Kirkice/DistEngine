#include "EditorUtils.h"
const int size = 84;
using namespace Dist;

//	绘制资源文件夹
void EditorUtils::DrawProjetcFolder(Microsoft::WRL::ComPtr<ID3D12Device> md3dDevice, Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> mSrvHeap, bool* show_folder_panel, bool* show_Textures_panel, bool* show_Mesh_panel, bool* show_Materials_panel, bool* show_Scene_panel)
{
	ImGui::Text("../Asset/");

	//	点击纹理按钮
	if (ImGui::ImageButton((ImTextureID)ResourceManager::mIconTextures["Folder"]->GpuHandle.ptr, ImVec2(size, size)))
	{
		*show_folder_panel = false;
		*show_Textures_panel = true;
		*show_Mesh_panel = false;
		*show_Materials_panel = false;
		*show_Scene_panel = false;
	}
	ImGui::SameLine();

	//	点击Mesh按钮
	if (ImGui::ImageButton((ImTextureID)ResourceManager::mIconTextures["Folder"]->GpuHandle.ptr, ImVec2(size, size)))
	{
		*show_folder_panel = false;
		*show_Textures_panel = false;
		*show_Mesh_panel = true;
		*show_Materials_panel = false;
		*show_Scene_panel = false;
	}
	ImGui::SameLine();

	//	点击Material按钮
	if (ImGui::ImageButton((ImTextureID)ResourceManager::mIconTextures["Folder"]->GpuHandle.ptr, ImVec2(size, size)))
	{
		*show_folder_panel = false;
		*show_Textures_panel = false;
		*show_Mesh_panel = false;
		*show_Materials_panel = true;
		*show_Scene_panel = false;
	}
	ImGui::SameLine();

	//	点击Scene按钮
	if (ImGui::ImageButton((ImTextureID)ResourceManager::mIconTextures["Folder"]->GpuHandle.ptr, ImVec2(size, size)))
	{
		*show_folder_panel = false;
		*show_Textures_panel = false;
		*show_Mesh_panel = false;
		*show_Materials_panel = false;
		*show_Scene_panel = true;
	}
	ImGui::SameLine();
}

//	绘制资源纹理
void EditorUtils::DrawProjectTextures(Microsoft::WRL::ComPtr<ID3D12Device> md3dDevice, Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> mSrvHeap, int width, bool* show_folder_panel, bool* show_Textures_panel, bool* show_Mesh_panel, bool* show_Materials_panel, bool* show_Scene_panel)
{
	ImGui::Text("../Asset/Textures");

	//	BACK ICON
	if (ImGui::ImageButton((ImTextureID)ResourceManager::mIconTextures["Back"]->GpuHandle.ptr, ImVec2(size, size)))
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
	for (size_t i = 0; i < ResourceManager::mResourcesTextures.size(); i++)
	{
		ImGui::Image((ImTextureID)ResourceManager::mResourcesTextures[mProjectResourceName[i]]->GpuHandle.ptr, ImVec2(size, size));
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


//	绘制资源Mesh
//void EditorUtils::DrawProjectMesh(Microsoft::WRL::ComPtr<ID3D12Device> md3dDevice, Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> mSrvHeap, int width, bool* show_folder_panel, bool* show_Textures_panel, bool* show_Mesh_panel, bool* show_Materials_panel, bool* show_Scene_panel)
//{
//	UINT handle_increment = md3dDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
//
//	//	从Back ICON开始
//	D3D12_CPU_DESCRIPTOR_HANDLE srv_cpu_handle = mSrvHeap->GetCPUDescriptorHandleForHeapStart();
//	srv_cpu_handle.ptr += handle_increment * (ICON_COUNT - 1);
//	D3D12_GPU_DESCRIPTOR_HANDLE srv_gpu_handle = mSrvHeap->GetGPUDescriptorHandleForHeapStart();
//	srv_gpu_handle.ptr += handle_increment * (ICON_COUNT - 1);
//
//
//	ImGui::Text("../Asset/Mesh");
//	//	BACK ICON
//	srv_cpu_handle.ptr += (handle_increment);
//	srv_gpu_handle.ptr += (handle_increment);
//	if (ImGui::ImageButton((ImTextureID)srv_gpu_handle.ptr, ImVec2(size, size)))
//	{
//		*show_folder_panel = true;
//		*show_Textures_panel = false;
//		*show_Mesh_panel = false;
//		*show_Materials_panel = false;
//		*show_Scene_panel = false;
//	}
//	ImGui::SameLine();
//
//
//	UINT mesh_increment = md3dDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
//	D3D12_CPU_DESCRIPTOR_HANDLE mesh_srv_cpu_handle = mSrvHeap->GetCPUDescriptorHandleForHeapStart();
//	D3D12_GPU_DESCRIPTOR_HANDLE mesh_srv_gpu_handle = mSrvHeap->GetGPUDescriptorHandleForHeapStart();
//	int descriptor_index = 2;
//	mesh_srv_cpu_handle.ptr += (mesh_increment * descriptor_index);
//	mesh_srv_gpu_handle.ptr += (mesh_increment * descriptor_index);
//
//	//	Mesh
//	int frame_padding = 1;
//	for (size_t i = 0; i < mProjectModels.size(); i++)
//	{
//		ImGui::Image((ImTextureID)mesh_srv_cpu_handle.ptr, ImVec2(size, size));
//
//		if ((frame_padding * size) < width)
//		{
//			ImGui::SameLine();
//			frame_padding++;
//		}
//		else
//		{
//			frame_padding = 0;
//		}
//	}
//}


