/*
 * Defines the classes used for kinematic movement.
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
 * Holds kinematic movement algorithms. Kinematic movement is movement
 * that doesn't take momentum or inertia into consideration:
 * characters have position but not velocity, and if they decide to
 * move, then their target velocity is instantly reached. This is in
 * contrast to dynamic movement, where (like the real world) movement
 * is achieved by applying acceleration to change velocity. Kinematic
 * movement is very useful because human beings can accelerate very
 * rapidly and have a relatively small top speed.
 */
#ifndef AICORE_KINEMATIC_H
#define AICORE_KINEMATIC_H

namespace aicore
{
    /**
     * The base class for all kinematic movement behaviours.
     */
    class KinematicMovement
    {
    public:
        /**
         * The character who is moving.
         */
        Location *character;

        /**
         * The maximum movement speed of the character.
         */
        real maxSpeed;

        /**
         * Works out the desired steering and writes it into the given
         * steering output structure.
         */
        virtual void getSteering(SteeringOutput* output) const = 0;
    };

    /**
     * This is a base class that adds a target vector to its parent
     * class definition.
     */
    class TargetedKinematicMovement : public KinematicMovement
    {
    public:
        /**
         * The target may be any vector (i.e. it might be something
         * that has no orientation, such as a point in space).
         */
        Vector3 *target;
    };


    // Concrete classes

    /**
     * Kinematic seek moves at full speed towards its target at each
     * frame.
     */
    class KinematicSeek : public TargetedKinematicMovement
    {
    public:
        /**
         * Works out the desired steering and writes it into the given
         * steering output structure.
         */
        virtual void getSteering(SteeringOutput* output) const;
    };

    /**
     * Flee seeks to maximise the distance from the target.
     */
    class KinematicFlee : public KinematicSeek
    {
    public:
        /**
         * Works out the desired steering and writes it into the given
         * steering output structure.
         */
        virtual void getSteering(SteeringOutput* output) const;
    };

    /**
     * Kinematic arrive behaves just like seek unless it is close to
     * the target, in which case it moves slower and eventually stops.
     */
    class KinematicArrive : public TargetedKinematicMovement
    {
    public:
        /**
         * At each step the character tries to reach its target in
         * this duration. This means it moves more slowly when nearby.
         */
        real timeToTarget;

        /**
         * If the character is closer than this to the target, it will
         * not attempt to move.
         */
        real radius;

        /**
         * Works out the desired steering and writes it into the given
         * steering output structure.
         */
        virtual void getSteering(SteeringOutput* output) const;
    };

    /**
     * Wander changes the orientation by a random amount then moves
     * the character forwards.
     */
    class KinematicWander : public KinematicMovement
    {
    public:
        /**
         * The maximum rate at which the character can turn.
         */
        real maxRotation;

        /**
         * Works out the desired steering and writes it into the given
         * steering output structure.
         */
        virtual void getSteering(SteeringOutput* output) const;
    };

}; // end of namespace

#endif // AICORE_KINEMATIC_H
