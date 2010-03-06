/*
 * The flocking demo.
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
#include <stdio.h>

#include <gl/glut.h>
#include <aicore/aicore.h>

#include "../common/gl/app.h"
#include "flock_steer.h"

// This is the size of the world in both directions from 0 (i.e. from
// -WORLD_SIZE to +WORLD_SIZE)
#define WORLD_SIZE 50

// This is the gap between grid lines
#define GRID_SIZE 5

// The number of boids in the simulation
#define BOIDS 30

// The relative blend weights
#define SEPARATION_WEIGHT ((aicore::real)1.0)
#define COHESION_WEIGHT ((aicore::real)1.0)
#define VMA_WEIGHT ((aicore::real)2.0)

/**
 * The main demo class.
 */
class FlockingDemo : public Application
{
    /** Holds the kinematic of all the boids. */
    aicore::Kinematic *kinematic;

	/** Holds the flock */
	Flock flock;


    /** Holds the enabled state of each behaviour. */
    bool separationOn;
    bool cohesionOn;
    bool velocityMatchOn;

    /** Holds the steering behaviours. */
	Separation *separation;
	Cohesion *cohesion;
	VelocityMatchAndAlign *vMA;
	aicore::BlendedSteering *steering;

public:
    FlockingDemo();
    virtual ~FlockingDemo();

    virtual bool key(unsigned char key);
    virtual void update();
    virtual void display();
    virtual const char* getTitle();
    virtual void getHelpText(const char*** lines, unsigned *lineCount);
    virtual unsigned getStatusCount();
    virtual const char* getStatusText(unsigned slot);
};


FlockingDemo::FlockingDemo()
    :
    Application(),
    separationOn(true), cohesionOn(true), velocityMatchOn(true)
{
    static const aicore::real accel = (aicore::real)20.0;

    // Set up the kinematics and all individual behaviours
    kinematic = new aicore::Kinematic[BOIDS];

    for (unsigned i = 0; i < BOIDS; i++)
    {
        kinematic[i].position.x = aicore::randomBinomial(WORLD_SIZE);
        kinematic[i].position.y = (aicore::real)0;
        kinematic[i].position.z = aicore::randomBinomial(WORLD_SIZE);
        kinematic[i].orientation = aicore::randomReal(aicore::M_PI);
        kinematic[i].velocity.x = aicore::randomBinomial((aicore::real)1.0);
        kinematic[i].velocity.y = (aicore::real)0;
        kinematic[i].velocity.z = aicore::randomReal((aicore::real)25.0);
        kinematic[i].rotation = (aicore::real)0;

		flock.boids.push_back(kinematic+i);
    }

	// Set up the steering behaviours (we use one for all)
	separation = new Separation;
	separation->maxAcceleration = accel;
	separation->neighbourhoodSize = (aicore::real)5.0;
	separation->neighbourhoodMinDP = (aicore::real)-1.0;
	separation->theFlock = &flock;

	cohesion = new Cohesion;
	cohesion->maxAcceleration = accel;
	cohesion->neighbourhoodSize = (aicore::real)10.0;
	cohesion->neighbourhoodMinDP = (aicore::real)0.0;
	cohesion->theFlock = &flock;

	vMA = new VelocityMatchAndAlign;
	vMA->maxAcceleration = accel;
	vMA->neighbourhoodSize = (aicore::real)15.0;
	vMA->neighbourhoodMinDP = (aicore::real)0.0;
	vMA->theFlock = &flock;

	steering = new aicore::BlendedSteering;
	steering->behaviours.push_back(aicore::BlendedSteering::BehaviourAndWeight(
		separation, SEPARATION_WEIGHT
		));
	steering->behaviours.push_back(aicore::BlendedSteering::BehaviourAndWeight(
		cohesion, COHESION_WEIGHT
		));
	steering->behaviours.push_back(aicore::BlendedSteering::BehaviourAndWeight(
		vMA, VMA_WEIGHT
		));

}

FlockingDemo::~FlockingDemo()
{
    delete[] kinematic;

	delete separation;
	delete cohesion;
	delete vMA;
}

static const char *defaultHelp[] = {
    "AI4G: Flocking Demo",
    "H - Toggle help.",
    "",
    "Toggle behaviour element:",
    "Q - Separation",
    "W - Cohesion",
    "E - Velocity Match / Align",
};

void FlockingDemo::getHelpText(const char*** lines,
                                        unsigned *lineCount)
{
    *lines = defaultHelp;
    *lineCount = sizeof(defaultHelp) / sizeof(const char*);
}

const char* FlockingDemo::getTitle()
{
    return defaultHelp[0];
}

bool FlockingDemo::key(unsigned char key)
{
    if (Application::key(key)) return true;

    switch(key) {
    case 'Q': case 'q':
        separationOn = !separationOn;
		steering->behaviours[0].weight = separationOn?SEPARATION_WEIGHT:0;
        return true;
    case 'W': case 'w':
        cohesionOn = !cohesionOn;
		steering->behaviours[1].weight = cohesionOn?COHESION_WEIGHT:0;
        return true;
    case 'E': case 'e':
        velocityMatchOn = !velocityMatchOn;
		steering->behaviours[2].weight = velocityMatchOn?VMA_WEIGHT:0;
        return true;
    }

    return false;
}

void FlockingDemo::display()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();
    gluLookAt(-53.0f, 53.0f, 0.0f,
              0.0f, -30.0f, 0.0f,
              0.0f, 1.0f, 0.0f);

    // Draw the characters.
	glColor3f(0.0f, 0.3f, 0.6f);
    for (unsigned i = 0; i < BOIDS; i++) {
		renderAgent(kinematic[i]);
    }

    // Draw some scale lines
    glColor3f(0.8, 0.8, 0.8);
    glBegin(GL_LINES);
    for (int i = -WORLD_SIZE; i <= WORLD_SIZE; i += GRID_SIZE) {

        glVertex3i(-WORLD_SIZE, -1, i);
        glVertex3i(WORLD_SIZE, -1, i);

        glVertex3i(i, -1, WORLD_SIZE);
        glVertex3i(i, -1, -WORLD_SIZE);
    }
    glEnd();

    // Draw the help (the method decides if it should be displayed)
    displayHelp();
}

#define TRIM_WORLD(var) \
    if (var < -WORLD_SIZE) var = WORLD_SIZE; \
    if (var > WORLD_SIZE) var = -WORLD_SIZE;

void FlockingDemo::update()
{
    float duration = aicore::TimingData::get().lastFrameDuration * 0.001f;
    aicore::SteeringOutput steer;
    aicore::SteeringOutput temp;

    for (unsigned i = 0; i < BOIDS; i++) 
	{
		// Get the steering output
		steering->character = kinematic+i;
		steering->getSteering(&steer);

        // Update the kinematic
        kinematic[i].integrate(steer, (aicore::real)0.7, duration);
		kinematic[i].setOrientationFromVelocity();

        // Check for maximum speed
        kinematic[i].trimMaxSpeed((aicore::real)20.0);

        // Keep in bounds of the world
        TRIM_WORLD(kinematic[i].position.x);
        TRIM_WORLD(kinematic[i].position.z);
    }

    glutPostRedisplay();
}

unsigned FlockingDemo::getStatusCount()
{
    return 3;
}

const char* FlockingDemo::getStatusText(unsigned slot)
{
    glColor3f(0.0f, 0.0f, 0.0f);

    // Get the text
    switch(slot)
    {
    case 0:
        if (separationOn) return "Separation";
        else return "";
    case 1:
        if (cohesionOn) return "Cohesion";
        else return "";
    case 2:
        if (velocityMatchOn) return "Velocity Match / Align";
        else return "";
    }
    return "";
}



Application* getApplication()
{
    return new FlockingDemo();
}
