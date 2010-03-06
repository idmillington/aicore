/*
 * The action manager demo.
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

    /**
     * The number of times the action can be done before it's complete.
     * This counts down.
     */
    int actCount;
    bool interrupt;
    DemoAction(int actCount, float priority, bool interrupt);
    virtual void act();
    virtual bool isComplete();
    virtual bool canInterrupt();
};

DemoAction::DemoAction(int actCount, float priority, bool interrupt)
    :
    actCount(actCount), interrupt(interrupt)
{
    this->priority = priority;
    next = NULL;
}

bool DemoAction::canInterrupt()
{
    return interrupt;
}

bool DemoAction::isComplete()
{
    return (actCount <= 0);
}

void DemoAction::act()
{
    actCount--;
    printf("Doing: %s [id=%d]\n", actionText, this);
}

/**
 * Outputs the action manager's contents in text form.
 */
void displayActive(const aicore::ActionManager* am)
{
    printf("Active actions:\n");
    const aicore::Action * next = am->active;
    if (next == NULL) printf("Empty\n");
    else
    {
        while (next != NULL)
        {
            printf("Action: [id=%d] - priority %f\n",
                   next,
                   next->priority
                   );

            next = next->next;
        }
    }
}

/**
 * Outputs the action manager's contents in text form.
 */
void displayQueue(const aicore::ActionManager* am)
{
    printf("Action queue:\n");
    const aicore::Action * next = am->actionQueue;
    if (next == NULL) printf("Empty\n");
    else
    {
        while (next != NULL)
        {
            printf("Action: [id=%d] - priority %f\n",
                   next,
                   next->priority
                   );

            next = next->next;
        }
    }
}

/** Prints the list of commands understood by the demo. */
void displayHelp()
{
    printf("\th - this list of commands\n");
    printf("\tr - run the action manager\n");
    printf("\t1 .. 3 - schedule different pre-set basic actions\n");
    printf("\tc - schedule action combination\n");
    printf("\ts - schedule action sequence\n");
}

/**
 * Process the given command on the given action manager. Returns true if
 * the application should quit.
 */
bool processOption(const char* command, aicore::ActionManager * am)
{
    DemoAction *action, *action2;
    aicore::ActionCombination * ac;
    aicore::ActionSequence * as;

    switch (command[0])
    {
    case 'h': case 'H':
        displayHelp();
        break;

    case 'r': case 'R':
        am->execute();
        break;

    case 's': case 'S':
        action = new DemoAction(1, 0, false);
        action->actionText = "Sequence action 1";
        action2 = new DemoAction(1, 0, false);
        action2->actionText = "Sequence action 2";
        action2->next = NULL;
        action->next = action2;

        as = new aicore::ActionSequence;
        as->subActions = action;
        am->scheduleAction(as);

        printf("Scheduling an action sequence.\n");
        break;

    case 'c': case 'C':
        action = new DemoAction(1, 0, false);
        action->actionText = "Combined action 1";
        action2 = new DemoAction(1, 0, false);
        action2->actionText = "Combined action 2";
        action2->next = NULL;
        action->next = action2;

        ac = new aicore::ActionCombination;
        ac->subActions = action;
        am->scheduleAction(ac);

        printf("Scheduling an action combination.\n");
        break;

    case 'q': case 'Q':
        return true;

    case '1':
        action = new DemoAction(1, 0, false);
        action->actionText = "One-Shot Action";
        am->scheduleAction(action);
        printf("Scheduling a one-shot action. %d\n", action);
        break;

    case '2':
        action = new DemoAction(5, 0, false);
        action->actionText = "Five-Duration Action";
        am->scheduleAction(action);
        printf("Scheduling a five-iterations action.\n");
        break;

    case '3':
        action = new DemoAction(5, 2, true);
        action->actionText = "Interrupting Low Priority Action";
        am->scheduleAction(action);
        printf("Scheduling a low priority action that interrupts.\n");
        break;

    case '4':
        action = new DemoAction(5, 6, true);
        action->actionText = "Interrupting High Priority Action";
        am->scheduleAction(action);
        printf("Scheduling a high priority action that interrupts.\n");
        break;

    default:
        printf("Command not understood\n");
    }
    return false;
}

/**
 * The main entry point, we set up our action manager here, and then
 * run it.
 */
int main(int argc, char** argv)
{
    // Create the action manager, empty initially
    aicore::ActionManager am;

    // Now we can run the demo
    printf("AI4G: Action Manager Demo (Ctrl+C to exit)\n");
    char buffer[10];

    // Loop through giving commands
    while (true)
    {
        printf("\n=======================================================\n");
        displayActive(&am);
        printf("-------------------------------------------------------\n");
        displayQueue(&am);
        printf("\nType 'h' for command list\n>");

        // Get the user's input
        fgets(buffer, 10, stdin);

        // Process the option
        if (processOption(buffer, &am)) break;
    }

    // We're fine
    return 0;
}
