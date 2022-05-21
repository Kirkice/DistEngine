#include "ObjLoader.h"
#define TINYOBJLOADER_IMPLEMENTATION

void ObjLoader::LoadObj(MeshData& data, char* filename)
{
	//	tinyObj∂¡»°OBJ
	tinyobj::attrib_t attrib;
	std::vector<tinyobj::shape_t> shapes;
	std::vector<tinyobj::material_t> materials;
	std::string warn;
	std::string err;
	bool ret = tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, filename);

	//º”‘ÿ ß∞‹
	if (!warn.empty()) {
		std::cout << "WARN: " << warn << std::endl;
	}

	if (!err.empty()) {
		std::cerr << "ERR: " << err << std::endl;
	}

	if (!ret) {
		printf("Failed to load/parse .obj.\n");
	}


	for (const auto& shape : shapes)
	{
		data.Vertices.resize(shape.mesh.num_face_vertices.size() * 3);
		for (const auto& index : shape.mesh.indices) {

			data.Vertices[index.vertex_index].Position.x = attrib.vertices[3 * index.vertex_index + 0];
			data.Vertices[index.vertex_index].Position.y = attrib.vertices[3 * index.vertex_index + 1];
			data.Vertices[index.vertex_index].Position.z = attrib.vertices[3 * index.vertex_index + 2];

			data.Vertices[index.texcoord_index].TexC.x = attrib.texcoords[2 * index.texcoord_index + 0];
			data.Vertices[index.texcoord_index].TexC.y = attrib.texcoords[2 * index.texcoord_index + 1];

			data.Vertices[index.normal_index].Normal.x = attrib.vertices[3 * index.normal_index + 0];
			data.Vertices[index.normal_index].Normal.y = attrib.vertices[3 * index.normal_index + 1];
			data.Vertices[index.normal_index].Normal.z = attrib.vertices[3 * index.normal_index + 2];

			data.Vertices[index.normal_index].TangentU.x = attrib.vertices[3 * index.normal_index + 0];
			data.Vertices[index.normal_index].TangentU.y = attrib.vertices[3 * index.normal_index + 1];
			data.Vertices[index.normal_index].TangentU.z = attrib.vertices[3 * index.normal_index + 2];

			data.Indices32.push_back(index.vertex_index);
		}
	}
}