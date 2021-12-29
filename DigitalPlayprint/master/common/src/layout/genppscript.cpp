/** @defgroup MleDPPMaster Magic Lantern Digital Playprint Library API - Master */

/**
 * @file genppscript.cpp
 * @ingroup MleDPPMaster
 *
 * PlayPrint Layout Script Generator
 *
 * Given a workprint, it creates a default layout TCL script which can be edited
 * and used as input to genpp.
 */


// COPYRIGHT_BEGIN
//
// Copyright (c) 2015-2021 Wizzer Works
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
//  For information concerning this source code, contact Mark S. Millard,
//  of Wizzer Works at msm@wizzerworks.com.
//
//  More information concerning Wizzer Works may be found at
//
//      http://www.wizzerworks.com
//
// COPYRIGHT_END

// Include system header files.
#if defined(WIN32)
#include <windows.h>
#endif
#include <stdio.h>
#include <string.h>

// Include Magic Lantern header files.
#include <mle/mlTypes.h>
#include <mle/mlMalloc.h>
#include <mle/mlFileio.h>
#include <mle/MlePath.h>
#if defined(WIN32)
#include <mle/mlGetOpt.h>
#include <mle/MleWin32Path.h>
#else
#include <unistd.h>
#include <mle/MleLinuxPath.h>
#endif

// Include DWP header files.
#include "mle/Dwp.h"
#include "mle/DwpItem.h"
#include "mle/DwpInput.h"
#include "mle/DwpFinder.h"
#include "mle/DwpGroup.h"
#include "mle/DwpMediaRef.h"
#include "mle/DwpScene.h"
#include "mle/DwpBoot.h"
#include "mle/DwpSet.h"
#include "mle/DwpTagAllDiscriminator.h"


// Declare external routines.
//extern MlBoolean mlVerifyTargetWorkprint(MleDwpItem *root,char *tags);
MlBoolean mlVerifyTargetWorkprint(MleDwpItem *root,char *tags)
{
	return TRUE;
}

#if defined(WIN32)
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
    char *commandName;      /* Name of command. */
    char *tags;             /* Digital Workprint tags. */
    char *workprint;        /* Name of workprint file. */
    char *playprint;        /* Name of playprint file. */
    char *script;           /* Name of script. */
    char *chunkIndexFile;   /* Name of chunk index file. */
	char *outputDir;
} ArgStruct;


const char *usage_str = "\
Syntax:   genppscript [-d <directory>] <tags> <workprint> <playprint> <script> <index filename>\n\
          -d <directory>       Directory where files are generated\n\
          <tags>               Digital Workprint tags\n\
          <workprint>          Digital Workprint\n\
          <playprint>          Runtime Playprint\n\
          <script>             Generated Playprint layout script\n\
          <index filename>     Generated chunk index file\n\
\n\
Function: Generate Magic Lantern Digital Playprint Layout Script.\n";


int parseArgs(int argc, char *argv[], ArgStruct *args)
{
    /* Declare local variables. */
    int c;
    int errflg;
    extern char *optarg;
    extern int optind;
    
    errflg = 0;
    //while ((c = getopt(argc, argv, "d::")) != -1)
    while ((c = getopt(argc, argv, "d:")) != -1)
	{
        switch (c)
		{
		  case 'd':
			/* Place files in this directory. */
			args->outputDir = optarg;
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
        } else if (! args->playprint)
		{
            args->playprint = strdup(argv[optind]);
        } else if (! args->script)
		{
            args->script = strdup(argv[optind]);
        } else if (! args->chunkIndexFile)
		{
            args->chunkIndexFile = strdup(argv[optind]);
        } else {
            fprintf(stderr,"%s\n",usage_str);
            return FALSE;
        }
    }
    
    /* If there is no specified workprint, complain. */
    if (args->tags == NULL ||
        args->workprint == NULL ||
        args->playprint == NULL ||
        args->script == NULL ||
        args->chunkIndexFile == NULL) {
        fprintf(stderr,"%s\n",usage_str);
        return FALSE;
    }
    
    /* Having made it to here implies that we have good arguments. */
    
    return TRUE;
}


/*
 * This routine is used to set the Digital Workprint discriminators.
 * It expects a ':' deliminated string of tags to set multiple
 * discriminators. For example:
 *
 * tags = "brender:win32"
 */
static MleDwpTagAllDiscriminator *_discriminator;

static MlBoolean setDiscriminators(MleDwpItem *tree, char *tags)
{
    /* Declare local variables. */
    char *processTags;
    char *tag,*nextTag;
    MleDwpTagAllDiscriminator *newDiscriminator = new MleDwpTagAllDiscriminator;

    if (! tags)
	{
        if (_discriminator)
		{
            tree->setDiscriminator(NULL);
            delete _discriminator;
        }
        return TRUE;
    } else
	{
        processTags = strdup(tags);
    }

    // Remove old discriminator.
    if (_discriminator)
	{
        tree->setDiscriminator(NULL);
        delete _discriminator;
    }

    tag = processTags;
    while (tag)
	{
        nextTag = strchr(tag,':');
        if (nextTag) *nextTag++ = '\0';
        newDiscriminator->addTag(tag);
        tag = nextTag;
    }

    tree->setDiscriminator(newDiscriminator);
    _discriminator = newDiscriminator;

    mlFree(processTags);
    return TRUE;
}


#if 0
static void
processAutoLoadGroup(FILE *scriptOut, MleDwpItem *root)
{
    int i, numItems;
    MleDwpFinder finder;
    MleDwpBoot **items;

    /*
     * Find all the Boot item in the workprint.  Currently, this assumes
     * only group can be associated with the Boot token in the workprint.
     */
    finder.setType(FwWpBoot::typeId);
    finder.setFindAll(TRUE);
    finder.find(root);
    items = (FwWpBoot **) finder.getItems();
    numItems = finder.getNumItems();

    if (numItems > 0)
	{
		fprintf(scriptOut, "%s%s%s",
			"# autoloadgroup declares the actor/delegate groups that ",
			"are automatically\n",
			"# loaded in at the beginning of the title.\n");

		for (i = 0; i < numItems; i++)
			fprintf(scriptOut, "autoloadgroup %s\n", items[i]->getName());

		fprintf(scriptOut, "\n");
    }
}
#endif


static void
processAddSet(FILE *scriptOut, MleDwpItem *root)
{
    const char *name;
    int i, numItems;
    MleDwpFinder finder;
    MleDwpBoot **items;

    /*
     * Find all the Set items in the workprint.
     */
    finder.setType(MleDwpSet::typeId);
    finder.setFindAll(TRUE);
    finder.find(root);
    items = (MleDwpBoot **) finder.getItems();
    numItems = finder.getNumItems();

    for(i = 0; i < numItems; i++)
	{
		name = items[i]->getName();
#if defined(USE_PYTHON)
        fprintf(scriptOut, "dpp.addset(\"%s.chk\", \"%s\")\n", name, name);
#else
		fprintf(scriptOut, "addset %s.chk %s\n", name, name);
#endif
    }
}


// The global directory for generated output.
static MlePath *g_outputDir = NULL;


// Utility to append the global directory path to the specified path.
static char *_expandPath(char * path)
{
	MLE_ASSERT(path);

	MlePath *dir = g_outputDir;

	// Expand directory path, if necessary.
	char *tmpPath;
	if (dir != NULL)
	{
		tmpPath = (char *)mlMalloc(strlen((char *)dir->getPlatformPath()) + strlen(path) + 2);
		strcpy(tmpPath,(char *)dir->getPlatformPath());
#if defined(WIN32)
		strcat(tmpPath,"\\");
#else /* ! WIN32 */
		strcat(tmpPath,"/");
#endif /* WIN32 */
		strcat(tmpPath,path);
	} else
		tmpPath = path;

	return tmpPath;
}


int
main(int argc, char **argv)
{
    // Declare local variables.
    FILE *scriptOut = NULL;
    const char *name;
    int numItems;
    int i;
    ArgStruct args;

    // Parse arguments.
    args.commandName = argv[0];
    args.tags = NULL;
    args.workprint = NULL;
    args.playprint = NULL;
    args.script = NULL;
    args.chunkIndexFile = NULL;
	args.outputDir = NULL;
    if (! parseArgs(argc, argv, &args))
	{
      exit(1);
    }
	
	// Redirect generated output.
#if defined(WIN32)
	if (args.outputDir != NULL)
	{
		g_outputDir = new MleWin32Path((MlChar *)args.outputDir,true);
	}
#else
	if (args.outputDir != NULL)
	{
		g_outputDir = new MleLinuxPath((MlChar *)args.outputDir,true);
	}
#endif /* WIN32 */

    // Open output file (TCL/Python script).
    if (args.script != NULL)
	{
		char * tmpPath;
		if (g_outputDir != NULL)
			tmpPath = _expandPath(args.script);
		else
			tmpPath = args.script;
        scriptOut = mlFOpen(tmpPath, "w");

		if (g_outputDir != NULL)
			mlFree(tmpPath);

	} else
    {
        fprintf(stderr,"%s Error: %s, %s, %s, %s\n",
               args.commandName,
               "Could not create script file",
               args.script, 
               "from workprint",
	       args.workprint);
        exit(1);
    }

    // Emit header comments:
#if defined(USE_PYTHON)
    fprintf(scriptOut, "%s: %s\n%s\n%s\n%s\n\n",
            "# Magic Lantern Digtial Playprint Layout Script",
            args.script, "#",
            "# This is a Python default script generated by the mastering system.",
            "# Edit this script if necessary and use it as input to gendpp.");

    fprintf(scriptOut, "import dpp;\n");
#else
    fprintf(scriptOut, "%s: %s\n%s\n%s\n%s\n\n",
            "# Magic Lantern Digtial Playprint Layout Script",
            args.script, "#",
            "# This is a TCL default script generated by the mastering system.",
            "# Edit this script if necessary and use it as input to gendpp.");
#endif

    // Initialize WorkPrint readers and writers.
    mleDwpInit();
    MleDwpInput *in = new MleDwpInput;

    // Open the DWP.
    in->openFile(args.workprint);

    // Begin script.
    fprintf(scriptOut,"%s\n%s\n%s%s.cxx and %s.h.\n%s\n",
        "# codefile declares the name of the file in which to put the Group,",
		"# Media, Set and Scene chunk indices. The files will be named",
		"# ",
        args.chunkIndexFile, args.chunkIndexFile,
        "# These files should be linked to your title.");

#if defined(USE_PYTHON)
    fprintf(scriptOut, "%s(\"%s\")\n\n",
        "dpp.codefile",
        args.chunkIndexFile);
#else
    fprintf(scriptOut,"%s %s\n\n",
        "codefile",
        args.chunkIndexFile);
#endif

    // Build the entire DWP, instantiating each MleDwpItem.
    MleDwpItem *root;
    root = MleDwpItem::readAll(in);

    // Verify digital workprint.
    if (! mlVerifyTargetWorkprint(root,args.tags))
	{
        fprintf(stderr,"%s : %s\n",args.commandName,
                "Unable to verify Digital Workprint");
        exit(1);
    }

    // Set the DWP discriminator tags.
    setDiscriminators(root, args.tags);

    fprintf(scriptOut,"%s\n%s\n\n",
        "# Following is the default script for adding Actor/Role",
        "# Groups, Media, Sets and Scenes into your title.");

#if defined(USE_PYTHON)
    fprintf(scriptOut, "%s(\"%s\")\n",
        "dpp.beginplayprint",
         args.playprint);
#else
    fprintf(scriptOut, "%s %s\n",
        "beginplayprint",
         args.playprint);
#endif

    // Find all Actor Group items in the DWP.
    MleDwpFinder groupFinder;
    MleDwpGroup **groupItems;

    groupFinder.setType(MleDwpGroup::typeId);
    groupFinder.setFindAll(1);
    groupFinder.find(root);
    groupItems = (MleDwpGroup **)groupFinder.getItems();
    numItems = groupFinder.getNumItems();

    // Output to Playprint script.
    for (i = 0; i < numItems; i++)
	{
        name = groupItems[i]->getName();
#if defined(USE_PYTHON)
        fprintf(scriptOut, "dpp.addgroup(\"%s.chk\", \"%s\")\n", name, name);
#else
		fprintf(scriptOut, "addgroup %s.chk %s\n", name, name);
#endif
    }

    // Find all MediaRef items in the DWP.
    MleDwpFinder mediaRefFinder;
    MleDwpMediaRef **mediaRefItems;

    mediaRefFinder.setType(MleDwpMediaRef::typeId);
    mediaRefFinder.setFindAll(1);
    mediaRefFinder.find(root);
    mediaRefItems = (MleDwpMediaRef **)mediaRefFinder.getItems();
    numItems = mediaRefFinder.getNumItems();

    // Output to Playprint script.
    for (i = 0; i < numItems; i++)
	{
        name = mediaRefItems[i]->getName();
#if defined(USE_PYTHON)
        fprintf(scriptOut, "dpp.addmedia(\"%s.chk\", \"%s\")\n", name, name);
#else
		fprintf(scriptOut, "addmedia %s.chk %s\n", name, name);
#endif
    }

    // Process all Set instances in the DWP.
    processAddSet(scriptOut, root);

    // Find all Scene items in the DWP.
    MleDwpFinder sceneFinder;
    MleDwpScene **sceneItems;

    sceneFinder.setType(MleDwpScene::typeId);
    sceneFinder.setFindAll(1);
    sceneFinder.find(root);
    sceneItems = (MleDwpScene **)sceneFinder.getItems();
    numItems = sceneFinder.getNumItems();

    // Output to Playprint script.
    for (i = 0; i < numItems; i++)
	{
        name = sceneItems[i]->getName();
#if defined(USE_PYTHON)
        fprintf(scriptOut, "dpp.addscene(\"%s.chk\", \"%s\")\n", name, name);
#else
		fprintf(scriptOut, "addscene %s.chk %s\n", name, name);
#endif
    }

    // End script.
#if defined(USE_PYTHON)
    fprintf(scriptOut, "dpp.endplayprint()\n");
#else
    fprintf(scriptOut, "endplayprint\n");
	fprintf(scriptOut, "exit\n");
#endif

    // Close file.
    mlFClose(scriptOut);

    return 0;
}

