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
#include <stdlib.h>
#include <aicore/aicore.h>

#include <cassert>

namespace aicore 
{
	Goal::Goal()
	{
		clear();
	}

	void Goal::clear()
	{
		positionSet = orientationSet = velocitySet = rotationSet = false;
	}

	void Goal::updateGoal(const Goal& goal)
	{
		// Make sure we can merge
		assert(canMergeGoals(goal));

		if (goal.positionSet) 
		{
			position = goal.position;
			positionSet = true;
		}
		if (goal.orientationSet) 
		{
			orientation = goal.orientation;
			orientationSet = true;
		}
		if (goal.velocitySet) 
		{
			velocity = goal.velocity;
			velocitySet = true;
		}
		if (goal.rotationSet) 
		{
			rotation = goal.rotation;
			rotationSet = true;
		}
	}

	bool Goal::canMergeGoals(const Goal& goal) const
	{
		return !(
			positionSet && goal.positionSet ||
			orientationSet && goal.orientationSet ||
			velocitySet && goal.velocitySet ||
			rotationSet && goal.rotationSet
			);
	}

	real Path::getMaxPriority()
	{
		return (character->position - goal.position).magnitude();
	}

	SteeringPipe::SteeringPipe()
		:
		fallback(0),
		constraintSteps(100),
		path(0)
	{
    }

	SteeringPipe::~SteeringPipe()
	{
		if (path) delete path;
	}

	void SteeringPipe::setActuator(aicore::Actuator *a)
	{
		actuator = a;
		if (path) delete path;
		path = 0;
	}

	void SteeringPipe::getSteering(SteeringOutput* output)
	{
		Goal goal;
		std::list<Targeter*>::iterator ti;
		for (ti = targeters.begin(); ti != targeters.end(); ti++)
		{
			Goal targeterResult = (*ti)->getGoal();
			if (goal.canMergeGoals(targeterResult)) 
			{
				goal.updateGoal(targeterResult);
			}
		}

		std::list<Decomposer*>::iterator di;
		for (di = decomposers.begin(); di != decomposers.end(); di++)
		{
			goal = (*di)->decomposeGoal(goal);
		}

		// Create an ampty path object of the correct type.
		if (!path) path = actuator->createPathObject();

		std::list<Constraint*>::iterator ci;
		real shortestViolation, currentViolation, maxViolation;
		Constraint *violatingConstraint;
		for (unsigned i = 0; i < constraintSteps; i++)
		{
			// Find the path to this goal
			actuator->getPath(path, goal);

			// Find the constraint that is violated first
			maxViolation = shortestViolation = path->getMaxPriority();
			for (ci = constraints.begin(); ci != constraints.end(); ci++)
			{	
				// Clear the flags that indicate the constraints used
				if (i == 0) (*ci)->suggestionUsed = false;

				// Check to see if this constraint is violated early than any other.
				currentViolation = (*ci)->willViolate(path, shortestViolation);
				if (currentViolation > 0 && currentViolation < shortestViolation)
				{
					shortestViolation = currentViolation;
					violatingConstraint = *ci;
				}
			}

			// Check if we found a violation
			if (shortestViolation < maxViolation)
			{
				// Update the goal and check constraints again.
				goal = violatingConstraint->suggest(path);
				violatingConstraint->suggestionUsed = true;
			}
			else
			{
				// We've found a solution - use it and return
				actuator->getSteering(output, path);
				return;
			}
		}

		// We've run out of constraint iterations, so use the fallback
		if (fallback) fallback->getSteering(output);
	}

	void SteeringPipe::registerComponents()
	{
		std::list<Targeter*>::iterator ti;
		for (ti = targeters.begin(); ti != targeters.end(); ti++)
		{
			(*ti)->pipe = this;
		}

		std::list<Decomposer*>::iterator di;
		for (di = decomposers.begin(); di != decomposers.end(); di++)
		{
			(*di)->pipe = this;
		}

		std::list<Constraint*>::iterator ci;
		for (ci = constraints.begin(); ci != constraints.end(); ci++)
		{
			(*ci)->pipe = this;
		}

		actuator->pipe = this;
	}

	// --------------------------------------------------------------------------
	// Basic implementations
	Goal FixedGoalTargeter::getGoal()
	{
		return goal;
	}

	real AvoidSpheresConstraint::willViolate(const Path* path, real maxPriority)
	{
		real priority = REAL_MAX;
		real thisPriority;
		std::list<Sphere*>::iterator soi;
		for (soi = obstacles.begin(); soi != obstacles.end(); soi++)
		{
			thisPriority = willViolate(path, priority, *(*soi));
			if (thisPriority < priority) priority = thisPriority;
		}
		return priority;
	}

	real AvoidSpheresConstraint::willViolate(
		const Path* path, 
		real maxPriority, 
		Sphere &obstacle
		)
	{
		// Make sure we've got a positional goal
		if (!path->goal.positionSet) return REAL_MAX;

		// Alias the character object
		const Kinematic *character = pipe->character;

		// Work out where we're going
		Vector3 direction = path->goal.position - character->position;

		// Make sure we're moving
		if (direction.squareMagnitude() > 0)
		{
			// Find the distance from the line we're moving along to the obstacle.
			Vector3 movementNormal = direction.unit();
			Vector3 characterToObstacle = obstacle.position - character->position;

			real distanceSquared = characterToObstacle * movementNormal;
			distanceSquared = characterToObstacle.squareMagnitude() - 
				distanceSquared*distanceSquared;

			// Check for collision
			real radius = obstacle.radius + avoidMargin;
			if (distanceSquared < radius*radius)
			{
				// Find how far along our movement vector the closest pass is
				real distanceToClosest = characterToObstacle * movementNormal;

				// Make sure this isn't behind us and is closer than our lookahead.
				if (distanceToClosest > 0 && distanceToClosest < maxPriority)
				{
					// Find the closest point
					Vector3 closestPoint = 
						character->position + movementNormal*distanceToClosest;

					// Find the point of avoidance
					suggestion.position = 
						obstacle.position +
						(closestPoint - obstacle.position).unit() *
						(obstacle.radius + avoidMargin);
					suggestion.positionSet = true;

					return distanceToClosest;
				}
			}
		}

		return REAL_MAX;
	}

	Goal AvoidSpheresConstraint::suggest(const Path* path)
	{
		return suggestion;
	}

	Path* BasicActuator::createPathObject()
	{
		return new Path;
	}

	void BasicActuator::getPath(Path* path, const Goal& goal)
	{
		path->character = pipe->character;
		path->goal = goal;
	}

	void BasicActuator::getSteering(SteeringOutput* output, const Path* path)
	{
		if (path->goal.positionSet) 
		{
			seek.character = pipe->character;
			seek.target = &path->goal.position;
			seek.maxAcceleration = maxAcceleration;
			seek.getSteering(output);
		}
		else
		{
			output->clear();
		}
	}

} // end of namespace
