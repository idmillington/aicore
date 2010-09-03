/*
 * Defines the core functions of the library.
 *
 * Part of the Artificial Intelligence for Games system.
 *
 * Copyright (c) Ian Millington 2003-2006. All Rights Reserved.
 *
 * This software is distributed under licence. Use of this software
 * implies agreement with all terms and conditions of the accompanying
 * software licence.
 */
#include <aicore/aicore.h>

#include <stdlib.h>

namespace aicore
{

    void randomSeed(unsigned value)
    {
        if (value == 0) {
            srand((unsigned)TimingData::get().getClock());
        } else {
            srand(value);
        }
    }

    /* Get a random number [0,max[ */
    int randomInt(int max)
    {
        return rand() % max;
    }

    real randomReal(real max)
    {
       return max * (real(rand()) / RAND_MAX);
    }

    real randomBinomial(real max)
    {
       return randomReal(max)-randomReal(max);
    }

    bool randomBoolean()
    {
        return (rand() % 2 == 0);
    }

}; // end of namespace
