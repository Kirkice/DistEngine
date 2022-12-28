#include "GUIUtils.h"

#define ICON_SIZE 84
/// <summary>
/// DrawProjetcFolder
/// </summary>
/// <param name="md3dDevice"></param>
/// <param name="mSrvHeap"></param>
/// <param name="Textures"></param>
/// <param name="show_folder_panel"></param>
/// <param name="show_Textures_panel"></param>
/// <param name="show_Mesh_panel"></param>
/// <param name="show_Materials_panel"></param>
/// <param name="show_Scene_panel"></param>
void GUIUtils::DrawProjetcFolder(
	Microsoft::WRL::ComPtr<ID3D12Device> md3dDevice,
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> mSrvHeap,
	std::unordered_map<std::string, std::unique_ptr<Texture2D>>& Textures,
	bool* show_folder_panel,
	bool* show_Textures_panel,
	bool* show_Mesh_panel,
	bool* show_Materials_panel,
	bool* show_Scene_panel
)
{
	ImGui::Text("../Assets/");

	//	点击纹理按钮
	if (ImGui::ImageButton((ImTextureID)Textures["FolderPic"]->GpuHandle.ptr, ImVec2(ICON_SIZE, ICON_SIZE)))
	{
		*show_folder_panel = false;
		*show_Textures_panel = true;
		*show_Mesh_panel = false;
		*show_Materials_panel = false;
		*show_Scene_panel = false;
	}
	ImGui::SameLine();

	//	点击Mesh按钮
	if (ImGui::ImageButton((ImTextureID)Textures["FolderMesh"]->GpuHandle.ptr, ImVec2(ICON_SIZE, ICON_SIZE)))
	{
		*show_folder_panel = false;
		*show_Textures_panel = false;
		*show_Mesh_panel = true;
		*show_Materials_panel = false;
		*show_Scene_panel = false;
	}
	ImGui::SameLine();

	//	点击Material按钮
	if (ImGui::ImageButton((ImTextureID)Textures["FolderMat"]->GpuHandle.ptr, ImVec2(ICON_SIZE, ICON_SIZE)))
	{
		*show_folder_panel = false;
		*show_Textures_panel = false;
		*show_Mesh_panel = false;
		*show_Materials_panel = true;
		*show_Scene_panel = false;
	}
	ImGui::SameLine();

	//	点击Scene按钮
	if (ImGui::ImageButton((ImTextureID)Textures["FolderScene"]->GpuHandle.ptr, ImVec2(ICON_SIZE, ICON_SIZE)))
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

/// <summary>
/// DrawProjectTextures
/// </summary>
/// <param name="md3dDevice"></param>
/// <param name="mSrvHeap"></param>
/// <param name="Textures"></param>
/// <param name="width"></param>
/// <param name="show_folder_panel"></param>
/// <param name="show_Textures_panel"></param>
/// <param name="show_Mesh_panel"></param>
/// <param name="show_Materials_panel"></param>
/// <param name="show_Scene_panel"></param>
void GUIUtils::DrawProjectTextures(
	Microsoft::WRL::ComPtr<ID3D12Device> md3dDevice,
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> mSrvHeap,
	std::unordered_map<std::string, std::unique_ptr<Texture2D>>& IconTextures,
	std::unordered_map<std::string, std::unique_ptr<Texture2D>>& ResTextures,
	int width,
	bool* show_folder_panel,
	bool* show_Textures_panel,
	bool* show_Mesh_panel,
	bool* show_Materials_panel,
	bool* show_Scene_panel
)
{
	ImGui::Text("../Assets/Textures/");

	//	BACK ICON
	if (ImGui::ImageButton((ImTextureID)IconTextures["Back"]->GpuHandle.ptr, ImVec2(ICON_SIZE, ICON_SIZE)))
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
	for (size_t i = 0; i < ResTextures.size(); i++)
	{
		ImGui::Image((ImTextureID)ResTextures[mProjectResourceName[i]]->GpuHandle.ptr, ImVec2(ICON_SIZE, ICON_SIZE));
		if ((frame_padding * (ICON_SIZE + 10)) < width)
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

/// <summary>
/// DrawProjectMesh
/// </summary>
/// <param name="md3dDevice"></param>
/// <param name="mSrvHeap"></param>
/// <param name="Textures"></param>
/// <param name="width"></param>
/// <param name="show_folder_panel"></param>
/// <param name="show_Textures_panel"></param>
/// <param name="show_Mesh_panel"></param>
/// <param name="show_Materials_panel"></param>
/// <param name="show_Scene_panel"></param>
void GUIUtils::DrawProjectMesh(
	Microsoft::WRL::ComPtr<ID3D12Device> md3dDevice,
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> mSrvHeap,
	std::unordered_map<std::string, std::unique_ptr<Texture2D>>& Textures,
	int width,
	bool* show_folder_panel,
	bool* show_Textures_panel,
	bool* show_Mesh_panel,
	bool* show_Materials_panel,
	bool* show_Scene_panel
)
{
	ImGui::Text("../Assets/Mesh/");

	//	BACK ICON
	if (ImGui::ImageButton((ImTextureID)Textures["Back"]->GpuHandle.ptr, ImVec2(ICON_SIZE, ICON_SIZE)))
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
		ImGui::Image((ImTextureID)Textures["Mesh"]->GpuHandle.ptr, ImVec2(ICON_SIZE, ICON_SIZE));
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

/// <summary>
/// DrawProjectMaterials
/// </summary>
/// <param name="md3dDevice"></param>
/// <param name="mSrvHeap"></param>
/// <param name="Textures"></param>
/// <param name="width"></param>
/// <param name="show_folder_panel"></param>
/// <param name="show_Textures_panel"></param>
/// <param name="show_Mesh_panel"></param>
/// <param name="show_Materials_panel"></param>
/// <param name="show_Scene_panel"></param>
void GUIUtils::DrawProjectMaterials(
	Microsoft::WRL::ComPtr<ID3D12Device> md3dDevice,
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> mSrvHeap,
	std::unordered_map<std::string, std::unique_ptr<Texture2D>>& Textures,
	int width,
	bool* show_folder_panel,
	bool* show_Textures_panel,
	bool* show_Mesh_panel,
	bool* show_Materials_panel,
	bool* show_Scene_panel
)
{
	ImGui::Text("../Assets/Materials/");

	//	BACK ICON
	if (ImGui::ImageButton((ImTextureID)Textures["Back"]->GpuHandle.ptr, ImVec2(ICON_SIZE, ICON_SIZE)))
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
		ImGui::Image((ImTextureID)Textures["Mat"]->GpuHandle.ptr, ImVec2(ICON_SIZE, ICON_SIZE));
	}

	ImGui::Text("                 ");
	ImGui::SameLine();
	ImGui::Text("             plane_mat");
	ImGui::SameLine();
	ImGui::Text("         wooden2");
}

/// <summary>
/// DrawProjectScene
/// </summary>
/// <param name="md3dDevice"></param>
/// <param name="mSrvHeap"></param>
/// <param name="Textures"></param>
/// <param name="width"></param>
/// <param name="show_folder_panel"></param>
/// <param name="show_Textures_panel"></param>
/// <param name="show_Mesh_panel"></param>
/// <param name="show_Materials_panel"></param>
/// <param name="show_Scene_panel"></param>
void GUIUtils::DrawProjectScene(
	Microsoft::WRL::ComPtr<ID3D12Device> md3dDevice,
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> mSrvHeap,
	std::unordered_map<std::string, std::unique_ptr<Texture2D>>& Textures,
	int width,
	bool* show_folder_panel,
	bool* show_Textures_panel,
	bool* show_Mesh_panel,
	bool* show_Materials_panel,
	bool* show_Scene_panel
)
{
	ImGui::Text("../Assets/Scene/");

	//	BACK ICON
	if (ImGui::ImageButton((ImTextureID)Textures["Back"]->GpuHandle.ptr, ImVec2(ICON_SIZE, ICON_SIZE)))
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
		ImGui::Image((ImTextureID)Textures["Scene"]->GpuHandle.ptr, ImVec2(ICON_SIZE, ICON_SIZE));
	}

	ImGui::Text("                 ");
	ImGui::SameLine();
	ImGui::Text("                Default");
}