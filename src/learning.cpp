/*
 * Defines the classes used for learning
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
#include <aicore/aicore.h>

namespace aicore
{
    unsigned LearningProblemAction::getCount()
    {
        if (next == NULL) return 1;
        else return next->getCount() + 1;
    }

    LearningProblemAction *
    LearningProblemAction::getAtPositionInList(unsigned pos)
    {
        if (pos <= 0 || next == NULL) return this;
        else return next->getAtPositionInList(pos-1);
    }


    LearningProblemState* LearningProblem::getInitialState()
    {
        return getRandomState();
    }

    ArrayBasedLearningProblem::ArrayBasedLearningProblem(
        unsigned stateCount,
        unsigned actionsPerState,
        unsigned *destination,
        real *rewards)
            :
            destination(destination),
            rewards(rewards),
            stateCount(stateCount),
            actionsPerState(actionsPerState)
    {
        // Create the arrays
        states = new LearningProblemState[stateCount];
        actions = new LearningProblemAction[actionsPerState];

        // Fill the array values with their index numbers
        for (unsigned i = 0; i < stateCount; i++)
        {
            states[i].index = i;
            states[i].data = NULL;
        }

        for (unsigned i = 0; i < actionsPerState; i++)
        {
            actions[i].index = i;
            actions[i].action = NULL;
            actions[i].next = NULL;
        }
    }

    ArrayBasedLearningProblem::~ArrayBasedLearningProblem()
    {
        delete[] states;
        delete[] actions;
    }

    unsigned ArrayBasedLearningProblem::getStateCount()
    {
        return stateCount;
    }

    unsigned ArrayBasedLearningProblem::getActionCount()
    {
        return actionsPerState;
    }

    LearningProblemState* ArrayBasedLearningProblem::getState(unsigned index)
    {
        return &states[index];
    }

    LearningProblemState* ArrayBasedLearningProblem::getRandomState()
    {
        return &states[randomInt(stateCount)];
    }

    LearningProblemAction*
    ArrayBasedLearningProblem::getActions(LearningProblemState* state)
    {
        LearningProblemAction * action = NULL;
        for (unsigned i = 0; i < actionsPerState; i++)
        {
            // Check if we've got a valid destination
            if (destination[state->index * actionsPerState + i] < 0xffffff)
            {
                // Add to the list
                actions[i].next = action;
                action = &actions[i];
            }
        }

        // Return the list
        return action;
    }

    LearningProblemActionResult ArrayBasedLearningProblem::getResult(
        LearningProblemState* state,
        LearningProblemAction* action)
    {
        // Create the result structure
        LearningProblemActionResult result;

        // Find the destination
        unsigned matrixIndex = state->index*actionsPerState + action->index;
        unsigned di = destination[matrixIndex];
        result.state = &states[di];
        result.reward = rewards[matrixIndex];

        return result;
    }

}; // end of namespace
