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


/** Holds the last selected option from the user. */
int option = 0;

/**
 * This is a very simple action that just holds some text to display.
 */
class DemoAction : public aicore::Action
{
public:
    const char *actionText;
    virtual void act();
};

void DemoAction::act()
{
    printf("%s\n", actionText);
}

/**
 * Extends the markov transition with some text, and a string to put into
 * an action.
 */
class DemoMarkovTransition :
    public aicore::FixedMarkovTransition,
    public aicore::ConditionalTransitionMixin
{
public:
    const char *text;
    const char *actionText;

    virtual aicore::Action* getActions();

    void set(int value, aicore::real *matrix,
             const char* text, const char* actionText);

    bool isTriggered();
};

void DemoMarkovTransition::set(int value, aicore::real *newMatrix,
                               const char* newText,
                               const char* newActionText)
{
    text = newText;
    actionText = newActionText;
    matrix = newMatrix;

    aicore::IntegerMatchCondition *c = new aicore::IntegerMatchCondition;
    c->watch = &option;
    c->target = value;
    condition = c;
}

bool DemoMarkovTransition::isTriggered()
{
    printf("Checking for transition %d - ",
           ((aicore::IntegerMatchCondition*)condition)->target);

    bool result = aicore::ConditionalTransitionMixin::isTriggered();
    if (result) {
        printf("Triggers\n");
    } else {
        printf("Doesn't trigger\n");
    }
    return result;
}

aicore::Action* DemoMarkovTransition::getActions()
{
    DemoAction* action = new DemoAction;
    action->next = NULL;
    action->actionText = actionText;
    return action;
}


// Holds the matrix data for all the transitions
aicore::real matrices[] = {
    // Transition one is conservative
    0.0, 1.0, 0.0, 0.0,
    0.3, 0.0, 0.4, 0.3,
    0.1, 0.0, 0.6, 0.3,
    0.6, 0.0, 0.0, 0.4,

    // Transition two is conservative
    0.5, 0.0, 0.0, 0.5,
    0.3, 0.2, 0.4, 0.1,
    0.2, 0.1, 0.6, 0.1,
    0.0, 0.7, 0.0, 0.3,

    // Transition three is non-conservative
    0.3, 0.3, 0.3, 0.3,
    0.2, 0.2, 0.2, 0.2,
    0.1, 0.1, 0.1, 0.1,
    1.0, 0.6, 0.3, 0.0,

    // Transition four (the timed-out default) is conservative
    0.4, 0.3, 0.2, 0.1,
    0.3, 0.1, 0.4, 0.2,
    0.2, 0.3, 0.2, 0.3,
    0.1, 0.3, 0.2, 0.4,
};

/**
 * The main entry point, we set up our state machine here, and then
 * run it.
 */
int main(int argc, char** argv)
{
    // Create the state machine and start with a crisp state
    aicore::MarkovStateMachine msm;
    msm.stateVectorSize = 4;
    msm.stateVector = new aicore::real[4];
    msm.stateVector[0] = (aicore::real)1.0;
    msm.stateVector[1] = (aicore::real)0.0;
    msm.stateVector[2] = (aicore::real)0.0;
    msm.stateVector[3] = (aicore::real)0.0;

    // Now create the transitions
    DemoMarkovTransition transitions[4];
    transitions[0].set(1, matrices+0,
                       "Transition 1", "Transition 1's Action");
    transitions[0].next = transitions+1;

    transitions[1].set(2, matrices+16,
                       "Transition 2", "Transition 2's Action");
    transitions[1].next = transitions+2;

    transitions[2].set(3, matrices+32,
                       "Transition 3", "Transition 3's Action");
    transitions[2].next = transitions+3;

    transitions[3].set(4, matrices+48,
                       "Transition 4", "Transition 4's Action (On Timer)");
    transitions[3].next = NULL;

    msm.firstTransition = transitions+0;
    msm.defaultTransition = transitions+3;
    msm.framesToDefault = 3;

    // Now we can run the demo
    printf("AI4G: Markov State Machine Demo (Ctrl+C to exit)\n");
    char buffer[4];

    while (true)
    {
        // Display the current situation
        printf("\n\nCurrent State: [%.3f %.3f %.3f %.3f]\n",
               msm.stateVector[0],
               msm.stateVector[1],
               msm.stateVector[2],
               msm.stateVector[3]);

        // Ask for input
        printf("Which transition should be allowed to trigger (0[None]-4)\n");

        // Get the user's input
        fgets(buffer, 3, stdin);
        option = atoi(buffer);

        // Run one iteration of the state machine
        aicore::Action * actions = msm.update();

        // Output the action text
        aicore::Action * next = actions;
        while (next != NULL)
        {
            next->act();
            next = next->next;
        }

        // Delete the actions
        if (actions != NULL) actions->deleteList();
    }

    // We're fine
    return 0;
}
