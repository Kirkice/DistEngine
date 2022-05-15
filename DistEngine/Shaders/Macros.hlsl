/* $Header: /Core/Macros.hlsl                                        6/26/21 20:55p KirkZhu $ */
/*--------------------------------------------------------------------------------------------*
*                                                                                             *
*                 Project Name : DistEngine                                                   *
*                                                                                             *
*                    File Name : Macros.hlsl                                                  *
*                                                                                             *
*                   Programmer : Kirk Zhu                                                     *
*                                                                                             *
*---------------------------------------------------------------------------------------------*/
#ifndef MACROS_INCLUDED
#define MACROS_INCLUDED

//Macros.hlsl
#define PI                                                                  3.14159265358979323846
#define TWO_PI                                                              6.28318530717958647693
#define FOUR_PI                                                             12.5663706143591729538
#define INV_PI                                                              0.31830988618379067154
#define INV_TWO_PI                                                          0.15915494309189533577
#define INV_FOUR_PI                                                         0.07957747154594766788
#define HALF_PI                                                             1.57079632679489661923
#define INV_HALF_PI                                                         0.63661977236758134308
#define LOG2_E                                                              1.44269504088896340736

#define MILLIMETERS_PER_METER                                               1000
#define METERS_PER_MILLIMETER                                               rcp(MILLIMETERS_PER_METER)
#define CENTIMETERS_PER_METER                                               100
#define METERS_PER_CENTIMETER                                               rcp(CENTIMETERS_PER_METER)

#define FLT_INF                                                             asfloat(0x7F800000)
#define FLT_EPS                                                             5.960464478e-8  // 2^-24, machine epsilon: 1 + EPS = 1 (half of the ULP for 1.0f)
#define FLT_MIN                                                             1.175494351e-38 // Minimum normalized positive floating-point number
#define FLT_MAX                                                             3.402823466e+38 // Maximum representable floating-point number
#define HALF_EPS                                                            4.8828125e-4    // 2^-11, machine epsilon: 1 + EPS = 1 (half of the ULP for 1.0f)
#define HALF_MIN                                                            6.103515625e-5  // 2^-14, the same value for 10, 11 and 16-bit: https://www.khronos.org/opengl/wiki/Small_Float_Formats
#define HALF_MIN_SQRT                                                       0.0078125
#define HALF_MAX                                                            65504.0
#define UINT_MAX                                                            0xFFFFFFFFu
#define INT_MAX                                                             0x7FFFFFFF

#endif