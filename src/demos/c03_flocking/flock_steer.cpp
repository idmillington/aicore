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
#include "flock_steer.h"
#include <memory.h>

Flock::Flock()
:
inNeighbourhood(0), arraySize(0)
{}

unsigned Flock::prepareNeighourhood(
    const aicore::Kinematic* of, 
	aicore::real size, 
	aicore::real minDotProduct /* = -1.0 */)
{
	// Make sure the array is of the correct size
	if (arraySize != boids.size())
	{
		if (arraySize) delete[] inNeighbourhood;
		arraySize = boids.size();
		if (arraySize) inNeighbourhood = new bool[arraySize];
		memset(inNeighbourhood, 0, sizeof(bool)*arraySize);
	}

	// Compile the look vector if we need it
	aicore::Vector3 look;
	if (minDotProduct > -1.0f)
	{
		look = of->getOrientationAsVector();
	}

	Flock result;
	std::list<aicore::Kinematic*>::iterator bi;
	unsigned i = 0, count = 0;;
	for (bi = boids.begin(); bi != boids.end(); bi++, i++)
	{
		aicore::Kinematic *k = *bi;
		inNeighbourhood[i] = false;

		// Ignore ourself
		if (k == of) continue;

		// Check for maximum distance
		if (k->position.distance(of->position) > size) continue;

		// Check for angle
		if (minDotProduct > -1.0)
		{
			aicore::Vector3 offset = k->position - of->position;
			if (look * offset.unit() < minDotProduct)
			{
				continue;
			}
		}

		// If we get here we've passed all tests
		inNeighbourhood[i] = true;
		count ++;
	}
	return count;
}

aicore::Vector3 Flock::getNeighbourhoodCenter()
{
	aicore::Vector3 center;
	std::list<aicore::Kinematic*>::iterator bi;
	unsigned i = 0;
	unsigned count = 0;
	for (bi = boids.begin(); bi != boids.end(); bi++, i++)
	{
		if (inNeighbourhood[i]) 
		{
			center += (*bi)->position;
			count ++;
		}
	}
	center *= 1.0 / (aicore::real)count;

	return center;
}

aicore::Vector3 Flock::getNeighbourhoodAverageVelocity()
{
	aicore::Vector3 center;
	std::list<aicore::Kinematic*>::iterator bi;
	unsigned i = 0;
	unsigned count = 0;
	for (bi = boids.begin(); bi != boids.end(); bi++, i++)
	{
		if (inNeighbourhood[i]) 
		{
			center += (*bi)->velocity;
			count ++;
		}
	}
	center *= 1.0 / (aicore::real)count;

	return center;
}

void Separation::getSteering(aicore::SteeringOutput* output)
{
	// Get the neighbourhood of boids
	unsigned count = theFlock->prepareNeighourhood(
		character, neighbourhoodSize, neighbourhoodMinDP
		);
	if (count <= 0) return;

	// Work out their center of mass
	aicore::Vector3 cofm = theFlock->getNeighbourhoodCenter();

	// Steer away from it.
	flee.maxAcceleration = maxAcceleration;
	flee.character = character;
	flee.target = &cofm;
	flee.getSteering(output);
	
}

void Cohesion::getSteering(aicore::SteeringOutput* output)
{
	// Get the neighbourhood of boids
	unsigned count = theFlock->prepareNeighourhood(
		character, neighbourhoodSize, neighbourhoodMinDP
		);
	if (count <= 0) return;

	// Work out their center of mass
	aicore::Vector3 cofm = theFlock->getNeighbourhoodCenter();

	// Steer away from it.
	seek.maxAcceleration = maxAcceleration;
	seek.character = character;
	seek.target = &cofm;
	seek.getSteering(output);
}

void VelocityMatchAndAlign::getSteering(aicore::SteeringOutput* output)
{
	// Get the neighbourhood of boids
	unsigned count = theFlock->prepareNeighourhood(
		character, neighbourhoodSize, neighbourhoodMinDP
		);
	if (count <= 0) return;

	// Work out their center of mass
	aicore::Vector3 vel = theFlock->getNeighbourhoodAverageVelocity();

	// Try to match it
	output->linear = vel - character->velocity;
	if (output->linear.squareMagnitude() > maxAcceleration*maxAcceleration)
	{
		output->linear.normalise();
		output->linear *= maxAcceleration;
	}
}
