/* $Header: /common/Render/include/D3D12Upload.h	    8/28/21 13:13pm Kirk 			    $ */
/*--------------------------------------------------------------------------------------------*
*                                                                                             *
*						Project Name : DistEngine                                             *
*                                                                                             *
*						File Name : D3D12Upload.h											  *
*                                                                                             *
*                       Programmer : Kirk                                                     *
*                                                                                             *
*---------------------------------------------------------------------------------------------*/
#pragma once
#include "../../Core/include/PCH.h"

namespace render
{
	struct MapResult
	{
		void* CPUAddress = nullptr;
		uint64 GPUAddress = 0;
		uint64 ResourceOffset = 0;
		ID3D12Resource* Resource = nullptr;
	};

	struct UploadContext
	{
		ID3D12GraphicsCommandList* CmdList;
		void* CPUAddress = nullptr;
		uint64 ResourceOffset = 0;
		ID3D12Resource* Resource = nullptr;
		void* Submission = nullptr;
	};

	struct ReadbackBuffer;
	struct Texture;

	namespace d3d12
	{
		void Initialize_Upload();
		void Shutdown_Upload();

		void EndFrame_Upload();

		void Flush_Upload();

		// Resource upload/init
		UploadContext ResourceUploadBegin(uint64 size);
		void ResourceUploadEnd(UploadContext& context);

		// Temporary CPU-writable buffer memory
		MapResult AcquireTempBufferMem(uint64 size, uint64 alignment);

		void ConvertAndReadbackTexture(const Texture& texture, DXGI_FORMAT outputFormat, ReadbackBuffer& buffer);
	}
}
