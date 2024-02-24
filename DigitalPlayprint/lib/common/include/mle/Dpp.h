/** @defgroup MleDPPBase Magic Lantern Digital Playprint Library API - Base */

/**
 * @file Dpp.h
 * @ingroup MleDPPBase
 *
 * Magic Lantern Digital Playprint Library API.
 *
 * @author Mark S. Millard
 * @date September 15, 2004
 *
 * \mainpage Magic Lantern Digital Playprint Library API
 *
 * \section overview Overview
 * <p>
 * The Magic Lantern Digital Playprint Library is a component of Magic Lantern,
 * an interactive, title development platform for authoring highly interactive titles,
 * such as interactive TV applications, games, educational programs, and
 * point-of-information/sales kiosks. The DPP library provides utilities for
 * constructing and maintaining a Digital Playprint.
 * </p>
 * <p>
 * The Magic Lantern DPP Library is distributed as two static libraries.
 * The build environment distributed with the source for the
 * DPP library can be used to build any or all of these libraries.
 * The libraries are
 * <ul>
 * <li>tools/DPP.lib       - Release Static Library</li>
 * <li>tools/DPPd.lib      - Debug Static Library</li>
 * </ul>
 * </p>
 * <p>
 * The tools version of the libraries are used with Magic Lantern
 * tools and can be found under <b>MLE_HOME/lib/tools</b>.
 * </p>
 *
 * \section ms_build Building for Microsoft Windows
 * <p>
 * Instructions for building on this platform are forthcoming.
 * </p>
 *
 * \section linux_build Building for Linux
 * <p>
 * Instructions for building on this platform are forthcoming.
 * </p>
 */


// COPYRIGHT_BEGIN
//
// Copyright (c) 2015-2024 Wizzer Works
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.
//
//  For information concerning this source file, contact Mark S. Millard,
//  of Wizzer Works at msm@wizzerworks.com.
//
//  More information concerning Wizzer Works may be found at
//
//      http://www.wizzerworks.com
//
// COPYRIGHT_END

#ifndef __MLE_DPP_H_
#define __MLE_DPP_H_

// Include Digital Workprint header files.
#include "mle/Dwp.h"

// Magic Lantern Digital Playprint export rules
#ifdef _MLE_DPP_API
#  define MLE_DPP_API     MLE_DWP_EXPORT
#  define MLE_DPP_MIN_API MLE_DWP_MIN_EXPORT
#else
#  define MLE_DPP_API     MLE_DWP_IMPORT
#  define MLE_DPP_MIN_API MLE_DWP_MIN_IMPORT
#endif


// Declare external protoypes.
extern void mleDppInit(void);

#endif /* __MLE_DPP_H_ */


