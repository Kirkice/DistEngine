//
// Created by Administrator on 2022/12/27.
//

#ifndef APPLICATION_UINT_H
#define APPLICATION_UINT_H

#pragma once

#include "MathHelper.h"

using namespace DirectX;

namespace Mathf {
// Unsigned 32-bit integer vector classes
    struct Uint2 {
        uint32 x;
        uint32 y;

        Uint2();

        Uint2(uint32 x, uint32 y);

        bool operator==(Uint2 other) const;

        bool operator!=(Uint2 other) const;
    };

    struct Uint3 {
        uint32 x;
        uint32 y;
        uint32 z;

        Uint3();

        Uint3(uint32 x, uint32 y, uint32 z);

        bool operator==(const Uint3 &other) const;

        bool operator!=(const Uint3 &other) const;
    };

    struct Uint4 {
        uint32 x;
        uint32 y;
        uint32 z;
        uint32 w;

        Uint4();

        Uint4(uint32 x, uint32 y, uint32 z, uint32 w);

        bool operator==(const Uint4 &other) const;

        bool operator!=(const Uint4 &other) const;
    };
}


#endif //APPLICATION_UINT_H
