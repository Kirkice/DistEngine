#pragma once
#include <string>
#include "../System/EngineSystem/SystemUtils.h"

namespace Dist
{
	struct Texture2D
	{
		int									Width;							//	���
		int									Height;							//	�߶�
		int									Bit;							//	λ��
		float								Size;							//  ��С
		D3D12_CPU_DESCRIPTOR_HANDLE			CpuHandle;						//	CPU���
		D3D12_GPU_DESCRIPTOR_HANDLE			GpuHandle;						//	GPU���
		std::string							Name;							//	name
		std::string							Path;							//	·��

		ID3D12Resource*						Resource = nullptr;				//	Resource
	};


	struct TextureCube
	{
		D3D12_CPU_DESCRIPTOR_HANDLE			CpuHandle;						//	CPU���
		D3D12_GPU_DESCRIPTOR_HANDLE			GpuHandle;						//	GPU���
		std::string							Name;							//	name
		std::wstring						Path;							//	·��
		Microsoft::WRL::ComPtr<ID3D12Resource> Resource = nullptr;			//	Resource
		Microsoft::WRL::ComPtr<ID3D12Resource> UploadHeap = nullptr;		//	�ϴ���
	};
}