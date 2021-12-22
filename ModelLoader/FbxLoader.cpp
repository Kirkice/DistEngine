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

		GetVerticesAndIndicesData(lRootNode, numVertices, numTriangles, numSubMeshs, vertices, indices, subsets);
	}

	lSdkManager->Destroy();
}

/// <summary>
/// LoadMeshData
/// </summary>
/// <param name="node"></param>
/// <param name="numVertices"></param>
void FbxLoader::GetVerticesAndIndicesData(
	FbxNode* node,
	UINT& numVertices,
	UINT& numTriangles,
	UINT& numSubMeshs,
	std::vector<Vertex>& vertices, 
	std::vector<USHORT>& indices,
	std::vector<Subset>& subsets
)
{
	if (node->GetChildCount())
	{
		for (int i = 0; i < node->GetChildCount(); i++)
		{
			if (node->GetChild(i)->GetMesh())
			{
				//Get Mesh
				FbxMesh* mesh = node->GetChild(i)->GetMesh();
				//Get Node Name
				std::string meshName = node->GetChild(i)->GetName();

				int CtrlCount = mesh->GetControlPointsCount();
				int PolyCount = mesh->GetPolygonCount();

				//Set Subset Data
				Subset mSubset;
				mSubset.Id = numSubMeshs;
				mSubset.VertexStart = numVertices;
				mSubset.VertexCount = CtrlCount;
				mSubset.FaceStart = numTriangles;
				mSubset.FaceCount = PolyCount;
				subsets.push_back(mSubset);


				//Read~~~~~~~Mesh~~~~~~
				FbxLayer* layer0 = mesh->GetLayer(0); 
				FbxLayerElementNormal* VertNormal = layer0->GetNormals();
				FbxLayerElementUV* VertUV = layer0->GetUVs();
				FbxLayerElementTangent* VertTangent = layer0->GetTangents();

				FbxVector4* controlPoints = mesh->GetControlPoints();
				for (int i = 0; i < CtrlCount; i++)
				{
					Vertex vertex;
					FbxVector4 Pos = controlPoints[i];
					vertex.Pos.x = Pos[0];
					vertex.Pos.y = Pos[1];
					vertex.Pos.z = Pos[2];


					FbxVector4 Normal = VertNormal->GetDirectArray()[i];
					vertex.Normal.x = Normal[0];
					vertex.Normal.y = Normal[1];
					vertex.Normal.z = Normal[2];

					FbxVector4 Tangent = VertTangent->GetDirectArray()[i];
					vertex.TangentU.x = Tangent[0];
					vertex.TangentU.y = Tangent[1];
					vertex.TangentU.z = Tangent[2];

					FbxVector2 UV = VertUV->GetDirectArray()[i];
					vertex.TexC.x = UV[0];
					vertex.TexC.y = UV[1];
					vertices.push_back(vertex);
				}

				for (unsigned int i = 0; i < PolyCount; i++)
				{
					if(mesh->GetPolygonSize(i)!=3)
					{
						return;
					}

					for (unsigned int j = 0; j < mesh->GetPolygonSize(i); j++) 
					{
						int ctrlPointIndex = mesh->GetPolygonVertex(i, j);

						indices.push_back(ctrlPointIndex);
					}
				}

				//Update Input Data
				numVertices += CtrlCount;
				numTriangles += PolyCount;
				numSubMeshs++;

			}

			GetVerticesAndIndicesData(node->GetChild(i), numVertices, numTriangles, numSubMeshs, vertices, indices, subsets);
		}
	}
}