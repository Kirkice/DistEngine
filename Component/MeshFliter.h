#pragma once
#include <cstdint>
#include <../../Core/Mathf/Mathf.h>
#include <vector>

namespace Dist
{
	using uint16 = std::uint16_t;
	using uint32 = std::uint32_t;

	//	����
	struct Vertex
	{
		Vertex() {}
		Vertex(
			const Vector3& p,
			const Vector3& n,
			const Vector3& t,
			const Vector2& uv) :
			Position(p),
			Normal(n),
			TangentU(t),
			TexC(uv) {}
		Vertex(
			float px, float py, float pz,
			float nx, float ny, float nz,
			float tx, float ty, float tz,
			float u, float v) :
			Position(px, py, pz),
			Normal(nx, ny, nz),
			TangentU(tx, ty, tz),
			TexC(u, v) {}

		Vector3 Position;
		Vector3 Normal;
		Vector3 TangentU;
		Vector2 TexC;
	};

	//	Mesh����
	struct MeshData
	{
		std::vector<Vertex> Vertices;
		std::vector<uint32> Indices32;

		std::vector<uint16>& GetIndices16()
		{
			if (mIndices16.empty())
			{
				mIndices16.resize(Indices32.size());
				for (size_t i = 0; i < Indices32.size(); ++i)
					mIndices16[i] = static_cast<uint16>(Indices32[i]);
			}

			return mIndices16;
		}

	private:
		std::vector<uint16> mIndices16;
	};

	class MeshFliter
	{
	public:
		MeshData data;

	public:
		MeshFliter();
		~MeshFliter();

		//	����Box
		void CreateBox(float width, float height, float depth, uint32 numSubdivisions);

		//	������
		void CreateSphere(float radius, uint32 sliceCount, uint32 stackCount);

		//	����Geo��
		void CreateGeosphere(float radius, uint32 numSubdivisions);

		//	����Բ����
		void CreateCylinder(float bottomRadius, float topRadius, float height, uint32 sliceCount, uint32 stackCount);

		//	������
		void CreateGrid(float width, float depth, uint32 m, uint32 n);

		//	����Quad
		void CreateQuad(float x, float y, float w, float h, float depth);

	private:
		void Subdivide(MeshData& meshData);
		Vertex MidPoint(const Vertex& v0, const Vertex& v1);
		void BuildCylinderTopCap(float bottomRadius, float topRadius, float height, uint32 sliceCount, uint32 stackCount, MeshData& meshData);
		void BuildCylinderBottomCap(float bottomRadius, float topRadius, float height, uint32 sliceCount, uint32 stackCount, MeshData& meshData);
	};
}