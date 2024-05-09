/** @defgroup MleDPPMaster Magic Lantern Digital Playprint Library API - Master */

/**
 * @file gentables.cpp
 * @ingroup MleDPPMaster
 *
 * Program to generate title runtime tables.
 *
 * This program creates files to support the actor build environment.
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
//  For information concerning this sourcde code, contact Mark S. Millard,
//  of Wizzer Works at msm@wizzerworks.com.
//
//  More information concerning Wizzer Works may be found at
//
//      http://www.wizzerworks.com
//
// COPYRIGHT_END

// Include system header files.
#ifdef WIN32
#include <windows.h>
#endif
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

// Include Magic Lantern header files.
#include <mle/mlAssert.h>
#include <mle/MlePath.h>
#ifdef WIN32
#include "mle/mlGetOpt.h"
#include <mle/MleWin32Path.h>
#else
#include <unistd.h>
#include <mle/MleLinuxPath.h>
#endif /* WIN32 */


// Include Magic Lantern Digital Workprint header files.
#include <mle/Dwp.h>

// Include Magic Lantern Digital Playprint header files.
#include <mle/DppGenCode.h>


// Declare external routines.
//extern MlBoolean mlVerifyTargetWorkprint(MleDwpItem *root,char *tags);
MlBoolean mlVerifyTargetWorkprint(MleDwpItem * /*root*/,char * /*tags*/)
{
	return TRUE;
}

#ifdef WIN32
static char *getCanonicalPath(char *path)
{
	char *cpath = NULL;
	MleWin32Path *wpath = new MleWin32Path((MlChar *)path, true);
    cpath = strdup((char *)wpath->getPath());
	delete wpath;
	return cpath;
}
#else
static char *getCanonicalPath(char *path)
{
	return strdup(path);
}
#endif /* WIN32 */

// Argument structures for parser.
typedef struct _pattern
{
    char            *str;
    struct _pattern *next;
} Pattern;

typedef struct _ArgStruct
{
    char       *commandName;  /* name of command */
    char       *workprint;    /* name of workprint file to build */
    char       *tags;         /* Digital Workprint tags */
	char       language;      /* TRUE = Java, FALSE = C++. */
	char       *package;      /* Java package encapsualtion. */
	char       *outputDir;    /* Directory to redirect output to. */
	MlBoolean  verbose;       /* Be verbose. */
} ArgStruct;


const char *usage_str = "\
Syntax:   gentables [-v] [-j <package>|-c] [-d dir] <tags> <workprint>\n\
\n\
          -v                   Be verbose\n\
          -c                   Generate C++ compliant files (default)\n\
          -j <package>         Generate Java compliant files\n\
          -d <dir>             Directory where tables are generated\n\
          <tags>               Digital Workprint tags\n\
          <workprint>          Digital Workprint\n\
\n\
Function: Generate Magic Lantern Runtime Tables.\n\
\n\
The tables used to populate the title are generated for the specified\n\
programming language.\n";


int parseArgs(int argc, char *argv[], ArgStruct *args)
{
    /* Declare local variables. */
    int c;
    int errflg;
    extern char *optarg;
    extern int optind;
    
    errflg = 0;
    while ((c = getopt(argc, argv, "vcj:d:")) != -1)
	{
        switch (c)
		{
		  case 'j':
			/* Generate code for Java programming language. */
			args->language = TRUE;
			args->package = optarg;
			break;
		  case 'c':
			/* Generate code for C++ programming language. */
			args->language = FALSE;
			break;
          case 'd':
            /* Place files in this directory. */
            args->outputDir = optarg;
            break;
          case 'v':
        	// Set verbosity flag.
        	args->verbose = TRUE;
        	break;
          case '?':
            errflg++;
        }
    }

    if (errflg)
	{
        (void)fprintf(stderr, "%s\n", usage_str);
        return FALSE;
    }

    for ( ; optind < argc; optind++)
	{
        if (! args->tags)
		{
            args->tags = strdup(argv[optind]);
        } else if (! args->workprint)
		{
            args->workprint = getCanonicalPath(argv[optind]);
        } else
		{
            fprintf(stderr,"%s\n",usage_str);
            return FALSE;
        }
    }
    
    /* If there is no specified workprint, complain. */
    if (args->tags == NULL ||
        args->workprint == NULL ||
		((args->language == TRUE) && (args->package == NULL)))
	{
        fprintf(stderr,"%s\n",usage_str);
        return FALSE;
    }
    
    /* Having made it to here implies that we have good arguments. */
    
    return TRUE;
}


int main(int argc,char *argv[])
{
    // declare local variables.
    MleDppGenCode *gc;  // Code generator object.
    ArgStruct args;
   
    // Parse arguments.
    args.commandName = argv[0];
    args.workprint = NULL;
    args.tags = NULL;
	args.language = FALSE;
	args.package = NULL;
	args.outputDir = NULL;
	args.verbose = FALSE;
    if (! parseArgs(argc, argv, &args))
	{
        exit(1);
    }

	//__asm int 3h
	
	// Redirect the generated output.
    if (args.outputDir != NULL)
		MleDppGenCode::setOutputDirectory(args.outputDir);

    // Create instances of the code generation object.
    gc = new MleDppGenCode(args.workprint,args.language,args.package);
    if (args.verbose == TRUE)
    	gc->setVerbose(TRUE);
    MLE_ASSERT(gc);

    if (gc->getRoot() == NULL)
	{
        fprintf(stderr,"%s : %s\n",args.commandName,
                "Unable to open Digital Workprint");
        exit(1);
    }

    // Verify digital workprint.
    if (! mlVerifyTargetWorkprint(gc->getRoot(),args.tags))
	{
        fprintf(stderr,"%s : %s\n",args.commandName,
                "Unable to process Digital Workprint");
        exit(1);
    }

    // Now employ the code generator object to build some code.
    if (gc->generateTitle(args.tags))
        exit(0);
    else
        exit(1);
}
