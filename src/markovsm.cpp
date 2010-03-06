/*
 * Defines the classes used for markov state machines.
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
#include <string.h>
#include <aicore/aicore.h>

namespace aicore
{
    real * FixedMarkovTransition::getMatrix()
    {
        return matrix;
    }

    void MarkovStateMachine::updateStateVector(real * matrix)
    {
        // Create temporary storage and zero it
        real * tempState = new real[stateVectorSize];
        memset(tempState, 0, sizeof(real) * stateVectorSize);

        // Now work through each element of the matrix
        for (unsigned i = 0; i < stateVectorSize*stateVectorSize; i++)
        {
            // Work out which item in the state vector and target
            // vector we're affecting. Note that if you want the
            // matrix to be post-multiplied, then change the % and /
            // operators in these two lines.
            unsigned stateIndex = i % stateVectorSize;
            unsigned targetIndex = i / stateVectorSize;

            // Add this component
            tempState[targetIndex] += matrix[i] * stateVector[stateIndex];
        }

        // Copy across the new vector's data
        memcpy(stateVector, tempState, sizeof(real) * stateVectorSize);
    }

    Action * MarkovStateMachine::update()
    {
        // The variable to hold the actions to perform
        Action * actions = NULL;

        // Start off with no transition
        MarkovTransition * transition = NULL;

        // Check through each transition in the current state.
        BaseTransition * testTransition = firstTransition;
        while (testTransition != NULL) {
            if (testTransition->isTriggered()) {
                transition = (MarkovTransition*)testTransition;
                break;
            }
            testTransition = testTransition->next;
        }

        // Check if we need to run the default transition
        framesPassed++;
        if (transition == NULL && framesPassed > framesToDefault)
        {
            transition = defaultTransition;
        }

        // Check if we found a transition
        if (transition != NULL) {

            // Find the matrix and update the state vector
            real * matrix = transition->getMatrix();
            updateStateVector(matrix);

            // The actions are those given by the transition
            actions = transition->getActions();

            // Stop the counting
            framesPassed = 0;
        }

        // NB: We don't have an else for what to do if there is no
        // transition, since there is no simple way to map a multitude
        // of states to responsibility for generating actions - its
        // left for the user to do in whatever way makes sense for
        // their application. One simple possibility would be to have
        // a default action to perform if no transitions were fired.

        return actions;
    }

}; // end of namespace
