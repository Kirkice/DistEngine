
#include "SystemUtils.h"
#include "DefaultFrameResource.h"
#include <comdef.h>
#include <fstream>

using Microsoft::WRL::ComPtr;

namespace Dist
{
	DxException::DxException(HRESULT hr, const std::wstring& functionName, const std::wstring& filename, int lineNumber) :
		ErrorCode(hr),
		FunctionName(functionName),
		Filename(filename),
		LineNumber(lineNumber)
	{
	}

	bool SystemUtils::IsKeyDown(int vkeyCode)
	{
		return (GetAsyncKeyState(vkeyCode) & 0x8000) != 0;
	}

	ComPtr<ID3DBlob> SystemUtils::LoadBinary(const std::wstring& filename)
	{
		std::ifstream fin(filename, std::ios::binary);

		fin.seekg(0, std::ios_base::end);
		std::ifstream::pos_type size = (int)fin.tellg();
		fin.seekg(0, std::ios_base::beg);

		ComPtr<ID3DBlob> blob;
		ThrowIfFailed(D3DCreateBlob(size, blob.GetAddressOf()));

		fin.read((char*)blob->GetBufferPointer(), size);
		fin.close();

		return blob;
	}

	Microsoft::WRL::ComPtr<ID3D12Resource> SystemUtils::CreateDefaultBuffer(
		ID3D12Device* device,
		ID3D12GraphicsCommandList* cmdList,
		const void* initData,
		UINT64 byteSize,
		Microsoft::WRL::ComPtr<ID3D12Resource>& uploadBuffer)
	{
		ComPtr<ID3D12Resource> defaultBuffer;

		// Create the actual default buffer resource.
		ThrowIfFailed(device->CreateCommittedResource(
			&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
			D3D12_HEAP_FLAG_NONE,
			&CD3DX12_RESOURCE_DESC::Buffer(byteSize),
			D3D12_RESOURCE_STATE_COMMON,
			nullptr,
			IID_PPV_ARGS(defaultBuffer.GetAddressOf())));

		// In order to copy CPU memory data into our default buffer, we need to create
		// an intermediate upload heap. 
		ThrowIfFailed(device->CreateCommittedResource(
			&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
			D3D12_HEAP_FLAG_NONE,
			&CD3DX12_RESOURCE_DESC::Buffer(byteSize),
			D3D12_RESOURCE_STATE_GENERIC_READ,
			nullptr,
			IID_PPV_ARGS(uploadBuffer.GetAddressOf())));


		// Describe the data we want to copy into the default buffer.
		D3D12_SUBRESOURCE_DATA subResourceData = {};
		subResourceData.pData = initData;
		subResourceData.RowPitch = byteSize;
		subResourceData.SlicePitch = subResourceData.RowPitch;

		// Schedule to copy the data to the default buffer resource.  At a high level, the helper function UpdateSubresources
		// will copy the CPU memory into the intermediate upload heap.  Then, using ID3D12CommandList::CopySubresourceRegion,
		// the intermediate upload heap data will be copied to mBuffer.
		cmdList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(defaultBuffer.Get(),
			D3D12_RESOURCE_STATE_COMMON, D3D12_RESOURCE_STATE_COPY_DEST));
		UpdateSubresources<1>(cmdList, defaultBuffer.Get(), uploadBuffer.Get(), 0, 0, 1, &subResourceData);
		cmdList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(defaultBuffer.Get(),
			D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_GENERIC_READ));

		// Note: uploadBuffer has to be kept alive after the above function calls because
		// the command list has not been executed yet that performs the actual copy.
		// The caller can Release the uploadBuffer after it knows the copy has been executed.


		return defaultBuffer;
	}

	ComPtr<ID3DBlob> SystemUtils::CompileShader(
		const std::wstring& filename,
		const D3D_SHADER_MACRO* defines,
		const std::string& entrypoint,
		const std::string& target)
	{
		UINT compileFlags = 0;
#if defined(DEBUG) || defined(_DEBUG)  
		compileFlags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#endif

		HRESULT hr = S_OK;

		ComPtr<ID3DBlob> byteCode = nullptr;
		ComPtr<ID3DBlob> errors;
		hr = D3DCompileFromFile(filename.c_str(), defines, D3D_COMPILE_STANDARD_FILE_INCLUDE,
			entrypoint.c_str(), target.c_str(), compileFlags, 0, &byteCode, &errors);

		if (errors != nullptr)
			OutputDebugStringA((char*)errors->GetBufferPointer());

		ThrowIfFailed(hr);

		return byteCode;
	}

	std::wstring DxException::ToString()const
	{
		// Get the string description of the error code.
		_com_error err(ErrorCode);
		std::wstring msg = err.ErrorMessage();

		return FunctionName + L" failed in " + Filename + L"; line " + std::to_wstring(LineNumber) + L"; error: " + msg;
	}

	std::array<const CD3DX12_STATIC_SAMPLER_DESC, 7> GetStaticSamplers() 
	{
		// Applications usually only need a handful of samplers.  So just define them all up front
		// and keep them available as part of the root signature.  

		const CD3DX12_STATIC_SAMPLER_DESC pointWrap(
			0, // shaderRegister
			D3D12_FILTER_MIN_MAG_MIP_POINT, // filter
			D3D12_TEXTURE_ADDRESS_MODE_WRAP,  // addressU
			D3D12_TEXTURE_ADDRESS_MODE_WRAP,  // addressV
			D3D12_TEXTURE_ADDRESS_MODE_WRAP); // addressW

		const CD3DX12_STATIC_SAMPLER_DESC pointClamp(
			1, // shaderRegister
			D3D12_FILTER_MIN_MAG_MIP_POINT, // filter
			D3D12_TEXTURE_ADDRESS_MODE_CLAMP,  // addressU
			D3D12_TEXTURE_ADDRESS_MODE_CLAMP,  // addressV
			D3D12_TEXTURE_ADDRESS_MODE_CLAMP); // addressW

		const CD3DX12_STATIC_SAMPLER_DESC linearWrap(
			2, // shaderRegister
			D3D12_FILTER_MIN_MAG_MIP_LINEAR, // filter
			D3D12_TEXTURE_ADDRESS_MODE_WRAP,  // addressU
			D3D12_TEXTURE_ADDRESS_MODE_WRAP,  // addressV
			D3D12_TEXTURE_ADDRESS_MODE_WRAP); // addressW

		const CD3DX12_STATIC_SAMPLER_DESC linearClamp(
			3, // shaderRegister
			D3D12_FILTER_MIN_MAG_MIP_LINEAR, // filter
			D3D12_TEXTURE_ADDRESS_MODE_CLAMP,  // addressU
			D3D12_TEXTURE_ADDRESS_MODE_CLAMP,  // addressV
			D3D12_TEXTURE_ADDRESS_MODE_CLAMP); // addressW

		const CD3DX12_STATIC_SAMPLER_DESC anisotropicWrap(
			4, // shaderRegister
			D3D12_FILTER_ANISOTROPIC, // filter
			D3D12_TEXTURE_ADDRESS_MODE_WRAP,  // addressU
			D3D12_TEXTURE_ADDRESS_MODE_WRAP,  // addressV
			D3D12_TEXTURE_ADDRESS_MODE_WRAP,  // addressW
			0.0f,                             // mipLODBias
			8);                               // maxAnisotropy

		const CD3DX12_STATIC_SAMPLER_DESC anisotropicClamp(
			5, // shaderRegister
			D3D12_FILTER_ANISOTROPIC, // filter
			D3D12_TEXTURE_ADDRESS_MODE_CLAMP,  // addressU
			D3D12_TEXTURE_ADDRESS_MODE_CLAMP,  // addressV
			D3D12_TEXTURE_ADDRESS_MODE_CLAMP,  // addressW
			0.0f,                              // mipLODBias
			8);                                // maxAnisotropy

		const CD3DX12_STATIC_SAMPLER_DESC shadow(
			6, // shaderRegister
			D3D12_FILTER_COMPARISON_MIN_MAG_LINEAR_MIP_POINT, // filter
			D3D12_TEXTURE_ADDRESS_MODE_BORDER,  // addressU
			D3D12_TEXTURE_ADDRESS_MODE_BORDER,  // addressV
			D3D12_TEXTURE_ADDRESS_MODE_BORDER,  // addressW
			0.0f,                               // mipLODBias
			16,                                 // maxAnisotropy
			D3D12_COMPARISON_FUNC_LESS_EQUAL,
			D3D12_STATIC_BORDER_COLOR_OPAQUE_BLACK);

		return {
			pointWrap, pointClamp,
			linearWrap, linearClamp,
			anisotropicWrap, anisotropicClamp,
			shadow
		};
	}

	//	¥”MeshDataº”‘ÿMeshGeometry
	MeshGeometry* SystemUtils::BuidlMeshGeometryFromMeshData(std::string name, MeshData data, Microsoft::WRL::ComPtr<ID3D12Device> md3dDevice, Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> mCommandList)
	{
		MeshGeometry* MeshGeo;
		MeshGeo->Name = name;

		SubmeshGeometry Submesh;
		Submesh.IndexCount = (UINT)data.Indices32.size();
		Submesh.StartIndexLocation = 0;
		Submesh.BaseVertexLocation = 0;

		auto totalVertexCount = data.Vertices.size();



		std::vector<DefaultVertex> vertices(totalVertexCount);
		UINT k = 0;
		for (size_t i = 0; i < data.Vertices.size(); ++i, ++k)
		{
			vertices[k].Pos = data.Vertices[i].Position;
			vertices[k].Normal = data.Vertices[i].Normal;
			vertices[k].TexC = data.Vertices[i].TexC;
			vertices[k].TangentU = data.Vertices[i].TangentU;
		}


		std::vector<std::uint16_t> indices;
		indices.insert(indices.end(), std::begin(data.GetIndices16()), std::end(data.GetIndices16()));

		const UINT vbByteSize = (UINT)vertices.size() * sizeof(DefaultVertex);
		const UINT ibByteSize = (UINT)indices.size() * sizeof(std::uint16_t);

		ThrowIfFailed(D3DCreateBlob(vbByteSize, &MeshGeo->VertexBufferCPU));
		CopyMemory(MeshGeo->VertexBufferCPU->GetBufferPointer(), vertices.data(), vbByteSize);

		ThrowIfFailed(D3DCreateBlob(ibByteSize, &MeshGeo->IndexBufferCPU));
		CopyMemory(MeshGeo->IndexBufferCPU->GetBufferPointer(), indices.data(), ibByteSize);

		MeshGeo->VertexBufferGPU = SystemUtils::CreateDefaultBuffer(md3dDevice.Get(),
			mCommandList.Get(), vertices.data(), vbByteSize, MeshGeo->VertexBufferUploader);

		MeshGeo->IndexBufferGPU = SystemUtils::CreateDefaultBuffer(md3dDevice.Get(),
			mCommandList.Get(), indices.data(), ibByteSize, MeshGeo->IndexBufferUploader);

		MeshGeo->VertexByteStride = sizeof(DefaultVertex);
		MeshGeo->VertexBufferByteSize = vbByteSize;
		MeshGeo->IndexFormat = DXGI_FORMAT_R16_UINT;
		MeshGeo->IndexBufferByteSize = ibByteSize;

		MeshGeo->DrawArgs["mesh"] = Submesh;

		return MeshGeo;
	}
}