#pragma once

#include <fbxsdk.h>
#include <map>
#include "d3dUtil.h"
#include "Mathf.h"
#include "UploadBuffer.h"
#include "GeometryGenerator.h"

using Microsoft::WRL::ComPtr;
using namespace DirectX;
using namespace DirectX::PackedVector;

namespace ModelLoader
{
	class FbxLoader
	{
	public:
		struct Vertex
		{
			DirectX::XMFLOAT3 Pos;
			DirectX::XMFLOAT3 Normal;
			DirectX::XMFLOAT2 TexC;
			DirectX::XMFLOAT4 TangentU;
		};

		struct Subset
		{
			UINT Id = -1;
			UINT VertexStart = 0;
			UINT VertexCount = 0;
			UINT FaceStart = 0;
			UINT FaceCount = 0;
		};

		void LoadFbxModel(const std::string filename, std::vector<Vertex>& vertices, std::vector<USHORT>& indices, std::vector<Subset>& subsets);

	private:
		//void SetMaterials(UINT numMaterials, std::vector<FbxMaterialData>& mats);
		//void SetSubsetTable(UINT numSubsets, std::vector<Subset>& subsets);
		void GetVerticesAndIndicesData(
			FbxNode* node,
			UINT& numVertices,
			UINT& numTriangles,
			UINT& numSubMeshs,
			std::vector<Vertex>& vertices,
			std::vector<USHORT>& indices,
			std::vector<Subset>& subsets
		);
	};
}

