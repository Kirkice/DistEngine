#pragma once
#include <string>
#include "../System/EngineSystem/SystemUtils.h"

namespace Dist
{
	struct Texture2D
	{
		int									Width;							//	宽度
		int									Height;							//	高度
		int									Bit;							//	位数
		float								Size;							//  大小
		D3D12_CPU_DESCRIPTOR_HANDLE			CpuHandle;						//	CPU句柄
		D3D12_GPU_DESCRIPTOR_HANDLE			GpuHandle;						//	GPU句柄
		std::string							Name;							//	name
		std::string							Path;							//	路径

		ID3D12Resource* Resource = nullptr;
	};
}