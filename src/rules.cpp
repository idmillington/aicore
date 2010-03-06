/*
 * Part of the Artificial Intelligence for Games system.
 *
 * Copyright (c) Ian Millington 2003-2006. All Rights Reserved.
 *
 * This software is distributed under licence. Use of this software
 * implies agreement with all terms and conditions of the accompanying
 * software licence.
 */
#include <assert.h>
#include <aicore/aicore.h>

namespace aicore
{
    bool isWildcard(id identifier)
    {
        return identifier & 0x80000000;
    }

    AndMatch::AndMatch(Match *one, Match *two)
    {
        subMatches[0] = one;
        subMatches[1] = two;

        // Make sure we have no NULLs.
        assert(one);
        assert(two);
    }

    bool AndMatch::matches(const Database *database,
                           DataBindings *bindings)
    {
        return
            subMatches[0]->matches(database, bindings)
            &&
            subMatches[1]->matches(database, bindings);
    }

    OrMatch::OrMatch(Match *one, Match *two)
    {
        subMatches[0] = one;
        subMatches[1] = two;

        // Make sure we have no NULLs.
        assert(one);
        assert(two);
    }

    bool OrMatch::matches(const Database *database,
                          DataBindings *bindings)
    {
        // Check if we need bindings
        if (bindings)
        {
            return
                subMatches[0]->matches(database, bindings)
                | // Deliberate single |, this is non-shortcutting
                subMatches[1]->matches(database, bindings);
        }
        else
        {
            return
                subMatches[0]->matches(database, 0)
                || // Use the shortcutting version
                subMatches[1]->matches(database, 0);
        }
    }

    NotMatch::NotMatch(Match *match) : match(match)
    {
        assert(match);
    }

    bool NotMatch::matches(const Database *database,
                           DataBindings *bindings)
    {
        return !match->matches(database, 0);
    }


    bool DataNodeMatch::matches(const Database *database,
                            DataBindings *bindings)
    {
        return matchesChildren((const DataGroup*)database, bindings);
    }

    bool DataNodeMatch::matchesChildren(const DataGroup * group,
                                        DataBindings *bindings)
    {
        const DataNode * node = group->firstChild;
        while (node)
        {
            if (matchesNode(node, bindings)) return true;
            node = node->nextSibling;
        }

        return false;
    }

    bool DataGroupMatch::matchesNode(const DataNode * node,
                                     DataBindings *bindings)
    {
        // Check we have a group
        if (!node->isGroup()) return false;

        // Does the identifier match?
        if (!isWildcard(identifier) && identifier != node->identifier) {
            return false;
        }

        // Convert to a Data group object
        DataGroup *group = (DataGroup*)node;

        // Check that all the matches children are present
        DataNodeMatch * match = firstChild;
        while (match)
        {
            if (!match->matchesChildren(group, bindings)) return false;
            match = match->nextSibling;
        }

        // All our children find matches in the group.

        // Do we need to add to the binding list?
        if (bindings && isWildcard(identifier))
        {
            // Add ourself to the bindings list.
        }
        return true;
    }


    template<typename T>
    RangeMatch<T>::RangeMatch(id identifier, T min, T max)
            : identifier(identifier), min(min), max(max)
    {
        assert(min <= max);
    }

    template<typename T>
    bool RangeMatch<T>::matchesNode(const DataNode *node,
                                    DataBindings *bindings)
    {
        // Make sure we have data
        if (!node->isDatum()) return false;

        // Does the identifier match?
        if (!isWildcard(identifier) && identifier != node->identifier) {
            return false;
        }

        // Convert to the appropriate type
        Datum<T> *datum = (Datum<T>*)node;

        // Check if we're in bounds
        if (min <= datum->value && datum->value <= max)
        {
            // Check if we need to add a binding
            if (bindings && isWildcard(identifier))
            {
                // Add the bindings
            }
            return true;
        }

        // We're out of bounds
        return false;
    }

    // Explicit template instantiations
    template RangeMatch<int>;
    template RangeMatch<real>;
    template RangeMatch<Vector3>;

}; // end of namespace
