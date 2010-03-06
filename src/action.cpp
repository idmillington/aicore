/*
 * Defines the general action class.
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
    Action* Action::getLast()
    {
        // If we're at the end, then end
        if (next == NULL) return this;

        // Otherwise find the end iteratively
        Action* thisAction = this;
        Action* nextAction = next;
        while(nextAction) {
            thisAction = nextAction;
            nextAction = nextAction->next;
        }

        // The final element is in thisAction, so return it
        return thisAction;
    }

    void Action::deleteList()
    {
        // Decurse first
        if (next != NULL) next->deleteList();

        // Delete ourself
        delete this;
    }

    void Action::act()
    {
        // Does nothing.
    }

    bool Action::canInterrupt()
    {
        return false;
    }

    bool Action::canDoBoth(const Action * other) const
    {
        return false;
    }

    bool Action::isComplete()
    {
        return true;
    }


    ActionManager::ActionManager()
            :
            activePriority(0),
            actionQueue(NULL),
            active(NULL)
    {
    }

    void ActionManager::scheduleAction(Action * newAction)
    {
        // Find the correct place to insert the new action
        Action ** previous = &actionQueue;
        Action * next = actionQueue;
        while (next != NULL)
        {
            // if we've found a lower priority, we go here. Note that
            // this will be much quicker with a >=, but it means in
            // the absence of priority ordering the queue defaults to
            // fifo, which isn't very useful.
            if (newAction->priority > next->priority)
            {
                break;
            }

            previous = &next->next;
            next = next->next;
        }

        // When we get here, we've either found the location mid-list
        // or reached the end of the list, so add it on
        *previous = newAction;
        newAction->next = next;
    }

    void ActionManager::execute()
    {
        // Check if we need to interrupt the currently active actions
        checkInterrupts();

        // Add as many actions to the active set as play with the
        // actions already there.
        addAllToActive();

        // Execute the active set
        runActive();
    }

    void ActionManager::checkInterrupts()
    {
        // Find any new interrupters
        Action ** previous = &actionQueue;
        Action * next = actionQueue;
        while (next != NULL)
        {
            // If we drop below the active priority, give up
            if (next->priority < activePriority)
            {
                break;
            }

            // Otherwise we're beating for priority, so check if we
            // need to interrupt.
            if (next->canInterrupt()) {

                // So we have to interrupt. Initially just replace the
                // active set.

                // Delete the previous active list
                if (active != NULL) active->deleteList();

                // Add the new one
                active = next;
                activePriority = active->priority;

                // Rewire the queue to extract our action
                *previous = next->next;
                next->next = NULL;

                // And stop looking (the highest priority interrupter
                // wins).
                break;
            }

            // Check the next one
            previous = &next->next;
            next = next->next;
        }
    }

    void ActionManager::addAllToActive()
    {
        Action ** previous = &actionQueue;
        Action * next = actionQueue;
        while (next != NULL)
        {
            Action * inActive = active;
            while (inActive != NULL)
            {
                // Check for compatibility
                if (!inActive->canDoBoth(next) ||
                    !next->canDoBoth(inActive)) {
                    goto nextInQueue;
                }
            }

            // We are compatible, so move from the queue to the active set
            *previous = next->next;
            next->next = active;
            active = next;

            // Move the next counter, but keep the previous as is.
            next = next->next;

            // Don't fall through, because we don't want to update
            // previous
            continue;

        nextInQueue:
            // We only get here if there was no compatibility, so chug along
            previous = &next->next;
            next = next->next;
        }
    }

    void ActionManager::runActive()
    {
        Action ** previous = &active;
        Action * next = active;

        while (next != NULL)
        {
            // Do the action first
            next->act();

            // Check if we're done with this action
            if (next->isComplete())
            {
                // Remove it from the list
                *previous = next->next;

                // Keep a temp of what we're about to delete
                Action * temp = next;

                // Move the next pointer only along (previous stays)
                next = next->next;

                // And delete the item
                delete temp;
            }
            else
            {
                // We're not done, just chug along
                previous = &next->next;
                next = next->next;
            }
        }
    }


    bool ActionCompound::canDoBoth(const Action* action) const
    {
        Action * next = subActions;
        while (next != NULL)
        {
            if (!next->canDoBoth(action)) return false;
            next = next->next;
        }
        return true;
    }

    void ActionCompound::deleteList()
    {
        if (subActions != NULL) subActions->deleteList();
        Action::deleteList();
    }


    bool ActionCombination::canInterrupt()
    {
        Action * next = subActions;
        while (next != NULL)
        {
            if (next->canInterrupt()) return true;
            next = next->next;
        }
        return false;
    }

    bool ActionCombination::isComplete()
    {
        Action * next = subActions;
        while (next != NULL)
        {
            if (!next->isComplete()) return false;
            next = next->next;
        }
        return true;
    }

    void ActionCombination::act()
    {
        Action * next = subActions;
        while (next != NULL)
        {
            if (!next->isComplete()) next->act();
            next = next->next;
        }
    }



    bool ActionSequence::canInterrupt()
    {
        if (subActions != NULL) return subActions->canInterrupt();
        else return false;
    }

    bool ActionSequence::isComplete()
    {
        return (subActions == NULL);
    }

    void ActionSequence::act()
    {
        // Check if we have anything to do
        if (subActions == NULL) return;

        // Run the first action in the list
        subActions->act();

        // Then consume it if its done
        if (subActions->isComplete()) {

            Action * temp = subActions;
            subActions = subActions->next;
            delete temp;
        }
    }

}; // end of namespace
