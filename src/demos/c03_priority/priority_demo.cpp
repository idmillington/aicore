/*
 * The steering priority demo.
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

// This is the size of the world in both directions from 0 (i.e. from
// -WORLD_SIZE to +WORLD_SIZE)
#define WORLD_SIZE 50

// This is the gap between grid lines
#define GRID_SIZE 5

// The number of obstacles in the simulation
#define OBSTACLES 10

/**
 * The main demo class.
 */
class SteeringPriorityDemo : public Application
{
    /** Holds the kinematic of the character. */
    aicore::Kinematic *kinematic;

    /** Holds the obstacles */
	aicore::Sphere *obstacles;

    /** Holds the type of blending being done. */
    bool isBlended;

    /** Holds the steering behaviours. */
    aicore::AvoidSphere *avoid;
    aicore::Wander *wander;
	aicore::BlendedSteering *blended;
	aicore::PrioritySteering *priority;

public:
    SteeringPriorityDemo();
    virtual ~SteeringPriorityDemo();

    virtual bool key(unsigned char key);
    virtual void update();
    virtual void display();
    virtual const char* getTitle();
    virtual void getHelpText(const char*** lines, unsigned *lineCount);
    virtual unsigned getStatusCount();
    virtual const char* getStatusText(unsigned slot);
};


SteeringPriorityDemo::SteeringPriorityDemo()
    :
    Application(),
    isBlended(false)
{
    static const aicore::real accel = (aicore::real)50.0;

    // Set up the kinematics and character steering behaviours
    kinematic = new aicore::Kinematic;
	kinematic->position.x = aicore::randomBinomial((aicore::real)5.0);
	kinematic->position.y = (aicore::real)0;
	kinematic->position.z = aicore::randomBinomial((aicore::real)5.0);
	kinematic->orientation = aicore::randomReal(aicore::M_PI);
	kinematic->velocity.x = aicore::randomBinomial((aicore::real)1.0);
	kinematic->velocity.y = (aicore::real)0;
	kinematic->velocity.z = aicore::randomReal((aicore::real)5.0);
	kinematic->rotation = (aicore::real)0;
	
	wander = new aicore::Wander;
	wander->volatility = (aicore::real)20.0;
	wander->maxAcceleration = accel;
	wander->turnSpeed = (aicore::real)2.0;
	wander->character = kinematic;

	avoid = new aicore::AvoidSphere[OBSTACLES];

    // Set up the obstacles and the avoid behaviours
    obstacles = new aicore::Sphere[OBSTACLES];
    for (unsigned i = 0; i < OBSTACLES; i++) {
        obstacles[i].position.x = -WORLD_SIZE + 4 + aicore::randomReal(2*WORLD_SIZE-8);
        obstacles[i].position.y = (aicore::real)0;
        obstacles[i].position.z = -WORLD_SIZE + 4 + aicore::randomReal(2*WORLD_SIZE-8);
        obstacles[i].radius = aicore::randomReal((aicore::real)2.0) + (aicore::real)4.0;

		avoid[i].obstacle = &obstacles[i];
		avoid[i].character = kinematic;
		avoid[i].maxAcceleration = accel;
		avoid[i].avoidMargin = (aicore::real)2.0;
		avoid[i].maxLookahead = (aicore::real)20.0;
    }

	// Set up the priority and blended structures
	blended = new aicore::BlendedSteering();
	blended->character = kinematic;
	priority = new aicore::PrioritySteering();
	priority->epsilon = (aicore::real)0.01;
	priority->character = kinematic;

	for (unsigned i = 0; i < OBSTACLES; i++)
	{
		blended->behaviours.push_back(
			aicore::BlendedSteering::BehaviourAndWeight(avoid + i)
			);
		priority->behaviours.push_back(avoid + i);
	}
	blended->behaviours.push_back(
		aicore::BlendedSteering::BehaviourAndWeight(wander, (aicore::real)OBSTACLES)
		);
	priority->behaviours.push_back(wander);

}

SteeringPriorityDemo::~SteeringPriorityDemo()
{
    delete kinematic;
	delete wander;
	delete[] avoid;

    delete[] obstacles;
}

static const char *defaultHelp[] = {
    "AI4G: Steering Priority Demo",
    "H - Toggle help.",
    "",
    "B - Toggle behaviour blending / priority",
};

void SteeringPriorityDemo::getHelpText(const char*** lines,
                                        unsigned *lineCount)
{
    *lines = defaultHelp;
    *lineCount = sizeof(defaultHelp) / sizeof(const char*);
}

const char* SteeringPriorityDemo::getTitle()
{
    return defaultHelp[0];
}

bool SteeringPriorityDemo::key(unsigned char key)
{
    if (Application::key(key)) return true;

    switch(key) {
    case 'B': case 'b':
        isBlended = !isBlended;
        return true;
    }

    return false;
}

void SteeringPriorityDemo::display()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();
    gluLookAt(-53.0f, 53.0f, 0.0f,
              0.0f, -30.0f, 0.0f,
              0.0f, 1.0f, 0.0f);

    // Draw the character
    glColor3f(0.6f, 0.0f, 0.0f);
    renderAgent(*kinematic);

    // Create the obstacle quadric
    GLUquadricObj* qobj = gluNewQuadric();

    // Draw the obstacles
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glColor3f(0.4f, 0.4f, 0.4f);
    for (unsigned i = 0; i < OBSTACLES; i++) {
		const aicore::Sphere &o = obstacles[i];
		
        glPushMatrix();
        glTranslatef(o.position.x, o.position.y, o.position.z);
        glRotatef(90, -1.0f, 0.0f, 0.0f);
        gluCylinder(qobj,
                    o.radius, o.radius*0.85f,
                    1.0f,
                    36, 1);
        glTranslatef(0.0f, 0.0f, 1.0f);
        gluDisk(qobj, 0.0f, o.radius*0.85f, 36, 1);
        glPopMatrix();
	}
    glDisable(GL_LIGHTING);

	// Free the quadric
	gluDeleteQuadric(qobj);

	// Draw a spot where the avoid steering's target was.
	if (!isBlended && priority->lastUsed != wander)
	{
		glColor3f(1,0,0);
		renderSpot(*((aicore::Seek*)priority->lastUsed)->target);
	}

	// Draw a spot where the wander wants to go.
	glColor3f(0,0.5f,0);
	renderSpot(*wander->target);

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

void SteeringPriorityDemo::update()
{
    float duration = aicore::TimingData::get().lastFrameDuration * 0.001f;
    aicore::SteeringOutput wanderOutput;
    aicore::SteeringOutput avoidOutput;
    aicore::SteeringOutput steer;

    // Start with no steering
	avoidOutput.clear();
    steer.clear();

	// Calculate the individual steering outputs
	wander->getSteering(&wanderOutput);

    // Do we blend or fall-back
    if (isBlended) {
		blended->getSteering(&steer);
    } else {
		priority->getSteering(&steer);
    }

    // Update the kinematic
    kinematic->integrate(steer, (aicore::real)0.1, duration);
    kinematic->setOrientationFromVelocity();

    // Check for maximum speed
    kinematic->trimMaxSpeed((aicore::real)20.0);

    // Keep in bounds of the world
    TRIM_WORLD(kinematic->position.x);
    TRIM_WORLD(kinematic->position.z);

    // Avoid penetration of obstacles
    for (unsigned i = 0; i < OBSTACLES; i++)
    {
		aicore::Sphere &o = obstacles[i];
		aicore::real distance = kinematic->position.distance(o.position);
		if (distance < o.radius + 1.0f)
		{
			aicore::Vector3 offset = o.position - kinematic->position;
			offset.normalise();
			offset *= o.radius + 1.0f;
			kinematic->position = o.position - offset;
		}
    }

    glutPostRedisplay();
}

unsigned SteeringPriorityDemo::getStatusCount()
{
    if (isBlended) return 1;
    else return OBSTACLES + 1;
}

const char* SteeringPriorityDemo::getStatusText(unsigned slot)
{

    if (isBlended) {
        glColor3f(0.0f, 0.0f, 0.0f);
        return "Blended";
    } else {
        glColor3f(0.8f, 0.8f, 0.8f);
		if (slot < OBSTACLES)
		{
			if (priority->lastUsed == avoid+slot) glColor3f(0.6f,0,0);
			return "Avoid Obstacle";
		}
		else
		{
			if (priority->lastUsed == wander) glColor3f(0.6f,0,0);
			return "Wandering";
		}
    }
    return "";
}


Application* getApplication()
{
    return new SteeringPriorityDemo();
}
