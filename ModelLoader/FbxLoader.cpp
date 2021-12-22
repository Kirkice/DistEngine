#include "FbxLoader.h"

using namespace Mathf;
using namespace ModelLoader;
using Microsoft::WRL::ComPtr;
using namespace DirectX;
using namespace DirectX::PackedVector;


void FbxLoader::LoadFbxModel(const std::string filename, std::vector<Vertex>& vertices, std::vector<USHORT>& indices, std::vector<Subset>& subsets)
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
		UINT numSubMeshs = 0;
		UINT numVertices = 0;
		UINT numTriangles = 0;

		std::map<int, int> mesh_vertices_start_count_map;
		std::map<int, int> mesh_triangles_start_count_map;

		GetVerticesAndIndicesCount(lRootNode, numVertices, numTriangles, numSubMeshs, mesh_vertices_start_count_map, mesh_triangles_start_count_map);

		vertices.resize(numVertices);
		indices.resize(numTriangles * 3);

		int VertexIndex = 0;
		int IndiceIndex = 0;
		ReadFbxChildNode(lRootNode,vertices,indices, VertexIndex, IndiceIndex);
	}

	lSdkManager->Destroy();
}


void FbxLoader::ReadFbxChildNode(FbxNode* node, std::vector<Vertex>& vertices, std::vector<USHORT>& indices, int& VertexIndex, int& IndiceIndex)
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
/// ��ȡģ�Ͷ�������
/// </summary>
/// <param name="node"></param>
/// <param name="numVertices"></param>
void FbxLoader::GetVerticesAndIndicesCount(
	FbxNode* node,
	UINT& numVertices,
	UINT& numTriangles,
	UINT& numSubMeshs,
	std::map<int, int>& mesh_vertices_start_count_map,
	std::map<int, int>& mesh_triangles_start_count_map
)
{
	if (node->GetChildCount())
	{
		for (int i = 0; i < node->GetChildCount(); i++)
		{
			if (node->GetChild(i)->GetMesh())
			{
				FbxMesh* mesh = node->GetChild(i)->GetMesh();
				std::string meshName = node->GetChild(i)->GetName();
				numVertices += mesh->GetControlPointsCount();
				mesh_vertices_start_count_map[numSubMeshs] = mesh->GetControlPointsCount();

				numTriangles += mesh->GetPolygonCount();
				mesh_triangles_start_count_map[numTriangles] = mesh->GetPolygonCount();

				numSubMeshs++;
			}

			GetVerticesAndIndicesCount(node->GetChild(i), numVertices, numTriangles, numSubMeshs, mesh_vertices_start_count_map, mesh_triangles_start_count_map);
		}
	}
}

//void readMesh(FbxMesh* mesh)
//{
//	int ctrlcount = mesh->GetControlPointsCount();
//	FbxVector4* ctrlpoints = mesh->GetControlPoints();
//	for (int i = 0; i < ctrlcount; i++)
//	{
//		Point p;
//		p.x = ctrlpoints[i].mData[0];
//		p.y = ctrlpoints[i].mData[1];
//		p.z = ctrlpoints[i].mData[2];
//		triangleVertexVec.push_back(p);
//	}
//	int pvcount = mesh->GetPolygonVertexCount();
//	int pcount = mesh->GetPolygonCount();
//	for (int i = 0; i < pcount; i++)
//	{
//		int psize = mesh->GetPolygonSize(i);
//		for (int j = 0; j < psize; j++)
//			triangleIndexVec.push_back(TriangleIndexCount + mesh->GetPolygonVertex(i, j));
//	}
//	TriangleVertexCount += ctrlcount;          //������һ��node�Ŀ��Ƶ����
//	TriangleIndexCount += ctrlcount;
//}
//
//
//void parseNode(FbxNode* node)              //��μ���fbx�ļ����RootNode�˴�ʡ��
//{
//	int count = node->GetChildCount();
//	parseNodeAttribute(node);
//
//	for (int i = 0; i < count; i++)
//	{
//		FbxNode* son = node->GetChild(i);
//		parseNode(node->GetChild(i));
//	}
//}
//
//void parseNodeAttribute(FbxNode* node)
//{
//	int nacount = node->GetNodeAttributeCount();
//	for (int i = 0; i < nacount; i++)
//	{
//		FbxNodeAttribute* na = node->GetNodeAttributeByIndex(i);
//		if (na->GetAttributeType() == FbxNodeAttribute::eMesh)
//		{
//			FbxMesh* mesh = (FbxMesh*)na;
//			readMesh(mesh);
//		}
//	}
//}

//http://t.zoukankan.com/nafio-p-9137423.html
//https://blog.csdn.net/lemon1235/article/details/88982564