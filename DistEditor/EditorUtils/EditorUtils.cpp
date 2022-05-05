#include "EditorUtils.h"
const int size = 84;
using namespace Dist;

//	绘制资源文件夹
void EditorUtils::DrawProjetcFolder(Microsoft::WRL::ComPtr<ID3D12Device> md3dDevice, Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> mSrvHeap, ResourceManager& manager, bool* show_folder_panel, bool* show_Textures_panel, bool* show_Mesh_panel, bool* show_Materials_panel, bool* show_Scene_panel)
{
	ImGui::Text("../Assets/");

	//	点击纹理按钮
	if (ImGui::ImageButton((ImTextureID)manager.mIconTextures["FolderPic"]->GpuHandle.ptr, ImVec2(size, size)))
	{
		*show_folder_panel = false;
		*show_Textures_panel = true;
		*show_Mesh_panel = false;
		*show_Materials_panel = false;
		*show_Scene_panel = false;
	}
	ImGui::SameLine();

	//	点击Mesh按钮
	if (ImGui::ImageButton((ImTextureID)manager.mIconTextures["FolderMesh"]->GpuHandle.ptr, ImVec2(size, size)))
	{
		*show_folder_panel = false;
		*show_Textures_panel = false;
		*show_Mesh_panel = true;
		*show_Materials_panel = false;
		*show_Scene_panel = false;
	}
	ImGui::SameLine();

	//	点击Material按钮
	if (ImGui::ImageButton((ImTextureID)manager.mIconTextures["FolderMat"]->GpuHandle.ptr, ImVec2(size, size)))
	{
		*show_folder_panel = false;
		*show_Textures_panel = false;
		*show_Mesh_panel = false;
		*show_Materials_panel = true;
		*show_Scene_panel = false;
	}
	ImGui::SameLine();

	//	点击Scene按钮
	if (ImGui::ImageButton((ImTextureID)manager.mIconTextures["FolderScene"]->GpuHandle.ptr, ImVec2(size, size)))
	{
		*show_folder_panel = false;
		*show_Textures_panel = false;
		*show_Mesh_panel = false;
		*show_Materials_panel = false;
		*show_Scene_panel = true;
	}

	ImGui::Text("      Textures");
	ImGui::SameLine();
	ImGui::Text("              Meshs");
	ImGui::SameLine();
	ImGui::Text("              Materials");
	ImGui::SameLine();
	ImGui::Text("              Scenes");
}

//	绘制资源纹理
void EditorUtils::DrawProjectTextures(Microsoft::WRL::ComPtr<ID3D12Device> md3dDevice, Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> mSrvHeap, ResourceManager& manager, int width, bool* show_folder_panel, bool* show_Textures_panel, bool* show_Mesh_panel, bool* show_Materials_panel, bool* show_Scene_panel)
{
	ImGui::Text("../Assets/Textures/");

	//	BACK ICON
	if (ImGui::ImageButton((ImTextureID)manager.mIconTextures["Back"]->GpuHandle.ptr, ImVec2(size, size)))
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
	for (size_t i = 0; i < manager.mResourcesTextures.size(); i++)
	{
		ImGui::Image((ImTextureID)manager.mResourcesTextures[mProjectResourceName[i]]->GpuHandle.ptr, ImVec2(size, size));
		if ((frame_padding * (size + 10)) < width)
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
void EditorUtils::DrawProjectMesh(Microsoft::WRL::ComPtr<ID3D12Device> md3dDevice, Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> mSrvHeap, ResourceManager& manager, int width, bool* show_folder_panel, bool* show_Textures_panel, bool* show_Mesh_panel, bool* show_Materials_panel, bool* show_Scene_panel)
{
	ImGui::Text("../Assets/Mesh/");

	//	BACK ICON
	if (ImGui::ImageButton((ImTextureID)manager.mIconTextures["Back"]->GpuHandle.ptr, ImVec2(size, size)))
	{
		*show_folder_panel = true;
		*show_Textures_panel = false;
		*show_Mesh_panel = false;
		*show_Materials_panel = false;
		*show_Scene_panel = false;
	}

	//	Mesh
	int frame_padding = 1;
	for (size_t i = 0; i < mProjectModels.size(); i++)
	{
		ImGui::SameLine();
		ImGui::Image((ImTextureID)manager.mIconTextures["Mesh"]->GpuHandle.ptr, ImVec2(size, size));
	}

	ImGui::Text("                   ");
	ImGui::SameLine();
	ImGui::Text("             Bunny");
	ImGui::SameLine();
	ImGui::Text("             Dragon");
	ImGui::SameLine();
	ImGui::Text("             Sample");
	ImGui::SameLine();
	ImGui::Text("             sponza");

}

//	绘制资源Materials
void EditorUtils::DrawProjectMaterials(Microsoft::WRL::ComPtr<ID3D12Device> md3dDevice, Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> mSrvHeap, ResourceManager& manager, int width, bool* show_folder_panel, bool* show_Textures_panel, bool* show_Mesh_panel, bool* show_Materials_panel, bool* show_Scene_panel)
{
	ImGui::Text("../Assets/Materials/");

	//	BACK ICON
	if (ImGui::ImageButton((ImTextureID)manager.mIconTextures["Back"]->GpuHandle.ptr, ImVec2(size, size)))
	{
		*show_folder_panel = true;
		*show_Textures_panel = false;
		*show_Mesh_panel = false;
		*show_Materials_panel = false;
		*show_Scene_panel = false;
	}

	//	Materials
	int frame_padding = 1;
	for (size_t i = 0; i < mProjectMaterials.size(); i++)
	{
		ImGui::SameLine();
		ImGui::Image((ImTextureID)manager.mIconTextures["Mat"]->GpuHandle.ptr, ImVec2(size, size));
	}

	ImGui::Text("                 ");
	ImGui::SameLine();
	ImGui::Text("             plane_mat");
	ImGui::SameLine();
	ImGui::Text("         wooden2");
}

void EditorUtils::DrawProjectScene(Microsoft::WRL::ComPtr<ID3D12Device> md3dDevice, Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> mSrvHeap, ResourceManager& manager, int width, bool* show_folder_panel, bool* show_Textures_panel, bool* show_Mesh_panel, bool* show_Materials_panel, bool* show_Scene_panel)
{
	ImGui::Text("../Assets/Scene/");

	//	BACK ICON
	if (ImGui::ImageButton((ImTextureID)manager.mIconTextures["Back"]->GpuHandle.ptr, ImVec2(size, size)))
	{
		*show_folder_panel = true;
		*show_Textures_panel = false;
		*show_Mesh_panel = false;
		*show_Materials_panel = false;
		*show_Scene_panel = false;
	}
	ImGui::SameLine();

	//	Materials
	int frame_padding = 1;
	for (size_t i = 0; i < mProjectScenes.size(); i++)
	{
		ImGui::SameLine();
		ImGui::Image((ImTextureID)manager.mIconTextures["Scene"]->GpuHandle.ptr, ImVec2(size, size));
	}

	ImGui::Text("                 ");
	ImGui::SameLine();
	ImGui::Text("                Default");
}