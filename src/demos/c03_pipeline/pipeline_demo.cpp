/*
 * The steering pipeline demo.
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
#define OBSTACLES 20

/**
 * The main demo class.
 */
class SteeringPipelineDemo : public Application
{
    /** Holds the kinematic of all the boids. */
    aicore::Kinematic *kinematic;

	/** Holds the obstacles */
	aicore::Sphere *obstacles;

    /** Holds the steering pipeline. */
    aicore::SteeringPipe *pipe;
	aicore::FixedGoalTargeter *targeter;
	aicore::AvoidSpheresConstraint *constraint;
	aicore::BasicActuator *actuator;

	/* Holds the fallback behaviour. */
	aicore::Wander *wander;

	/** If set, creates a new goal when the character nears the old one. */
	bool autoNewGoal;

public:
    SteeringPipelineDemo();
    virtual ~SteeringPipelineDemo();

    virtual bool key(unsigned char key);
    virtual void update();
    virtual void display();
    virtual const char* getTitle();
    virtual void getHelpText(const char*** lines, unsigned *lineCount);
    virtual unsigned getStatusCount();
    virtual const char* getStatusText(unsigned slot);

	virtual void createRandomGoal();
};


SteeringPipelineDemo::SteeringPipelineDemo()
    :
    Application(),
	autoNewGoal(true)
{
	aicore::real accel = (aicore::real)50.0;

    // Set up the kinematic
    kinematic = new aicore::Kinematic;
    kinematic->position.x = aicore::randomBinomial((aicore::real)5.0);
    kinematic->position.y = (aicore::real)0;
    kinematic->position.z = aicore::randomBinomial((aicore::real)5.0);
    kinematic->orientation = aicore::randomReal(aicore::M_PI);
    kinematic->velocity.x = aicore::randomBinomial((aicore::real)1.0);
    kinematic->velocity.y = (aicore::real)0;
    kinematic->velocity.z = aicore::randomReal((aicore::real)5.0);
    kinematic->rotation = (aicore::real)0;


	// Create the fallback behaviour
	wander = new aicore::Wander;
	wander->maxAcceleration = accel;
	wander->character = kinematic;

	// Create the obstacles
	obstacles = new aicore::Sphere[OBSTACLES];
	for (unsigned i = 0; i < OBSTACLES; i++) {
		obstacles[i].position.x = -WORLD_SIZE + 4 + aicore::randomReal(2*WORLD_SIZE-8);
		obstacles[i].position.y = (aicore::real)0;
		obstacles[i].position.z = -WORLD_SIZE + 4 + aicore::randomReal(2*WORLD_SIZE-8);
		obstacles[i].radius = aicore::randomReal((aicore::real)2.0) + (aicore::real)2.0;
	}

	// Set up the steering pipe
	pipe = new aicore::SteeringPipe;
	pipe->character = kinematic;

	targeter = new aicore::FixedGoalTargeter;
	pipe->targeters.push_back(targeter);

	constraint = new aicore::AvoidSpheresConstraint;
	for (unsigned i = 0; i < OBSTACLES; i++) 
	{
		constraint->obstacles.push_back(obstacles+i);
	}
	constraint->avoidMargin = (aicore::real)2.0;
	pipe->constraints.push_back(constraint);

	actuator = new aicore::BasicActuator;
	actuator->maxAcceleration = (aicore::real)accel;
	pipe->setActuator(actuator);

	pipe->fallback = wander;

	pipe->registerComponents();

	createRandomGoal();
}

SteeringPipelineDemo::~SteeringPipelineDemo()
{
    delete kinematic;

	delete pipe;
	delete actuator;
	delete constraint;
	delete targeter;
	delete wander;

	delete[] obstacles;
}

static const char *defaultHelp[] = {
    "AI4G: Steering Pipeline Demo",
    "H - Toggle help.",
	"",
	"A - Automatically move the goal"
	"N - Choose a new goal"
};

void SteeringPipelineDemo::getHelpText(const char*** lines,
                                        unsigned *lineCount)
{
    *lines = defaultHelp;
    *lineCount = sizeof(defaultHelp) / sizeof(const char*);
}

const char* SteeringPipelineDemo::getTitle()
{
    return defaultHelp[0];
}

bool SteeringPipelineDemo::key(unsigned char key)
{
    if (Application::key(key)) return true;

    switch(key) {
	case 'a': case 'A':
		autoNewGoal = !autoNewGoal;
		return true;

	case 'n': case 'N':
		createRandomGoal();
		return true;
    }

    return false;
}


inline void vectorAsGLVertex(const aicore::Vector3 &v)
{
	glVertex3f(v.x, v.y, v.z);
}


void SteeringPipelineDemo::display()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();
    gluLookAt(-53.0f, 53.0f, 0.0f,
              0.0f, -30.0f, 0.0f,
              0.0f, 1.0f, 0.0f);

    // Draw the characters.
    glColor3f(0.0f, 0.3f, 0.6f);
    renderAgent(*kinematic);

	// Draw the current goal
	glColor3f(0.6f, 0.0f, 0.0f);
	renderSpot(targeter->goal.position);

	// Draw the path
	if (pipe->path)
	{
		const aicore::Path * path = pipe->path;
		glColor3f(0.0f, 0.6f, 0.0f);
		glBegin(GL_LINES);
		vectorAsGLVertex(path->goal.position);
		vectorAsGLVertex(path->character->position);
		glEnd();
	}

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

void SteeringPipelineDemo::update()
{
    float duration = aicore::TimingData::get().lastFrameDuration * 0.001f;
    aicore::SteeringOutput steer;

    pipe->getSteering(&steer);

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

	// Check for proximity to the goal, and create a new one if we are near.
	if (autoNewGoal && 
		kinematic->position.distance(targeter->goal.position) < 2.0f)
	{
		createRandomGoal();
	}

    glutPostRedisplay();
}

unsigned SteeringPipelineDemo::getStatusCount()
{
    return 1;
}

const char* SteeringPipelineDemo::getStatusText(unsigned slot)
{
	if (constraint->suggestionUsed) 
	{
		return "Honouring Constraint";
	}
	else if (pipe->path->goal.position == targeter->goal.position)
	{
		return "Heading for goal";
	}
	else
	{
		return "Wandering";
	}
}

void SteeringPipelineDemo::createRandomGoal()
{
	targeter->goal.positionSet = true;
	bool okay;
	do 
	{
		okay = true;

		// Pick a random location
		targeter->goal.position = aicore::Vector3(
			aicore::randomBinomial(WORLD_SIZE),
			0,
			aicore::randomBinomial(WORLD_SIZE)
			);

		// Make sure it is out of reach of any obstacle
		for (unsigned i = 0; i < OBSTACLES; i++)
		{
			aicore::real distance = 
				(targeter->goal.position - obstacles[i].position).magnitude();

			if (distance < obstacles[i].radius + 2.0f) 
			{
				okay = false;
				break;
			}
		}
	} while (!okay);
}


Application* getApplication()
{
    return new SteeringPipelineDemo();
}
