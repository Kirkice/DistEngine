#pragma once

#include <string>
#include "../../Resources/ResourcesManager.h"
#include "imgui.h"
#include "imgui_impl_win32.h"
#include "imgui_impl_dx12.h"
#include "../../System/EngineSystem/SystemUtils.h"

using namespace Dist;


class EditorUtils
{
public:

	static void DrawProjetcFolder(Microsoft::WRL::ComPtr<ID3D12Device> md3dDevice, Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> mSrvHeap, ResourceManager& manager, bool* show_folder_panel, bool* show_Textures_panel, bool* show_Mesh_panel, bool* show_Materials_panel, bool* show_Scene_panel);

	static void DrawProjectTextures(Microsoft::WRL::ComPtr<ID3D12Device> md3dDevice, Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> mSrvHeap, ResourceManager& manager, int width, bool* show_folder_panel, bool* show_Textures_panel, bool* show_Mesh_panel, bool* show_Materials_panel, bool* show_Scene_panel);

	static void DrawProjectMesh(Microsoft::WRL::ComPtr<ID3D12Device> md3dDevice, Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> mSrvHeap, ResourceManager& manager, int width, bool* show_folder_panel, bool* show_Textures_panel, bool* show_Mesh_panel, bool* show_Materials_panel, bool* show_Scene_panel);

	static void DrawProjectMaterials(Microsoft::WRL::ComPtr<ID3D12Device> md3dDevice, Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> mSrvHeap, ResourceManager& manager, int width, bool* show_folder_panel, bool* show_Textures_panel, bool* show_Mesh_panel, bool* show_Materials_panel, bool* show_Scene_panel);

	static void DrawProjectScene(Microsoft::WRL::ComPtr<ID3D12Device> md3dDevice, Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> mSrvHeap, ResourceManager& manager, int width, bool* show_folder_panel, bool* show_Textures_panel, bool* show_Mesh_panel, bool* show_Materials_panel, bool* show_Scene_panel);
};