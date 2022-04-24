#include "ObjLoader.h"

namespace Dist
{
	MeshData ObjLoader::LoadObj(const char* filename)
	{
		//	返回的MeshData
		MeshData data;

		//	tinyObj读取OBJ
		tinyobj::attrib_t attrib;
		std::vector<tinyobj::shape_t> shapes;
		std::vector<tinyobj::material_t> materials;
		std::string warn;
		std::string err;
		bool ret = tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, filename);

		//	加载失败
		//if (!warn.empty()) {
		//	std::cout << "WARN: " << warn << std::endl;
		//}

		//if (!err.empty()) {
		//	std::cerr << "ERR: " << err << std::endl;
		//}

		//if (!ret) {
		//	printf("Failed to load/parse .obj.\n");
		//}

		//std::vector<Vertex> vertices;
		//std::vector<uint32> Indices;

		for (size_t i = 0; i < shapes.size(); i++)
		{
			size_t index_offset = 0;

			// For each face
			for (size_t f = 0; f < shapes[i].mesh.num_face_vertices.size(); f++) 
			{
				size_t fnum = shapes[i].mesh.num_face_vertices[f];

				// 获得所索引下标
				tinyobj::index_t idx;
				int vertex_index[3];
				int normal_index[3];
				int texcoord_index[3];
				for (size_t v = 0; v < fnum; v++) 
				{
					idx = shapes[i].mesh.indices[index_offset + v];
					vertex_index[v] = idx.vertex_index;
					texcoord_index[v] = idx.texcoord_index;
					normal_index[v] = idx.normal_index;

					//	添加索引
					uint32 index;
					index = vertex_index[v];
					data.Indices32.push_back(index);
				}


				for (size_t v = 0; v < fnum; v++) {
					
					Vertex newVertex;
					newVertex.Position.x = attrib.vertices[(vertex_index[v]) * 3 + 0];
					newVertex.Position.y = attrib.vertices[(vertex_index[v]) * 3 + 1];
					newVertex.Position.z = attrib.vertices[(vertex_index[v]) * 3 + 2];

					// vt
					newVertex.TexC.x = attrib.texcoords[texcoord_index[v] * 2 + 0];
					newVertex.TexC.y = attrib.texcoords[texcoord_index[v] * 2 + 1];

					// vn
					newVertex.Normal.x = attrib.normals[normal_index[v] * 3 + 0];
					newVertex.Normal.y = attrib.normals[normal_index[v] * 3 + 1];
					newVertex.Normal.z = attrib.normals[normal_index[v] * 3 + 2];

					// vtangent
					newVertex.TangentU.x = 1.0f;
					newVertex.TangentU.y = 1.0f;
					newVertex.TangentU.z = 1.0f;

					data.Vertices.push_back(newVertex);
				}

			}
		}
		return data;
	}
}