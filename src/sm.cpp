/*
 * Defines the classes used for state machines.
 *
 * Part of the Artificial Intelligence for Games system.
 *
 * Copyright (c) Ian Millington 2003-2006. All Rights Reserved.
 *
 * This software is distributed under licence. Use of this software
 * implies agreement with all terms and conditions of the accompanying
 * software licence.
 */
#include <aicore/aicore.h>

namespace aicore
{
    StateMachineState * FixedTargetTransitionMixin::getTargetState()
    {
        return target;
    }


    Action * StateMachineState::getEntryActions()
    {
        return NULL;
    }

    Action * StateMachineState::getActions()
    {
        return NULL;
    }

    Action * StateMachineState::getExitActions()
    {
        return NULL;
    }

    Action* StateMachine::update()
    {
        // The variable to hold the actions to perform
        Action * actions = NULL;

        // First case - we have no current state.
        if (currentState == NULL)
        {
            // In this case we use the entry action for the initial state.
            if (initialState != NULL) {

                // Transition to the first state
                currentState = initialState;

                // Returns the initial states entry actions
                actions = currentState->getEntryActions();

            } else {

                // We have nothing to do
                actions = NULL;
            }
        }

        // Otherwise we have a current state to work with
        else {
            // Start off with no transition
            Transition * transition = NULL;

            // Check through each transition in the current state.
            BaseTransition * testTransition = currentState->firstTransition;
            while (testTransition != NULL) {
                if (testTransition->isTriggered()) {
                    transition = (Transition*)testTransition;
                    break;
                }
                testTransition = testTransition->next;
            }

            // Check if we found a transition
            if (transition) {
                // Find our destination
                StateMachineState * nextState = transition->getTargetState();

                // Accumulate our list of actions
                Action * tempList = NULL;
                Action * last = NULL;

                // Add each element to the list in turn
                actions = currentState->getExitActions();
                last = actions->getLast();

                tempList = transition->getActions();
                last->next = tempList;
                last = tempList->getLast();

                tempList = nextState->getActions();
                last->next = tempList;

                // Update the change of state
                currentState = nextState;
            }

            // Otherwise our actions to perform are simply those for the
            // current state.
            else {

                actions = currentState->getActions();
            }
        }

        return actions;
    }

}; // end of namespace
