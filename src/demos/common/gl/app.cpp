/*
 * The definition file for the default application object.
 *
 * Part of the Artificial Intelligence for Games system.
 *
 * Copyright (c) Ian Millington 2003-2006. All Rights Reserved.
 *
 * This software is distributed under licence. Use of this software
 * implies agreement with all terms and conditions of the accompanying
 * software licence.
 */
#include <stdio.h>
#include <gl/glut.h>
#include "app.h"

Application::Application()
    :
    help(false)
{
}

void Application::initGraphics()
{
    glClearColor(0.9,0.95,1.0,1.0);
    glEnable(GL_DEPTH_TEST);
    glShadeModel(GL_SMOOTH);

    setView();
}

void Application::setView()
{
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(60.0, float(width)/float(height), 1, 500.0);
    glMatrixMode(GL_MODELVIEW);
}

void Application::display()
{
    glClear(GL_COLOR_BUFFER_BIT);

    glBegin(GL_LINES);
    glVertex2i(1, 1);
    glVertex2i(639, 319);
    glEnd();

    displayHelp();
}

const char* Application::getTitle()
{
    return "Artificial Intelligence for Games Demo";
}

void Application::deinit()
{
}

void Application::update()
{
    glutPostRedisplay();
}

bool Application::key(unsigned char key)
{
    switch(key)
    {
    case 'h': case 'H':
        help = !help;
        return true;
    }

    return false;
}

void Application::resize(int width, int height)
{
    // Avoid the divide by zero.
    if (height <= 0) height = 1;

    // Set the internal variables and update the view
    Application::width = width;
    Application::height = height;
    glViewport(0, 0, width, height);
    setView();
}

void Application::mouse(int button, int state, int x, int y)
{
}

static const char *defaultHelp[] = {
    "No help is available."
};

void Application::getHelpText(const char*** lines, unsigned *lineCount)
{
    *lines = defaultHelp;
    *lineCount = sizeof(defaultHelp) / sizeof(const char*);
}

unsigned Application::getStatusCount()
{
    return 0;
}

const char* Application::getStatusText(unsigned slot)
{
    return "NONE";
}

// Implementations

void Application::renderAgent(const aicore::Location &loc)
{
    glPushMatrix();
    glTranslatef(loc.position.x, loc.position.y, loc.position.z);
    glRotatef(loc.orientation * aicore::M_180_PI, 0.0f, 1.0f, 0.0f);
    glutSolidSphere(1.0f, 12, 12);
    glBegin(GL_TRIANGLES);
    glNormal3f(0.0f, 1.0f, 1.0f);
    glVertex3f(0.5f, 0.0f, 1.0f);
    glVertex3f(0.0f, 0.0f, 2.0f);
    glVertex3f(-0.5f, 0.0f, 0.0f);
    glEnd();
    glPopMatrix();
}

void Application::renderSpot(const aicore::Vector3 &pos)
{
	glPushMatrix();
	glTranslatef(pos.x, pos.y, pos.z);
	glutSolidSphere(0.5f, 12, 12);
	glPopMatrix();
}

static const char *helpHidden[] = {
    "Press H for help."
};

void Application::displayHelp()
{
    const char** helpText = helpHidden;
    unsigned lines = 1;

    // Retrieve the text for this application
    if (help) {
        getHelpText(&helpText, &lines);
    }

    // Set up 2D mode
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    glOrtho(0, width, 0, height, -1, 1);

    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();

    // Render the text
    glColor3f(0,0,0);
    for (unsigned i = 0; i < lines; i++)
    {
        glRasterPos2i(10, height - 20 - i*15);

        const char *c = helpText[i];
        while (*c) {
            glutBitmapCharacter(GLUT_BITMAP_HELVETICA_10, *c);
            c++;
        }
    }

    // Render the status slots (always done)
    unsigned slots = getStatusCount();
    for (unsigned i = 0; i < slots; i++)
    {
        const char *text = getStatusText(i);
        int textWidth = glutBitmapLength(GLUT_BITMAP_HELVETICA_10,
                                         (const unsigned char*)text);

        // Right align the string
        glRasterPos2i(width-10-textWidth, height-20-i*15);

        // And render
        const char *c = text;
        while (*c) {
            glutBitmapCharacter(GLUT_BITMAP_HELVETICA_10, *c);
            c++;
        }
    }


    // Break down 2D mode
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();
}
