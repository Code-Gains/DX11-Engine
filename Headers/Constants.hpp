#ifndef CONSTANTS_HPP
#define CONSTANTS_HPP
#include <corecrt_math_defines.h>

namespace Constants
{
    const double DegreesToRadiansConst = M_PI * 2.0 / 360.0;

    inline double DegreesToRadians(double degrees)
    {
        return DegreesToRadiansConst * degrees;
    }

    const double G = 6.67430e-11;
}

#endif