#pragma once

#include <string>
#include "imgui.h"
#include "imgui_impl_win32.h"
#include "imgui_impl_dx12.h"
#include "../../ThirdParty/Stb/stb_image.h"
#include "../../System/EngineSystem/SystemUtils.h"

using namespace Dist;


class EditorUtils
{
public:

	static bool LoadTextureFromFile(const char* filename, Microsoft::WRL::ComPtr<ID3D12Device> d3d_device, D3D12_CPU_DESCRIPTOR_HANDLE srv_cpu_handle, ID3D12Resource** out_tex_resource, int* out_width, int* out_height);

	static void InitProjectResources(Microsoft::WRL::ComPtr<ID3D12Device> md3dDevice, Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> mSrvHeap);

	static void DrawProjetcFolder(Microsoft::WRL::ComPtr<ID3D12Device> md3dDevice, Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> mSrvHeap, bool* show_folder_panel, bool* show_Textures_panel, bool* show_Mesh_panel, bool* show_Materials_panel, bool* show_Scene_panel);

	static void DrawProjectTextures(Microsoft::WRL::ComPtr<ID3D12Device> md3dDevice, Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> mSrvHeap, int width, bool* show_folder_panel, bool* show_Textures_panel, bool* show_Mesh_panel, bool* show_Materials_panel, bool* show_Scene_panel);

	static void DrawProjectMesh(Microsoft::WRL::ComPtr<ID3D12Device> md3dDevice, Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> mSrvHeap, int width, bool* show_folder_panel, bool* show_Textures_panel, bool* show_Mesh_panel, bool* show_Materials_panel, bool* show_Scene_panel);

	static void DrawProjectMaterials(Microsoft::WRL::ComPtr<ID3D12Device> md3dDevice, Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> mSrvHeap, int width, bool* show_folder_panel, bool* show_Textures_panel, bool* show_Mesh_panel, bool* show_Materials_panel, bool* show_Scene_panel);

	static void DrawProjectScene(Microsoft::WRL::ComPtr<ID3D12Device> md3dDevice, Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> mSrvHeap, int width, bool* show_folder_panel, bool* show_Textures_panel, bool* show_Mesh_panel, bool* show_Materials_panel, bool* show_Scene_panel);
};