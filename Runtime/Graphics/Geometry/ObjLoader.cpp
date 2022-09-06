#include "ObjLoader.h"
#define TINYOBJLOADER_IMPLEMENTATION

void ObjLoader::LoadObj(MeshData& data, char* filename)
{
	//	tinyObj读取OBJ
	tinyobj::attrib_t attrib;
	std::vector<tinyobj::shape_t> shapes;
	std::vector<tinyobj::material_t> materials;
	std::string warn;
	std::string err;
	bool ret = tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, filename);

	//加载失败
	if (!warn.empty()) {
		std::cout << "WARN: " << warn << std::endl;
	}

	if (!err.empty()) {
		std::cerr << "ERR: " << err << std::endl;
	}

	if (!ret) {
		printf("Failed to load/parse .obj.\n");
	}

	//这里对贴图UV做一个修正：反转V轴：
	for (int i = 0; i < attrib.texcoords.size(); i++)
	{
		if (i % 2 == 1)	//第0个是u，第1个是v
			attrib.texcoords[i] = 1.0f - attrib.texcoords[i];
	}

	int PresentVertexIndex = 0;
	for (const auto& shape : shapes)
	{
		for (const auto& index : shape.mesh.indices) {

			PVertex vert;
			vert.Position.x = attrib.vertices[3 * index.vertex_index + 0];
			vert.Position.y = attrib.vertices[3 * index.vertex_index + 1];
			vert.Position.z = attrib.vertices[3 * index.vertex_index + 2];

			if (attrib.texcoords.size() > 0)
			{
				vert.TexC.x = attrib.texcoords[2 * index.texcoord_index + 0];
				vert.TexC.y = attrib.texcoords[2 * index.texcoord_index + 1];
			}
			else
			{
				vert.TexC.x = 0;
				vert.TexC.y = 0;
			}


			vert.Normal.x = 0; //attrib.vertices[3 * index.normal_index + 0];
			vert.Normal.y = 0; //attrib.vertices[3 * index.normal_index + 1];
			vert.Normal.z = 0; //attrib.vertices[3 * index.normal_index + 2];

			vert.TangentU.x = 0;
			vert.TangentU.y = 0;
			vert.TangentU.z = 0;

			data.Vertices.push_back(vert);
			data.Indices32.push_back(PresentVertexIndex);
			PresentVertexIndex++;
		}
	}

	ReconStructNormals(data);

	ComputeTangentSpace(data);
}

void ObjLoader::ReconStructNormals(MeshData& data)
{
	// iterator over faces (given by index triplets) and calculate normals for each incident vertex //
	for (size_t i = 0; i < data.Indices32.size() - 2; i += 3)
	{
		// face edges incident with vertex 0 //
		Vector3 edge0 = { data.Vertices[data.Indices32[i + 1]].Position[0] - data.Vertices[data.Indices32[i]].Position[0],
						   data.Vertices[data.Indices32[i + 1]].Position[1] - data.Vertices[data.Indices32[i]].Position[1],
						   data.Vertices[data.Indices32[i + 1]].Position[2] - data.Vertices[data.Indices32[i]].Position[2] };
		Vector3 edge1 = { data.Vertices[data.Indices32[i + 2]].Position[0] - data.Vertices[data.Indices32[i]].Position[0],
						   data.Vertices[data.Indices32[i + 2]].Position[1] - data.Vertices[data.Indices32[i]].Position[1],
						   data.Vertices[data.Indices32[i + 2]].Position[2] - data.Vertices[data.Indices32[i]].Position[2] };

		edge0 = Vector3::Normalize(edge0);
		edge1 = Vector3::Normalize(edge1);

		// compute normal using cross product //
		Vector3 normal = { edge0[1] * edge1[2] - edge0[2] * edge1[1],
							 edge0[2] * edge1[0] - edge0[0] * edge1[2],
							 edge0[0] * edge1[1] - edge0[1] * edge1[0] };

		normal = Vector3::Normalize(normal);

		// add this normal to all face-vertices //
		for (int j = 0; j < 3; ++j) {
			if (j == 0)
			{
				data.Vertices[data.Indices32[i]].Normal.x += normal[j];
				data.Vertices[data.Indices32[i + 1]].Normal.x += normal[j];
				data.Vertices[data.Indices32[i + 2]].Normal.x += normal[j];
			}
			else if (j == 1)
			{
				data.Vertices[data.Indices32[i]].Normal.y += normal[j];
				data.Vertices[data.Indices32[i + 1]].Normal.y += normal[j];
				data.Vertices[data.Indices32[i + 2]].Normal.y += normal[j];
			}
			else if (j == 2)
			{
				data.Vertices[data.Indices32[i]].Normal.z += normal[j];
				data.Vertices[data.Indices32[i + 1]].Normal.z += normal[j];
				data.Vertices[data.Indices32[i + 2]].Normal.z += normal[j];
			}
		}
	}

	// normalize all normals //
	for (unsigned int i = 0; i < data.Vertices.size(); ++i) {
		data.Vertices[i].Normal = Vector3::Normalize(data.Vertices[i].Normal);
	}
}

void ObjLoader::ComputeTangentSpace(MeshData& data)
{
	//	Compute Normal Tangent
	for (size_t i = 0; i < data.Indices32.size() - 2; i += 3)
	{
		PVertex& v0 = data.Vertices[data.Indices32[i]];
		PVertex& v1 = data.Vertices[data.Indices32[i + 1]];
		PVertex& v2 = data.Vertices[data.Indices32[i + 2]];

		// compute traingle edges Q1, Q2 //
		Vector3 Q1 = { v1.Position.x - v0.Position.x,
					   v1.Position.y - v0.Position.y,
					   v1.Position.z - v0.Position.z };

		Vector3 Q2 = { v2.Position.x - v0.Position.x,
					   v2.Position.y - v0.Position.y,
					   v2.Position.z - v0.Position.z };

		// compute dU and dV //
		float du1 = v1.TexC.x - v0.TexC.x;
		float dv1 = v1.TexC.y - v0.TexC.y;
		float du2 = v2.TexC.x - v0.TexC.x;
		float dv2 = v2.TexC.y - v0.TexC.y;

		// compute determinant //
		float det = du1 * dv2 - dv1 * du2;

		if (det != 0) {
			float invDet = 1.0f / det;
			// compute only tangent (bitangent gets recomputed later anyways) //
			for (unsigned int j = 0; j < 3; ++j)
			{
				if (j == 0)
				{
					float T = (Q1.x * dv2 - Q2.x * dv1) * invDet;
					v0.TangentU.x += T;
					v1.TangentU.x += T;
					v2.TangentU.x += T;
				}
				else if (j == 1)
				{
					float T = (Q1.y * dv2 - Q2.y * dv1) * invDet;
					v0.TangentU.y += T;
					v1.TangentU.y += T;
					v2.TangentU.y += T;
				}
				else if (j == 2)
				{
					float T = (Q1.z * dv2 - Q2.z * dv1) * invDet;
					v0.TangentU.z += T;
					v1.TangentU.z += T;
					v2.TangentU.z += T;
				}
			}
		}
	}
}