#pragma once
#include <cstdint>
#include "../Core/Mathf/Mathf.h"
#include <vector>

using namespace Mathf;

using uint16 = std::uint16_t;
using uint32 = std::uint32_t;

//	顶点
struct PVertex
{
	PVertex() {}
	PVertex(
		const Vector3& p,
		const Vector3& n,
		const Vector3& t,
		const Vector2& uv) :
		Position(p),
		Normal(n),
		TangentU(t),
		TexC(uv) {}
	PVertex(
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

struct Subset
{
	UINT Id = -1;
	UINT VertexStart = 0;
	UINT VertexCount = 0;
	UINT FaceStart = 0;
	UINT FaceCount = 0;
};

//	Mesh数据
struct MeshData
{
	std::vector<PVertex> Vertices;
	std::vector<uint32> Indices32;
	std::vector<Subset> Subsetes;
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

	//	创建Box
	void CreateBox(float width, float height, float depth, uint32 numSubdivisions);

	//	创建球
	void CreateSphere(float radius, uint32 sliceCount, uint32 stackCount);

	//	创建Geo球
	void CreateGeosphere(float radius, uint32 numSubdivisions);

	//	创建圆柱体
	void CreateCylinder(float bottomRadius, float topRadius, float height, uint32 sliceCount, uint32 stackCount);

	//	创建面
	void CreateGrid(float width, float depth, uint32 m, uint32 n);

	//	创建Quad
	void CreateQuad(float x, float y, float w, float h, float depth);

private:
	void Subdivide(MeshData& meshData);
	PVertex MidPoint(const PVertex& v0, const PVertex& v1);
	void BuildCylinderTopCap(float bottomRadius, float topRadius, float height, uint32 sliceCount, uint32 stackCount, MeshData& meshData);
	void BuildCylinderBottomCap(float bottomRadius, float topRadius, float height, uint32 sliceCount, uint32 stackCount, MeshData& meshData);
};