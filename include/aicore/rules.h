/*
 * Part of the Artificial Intelligence for Games system.
 *
 * Copyright (c) Ian Millington 2003-2006. All Rights Reserved.
 *
 * This software is distributed under licence. Use of this software
 * implies agreement with all terms and conditions of the accompanying
 * software licence.
 */

/**
 * @file
 *
 * Holds the definitions for rule-based decision making. This includes
 * the database format and objects involved in matching against the
 * database. This file also includes a simple rule-based system that
 * is illustrative, but should probably not be used for serious
 * development. For a more efficient rule-based decision maker that
 * uses the rule definitions in this file, see the Rete
 * implementation.
 */
#ifndef AICORE_RULES_H
#define AICORE_RULES_H

namespace aicore
{

    /**
     * @name Database Structures
     *
     * These classes are used to represent information in the database
     * that can then be matched against rules.
     */
    /* @{ */

    /**
     * Identifies an item of data in the database.
     *
     * You may want to use a
     * different system that unsigned integers, especially if you
     * have a very large number of different identifiers to keep
     * track of. Be aware that using character strings with
     * string-matching will slow the algorithm down dramatically,
     * however. Most production systems use numerical identifiers,
     * but then use a dictionary mapping these identifiers to and
     * from human-readable strings so that they can be more easily
     * manipulated.
     *
     * @note Values with the most-significant bit set (i.e. values
     * 0x80000000-0xffffffff) are reserved to represent wildcards
     * in the matching system. You should not use them for data in
     * the database.
     */
    typedef unsigned id;

    /**
     * Checks if the given database item identifier is a wildcard.
     *
     * @see identifier
     */
    bool isWildcard(id identifier);

    /**
     * Data nodes represent any item in the database, either a group
     * of data, or a single datum.
     */
    struct DataNode
    {
        /**
         * Uniquely identifies the data node.
         */
        id identifier;

        /**
         * Data nodes are arranged in a first-child, next-sibling
         * tree. This member variable links to the next sibling. The
         * first child variable only applies to DataGroups (since
         * other types of data node can't have children).
         */
        DataNode *nextSibling;

        /**
         * Allows calling code to determine whether the node it is
         * considering is a group or not.
         */
        virtual bool isGroup() const { return false; }

        /**
         * Allows calling code to determine whether the node it is
         * considering has data in it.
         */
        virtual bool isDatum() const { return false; }
    };

    /**
     * Data groups represent a set of data in the database. It holds a
     * list of DataNode instances, which in turn may be DataGroups,
     * producing a tree of data.
     */
    struct DataGroup : public DataNode
    {
        /**
         * Data nodes are arranged in a first-child, next-sibling
         * tree. This variable points to the head of the sub-tree
         * descending from this node: it is defined here because only
         * data groups can have children. The nextSibling member
         * variable is defined in the DataNode base class, because all
         * data nodes can be part of the tree.
         */
        DataNode *firstChild;

        /** This is a group. */
        bool isGroup() const { return true; }
    };

    /**
     * The root node of a database is basically just a data
     * group. This may change from a typedef in the future as more
     * functionality is added, so you should use the typedef, and not
     * assume it will still be a DataGroup.
     */
    typedef DataNode Database;

    /**
     * A datum is a single atom of data found at the bottom of a tree
     * of data nodes. It represents one piece of information in the
     * game. The type of the data it holds, depends on the
     * instantiation of this template.
     */
    template <typename T>
    struct Datum : public DataNode
    {
        /**
         * The value of this datum.
         */
        T value;

        /** This is a datum. */
        bool isDatum() const { return true; }
    };

    /** A typedef for a datum containing an integer. */
    typedef Datum<int> IntegerDatum;

    /** A typedef for a datum containing a real number. */
    typedef Datum<real> RealDatum;

    /** A typedef for a datum containing a position vector. */
    typedef Datum<Vector3> VectorDatum;

    /* @} */


    /**
     * @name Structures for Rules
     *
     * These classes are used to create rules, their matching
     * logic, and their actions.
     */
    /* @{ */

    /**
     * Temporary definition: TODO: change this.
     */
    typedef void DataBindings;

    /**
     * A match object is used to check against the database. It is an
     * opaque structure that can consist of any logic.
     *
     * You can use the concrete boolean sub-classes of Match to
     * combine match logic together:
     *
     * <code>
     * ammo = new RangeMatch<int>(AMMO, 10, 100);<br>
     * health = new RangeMatch<int>(HEALTH, 0, 15);<br>
     * match = new AndMatch(NotMatch(ammo), health);
     * </code>
     */
    struct Match
    {
        /**
         * Tries to perform a match on the given database, subject to
         * the given set of data bindings.
         *
         * @param database The database to match on. Note that this is
         * const, because rules should only change the database when
         * they have fired.
         *
         * @param bindings On a successful match the data bindings may
         * be updated so that they reflect the match found. They are
         * therefore both an input and an output parameter. The match
         * method must be able to cope with a null-pointer for the
         * bindings parameter. This indicates that the bindings are
         * not needed, and that the match is only being processed for
         * whether it returns true or false.
         *
         * @returns True if a match was found.
         *
         * @note The method shouldn't update the data bindings unless
         * it will return True, even though it is theoretically
         * possible for a failed match to reject additional bindings
         * that are present. This is because some rule-based
         * algorithms rely on failed matches having no side-effects.
         */
        virtual bool matches(const Database *database,
                             DataBindings *bindings) = 0;
    };

    /**
     * An data node match is a subclass of match that tries to match a
     * single data node in the database.
     *
     * Conceptually, the match class could match the whole database in
     * a single operation: it is only ever fed the whole database, so
     * it can do with that what it likes. Practically, however, the
     * vast majority of matching requirements involve trying to find a
     * single item in the database that matches some criterion. This
     * class is designed to support this. Its match method iterates
     * through the items in the database, and calls matchesNode on
     * each one. Matches item can then be implemented in sub-classes
     * to check if the item fulfils the match criteria.
     */
    struct DataNodeMatch : public Match
    {
        /**
         * The next match object in a hierarchy of matches.
         *
         * Data node matches can be combined into matches on
         * hierarchical data. In the same way that data nodes
         * themselves are composed into a tree. This member variable
         * is only used when this match forms part of a DataGroupMatch
         * (defined in this file). In this case the DataNodeMatch
         * objects form a first-child, next-sibling tree that mirrors
         * the database structure they are trying to match.
         */
        DataNodeMatch * nextSibling;

        /**
         * Matches the given database, by checking each element in the
         * database against the matchesNode method.
         */
        virtual bool matches(const Database *database,
                             DataBindings *bindings);

        /**
         * Checks all the children of the given group to see if any of
         * them pass the matchesNode test. This is used in the
         * implementation of the matches method to iterate through the
         * items in the database. It can also be useful in subclasses
         * to recursively iterate through a data tree, and that is why
         * it has been exposed as a separate method here.
         */
        bool matchesChildren(const DataGroup * group,
                             DataBindings *bindings);

        /**
         * Tries to match the given data node from the database
         * against the criteria in this match. The behaviour and
         * return value of this method should be exactly as for
         * Match::matches, with the only adjustment that it is given
         * single item from the database, rather than the entire
         * database.
         */
        virtual bool matchesNode(const DataNode * node,
                                 DataBindings *bindings) = 0;
    };

    /**
     * Matches a group of data in the database. This is done by
     * building a match data structure that mirrors the data structure
     * that is being searched for in the database. for example: if we
     * are looking for someone with no ammo:
     *
     * <code>(bob (rifle (ammo 0)))</code>
     *
     * we would have a match:
     *
     * <code>(?person (?weapon (ammo 0)))</code>
     *
     * The structure of the match mirrors the structure of the data
     * we're trying to find.
     */
    struct DataGroupMatch : public DataNodeMatch
    {
        /**
         * The identifier to match. This may also be a wild-card (if
         * it has its most significant bit set).
         */
        id identifier;

        /**
         * The first sub-match in this group. Additional children are
         * found using the DataMatchGroup's nextSibling member.
         */
        DataNodeMatch * firstChild;

        /**
         * Tries to match the given data node from the database
         * against the criteria in this match. This uses a recursive
         * matching algorithm that walks down the given node, trying
         * to match it against the structure of this match and its
         * children.
         */
        virtual bool matchesNode(const DataNode * node,
                                 DataBindings *bindings);
    };

    /**
     * A range match matches if the value of a given item of data is
     * within the given range.
     *
     * @note The implementation file for this template explicitly gets
     * the compiler to instantiate position vector (Vector3) real
     * number (real) and integer (int) versions of this template,
     * corresponding to the XXXRangeMatch typedefs in this file. If
     * you are compiling the aicore system into a library, to link
     * with your main program, you may get bugs if you use other
     * instantiations of the template. To solve this, adjust the
     * rules.cpp file to add the explicit instantiations you need, or
     * else compile the source code directly into your project. The
     * same limitation doesn't apply to the Datum template defined in
     * this file, because that doesn't have additional implementation
     * elsewhere, so the compiler should be able to instantiate it
     * just from this header file.
     */
    template <typename T>
    struct RangeMatch : public DataNodeMatch
    {
        /**
         * The minimum value of the range, inclusive.
         */
        T min;

        /**
         * The maximum value of the range, inclusive.
         */
        T max;

        /**
         * The identifier to match. This may also be a wild-card (if
         * it has its most significant bit set).
         */
        id identifier;

        /**
         * Creates a range match object with the given identifier and
         * range. The max value must be greater than or equal to the
         * min value.
         */
        RangeMatch(id identifier, T min, T max);

        /**
         * Matches the given database node. If the match is valid, and
         * the identifier was a wildcard, then the bindings will be
         * updated with the correspondence.
         */
        virtual bool matchesNode(const DataNode *database,
                                 DataBindings *bindings);
    };

    /** A typedef for a range match on an integer. */
    typedef RangeMatch<int> IntegerRangeMatch;

    /** A typedef for a range match on a real number. */
    typedef RangeMatch<real> RealRangeMatch;

    /** A typedef for a range match on a position vector. */
    typedef RangeMatch<Vector3> VectorRangeMatch;


    /**
     * A match object that matches if both its children match.
     */
    struct AndMatch : public Match
    {
        Match* subMatches[2];

        /**
         * Creates a new and-match combining the given two matches.
         */
        AndMatch(Match *one, Match *two);

        /**
         * Matches the given database, by trying its two sub-matches
         * in turn. If either return false, then this method returns
         * false.
         *
         * @note This class shortcuts, so the second sub-match isn't
         * tried if the first fails.
         */
        virtual bool matches(const Database *database,
                             DataBindings *bindings);
    };

    /**
     * A match object that matches if either of its children match.
     */
    struct OrMatch : public Match
    {
        Match* subMatches[2];

        /**
         * Creates a new or-match combining the given two matches.
         */
        OrMatch(Match *one, Match *two);

        /**
         * Matches the given database, by trying its two sub-matches
         * in turn. If either return true, then this method returns
         * true.
         *
         * @note Unlike AndMatch, this class does not shortcut if
         * bindings are passed into the method. If the first match
         * returns true, it will still try the second match. This is
         * because the matches aren't just about finding an answer,
         * they are also about updating the databindings, so both
         * sub-matches need the opportunity to do that. If the
         * bindings are given as a null-pointer, then no bindings need
         * calculating, so shortcutting is enabled.
         */
        virtual bool matches(const Database *database,
                             DataBindings *bindings);
    };

    /**
     * A match object that matches if its child match is not
     * successful.
     */
    struct NotMatch : public Match
    {
        /**
         * The child-match: if this match succeeds, then this instance
         * fails, and vice versa.
         */
        Match * match;

        /**
         * Creates a new not-match with the given child match.
         */
        NotMatch(Match *match);

        /**
         * Matches the given database, by making sure that its
         * sub-match fails.
         *
         * @note Because failed matches aren't allowed to update their
         * bindings, this method may not change the bindings at all:
         * if its child-match succeeds, then this method must fail and
         * the bindings should be left as is; but if its child fails
         * then no bindings change will have taken place anyway. This
         * is enforced by sending in a null-pointer for bindings to
         * the child match.
         *
         * @see Match::matches
         */
        virtual bool matches(const Database *database,
                             DataBindings *bindings);
    };

    /**
     * Represents a rule in a rule-based system. Rules consist of two
     * parts: the ifClause is a matching structure that can be checked
     * against the database, and the action is to be carried out when
     * the match is made.
     */
    struct Rule
    {
        /**
         * The match required before the rule can fire.
         */
        Match *ifClause;

        /**
         * Carries out an action when the rule matches. This is a
         * method rather than some structure defining the action to
         * carry out, because actions can vary dramatically depending
         * on the application.
         */
        virtual void action() = 0;
    };

    /* @} */

}; // end of namespace

#endif // AICORE_RULES_H
