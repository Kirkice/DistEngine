/* $Header: /common/Render/include/Model.h	            8/28/21 13:13pm Kirk 			    $ */
/*--------------------------------------------------------------------------------------------*
*                                                                                             *
*						Project Name : DistEngine                                             *
*                                                                                             *
*						File Name : Model.h												      *
*                                                                                             *
*                       Programmer : Kirk                                                     *
*                                                                                             *
*---------------------------------------------------------------------------------------------*/
#pragma once

#include "../../Core/include/PCH.h"
#include "../../Core/include/InterfacePointers.h"
#include "../../DistMath/include/DistMath.h"
#include "../../Core/include/Serialization.h"
#include "../../Core/include/Containers.h"
#include "../../render/include/GraphicsTypes.h"

struct aiMesh;

namespace render
{
	struct MeshVertex
	{
		Vector3 Position;
		Vector3 Normal;
		Vector2 UV;
		Vector3 Tangent;
		Vector3 Bitangent;

		MeshVertex()
		{
		}

		MeshVertex(const Vector3& p, const Vector3& n, const Vector2& uv, const Vector3& t, const Vector3& b)
		{
			Position = p;
			Normal = n;
			UV = uv;
			Tangent = t;
			Bitangent = b;
		}

		void Transform(const Vector3& p, const Vector3& s, const Quaternion& q)
		{
			Position *= s;
			Position = Vector3::Transform(Position, q);
			Position += p;

			Normal = Vector3::Transform(Normal, q);
			Tangent = Vector3::Transform(Tangent, q);
			Bitangent = Vector3::Transform(Bitangent, q);
		}

		template<typename TSerializer> void Serialize(TSerializer& serializer)
		{
			SerializeItem(serializer, Position);
			SerializeItem(serializer, Normal);
			SerializeItem(serializer, UV);
			SerializeItem(serializer, Tangent);
			SerializeItem(serializer, Bitangent);
		}
	};

	enum class MaterialTextures
	{
		Albedo = 0,
		Normal,
		Roughness,
		Metallic,
		Opacity,
		Emissive,

		Count
	};

	struct MeshMaterial
	{
		std::string Name;
		std::wstring TextureNames[uint64(MaterialTextures::Count)];
		const Texture* Textures[uint64(MaterialTextures::Count)] = { };
		uint32 TextureIndices[uint64(MaterialTextures::Count)] = { };

		uint32 Texture(MaterialTextures texType) const
		{
			Assert_(uint64(texType) < uint64(MaterialTextures::Count));
			Assert_(Textures[uint64(texType)] != nullptr);
			return Textures[uint64(texType)]->SRV;
		}

		template<typename TSerializer> void Serialize(TSerializer& serializer)
		{
			for (uint64 i = 0; i < uint64(MaterialTextures::Count); ++i)
				SerializeItem(serializer, TextureNames[i]);
			BulkSerializeArray(serializer, TextureIndices, ArraySize_(TextureIndices));
		}
	};

	struct MeshPart
	{
		uint32 VertexStart;
		uint32 VertexCount;
		uint32 IndexStart;
		uint32 IndexCount;
		uint32 MaterialIdx;

		MeshPart() : VertexStart(0), VertexCount(0), IndexStart(0), IndexCount(0), MaterialIdx(0)
		{
		}
	};

	enum class IndexType
	{
		Index16Bit = 0,
		Index32Bit = 1
	};

	enum class InputElementType : uint64
	{
		Position = 0,
		Normal,
		Tangent,
		Bitangent,
		UV,

		NumTypes,
	};

	struct MaterialTexture
	{
		std::wstring Name;
		Texture Texture;
	};

	struct ModelSpotLight
	{
		Vector3 Position;
		Vector3 Intensity;
		Vector3 Direction;
		Quaternion Orientation;
		Vector2 AngularAttenuation;
	};

	struct PointLight
	{
		Vector3 Position;
		Vector3 Intensity;
	};

	class Mesh
	{
		friend class Model;

	public:

		~Mesh()
		{
			Assert_(numVertices == 0);
		}

		// Init from loaded files
		void InitFromAssimpMesh(const aiMesh& assimpMesh, float sceneScale,
			MeshVertex* dstVertices, uint8* dstIndices, IndexType indexType);

		// Procedural generation
		void InitBox(const Vector3& dimensions, const Vector3& position,
			const Quaternion& orientation, uint32 materialIdx,
			MeshVertex* dstVertices, uint16* dstIndices);

		void InitPlane(const Vector2& dimensions, const Vector3& position,
			const Quaternion& orientation, uint32 materialIdx,
			MeshVertex* dstVertices, uint16* dstIndices);

		void InitCommon(const MeshVertex* vertices, const uint8* indices, uint64 vbAddress, uint64 ibAddress, uint64 vtxOffset, uint64 idxOffset);

		void Shutdown();

		// Accessors
		const Array<MeshPart>& MeshParts() const { return meshParts; }
		uint64 NumMeshParts() const { return meshParts.Size(); }

		uint32 NumVertices() const { return numVertices; }
		uint32 NumIndices() const { return numIndices; }
		uint32 VertexOffset() const { return vtxOffset; }
		uint32 IndexOffset() const { return idxOffset; }

		IndexType IndexBufferType() const { return indexType; }
		DXGI_FORMAT IndexBufferFormat() const { return indexType == IndexType::Index32Bit ? DXGI_FORMAT_R32_UINT : DXGI_FORMAT_R16_UINT; }
		uint32 IndexSize() const { return indexType == IndexType::Index32Bit ? 4 : 2; }

		const MeshVertex* Vertices() const { return vertices; }
		const uint16* Indices() const { Assert_(indexType == IndexType::Index16Bit); return (const uint16*)indices; }
		const uint32* Indices32() const { Assert_(indexType == IndexType::Index32Bit); return (const uint32*)indices; }

		const D3D12_VERTEX_BUFFER_VIEW* VBView() const { return &vbView; }
		const D3D12_INDEX_BUFFER_VIEW* IBView() const { return &ibView; }

		const Vector3& AABBMin() const { return aabbMin; }
		const Vector3& AABBMax() const { return aabbMax; }

		static const char* InputElementTypeString(InputElementType elemType);

		template<typename TSerializer> void Serialize(TSerializer& serializer)
		{
			BulkSerializeItem(serializer, meshParts);
			SerializeItem(serializer, numVertices);
			SerializeItem(serializer, numIndices);
			SerializeItem(serializer, vtxOffset);
			SerializeItem(serializer, idxOffset);
			uint32 idxType = uint32(indexType);
			SerializeItem(serializer, idxType);
			indexType = IndexType(idxType);
			SerializeItem(serializer, aabbMin);
			SerializeItem(serializer, aabbMax);
		}

	protected:

		Array<MeshPart> meshParts;

		uint32 numVertices = 0;
		uint32 numIndices = 0;
		uint32 vtxOffset = 0;
		uint32 idxOffset = 0;

		IndexType indexType = IndexType::Index16Bit;

		const MeshVertex* vertices = nullptr;
		const uint8* indices = nullptr;

		D3D12_VERTEX_BUFFER_VIEW vbView = { };
		D3D12_INDEX_BUFFER_VIEW ibView = { };

		Vector3 aabbMin;
		Vector3 aabbMax;
	};

	struct ModelLoadSettings
	{
		const wchar* FilePath = nullptr;
		const wchar* TextureDir = nullptr;
		float SceneScale = 1.0f;
		bool ForceSRGB = false;
		bool MergeMeshes = true;
	};

	class Model
	{
	public:

		~Model()
		{
			Assert_(meshes.Size() == 0);
		}

		// Loading from file formats
		void CreateWithAssimp(const ModelLoadSettings& settings);

		void CreateFromMeshData(const wchar* filePath);

		// Procedural generation
		void GenerateBoxScene(const Vector3& dimensions = Vector3(1.0f, 1.0f, 1.0f),
			const Vector3& position = Vector3(),
			const Quaternion& orientation = Quaternion(),
			const wchar* colorMap = L"",
			const wchar* normalMap = L"");
		void GenerateBoxTestScene();
		void GeneratePlaneScene(const Vector2& dimensions = Vector2(1.0f, 1.0f),
			const Vector3& position = Vector3(),
			const Quaternion& orientation = Quaternion(),
			const wchar* colorMap = L"",
			const wchar* normalMap = L"");

		void Shutdown();

		// Accessors
		const Array<Mesh>& Meshes() const { return meshes; }
		uint64 NumMeshes() const { return meshes.Size(); }

		const Vector3& AABBMin() const { return aabbMin; }
		const Vector3& AABBMax() const { return aabbMax; }

		const Array<MeshMaterial>& Materials() const { return meshMaterials; }
		const GrowableList<MaterialTexture*>& MaterialTextures() const { return materialTextures; }

		const Array<ModelSpotLight>& SpotLights() const { return spotLights; }
		const Array<PointLight>& PointLights() const { return pointLights; }

		const StructuredBuffer& VertexBuffer() const { return vertexBuffer; }
		const FormattedBuffer& IndexBuffer() const { return indexBuffer; }

		const MeshVertex* Vertices() const { return vertices.Data(); }
		const uint16* Indices() const { Assert_(indexType == IndexType::Index16Bit); return (const uint16*)indices.Data(); }
		const uint32* Indices32() const { Assert_(indexType == IndexType::Index32Bit); return (const uint32*)indices.Data(); }

		const std::wstring& FileDirectory() const { return fileDirectory; }

		static const D3D12_INPUT_ELEMENT_DESC* InputElements();
		static const InputElementType* InputElementTypes();
		static uint64 NumInputElements();

		IndexType IndexBufferType() const { return indexType; }
		DXGI_FORMAT IndexBufferFormat() const { return indexType == IndexType::Index32Bit ? DXGI_FORMAT_R32_UINT : DXGI_FORMAT_R16_UINT; }
		uint32 IndexSize() const { return indexType == IndexType::Index32Bit ? 4 : 2; }

		// Serialization
		template<typename TSerializer>
		void Serialize(TSerializer& serializer)
		{
			SerializeItem(serializer, meshes);
			SerializeItem(serializer, meshMaterials);
			BulkSerializeItem(serializer, spotLights);
			BulkSerializeItem(serializer, pointLights);
			SerializeItem(serializer, forceSRGB);
			SerializeItem(serializer, aabbMin);
			SerializeItem(serializer, aabbMax);
			BulkSerializeItem(serializer, vertices);
			BulkSerializeItem(serializer, indices);
			uint32 idxType = uint32(indexType);
			SerializeItem(serializer, idxType);
			indexType = IndexType(idxType);
		}

	protected:

		void CreateBuffers();

		Array<Mesh> meshes;
		Array<MeshMaterial> meshMaterials;
		Array<ModelSpotLight> spotLights;
		Array<PointLight> pointLights;
		std::wstring fileDirectory;
		bool32 forceSRGB = false;
		Vector3 aabbMin;
		Vector3 aabbMax;

		StructuredBuffer vertexBuffer;
		FormattedBuffer indexBuffer;
		Array<MeshVertex> vertices;
		Array<uint8> indices;
		IndexType indexType = IndexType::Index16Bit;

		GrowableList<MaterialTexture*> materialTextures;
	};

	void MakeSphereGeometry(uint64 uDivisions, uint64 vDivisions, StructuredBuffer& vtxBuffer, FormattedBuffer& idxBuffer);
	void MakeBoxGeometry(StructuredBuffer& vtxBuffer, FormattedBuffer& idxBuffer, float scale = 1.0f);
	void MakeConeGeometry(uint64 divisions, StructuredBuffer& vtxBuffer, FormattedBuffer& idxBuffer, Array<Vector3>& positions);
	void MakeConeGeometry(uint64 divisions, StructuredBuffer& vtxBuffer, FormattedBuffer& idxBuffer);
}
