/*
 * Defines the pipeline steering system.
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
 * Holds the pipeline steering system. Pipeline steering is a
 * algorithm that generates steering output based on co-operation
 * between different experts: a Targeter that knows where it wants to
 * go, Constraints that know where to avoid, Decomposers know how to
 * decompose goals into sub-goals (pathfinding for example). This file
 * contains the interfaces for you to create new experts, and a series
 * of concrete classes that can be used directly.
 */
#ifndef AICORE_STEERPIPE_H
#define AICORE_STEERPIPE_H

#include <list>
#include <vector>

namespace aicore
{
	/**
	 * Goals represent what a steering character wants to achieve. It
	 * has multiple channels, one for each positional and velocity.
	 */
	struct Goal
	{
		Vector3 position;
		bool positionSet;

		real orientation;
		bool orientationSet;

		Vector3 velocity;
		bool velocitySet;

		real rotation;
		bool rotationSet;

		/**
		 * Creates a new goal with no channels.
		 */
		Goal();

		/**
		 * Clears all channels from the goal.
		 */
		void clear();

		/**
		 * Updates this goal by adding additional channels from the
		 * given goal.
		 */
		void updateGoal(const Goal& goal);

		/**
		 * Checks if the given goal can be merged with this one. This is
		 * true if they don't both try to set the same channels.
		 */
		bool canMergeGoals(const Goal& goal) const;
	};

	/**
	 * The base super-class for paths. Particular path implementations will 
	 * derive from this.
	 */
	class Path
	{
	public:
		const Kinematic *character;
		Goal goal;

		/**
		 * What is the largest priority that a constraint could have and still
		 * affect this path.
		*/
		virtual real getMaxPriority();

	};

	// Forward declaration.
	class SteeringPipe;

	/**
	 * This is the base class for any of the components managed in a steering
	 * pipeline. It keeps track of communication with the steering pipeline.
	 */
	class SteeringPipeComponent
	{
	protected:
		friend SteeringPipe;
		SteeringPipe *pipe;
	};

	/**
	 * A targeter tells the system where the character should be going.
	 */
	class Targeter : public SteeringPipeComponent
	{
	public:
		/**
		 * Returns the goal that this targeter is heading for.
		 */
		virtual Goal getGoal() = 0;
	};

	/**
	 * A decomposer takes a goal and decomposes it into another goal.
	 */
	class Decomposer : public SteeringPipeComponent
	{
	public:
		/**
		 * Decomposes the given goal into a sub-goal.
		 */
		virtual Goal decomposeGoal(const Goal& goal) = 0;
	};

	/**
	 * A Constraint takes a path to a goal, determines if it violates
	 * the constraint, then returns an action to avoid the constraint.
	 */
	class Constraint : public SteeringPipeComponent
	{
	public:
		/**
		 * The steering pipe sets this to be true if the constraint was asked
		 * to suggest its goal. This is mostly used for debugging and feedback: to
		 * see what constraints influenced the final result.
		 */
		bool suggestionUsed;

		/**
		 * Checks if the given path, to the given goal will violate this 
		 * constraint. 
		 *
		 * @returns The priority of this constraint. This could potentially
		 * be any value, where the lower the value the higher priority. Normally,
		 * however, it is interpreted as the distance along the path when the 
		 * violation will occur. A result of REAL_MAX indicates no violation will occur.
		 *
		 * @param maxPriority This indicates the maximum priority to consider. If the 
		 * constraint can't generate a violation with a lower priority, then it
		 * can give up looking. For algorithms that look progressively further down the
		 * path, they can stop looking at a distance equal to this value and return 
		 * REAL_MAX.
		 */
		virtual real willViolate(const Path* path, real maxPriority) = 0;

		/**
		 * Suggests a new sub-goal that the character could head for that
		 * wouldn't violate this constraint.
		 */
		virtual Goal suggest(const Path* path) = 0;
	};

	/**
	 * An actuator turns a goal into a path: taking the character's capabilities
	 * into account.
	 */
	class Actuator : public SteeringPipeComponent
	{
	public:
		/**
		 * Creates a new path instance (with no data in it). This allows actuators to
		 * have their own path formats, without the system being tied to a particular 
		 * implementation.
		 */
		virtual Path* createPathObject() = 0;

		/**
		 * Works out the path that needs to be taken to get to the given goal.
		 */
		virtual void getPath(Path* path, const Goal& goal) = 0;

		/**
		 * Works out the steering needed to follow the given path.
		 */
		virtual void getSteering(SteeringOutput* output, const Path* path) = 0;
	};

	/**
	 * The main steering system that processes the steering pipeline and outputs
	 * a steering requirements. This should be capable of managing any set of 
	 * targeters, decomposers, constraints and actuator, although those components 
	 * themselves may have requirements for one another (e.g. the paths output by a 
	 * particular actuator might not be unserstandable by every constraint).
	 */
	class SteeringPipe : public SteeringBehaviour
	{
		Actuator* actuator;

	public:
		std::list<Targeter*> targeters;
		std::list<Decomposer*> decomposers;
		std::list<Constraint*> constraints;

		/**
		 * Holds the maximum number of steps to try before giving up on the 
		 * constraint resolution.
		 */
		unsigned constraintSteps;

		/**
		 * Holds the steering behaviour to run if the pipeline fails.
		 */
		SteeringBehaviour * fallback;

		/**
		 * Holds the final path that was generated by the system. This is written
		 * to each time the code is run.
		 */
		Path * path;

		/**
		 * Creates a new steering pipeline.
		 */
		SteeringPipe();
		~SteeringPipe();

		/**
		 * Sets the actuator for this pipe.
		 */
		void setActuator(Actuator* actuator);

		/**
		 * Returns the steering output after processing the pipeline.
		 */
		void getSteering(SteeringOutput* output);

		/**
		 * Call this method to initialise all the components after you have
		 * added them, and before you call getSteering. This only needs to
		 * be done after you change the lists of targeters, decomposers or
		 * constraints, or if you change actuator.
		 */
		void registerComponents();
	};


	// --------------------------------------------------------------------------
	// An implementation of steering pipeline components based on very simple 
	// position-seeking.

	/**
	 * This targeter has a static goal that it always tries to achieve.
	 */
	class FixedGoalTargeter : public Targeter
	{
	public:
		Goal goal;
		virtual Goal getGoal();
	};

	/**
	 * This constraint tries to avoid a set of spherical exclusion zones.
	 * It is designed for use with the basic Path objects, and only uses
	 * the direct line between the character and its goal. More complex
	 * paths are ignored.
	 */
	class AvoidSpheresConstraint : public Constraint
	{
		/**
		 * As in many constraints, the suggested sub-goal is calculated
		 * when the constraint determines whether it is violated. It is
		 * stored here so that the 'suggest' method returns very quickly.
		 */
		Goal suggestion;

		/** 
		 * Checks for violation on one obstacle.
		 */
		real willViolate(
			const Path* path, 
			real maxPriority, 
			Sphere &obstacle
			);
	public:
		/**
		 * Holds the list of obstacles to avoid.
		 */
		std::list<Sphere*> obstacles;

		/**
		 * How much margin to avoid the obstacle by.
		 */
		real avoidMargin;

		virtual real willViolate(const Path* path, real maxPriority);
		virtual Goal suggest(const Path* path);
	};
	
	/**
	 * This actuator creates a path to its goal in a straight line directly
	 * to its destination. It uses the base class Path for its path object.
	 */
	class BasicActuator : public Actuator
	{
		/** The seek behaviour performs the actuation for us. */
		Seek seek;

	public:
		/** The maximum acceleration force the character can muster. */
		real maxAcceleration;

		virtual Path* createPathObject();
		virtual void getPath(Path* path, const Goal& goal);
		virtual void getSteering(SteeringOutput* output, const Path* path);
	};

}; // end of namespace

#endif // AICORE_STEERPIPR_H
