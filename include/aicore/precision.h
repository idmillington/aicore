/*
 * Interface file for code that changes when the core's precision is
 * altered.
 *
 * Part of the Artificial Intelligence for Games system.
 *
 * Copyright (c) Ian Millington 2003-2006. All Rights Reserved.
 *
 * This software is distributed under licence. Use of this software
 * implies agreement with all terms and conditions of the accompanying
 * software licence.
 */

/**
 * @file
 *
 * Because this library is designed to work at either single or double
 * precision, mathematical functions such as sqrt cannot be used in
 * the source code or headers: this file provides defines for the real
 * number typedef and mathematical formulae that work on it.
 *
 * @note The contents of this file don't need to be changed to compile
 * at a different precision. Just change the first #define to be:
 *
 * <code>#define DOUBLE_PRECISION</code>
 *
 * If you want to do something more exotic, like using 20bit extended
 * doubles, or fixed point arithmetic, then you'll need to change most
 * of the definitions.
 */
#ifndef AICORE_PRECISION_H
#define AICORE_PRECISION_H

// Change this to DOUBLE_PRECISION if you want.
#define SINGLE_PRECISION



// Import the mathematical functions for both precisions
#include <math.h>

// Work out which limits we need
#if defined(SINGLE_PRECISION)
    #include <float.h>
#elif defined(DOUBLE_PRECISION)
    #include <limits.h>
#endif

#undef M_PI
#undef M_PI_2
#undef M_PI_4
#undef M_1_PI
#undef M_2_PI

namespace aicore
{

// Change this to #if 0 to get double precision
#if defined(SINGLE_PRECISION)

    /**
     * Defines a real number precision. AI-Core can be compiled in
     * single or double precision versions.
     */
    typedef float real;

    /** Defines the highest value for the real number. */
    #define REAL_MAX FLT_MAX

    /** Defines the precision of the square root operation. */
    #define real_sqrt sqrtf

    /** Defines the precision of the absolute magnitude operation. */
    #define real_abs fabsf

    /** Defines the precision of the sine operation. */
    #define real_sin sinf

    /** Defines the precision of the cosine operation. */
    #define real_cos cosf

    /** Defines the precision of the exponent operation. */
    #define real_exp expf

    /** Defines the precision of the power operation. */
    #define real_pow powf

    /** Defines the precision of the two-part arctan operation. */
    #define real_atan2 atan2f

    /** Defines the precision of the float modulo division operation. */
    #define real_mod_real fmodf

#elif defined(DOUBLE_PRECISION)

    /**
     * Defines a real number precision. AI-Core can be compiled in
     * single or double precision versions.
     */
    typedef double real;

    /** Defines the highest value for the real number. */
    #define REAL_MAX DBL_MAX

    /** Defines the precision of the square root operation. */
    #define real_sqrt sqrt

    /** Defines the precision of the absolute magnitude operation. */
    #define real_abs fabs

    /** Defines the precision of the sine operation. */
    #define real_sin sin

    /** Defines the precision of the cosine operation. */
    #define real_cos cos

    /** Defines the precision of the exponent operation. */
    #define real_exp exp

    /** Defines the precision of the power operation. */
    #define real_pow pow

    /** Defines the precision of the two-part arctan operation. */
    #define real_atan2 atan2f

    /** Defines the precision of the float modulo division operation. */
    #define real_mode_real fmodf

#endif

    /**
     * @name Mathematical Constants
     *
     * Various constants involving PI.
     *
     * @note These are dramatically
     * over-accurate for a float definition of real, but this won't
     * matter, and its easier to keep one lot of numbers for both
     * cases.
     */
    /* @{ */

    /** 2*pi */
    const real M_2PI = (real)6.28318530717958647692;

    /** pi/180 */
    const real M_PI_180 = (real)0.01745329251994329576;

    /** 180/pi */
    const real M_180_PI = (real)57.2957795130823208767;

    /** pi */
    const real M_PI = (real)3.14159265358979323846;

    /** pi/2 */
    const real M_PI_2 = (real)1.57079632679489661923;

    /** pi/4 */
    const real M_PI_4 = (real)0.78539816339744830962;

    /** 1/pi */
    const real M_1_PI = (real)0.31830988618379067154;

    /** 2/pi */
    const real M_2_PI = (real)0.63661977236758134308;

    /* @} */
}

#endif // AICORE_PRECISION_H
