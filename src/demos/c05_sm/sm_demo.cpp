/*
 * The state machine demo.
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
 * This class adds a mechanism to output demo actions with text.
 */
class DemoTransition :
    public aicore::Transition,
    public aicore::ConditionalTransitionMixin,
    public aicore::FixedTargetTransitionMixin
{
public:
    const char ** text;
    unsigned textCount;

    virtual aicore::Action* getActions();

    aicore::StateMachineState * getTargetState()
    {
        return aicore::FixedTargetTransitionMixin::getTargetState();
    }

    bool isTriggered()
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
};

/* This is a convenience function that returns a list of actions from
 * a set of text strings. */
aicore::Action* getActionsFromText(const char ** text, unsigned textCount)
{
    DemoAction* first = NULL;
    DemoAction* last = NULL;
    DemoAction* current = NULL;
    for (unsigned i = 0; i < textCount; i++)
    {
        // Create a new action and set its text
        current = new DemoAction();
        current->actionText = (const char*) text[i];
        current->next = NULL;

        // Wire it into the action list
        if (i == 0) first = current;
        if (last) last->next = current;
        last = current;
    }

    return first;
}


aicore::Action* DemoTransition::getActions()
{
    return getActionsFromText(text, textCount);
}

/**
 * This class extends the state machine state to have text-based
 * actions returned.
 */
class DemoState : public aicore::StateMachineState
{
public:
    const char ** text;
    unsigned textCount;

    const char ** entryText;
    unsigned entryTextCount;

    const char ** exitText;
    unsigned exitTextCount;

    virtual aicore::Action * getActions();
    virtual aicore::Action * getEntryActions();
    virtual aicore::Action * getExitActions();
};

aicore::Action* DemoState::getActions()
{
    return getActionsFromText(text, textCount);
}

aicore::Action* DemoState::getEntryActions()
{
    return getActionsFromText(entryText, entryTextCount);
}

aicore::Action* DemoState::getExitActions()
{
    return getActionsFromText(exitText, exitTextCount);
}


/**
 * The main entry point, we set up our state machine here, and then
 * run it.
 */
int main(int argc, char** argv)
{
    // Create the array of text used in all states
    const char *allText[] = {
        // State text
        "Entering State A", "In State A", "Exiting State A",
        "Entering State B", "In State B", "Exiting State B",
        "Entering State C", "In State C", "Exiting State C",
        "Entering State D", "In State D", "Exiting State D",
        "Entering State E", "In State E", "Exiting State E",
        "Entering State F", "In State F", "Exiting State F",
        "Entering State G", "In State G", "Exiting State G",

        // Transition text
        "Transition 1",
        "Transition 2",
        "Transition 3",
        "Transition 4",
        "Transition 5",
        "Transition 6",
        "Transition 7",
        "Transition 8",
        "Transition 9",
        "Transition 10",
        "Transition 11",
        "Transition 12",
        "Transition 13",
        "Transition 14",
        "Transition 15"
    };

    // Create our state machine
    DemoState states[7];
    DemoTransition transitions[15];

    // Set the text in each case
    for (unsigned i = 0; i < 7; i++)
    {
        states[i].entryText = allText+(i*3);
        states[i].entryTextCount = 1;
        states[i].text = allText+(i*3+1);
        states[i].textCount = 1;
        states[i].exitText = allText+(i*3+2);
        states[i].exitTextCount = 1;
    }
    for (unsigned i = 0; i < 15; i++)
    {
        transitions[i].text = allText+(21+i);
        transitions[i].textCount = 1;

        transitions[i].next = NULL;

        aicore::IntegerMatchCondition *condition =
            new aicore::IntegerMatchCondition;
        condition->watch = &option;
        condition->target = (i+1);
        transitions[i].condition = condition;
    }

    // Connect the transitions to their targets
    transitions[0].target = states+1;
    transitions[1].target = states+1;
    transitions[2].target = states+2;
    transitions[3].target = states+3;
    transitions[4].target = states+4;
    transitions[5].target = states+0;
    transitions[6].target = states+5;
    transitions[7].target = states+6;
    transitions[8].target = states+5;
    transitions[9].target = states+2;
    transitions[10].target = states+6;
    transitions[11].target = states+4;
    transitions[12].target = states+6;
    transitions[13].target = states+4;
    transitions[14].target = states+6;

    // Place the transitions in their states
    states[0].firstTransition = transitions+0;
    transitions[0].next = transitions+4;

    states[1].firstTransition = transitions+1;
    transitions[1].next = transitions+2;
    transitions[2].next = transitions+6;
    transitions[6].next = transitions+7;

    states[2].firstTransition = transitions+3;
    transitions[3].next = transitions+8;

    states[3].firstTransition = transitions+10;

    states[4].firstTransition = transitions+13;
    transitions[13].next = transitions+14;

    states[5].firstTransition = transitions+5;
    transitions[5].next = transitions+11;
    transitions[11].next = transitions+12;

    states[6].firstTransition = transitions+9;

    // Set up the state machine
    aicore::StateMachine sm;
    sm.initialState = states+0;
    sm.currentState = NULL;

    // Now we can run the demo
    printf("AI4G: State Machine Demo (Ctrl+C to exit)\n");
    char buffer[4];

    while (true)
    {
        // Display the current situation
        if (sm.currentState != NULL) {
            printf("\n\nCurrent State: %s\n",
                   ((DemoState*)sm.currentState)->text[0]);
            printf("Transitions from this state:\n");

            // Output all valid transitions from here
            aicore::BaseTransition * next = sm.currentState->firstTransition;

            while (next != NULL)
            {
                DemoTransition* nextDT = (DemoTransition*)(next);
                DemoState* to = (DemoState*)nextDT->target;
                printf("%s to be %s\n", nextDT->text[0], to->text[0]);

                // Find the next transition
                next = next->next;
            }
        } else {
            printf("\n\nNo Current state\nNo Current transitions\n");
        }

        // Ask for input
        printf("Which transition should be allowed to trigger (0[None]-15)\n");

        // Get the user's input
        fgets(buffer, 3, stdin);
        option = atoi(buffer);

        // Run one iteration of the state machine
        aicore::Action * actions = sm.update();

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
