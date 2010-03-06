/*
 * The markov state machine demo.
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
#include <stdlib.h>
#include <aicore/aicore.h>


/**
 * Outputs the q-values.
 */
aicore::QLearner *ql;

void displayQValues()
{
    printf("Learning state:\n");

    for (unsigned s = 0; s < 4; s++)
    {
        printf("State %d:", s);

        for (unsigned a = 0; a < 2; a++)
        {
            printf("\taction %d has q %.4f", a, ql->qvalues[s*2+a]);
        }

        printf("\n");
    }
}

/**
 * A simple learning problem that we can use to interactively explore
 * Q-learning.
 */
class SimpleLP : public aicore::ArrayBasedLearningProblem
{
public:
    SimpleLP();

    virtual aicore::LearningProblemActionResult getResult(
        aicore::LearningProblemState* state,
        aicore::LearningProblemAction* action
        );
};

// Holds the array of what actions lead to what
unsigned destinations[] = {
    1, 2,
    2, 0,
    3, 0,
    2, 1
};

SimpleLP::SimpleLP()
    : ArrayBasedLearningProblem(4, 2, destinations, NULL)
{
}

aicore::LearningProblemActionResult SimpleLP::getResult(
    aicore::LearningProblemState* state,
    aicore::LearningProblemAction* action
    )
{
    // Create the result structure
    aicore::LearningProblemActionResult result;

    // Find the destination
    unsigned matrixIndex = state->index*actionsPerState + action->index;
    unsigned di = destination[matrixIndex];
    result.state = &states[di];

    // Find the reward from the user
    displayQValues();
    printf("Enter reward for moving from state %d to %d by action %d\n>",
           state->index,
           result.state->index,
           action->index);

    // Get the user's input
    char buffer[10];
    fgets(buffer, 10, stdin);
    result.reward = atof(buffer);

    return result;
}


/**
 * The main entry point, we set up our state machine here, and then
 * run it.
 */
int main(int argc, char** argv)
{
    SimpleLP slp;

    // Create the action manager, empty initially
    ql = new aicore::QLearner(&slp,
                              (aicore::real)0.4,
                              (aicore::real)0.75,
                              (aicore::real)0.2,
                              (aicore::real)0.1);

    // Now we can run the demo
    printf("AI4G: Simple Q Learning Demo (Ctrl+C to exit)\n");

    // Run a large number of iterations
    ql->learn(1000);
    delete ql;

    // We're fine
    return 0;
}
