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

			vert.TexC.x = attrib.texcoords[2 * index.texcoord_index + 0];
			vert.TexC.y = attrib.texcoords[2 * index.texcoord_index + 1];

			vert.Normal.x = attrib.vertices[3 * index.normal_index + 0];
			vert.Normal.y = attrib.vertices[3 * index.normal_index + 1];
			vert.Normal.z = attrib.vertices[3 * index.normal_index + 2];

			vert.TangentU.x = attrib.vertices[3 * index.normal_index + 0];
			vert.TangentU.y = attrib.vertices[3 * index.normal_index + 1];
			vert.TangentU.z = attrib.vertices[3 * index.normal_index + 2];

			data.Vertices.push_back(vert);
			data.Indices32.push_back(PresentVertexIndex);
			PresentVertexIndex++;
		}
	}
}