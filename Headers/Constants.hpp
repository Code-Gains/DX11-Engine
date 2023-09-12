#ifndef CONSTANTS_HPP
#define CONSTANTS_HPP
#include <corecrt_math_defines.h>

namespace Constants
{
    const float DegreesToRadiansConst = M_PI * 2.0f / 360.0f;

    inline float DegreesToRadians(float degrees)
    {
        return DegreesToRadiansConst * degrees;
    }

    const double G = 6.67430e-11;
}

#endif