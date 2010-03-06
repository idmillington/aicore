/*
 * The global include header file. This pulls in all further headers
 * in the correct order. You should only ever need this one.
 *
 * Part of the Artificial Intelligence for Games system.
 *
 * Copyright (c) Ian Millington 2003-2006. All Rights Reserved.
 *
 * This software is distributed under licence. Use of this software
 * implies agreement with all terms and conditions of the accompanying
 * software licence.
 */

/**
 * @mainpage AI Core Documentation
 *
 * @section purpose Purpose
 *
 * This documentation contains a detailed reference to the classes,
 * functions, methods and variables in the AI-Core artificial
 * intelligence toolkit.
 *
 * The contents of this reference are highly technical and probably
 * not suitable for learning how to use the AI techniques that they
 * implement. The source code is based on code produced for the book
 * Artificial Intelligence for Games, Ian Millington, Morgan Kaufmann
 * Publishers 2006. If you can get hold of that book, it will explain
 * in depth the algorithms you'll find here.
 *
 * @section conents Contents
 *
 * Use the navigation system on the left side of the page to view the
 * documentation. The navigation tool uses JavaScript and requires a
 * relatively recent browser.
 *
 * AI-Core is contained in a single namespace:
 * <code>aicore</code>. Its contents can be viewed in the Compound
 * List section, or in the Class Hierarchy. The Graphical Class
 * Hierarchy provides an overview of how the components are linked
 * together.
 *
 * @section legal Legal
 *
 * This documentation is Copyright &copy; IPR Ventures 2005-2006 and
 * Copyright &copy; Ian Millington 2003-2005. All rights of both
 * copyright holders are reserved. This documentation is distributed
 * under the same license as the source code it describes, which is
 * contained in the LICENSE file in the source code distribution. All
 * trademarks, whether or not indicated, are the property of their
 * respective owners.
 */

/**
 * @file
 *
 * This is the main header that imports the rest of the library into
 * your project. You should normally just be able to import this
 * header, and optionally use the namespace, rather than importing
 * specific headers independently. For example:
 *
 * <code>
 * #include "aicore/aicore.h"<br/>
 * using namespace aicore;
 * </code>
 */
#include "precision.h"

#include "core.h"
#include "timing.h"
#include "aimath.h"
#include "primitives.h"

#include "action.h"

#include "location.h"
#include "kinematic.h"
#include "steering.h"
#include "steerpipe.h"

#include "dectree.h"
#include "basesm.h"
#include "sm.h"
#include "markovsm.h"

#include "rules.h"

#include "learning.h"
#include "qlearning.h"
