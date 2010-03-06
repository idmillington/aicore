/*
 * Defines the classes used for steering.
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
 * Holds the algorithms for dynamic steering. Dynamic steering takes
 * momentum and inertia into consideration: the output from the
 * steering algorithm is an acceleration rather than a velocity. To
 * move to a particular location, the steering algorithm needs to
 * acceleration in the right direction and then acceleration in the
 * opposite direction to slow-down.
 */
#ifndef AICORE_STEERING_H
#define AICORE_STEERING_H

#include <vector>

namespace aicore
{
    /**
     * The steering behaviour is the base class for all dynamic
     * steering behaviours.
     */
    class SteeringBehaviour
    {
    public:
        /**
         * The character who is moving.
         */
        Kinematic *character;

        /**
         * Works out the desired steering and writes it into the given
         * steering output structure.
         */
        virtual void getSteering(SteeringOutput* output) = 0;
    };


    /**
     * The seek steering behaviour takes a target and aims right for
     * it with maximum acceleration.
     */
    class Seek : public SteeringBehaviour
    {
    public:
        /**
         * The target may be any vector (i.e. it might be something
         * that has no orientation, such as a point in space).
         */
        const Vector3 *target;

        /**
         * The maximum acceleration that can be used to reach the
         * target.
         */
        real maxAcceleration;

        /**
         * Works out the desired steering and writes it into the given
         * steering output structure.
         */
        virtual void getSteering(SteeringOutput* output);
    };

    /**
     * The seek steering behaviour takes a target and aims in the
     * opposite direction with maximum acceleration.
     */
    class Flee : public Seek
    {
    public:
        /**
         * Works out the desired steering and writes it into the given
         * steering output structure.
         */
        virtual void getSteering(SteeringOutput* output);
    };

	/**
	 * This subclass of seek is only intended as a base class for steering
	 * behaviours that create their own internal target, rather than having
	 * one assigned.
	 */
	class SeekWithInternalTarget : public Seek
	{
	protected:
		/**
		 * Holds the actual target we're aiming for. This can be written
		 * to by sub-classes (whereas the 'target' member cannot because
		 * it is const).
		 */
		Vector3 internal_target;

		/** 
		 * Creates a new behaviour and target. This method is protected
		 * because this class isn't meant to be instantiated directly.
		 */
		SeekWithInternalTarget();
	};

	/**
	 * The wander behaviour moves the relative target around the moving
	 * agent at random, then uses seek to head for it.
	 *
	 * The seek target for this class is created and destroyed by the 
	 * class, and should not be assigned to.
	 */
	class Wander : public SeekWithInternalTarget
	{
	public:
		/** 
		 * This controls the degree to which the character moves in a 
		 * straight line. Specifically it controls how far ahead to aim
		 * when the character is in motion. Too small values means the 
		 * character may be able to overshoot their target, and so will
		 * oscillate wildly.
		 */
		real volatility;

		/**
		 * This controls how fast the character may turn.
		 */
		real turnSpeed;

		/**
		 * Works out the desired steering and writes it into the given
		 * steering output structure.
		 */
		virtual void getSteering(SteeringOutput* output);
	};

	/**
	 * If the current character 
	 * movement would collide with the obstacles, then the behaviour
	 * returns a steering output (it flees the obstacle), otherwise the 
	 * steering output will be zero.
	 */
	class AvoidSphere : public SeekWithInternalTarget
	{
	public:
		/**
		 * The spherical obstacle we're avoiding.
		 */
		Sphere *obstacle;

		/**
		 * By how much do we want to avoid the collision?
		 */
		real avoidMargin;

		/**
		 * How far ahead do we want to look for collisions?
		 */
		real maxLookahead;

		/**
		* Works out the desired steering and writes it into the given
		* steering output structure.
		*/
		virtual void getSteering(SteeringOutput* output);
	};

	/**
	 * Blended steering takes a set of steering behaviours and generates an
	 * output by doing a weighted blend of their outputs.
	 */
	class BlendedSteering : public SteeringBehaviour
	{
	public:
		/**
		 * Holds a steering behaviour with its associated weight.
		 */
		struct BehaviourAndWeight
		{
			SteeringBehaviour *behaviour;
			real weight;

			BehaviourAndWeight(SteeringBehaviour *behaviour, real weight=(real)1.0)
				:
				behaviour(behaviour), weight(weight)
			{}
		};

		/**
		 * Holds the list of behaviour and their corresponding blending 
		 * weights.
		 */
		std::vector<BehaviourAndWeight> behaviours;

		/**
		 * Works out the desired steering and writes it into the given
		 * steering output structure.
		 */
		virtual void getSteering(SteeringOutput* output);			
	};

	/**
	 * Priority steering takes an ordered list of steering behaviours
	 * and uses the output of the first one that returns a result.
	 */
	class PrioritySteering : public SteeringBehaviour
	{
	public:
		/** Holds the list of steering behaviours in priority order. The
		* first item in the list is tried first, the subsequent entries
		* are only considered if the first one does not return a result.
		*/
		std::vector<SteeringBehaviour*> behaviours;

		/**
		 * After running this behaviour, this data member contains the 
		 * steering behaviour that was used. This allows you to track what
		 * the priority steering behaviuor did.
		 */
		SteeringBehaviour* lastUsed;

		/** 
		 * The threshold of the steering output magnitude below which a
		 * steering behaviour is considered to have given no output.
		 */
		real epsilon;

		/**
		* Works out the desired steering and writes it into the given
		* steering output structure.
		*/
		virtual void getSteering(SteeringOutput* output);
	};

}; // end of namespace

#endif // AICORE_STEERING_H
