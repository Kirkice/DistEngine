//
// Created by Administrator on 2022/12/27.
//

#ifndef CORE_INT_H
#define CORE_INT_H
#pragma once

#include "MathHelper.h"

using namespace DirectX;

namespace Mathf {
    // Signed 32-bit integer vector classes
    struct Int2
    {
        int32 x;
        int32 y;

        Int2();
        Int2(int32 x, int32 y);

        bool operator==(Int2 other) const;
        bool operator!=(Int2 other) const;
    };

    struct Int3
    {
        int32 x;
        int32 y;
        int32 z;

        Int3();
        Int3(int32 x, int32 y, int32 z);
    };

    struct Int4
    {
        int32 x;
        int32 y;
        int32 z;
        int32 w;

        Int4();
        Int4(int32 x, int32 y, int32 z, int32 w);
    };


// Conversion classes for reduced-precision representations
    struct Half2
    {
        uint16 x;
        uint16 y;

        Half2() : x(0), y(0)
        {
        }

        Half2(uint16 x, uint16 y) : x(x), y(y)
        {
        }

        Half2(float x, float y)
        {
            DirectX::PackedVector::XMStoreHalf2(reinterpret_cast<DirectX::PackedVector::XMHALF2*>(this), DirectX::XMVectorSet(x, y, 0.0f, 0.0f));
        }

        explicit Half2(const Float2& v)
        {
            DirectX::PackedVector::XMStoreHalf2(reinterpret_cast<DirectX::PackedVector::XMHALF2*>(this), v.ToSIMD());
        }

        DirectX::XMVECTOR ToSIMD() const
        {
            return DirectX::PackedVector::XMLoadHalf2(reinterpret_cast<const DirectX::PackedVector::XMHALF2*>(this));
        }

        Float2 ToFloat2() const
        {
            return Float2(ToSIMD());
        }
    };

    struct Half4
    {
        uint16 x;
        uint16 y;
        uint16 z;
        uint16 w;

        Half4() : x(0), y(0), z(0), w(0)
        {
        }

        Half4(uint16 x, uint16 y, uint16 z, uint16 w) : x(x), y(y), z(z), w(w)
        {
        }

        Half4(float x, float y, float z, float w)
        {
            DirectX::PackedVector::XMStoreHalf4(reinterpret_cast<DirectX::PackedVector::XMHALF4*>(this), DirectX::XMVectorSet(x, y, z, w));
        }

        explicit Half4(const Float4& v)
        {
            DirectX::PackedVector::XMStoreHalf4(reinterpret_cast<DirectX::PackedVector::XMHALF4*>(this), v.ToSIMD());
        }

        DirectX::XMVECTOR ToSIMD() const
        {
            return DirectX::PackedVector::XMLoadHalf4(reinterpret_cast<const DirectX::PackedVector::XMHALF4*>(this));
        }

        Float3 ToFloat3() const
        {
            return Float3(ToSIMD());
        }

        Float4 ToFloat4() const
        {
            return Float4(ToSIMD());
        }
    };

    struct UByte4N
    {
        uint32 Bits;

        UByte4N() : Bits(0)
        {
        }

        explicit UByte4N(uint32 bits) : Bits(bits)
        {
        }

        UByte4N(uint8 x, uint8 y, uint8 z, uint8 w)
        {
            Bits = x | (y << 8) | (z << 16) | (w << 14);
        }

        UByte4N(float x, float y, float z, float w)
        {
            DirectX::PackedVector::XMStoreUByteN4(reinterpret_cast<DirectX::PackedVector::XMUBYTEN4*>(this), DirectX::XMVectorSet(x, y, z, w));
        }

        explicit UByte4N(const Float4& v)
        {
            DirectX::PackedVector::XMStoreUByteN4(reinterpret_cast<DirectX::PackedVector::XMUBYTEN4*>(this), v.ToSIMD());
        }

        DirectX::XMVECTOR ToSIMD() const
        {
            return DirectX::PackedVector::XMLoadUByteN4(reinterpret_cast<const DirectX::PackedVector::XMUBYTEN4*>(this));
        }

        Float4 ToFloat4() const
        {
            return Float4(ToSIMD());
        }
    };
}


#endif //CORE_INT_H
