/*
 * Defines the classes used for decision trees.
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

    DecisionTreeNode* Decision::makeDecision()
    {
        // Choose a branch based on the getBranch method
        if (getBranch()) {
            // Make sure its not null before recursing.
            if (trueBranch == NULL) return NULL;
            else return trueBranch->makeDecision();
        } else {
            // Make sure its not null before recursing.
            if (falseBranch == NULL) return NULL;
            else return falseBranch->makeDecision();
        }
    }

    RandomDecision::RandomDecision()
        :
        lastDecisionFrame(0),
        lastDecision(false)
    {
    }

    bool RandomDecision::getBranch()
    {
        unsigned thisFrame = TimingData::get().frameNumber;

        // If we didn't get here last time, then things may change
        if (thisFrame > lastDecisionFrame + 1) {
            lastDecision = randomBoolean();
        }

        // In any case, store the frame number
        lastDecisionFrame = thisFrame;

        // And return the stored value
        return lastDecision;
    }

    RandomDecisionWithTimeOut::RandomDecisionWithTimeOut()
        :
        RandomDecision(),
        firstDecisionFrame(0)
    {
    }

    bool RandomDecisionWithTimeOut::getBranch()
    {
        unsigned thisFrame = TimingData::get().frameNumber;

        // Check if the stored decision is either too old, or if we
        // timed out.
        if (thisFrame > lastDecisionFrame + 1 ||
            thisFrame > firstDecisionFrame + timeOutDuration) {

            // Make a new decision
            lastDecision = randomBoolean();

            // And record that it was just made
            firstDecisionFrame = thisFrame;
        }

        // Update the frame number
        lastDecisionFrame = thisFrame;

        // And return the stored value
        return lastDecision;
    }

}; // end of namespace
