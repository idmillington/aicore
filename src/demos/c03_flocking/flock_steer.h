/*
* Steering behaviours for the flocking demo.
*
* Part of the Artificial Intelligence for Games system.
*
* Copyright (c) Ian Millington 2003-2006. All Rights Reserved.
*
* This software is distributed under licence. Use of this software
* implies agreement with all terms and conditions of the accompanying
* software licence.
*/
#ifndef DEMO_FLOCK_STEER_H
#define DEMO_FLOCK_STEER_H

#include <aicore/aicore.h>
#include <list>


/**
 * This class stores a flock of creatures.
 */
class Flock
{
public:
	std::list<aicore::Kinematic*> boids;
	bool *inNeighbourhood;
	unsigned arraySize;

	Flock();

	/**
	 * Sets up the boolean flags of boids in the neighbourhood of the given boid.
	 */
	unsigned prepareNeighourhood(
		const aicore::Kinematic* of,
		aicore::real size,
		aicore::real minDotProduct = -1.0
		);

	/**
	 * Returns the geometric center of the flock.
	 */
	aicore::Vector3 getNeighbourhoodCenter();

	/**
	 * Returns the average velocity of the flock.
	 */
	aicore::Vector3 getNeighbourhoodAverageVelocity();
};

class BoidSteeringBehaviour : public aicore::SteeringBehaviour
{
public:
	Flock *theFlock;
	aicore::real neighbourhoodSize;
	aicore::real neighbourhoodMinDP;
	aicore::real maxAcceleration;
};

class Separation : public BoidSteeringBehaviour
{
	aicore::Flee flee;

public:
	virtual void getSteering(aicore::SteeringOutput* output);
};

class Cohesion : public BoidSteeringBehaviour
{
	aicore::Seek seek;

public:
	virtual void getSteering(aicore::SteeringOutput* output);
};

class VelocityMatchAndAlign : public BoidSteeringBehaviour
{
public:
	virtual void getSteering(aicore::SteeringOutput* output);
};



#endif
