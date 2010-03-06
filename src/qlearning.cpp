/*
 * Defines the classes used for Q-learning
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

    QLearner::QLearner(LearningProblem * problem,
                       real alpha, real gamma, real rho, real nu)
            :
            problem(problem), alpha(alpha), gamma(gamma), rho(rho), nu(nu)
    {
        stride = problem->getActionCount();
        unsigned size = problem->getStateCount() * stride;
        qvalues = new real[size];
        memset(qvalues, 0, sizeof(real) * size);
    }

    QLearner::~QLearner()
    {
        delete[] qvalues;
    }

    real QLearner::getQValue(LearningProblemState *state,
                             LearningProblemAction *action)
    {
        return qvalues[state->index*stride + action->index];
    }

    void QLearner::storeQValue(LearningProblemState* state,
                               LearningProblemAction* action,
                               real qvalue)
    {
        qvalues[state->index*stride + action->index] = qvalue;
    }

    real QLearner::getBestQValue(LearningProblemState *state)
    {
        real best = (real)0;
        for (unsigned i = 0; i < stride; i++)
        {
            real q = qvalues[state->index*stride + i];
            if (q > best) best = q;
        }
        return best;
    }

    LearningProblemAction*
    QLearner::getBestAction(LearningProblemState *state)
    {
        // Get the actions
        LearningProblemAction* action = problem->getActions(state);

        // Check them in turn
        real best = (real)0;
        LearningProblemAction * bestAction = action;
        while (action != NULL)
        {
            real q = getQValue(state, action);
            if (q > best) {
                best = q;
                bestAction = action;
            }

            action = action->next;
        }

        return bestAction;
    }

    LearningProblemState*
    QLearner::doLearningIteration(LearningProblemState * state)
    {
        // Pick a new state once in a while
        if (randomReal() < nu) {
            state = problem->getRandomState();
        }

        // Get the list of actions
        LearningProblemAction* actions = problem->getActions(state);
        LearningProblemAction* action = NULL;

        // Check if we should use a random action, or the best one
        if (randomReal() < rho) {
            unsigned randPos = randomInt(actions->getCount());
            action = actions->getAtPositionInList(randPos);
        } else {
            action = getBestAction(state);
        }

        // Make sure we've got something to do
        if (action != NULL)
        {
            // Carry out the action
            LearningProblemActionResult result =
                problem->getResult(state, action);

            // Get the current q value
            real q = getQValue(state, action);

            // Get the q of the best action from the new state
            real maxQ = getBestQValue(result.state);

            // recalculate the q
            q = ((real)1.0-alpha) * q + alpha * (result.reward + gamma * maxQ);

            // Store the new Q value
            storeQValue(state, action, q);

            return result.state;
        }
        // Otherwise we need to get a new state - we've reached the
        // end of the road.
        else
        {
            return problem->getRandomState();
        }
    }

    void QLearner::learn(unsigned iterations)
    {
        // We choose a random place to start
        LearningProblemState * state = problem->getInitialState();

        // Then walk a little
        for (unsigned i = 0; i < iterations; i++)
        {
            state = doLearningIteration(state);
        }
    }

}; // end of namespace
