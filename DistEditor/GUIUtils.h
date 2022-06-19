#pragma once
#include "ResourcesManager.h"
#include "ResourcesPath.h"


//Render Layer
enum class HierachyType : int
{
	DirectionLight = 0,
	PointLight = 1,
	SpotLight = 2,
	MeshRender = 3,
};


class GUIUtils
{
public:
	static void DrawProjetcFolder(
		Microsoft::WRL::ComPtr<ID3D12Device> md3dDevice,
		Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> mSrvHeap,
		std::unordered_map<std::string, std::unique_ptr<Texture2D>>& Textures,
		bool* show_folder_panel,
		bool* show_Textures_panel,
		bool* show_Mesh_panel,
		bool* show_Materials_panel,
		bool* show_Scene_panel
	);

	static void DrawProjectTextures(
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
	);

	static void DrawProjectMesh(
		Microsoft::WRL::ComPtr<ID3D12Device> md3dDevice,
		Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> mSrvHeap,
		std::unordered_map<std::string, std::unique_ptr<Texture2D>>& Textures,
		int width,
		bool* show_folder_panel,
		bool* show_Textures_panel,
		bool* show_Mesh_panel,
		bool* show_Materials_panel,
		bool* show_Scene_panel
	);

	static void DrawProjectMaterials(
		Microsoft::WRL::ComPtr<ID3D12Device> md3dDevice,
		Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> mSrvHeap,
		std::unordered_map<std::string, std::unique_ptr<Texture2D>>& Textures,
		int width,
		bool* show_folder_panel,
		bool* show_Textures_panel,
		bool* show_Mesh_panel,
		bool* show_Materials_panel,
		bool* show_Scene_panel
	);

	static void DrawProjectScene(
		Microsoft::WRL::ComPtr<ID3D12Device> md3dDevice,
		Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> mSrvHeap,
		std::unordered_map<std::string, std::unique_ptr<Texture2D>>& Textures,
		int width,
		bool* show_folder_panel,
		bool* show_Textures_panel,
		bool* show_Mesh_panel,
		bool* show_Materials_panel,
		bool* show_Scene_panel
	);
};