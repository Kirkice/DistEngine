//***************************************************************************************
// MathHelper.cpp by Frank Luna (C) 2011 All Rights Reserved.
//***************************************************************************************

#include "MathHelper.h"
#include <float.h>
#include <cmath>

using namespace DirectX;

namespace Mathf
{
    float AngleFromXY(float x, float y)
    {
        float theta = 0.0f;

        if (x >= 0.0f)
        {
            theta = atanf(y / x);

            if (theta < 0.0f)
                theta += 2.0f * Pi;
        }

        else
            theta = atanf(y / x) + Pi;

        return theta;
    }

    XMVECTOR RandUnitVec3()
    {
        XMVECTOR One = XMVectorSet(1.0f, 1.0f, 1.0f, 1.0f);
        XMVECTOR Zero = XMVectorZero();

        while (true)
        {
            XMVECTOR v = XMVectorSet(RandF(-1.0f, 1.0f),RandF(-1.0f, 1.0f),RandF(-1.0f, 1.0f), 0.0f);
            if (XMVector3Greater(XMVector3LengthSq(v), One))
                continue;

            return XMVector3Normalize(v);
        }
    }

    XMVECTOR RandHemisphereUnitVec3(XMVECTOR n)
    {
        XMVECTOR One = XMVectorSet(1.0f, 1.0f, 1.0f, 1.0f);
        XMVECTOR Zero = XMVectorZero();

        while (true)
        {
            XMVECTOR v = XMVectorSet(RandF(-1.0f, 1.0f), RandF(-1.0f, 1.0f), RandF(-1.0f, 1.0f), 0.0f);

            if (XMVector3Greater(XMVector3LengthSq(v), One))
                continue;

            if (XMVector3Less(XMVector3Dot(n, v), Zero))
                continue;

            return XMVector3Normalize(v);
        }
    }
}