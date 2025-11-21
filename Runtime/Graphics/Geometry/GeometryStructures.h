#pragma once

#include <vector>
#include <cstdint>
#include "Mathf.h"

using uint16 = std::uint16_t;
using uint32 = std::uint32_t;

struct Vertex
{
    Vertex() {}
    Vertex(
        const Mathf::Vector3& p,
        const Mathf::Vector3& n,
        const Mathf::Vector3& t,
        const Mathf::Vector2& uv) :
        Position(p),
        Normal(n),
        TexC(uv),
        TangentU(t) {}
    Vertex(
        float px, float py, float pz,
        float nx, float ny, float nz,
        float tx, float ty, float tz,
        float u, float v) :
        Position(px, py, pz),
        Normal(nx, ny, nz),
        TexC(u, v),
        TangentU(tx, ty, tz) {}

    Mathf::Vector3 Position;
    Mathf::Vector3 Normal;
    Mathf::Vector2 TexC;
    Mathf::Vector3 TangentU;
};

struct Subset
{
    uint32 Id = -1;
    uint32 VertexStart = 0;
    uint32 VertexCount = 0;
    uint32 FaceStart = 0;
    uint32 FaceCount = 0;
};

struct MeshData
{
    std::vector<Vertex> Vertices;
    std::vector<uint32> Indices32;
    std::vector<Subset> Subsets;

    const std::vector<uint16>& GetIndices16() const
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
    mutable std::vector<uint16> mIndices16;
};
