#pragma once
#include <string>
#include "imgui.h"
#include "imgui_impl_win32.h"
#include "imgui_impl_dx12.h"
#include "../ThirdParty/Stb/stb_image.h"
#include "ResourcesPath.h"
#include "Texture2D.h"

namespace Dist
{
	class ResourceManager
	{

	public:
		ResourceManager() = default;
		virtual ~ResourceManager() = default;

		std::unordered_map<std::string, std::unique_ptr<Texture2D>> mIconTextures;
		std::unordered_map<std::string, std::unique_ptr<Texture2D>> mResourcesTextures;
		std::unordered_map<std::string, std::unique_ptr<Texture2D>> mGizmosTextures;
		std::unordered_map<std::string, std::unique_ptr<TextureCube>> mCubeMapTextures;

		UINT ResourcesTextureEndIndex;

		void LoadTexture2D(Microsoft::WRL::ComPtr<ID3D12Device> d3d_device, Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> mSrvHeap, Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> mCommandList);

	private:

		bool LoadTextureFromFile(const char* filename, Microsoft::WRL::ComPtr<ID3D12Device> d3d_device, D3D12_CPU_DESCRIPTOR_HANDLE srv_cpu_handle, ID3D12Resource** out_tex_resource, int* out_width, int* out_height);
	};
}