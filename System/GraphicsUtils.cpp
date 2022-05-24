#include "GraphicsUtils.h"

//	¥”MeshDataº”‘ÿMeshGeometry
MeshGeometry* GraphicsUtils::BuidlMeshGeometryFromMeshData(std::string name, MeshData data, Microsoft::WRL::ComPtr<ID3D12Device> md3dDevice, Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> mCommandList)
{
	MeshGeometry* MeshGeo = new MeshGeometry;
	MeshGeo->Name = name;

	SubmeshGeometry Submesh;
	Submesh.IndexCount = (UINT)data.Indices32.size();
	Submesh.StartIndexLocation = 0;
	Submesh.BaseVertexLocation = 0;

	auto totalVertexCount = data.Vertices.size();



	std::vector<Vertex> vertices(totalVertexCount);
	UINT k = 0;
	for (size_t i = 0; i < data.Vertices.size(); ++i, ++k)
	{
		vertices[k].Pos = data.Vertices[i].Position.ToXMFLOAT3();
		vertices[k].Normal = data.Vertices[i].Normal.ToXMFLOAT3();
		vertices[k].TexC = data.Vertices[i].TexC.ToXMFLOAT2();
		vertices[k].TangentU = data.Vertices[i].TangentU.ToXMFLOAT3();
	}


	std::vector<std::uint16_t> indices;
	indices.insert(indices.end(), std::begin(data.GetIndices16()), std::end(data.GetIndices16()));

	const UINT vbByteSize = (UINT)vertices.size() * sizeof(Vertex);
	const UINT ibByteSize = (UINT)indices.size() * sizeof(std::uint16_t);

	ThrowIfFailed(D3DCreateBlob(vbByteSize, &MeshGeo->VertexBufferCPU));
	CopyMemory(MeshGeo->VertexBufferCPU->GetBufferPointer(), vertices.data(), vbByteSize);

	ThrowIfFailed(D3DCreateBlob(ibByteSize, &MeshGeo->IndexBufferCPU));
	CopyMemory(MeshGeo->IndexBufferCPU->GetBufferPointer(), indices.data(), ibByteSize);

	MeshGeo->VertexBufferGPU = d3dUtil::CreateDefaultBuffer(md3dDevice.Get(),
		mCommandList.Get(), vertices.data(), vbByteSize, MeshGeo->VertexBufferUploader);

	MeshGeo->IndexBufferGPU = d3dUtil::CreateDefaultBuffer(md3dDevice.Get(),
		mCommandList.Get(), indices.data(), ibByteSize, MeshGeo->IndexBufferUploader);

	MeshGeo->VertexByteStride = sizeof(Vertex);
	MeshGeo->VertexBufferByteSize = vbByteSize;
	MeshGeo->IndexFormat = DXGI_FORMAT_R16_UINT;
	MeshGeo->IndexBufferByteSize = ibByteSize;

	MeshGeo->DrawArgs["mesh"] = Submesh;

	return MeshGeo;
}

void GraphicsUtils::BuildTextureCubeSrvDesc(
	Microsoft::WRL::ComPtr<ID3D12Device> md3dDevice,
	UINT mCbvSrvUavDescriptorSize,
	CD3DX12_CPU_DESCRIPTOR_HANDLE& CPUDescriptor,
	CD3DX12_GPU_DESCRIPTOR_HANDLE& GPUDescriptor,
	std::unordered_map<std::string, std::unique_ptr<TextureCube>>& mCubeMapTextures,
	std::string TexName
)
{
	CPUDescriptor.Offset(1, mCbvSrvUavDescriptorSize);
	GPUDescriptor.Offset(1, mCbvSrvUavDescriptorSize);

	ComPtr<ID3D12Resource> diffuseIBL = mCubeMapTextures[TexName]->Resource;

	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MostDetailedMip = 0;
	srvDesc.Texture2D.ResourceMinLODClamp = 0.0f;

	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURECUBE;
	srvDesc.TextureCube.MostDetailedMip = 0;
	srvDesc.TextureCube.MipLevels = diffuseIBL->GetDesc().MipLevels;
	srvDesc.TextureCube.ResourceMinLODClamp = 0.0f;
	srvDesc.Format = diffuseIBL->GetDesc().Format;
	md3dDevice->CreateShaderResourceView(diffuseIBL.Get(), &srvDesc, CPUDescriptor);
	mCubeMapTextures[TexName]->CpuHandle = CPUDescriptor;
	mCubeMapTextures[TexName]->GpuHandle = GPUDescriptor;
}

