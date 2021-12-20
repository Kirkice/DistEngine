#include "FbxLoader.h"

using namespace Mathf;
using namespace ModelLoader;
using Microsoft::WRL::ComPtr;
using namespace DirectX;
using namespace DirectX::PackedVector;


void FbxLoader::LoadFbxModel(const std::string filename,
	std::vector<Vertex>& vertices,
	std::vector<USHORT>& indices)
{
	const char* lFilename = filename.c_str();
	// Initialize the SDK manager. This object handles all our memory management.
	FbxManager* lSdkManager = FbxManager::Create();

	// Create the IO settings object.
	FbxIOSettings* ios = FbxIOSettings::Create(lSdkManager, IOSROOT);
	lSdkManager->SetIOSettings(ios);

	// Create an importer using the SDK manager.
	FbxImporter* lImporter = FbxImporter::Create(lSdkManager, "");

	// Use the first argument as the filename for the importer.
	if (!lImporter->Initialize(lFilename, -1, lSdkManager->GetIOSettings())) {
		printf("Call to FbxImporter::Initialize() failed.\n");
		printf("Error returned: %s\n\n", lImporter->GetStatus().GetErrorString());
		exit(-1);
	}

	// Create a new scene so that it can be populated by the imported file.
	FbxScene* lScene = FbxScene::Create(lSdkManager, "myScene");

	// Import the contents of the file into the scene.
	lImporter->Import(lScene);
	lImporter->Destroy();

	FbxNode* lRootNode = lScene->GetRootNode();


	if (lRootNode)
	{
		UINT numVertices = 0;
		UINT numTriangles = 0;

		GetVerticesCount(lRootNode, &numVertices);
		GetIndicesCount(lRootNode, &numTriangles);

		vertices.resize(numVertices);
		indices.resize(numTriangles * 3);

		int VertexIndex = 0;
		int IndiceIndex = 0;
		ReadFbxChildNode(lRootNode,vertices,indices, VertexIndex, IndiceIndex);
	}

	lSdkManager->Destroy();
}


void FbxLoader::ReadFbxChildNode(FbxNode* node, std::vector<Vertex>& vertices, std::vector<USHORT>& indices, int VertexIndex, int IndiceIndex)
{
	if (node->GetChildCount())
	{
		for (int i = 0; i < node->GetChildCount(); i++)
		{
			if (node->GetChild(i)->GetMesh())
			{
				FbxMesh* mesh = node->GetChild(i)->GetMesh();
				int cpCount1 = mesh->GetControlPointsCount();

				FbxLayer* layer0 = mesh->GetLayer(0);
				FbxLayerElementNormal* VertNormal = layer0->GetNormals();
				FbxLayerElementUV* VertUV = layer0->GetUVs();
				FbxLayerElementTangent* VertTangent = layer0->GetTangents();

				FbxVector4* controlPoints = mesh->GetControlPoints();
				for (int i = 0; i < cpCount1; i++)
				{
					FbxVector4 Pos = controlPoints[i];
					vertices[VertexIndex].Pos.x = Pos[0];
					vertices[VertexIndex].Pos.y = Pos[1];
					vertices[VertexIndex].Pos.z = Pos[2] * -1.0f;

					FbxVector4 Normal = VertNormal->GetDirectArray()[i];
					vertices[VertexIndex].Normal.x = Normal[0];
					vertices[VertexIndex].Normal.y = Normal[1];
					vertices[VertexIndex].Normal.z = Normal[2];

					FbxVector4 Tangent = VertTangent->GetDirectArray()[i];
					vertices[VertexIndex].TangentU.x = Tangent[0];
					vertices[VertexIndex].TangentU.y = Tangent[1];
					vertices[VertexIndex].TangentU.z = Tangent[2];

					FbxVector2 UV = VertUV->GetDirectArray()[i];
					vertices[VertexIndex].TexC.x = UV[0];
					vertices[VertexIndex].TexC.y = UV[1];

					VertexIndex++;
				}

				int polyCount = mesh->GetPolygonCount();
				for (int i = 0; i < polyCount; i++)
				{
					int polyItemSize = mesh->GetPolygonSize(i);
					assert(polyItemSize == 3);
					for (int j = 0; j < polyItemSize; j++)
					{
						int cpIndex = mesh->GetPolygonVertex(i, j);
						indices[IndiceIndex * 3 + 0] = controlPoints[cpIndex].mData[0];
						indices[IndiceIndex * 3 + 1] = controlPoints[cpIndex].mData[1];
						indices[IndiceIndex * 3 + 2] = controlPoints[cpIndex].mData[2];
						IndiceIndex++;
					}
				}
			}
			ReadFbxChildNode(node->GetChild(i), vertices, indices, VertexIndex, IndiceIndex);
		}
	}

}

/// <summary>
/// 获取模型顶点数据
/// </summary>
/// <param name="node"></param>
/// <param name="numVertices"></param>
void FbxLoader::GetVerticesCount(FbxNode* node, UINT* numVertices)
{
	if (node->GetChildCount())
	{
		for (int i = 0; i < node->GetChildCount(); i++)
		{
			if (node->GetChild(i)->GetMesh())
			{
				FbxMesh* mesh = node->GetChild(i)->GetMesh();
				numVertices += mesh->GetControlPointsCount();
			}
			GetIndicesCount(node->GetChild(i), numVertices);
		}
	}
}

/// <summary>
/// 获取模型索引数据
/// </summary>
/// <param name="node"></param>
/// <param name="numTriangles"></param>
void FbxLoader::GetIndicesCount(FbxNode* node, UINT* numTriangles)
{
	if (node->GetChildCount())
	{
		for (int i = 0; i < node->GetChildCount(); i++)
		{
			if (node->GetChild(i)->GetMesh())
			{
				FbxMesh* mesh = node->GetChild(i)->GetMesh();
				numTriangles += mesh->GetPolygonVertexCount();
			}
			GetIndicesCount(node->GetChild(i), numTriangles);
		}
	}
}