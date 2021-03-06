/** @defgroup MleDPPMaster Magic Lantern Digital Playprint Library API - Master */

/**
 * @file genmakefile.cpp
 * @ingroup MleDPPMaster
 *
 * This program creates the Makefile for mastering with
 * GNU make.
 *
 * @author Mark S. Millard
 * @date September 15, 2004
 */


// COPYRIGHT_BEGIN
//
// Copyright (c) 2015 Wizzer Works
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

// Include system header files.
#include <stdlib.h>
#include <stdio.h>

// Include Magic Lantern header files.
#include <mle/mlAssert.h>
#include <mle/Dwp.h>
#include <mle/DppGenCode.h>
#include <mle/DppGenMakefile.h>

// Declare external references.
extern int generateMakefile(MleDwpItem *);


static void print_usage(char *program)
{
    printf("Usage: %s <workprint>\n\t<workprint>    the Digital Workprint file to read\n",
    program);
}


int main(int argc,char *argv[])
{
    // Declare local variables.
    MleDppGenMakefile *gm; // makefile generator object.
    char *wpName;          // input workprint file.
    
    // Parse command line.
    if (argc != 2)
	{
		print_usage(argv[0]);
		exit(1);
    }
    wpName = argv[1];

    // Create instances of the code generation object.
    gm = new MleDppGenMakefile(wpName);
    MLE_ASSERT(gm);

    if ( gm->isError() )
	{
		//perror(wpName);
		print_usage(argv[0]);
		exit(1);
    }

    // Now employ the code generator object to build code.
    if (gm->generateMakefile())
        exit(0);
    else
        exit(1);
}

