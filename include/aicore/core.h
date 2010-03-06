/*
 * The definitions for global routines and helpers.
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
 * Holds any utility functions and classes that are useful in the rest
 * of the toolkit. Currently this consists of a series of random
 * number generators.
 */
#ifndef AICORE_CORE_H
#define AICORE_CORE_H

/**
 * Holds all classes and functions for AI-Core.
 *
 * There is a single namespace in which the whole library lives. This
 * allows you to import the aicore.h header without fear of naming
 * clashes, then either do:
 *
 * <code>using namespace aicore;</code>
 *
 * If you are convinced that there will be no clashes, or access elements of
 * the code using the <code>aicore::</code> prefix.
 */
namespace aicore
{
    /**
     * @name Random Number Generators
     */
    /* @{ */

    /**
     * Seeds the random number generator with the given value. If the
     * value is 0 then a value taken from the current system clock is
     * used.
     *
     * @param value The value to use to seed the random number generator.
     */
    void randomSeed(unsigned value);

    /**
     * Gets a random integer in the range [0, max].
     *
     * @param max The upper bound of the range to generate the random
     * number from.
     *
     * @return The random number.
     */
    int randomInt(int max=100);

    /**
     * Gets a random number in the range [0, max].
     *
     * @param max The upper bound of the range to generate the random
     * number from.
     *
     * @return The random number.
     */
    real randomReal(real max=1);

    /**
     * Gets a random binomial in the range [-max, max]. This involves
     * two calls to randomReal().
     *
     * @param max The bound of the random binomial.
     * @return The random number.
     */
    real randomBinomial(real max=1);

    /**
     * Gets a random boolean value.
     *
     * @return The random value
     */
    bool randomBoolean();

    /* @} */

}; // end of namespace

#endif // AICORE_CORE_H
