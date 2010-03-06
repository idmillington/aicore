/*
 * The steering behaviour demo.
 *
 * Part of the Artificial Intelligence for Games system.
 *
 * Copyright (c) Ian Millington 2003-2006. All Rights Reserved.
 *
 * This software is distributed under licence. Use of this software
 * implies agreement with all terms and conditions of the accompanying
 * software licence.
 */
#include <cstdlib>
#include <cstdio>

#include <gl/glut.h>
#include <aicore/aicore.h>

#include "../common/gl/app.h"

// This is the size of the world in both directions from 0 (i.e. from
// -WORLD_SIZE to +WORLD_SIZE)
#define WORLD_SIZE 50

// This is the gap between grid lines
#define GRID_SIZE 5

/**
 * The main demo class.
 */
class SteeringDemo : public Application
{
    /** Holds the kinematic of the two agents. */
    aicore::Kinematic *kinematic;

    /** Holds the steering behaviours. */
    aicore::Seek *seek;
    aicore::Flee *flee;
	aicore::Wander *wander;

    /** Holds the currently used behaviours. */
    aicore::SteeringBehaviour **current;
    const char **status;

public:
    SteeringDemo();
    virtual ~SteeringDemo();

    virtual bool key(unsigned char key);
    virtual void update();
    virtual void display();
    virtual const char* getTitle();
    virtual void getHelpText(const char*** lines, unsigned *lineCount);
    virtual unsigned getStatusCount();
    virtual const char* getStatusText(unsigned slot);
};


SteeringDemo::SteeringDemo()
    :
    Application()
{
    static const aicore::real accel = (aicore::real)20.0;

    kinematic = new aicore::Kinematic[2];

    kinematic[0].position.x = (aicore::real)10.0;
    kinematic[0].position.z = (aicore::real)10.0;
    kinematic[0].orientation = (aicore::real)2.1;
    kinematic[0].velocity.clear();
    kinematic[0].rotation = (aicore::real)0;

    kinematic[1].position.x = (aicore::real)-10.0;
    kinematic[1].position.z = (aicore::real)-20.0;
    kinematic[1].orientation = (aicore::real)0.75;
    kinematic[1].velocity.clear();
    kinematic[1].rotation = (aicore::real)0;

    // Set up all possible behaviours
    seek = new aicore::Seek[2];
    flee = new aicore::Flee[2];
    wander = new aicore::Wander[2];

    for (unsigned i = 0; i < 2; i++) {
        seek[i].character = &kinematic[i];
        seek[i].target = &kinematic[1-i].position;
        seek[i].maxAcceleration = accel;

        flee[i].character = &kinematic[i];
        flee[i].target = &kinematic[1-i].position;
        flee[i].maxAcceleration = accel;

        wander[i].character = &kinematic[i];
        wander[i].maxAcceleration = accel;
		wander[i].volatility = (aicore::real)20.0;
		wander[i].turnSpeed = (aicore::real)2.0;
    }

    // Set the current behaviours
    current = new aicore::SteeringBehaviour*[2];
    status = new const char*[2];

    current[0] = &seek[0];
    status[0] = "Seek";
    current[1] = &flee[1];
    status[1] = "Flee";
}

SteeringDemo::~SteeringDemo()
{
    delete[] kinematic;

    delete[] seek;
    delete[] flee;
    delete[] wander;

    delete[] current;
    delete[] status;
}

static const char *defaultHelp[] = {
    "AI4G: Steering Demo",
    "H - Toggle help.",
    "",
    "Red character:",
    "Q - Stationary",
    "W - Seek",
    "E - Flee",
    "R - Wander",
    "",
    "Green character:",
    "A - Stationary",
    "S - Seek",
    "D - Flee",
	"F - Wander",
};

void SteeringDemo::getHelpText(const char*** lines,
                                        unsigned *lineCount)
{
    *lines = defaultHelp;
    *lineCount = sizeof(defaultHelp) / sizeof(const char*);
}

const char* SteeringDemo::getTitle()
{
    return defaultHelp[0];
}

bool SteeringDemo::key(unsigned char key)
{
    if (Application::key(key)) return true;

    switch(key) {
    case 'Q': case 'q':
        current[0] = NULL;
        status[0] = "Stationary";
        return true;
    case 'W': case 'w':
        current[0] = &seek[0];
        status[0] = "Seek";
        return true;
    case 'E': case 'e':
        current[0] = &flee[0];
        status[0] = "Flee";
        return true;
	case 'R': case 'r':
		current[0] = &wander[0];
		status[0] = "Wander";
		return true;

    case 'A': case 'a':
        current[1] = NULL;
        status[1] = "Stationary";
        return true;
    case 'S': case 's':
        current[1] = &seek[1];
        status[1] = "Seek";
        return true;
    case 'D': case 'd':
        current[1] = &flee[1];
        status[1] = "Flee";
        return true;
	case 'F': case 'f':
		current[1] = &wander[1];
		status[1] = "Wander";
		return true;
    }

    return false;
}

void SteeringDemo::display()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();
    gluLookAt(-53.0f, 53.0f, 0.0f,
              0.0f, -30.0f, 0.0f,
              0.0f, 1.0f, 0.0f);

    // Draw the characters.
    glColor3f(0.6f, 0.0f, 0.0f);
    renderAgent(kinematic[0]);
    glColor3f(0.0f, 0.6f, 0.0f);
    renderAgent(kinematic[1]);

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

void SteeringDemo::update()
{
    float duration = aicore::TimingData::get().lastFrameDuration * 0.001f;
    aicore::SteeringOutput steer;

    for (unsigned i = 0; i < 2; i++) {

        // Update the steering if we have a movement algorithm
        if (current[i]) current[i]->getSteering(&steer);
        else steer.clear();

        // Update the kinematic
        kinematic[i].integrate(steer, (aicore::real)0.95, duration);
        kinematic[i].setOrientationFromVelocity();

        // Check for maximum speed
        kinematic[i].trimMaxSpeed((aicore::real)20.0);

        // Keep in bounds of the world
        TRIM_WORLD(kinematic[i].position.x);
        TRIM_WORLD(kinematic[i].position.z);
    }

    glutPostRedisplay();
}

unsigned SteeringDemo::getStatusCount()
{
    return 2;
}

const char* SteeringDemo::getStatusText(unsigned slot)
{
    // Set the color
    switch(slot)
    {
    case 0:
        glColor3f(0.6f, 0.0f, 0.0f);
        break;
    case 1:
        glColor3f(0.0f, 0.6f, 0.0f);
        break;
    }

    // Return the text
    return status[slot];
}


Application* getApplication()
{
    return new SteeringDemo();
}
