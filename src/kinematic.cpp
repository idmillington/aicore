/*
 * Defines the behaviour classes used for kinematic movement.
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

namespace aicore
{

    void KinematicSeek::getSteering(SteeringOutput* output) const
    {
        // First work out the direction
        output->linear = *target;
        output->linear -= character->position;

        // If there is no direction, do nothing
        if (output->linear.squareMagnitude() > 0)
        {
            output->linear.normalise();
            output->linear *= maxSpeed;
        }
    }

    void KinematicFlee::getSteering(SteeringOutput* output) const
    {
        // First work out the direction
        output->linear = character->position;
        output->linear -= *target;

        // If there is no direction, do nothing
        if (output->linear.squareMagnitude() > 0)
        {
            output->linear.normalise();
            output->linear *= maxSpeed;
        }
    }

    void KinematicArrive::getSteering(SteeringOutput* output) const
    {
        // First work out the direction
        output->linear = *target;
        output->linear -= character->position;

        // If there is no direction, do nothing
        if (output->linear.squareMagnitude() < radius*radius)
        {
            output->linear.clear();
        }
        else
        {
            // We'd like to arrive in timeToTarget seconds
            output->linear *= ((real)1.0 / timeToTarget);

            // If that is too fast, then clip the speed
            if (output->linear.squareMagnitude() > maxSpeed*maxSpeed)
            {
                output->linear.normalise();
                output->linear *= maxSpeed;
            }
        }
    }

    void KinematicWander::getSteering(SteeringOutput* output) const
    {
        // Move forward in the current direction
        output->linear = character->getOrientationAsVector();
        output->linear *= maxSpeed;

        // Turn a little
        real change = randomBinomial();
        output->angular = change * maxRotation;
    }

}; // end of namespace
