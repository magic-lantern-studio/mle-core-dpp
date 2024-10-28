/** @defgroup MleDPPMaster Magic Lantern Digital Playprint Library API - Master */

/**
 * @file genscene.cpp
 * @ingroup MleDPPMaster
 *
 * Program to generate Scene chunk files.
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
//  For information concerning this source code, contact Mark S. Millard,
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
#include <stdio.h>
#include <string.h>

// Include Magic Lantern header files.
//#include "mle/mlTitle.h"
#include <mle/mlFileio.h>
#include <mle/mlAssert.h>
#include <mle/MlePath.h>
#ifdef WIN32
#include <mle/mlGetOpt.h>
#include <mle/MleWin32Path.h>
#else
#include <unistd.h>
#include <mle/MleLinuxPath.h>
#endif /* WIN32 */

// Include Digital Workprint header files.
#include <mle/Dwp.h>
#include <mle/DwpScene.h>
#include <mle/DwpItem.h>
#include <mle/DwpInput.h>
#include <mle/DwpOutput.h>
#include <mle/DwpFinder.h>
#include <mle/DwpGroup.h>


// Include Digital Playprint header files.
#include "mle/DppSceneOutput.h"
#include "mle/DppScene.h"
#include "mle/scenechk.h"


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
    //cpath = strdup((char *)wpath->getPath());
    cpath = _strdup((char *)wpath->getPath());
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
    char       *commandName;  /* Name of command. */
    MlBoolean  byteOrder;     /* TRUE = Little Endian, FALSE = Big Endian. */
    MlBoolean  fixedPt;       /* TRUE = Fixed-Point, FALSE = Floating-Point. */
    char       *workprint;    /* Name of workprint file to build. */
    char       *sceneid;      /* Name of scene id include file to build. */
    char       language;      /* TRUE = Java, FALSE = C++. */
    char       *package;      /* The Java package. */
    char       *tags;         /* Digital Workprint tags. */
    int        verbose;       /* Be verbose. */
    char       *outputDir;    /* Directory to redirect output to. */
} ArgStruct;


const char *usage_str = "\
Syntax:   genscene  [-b|-l] [-f] [-j <package>|-c] [-v] [-d <dir>]\n\
                    <tags> <workprint> <scene_id_file>\n\
\n\
          -b                   Use Big Endian byte ordering (default)\n\
          -l                   Use Little Endian byte ordering\n\
          -f                   Generate fixed-point arithmetic values\n\
          -c                   Generate C++ compliant files (default)\n\
          -j <package>         Gerneate Java compliant files\n\
          -v                   Be verbose\n\
          -d <dir>             Directory where chunk files are generated\n\
          <tags>               Digital Workprint tags\n\
          <workprint>          Digital Workprint\n\
          <scene_id_file>      Generated Scene id file\n\
\n\
Function: Generate Magic Lantern Scene Chunk Files.\n\
\n\
A Scene chunk file will be generated for each Scene in the Digital Workprint.\n\
By default, C++ header files will be generated for the Scene ID file.\n\
If the -j option is specified, then Java files will be generated for the\n\
and Scene ID file.\n";


int parseArgs(int argc, char *argv[], ArgStruct *args)
{
    /* declare local variables. */
    int c;
    int errflg;
    extern char *optarg;
    extern int optind;
    
    errflg = 0;
    while ((c = getopt(argc, argv, "blfj:cvd:")) != -1)\
    {
        switch (c)
        {
          case 'b':
            /* Use Big Endian byte ordering. */
            args->byteOrder = FALSE;
            break;
          case 'l':
            /* Use Little Endian byte ordering. */
            args->byteOrder = TRUE;
            break;
          case 'f':
            /* Use Fixed-Point arithmetic. */
            args->fixedPt = TRUE;
            break;
          case 'j':
            /* Generate code for Java programming language. */
            args->language = TRUE;
            args->package = optarg;
            break;
          case 'c':
            /* Generate code for C++ programming language. */
            args->language = FALSE;
            break;
          case 'v':
            args->verbose = TRUE;
            break;
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
#ifdef WIN32
            args->tags = _strdup(argv[optind]);
#else
            args->tags = strdup(argv[optind]);
#endif /* WIN32 */
        } else if (! args->workprint)
        {
            args->workprint = getCanonicalPath(argv[optind]);
        } else if (! args->sceneid)
        {
#ifdef WIN32
            args->sceneid = _strdup(argv[optind]);
#else
            args->sceneid = strdup(argv[optind]);
#endif /* WIN32 */
        } else
        {
            fprintf(stderr,"%s\n",usage_str);
            return FALSE;
        }
    }
    
    /* If there is no specified workprint, complain. */
    if (args->tags == NULL ||
        args->workprint == NULL ||
        args->sceneid == NULL ||
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
    // Declare local variables.
    int i;
    ArgStruct args;
    MleDwpInput *in;
    MleDwpItem *root;
    MleDppSceneOutput *out;
    
    // Parse arguments.
    args.commandName = argv[0];
    args.byteOrder = FALSE;
    args.fixedPt = FALSE;
    args.workprint = NULL;
    args.sceneid = NULL;
    args.language = FALSE;
    args.tags = NULL;
    args.verbose = FALSE;
    args.outputDir = NULL;
    args.package = NULL;
    if (! parseArgs(argc, argv, &args))
    {
      exit(1);
    }
    
    // Initialize Digital Workprint/Playprint toolkit.
    mleDppInit();
    
    // Create a new DWP input object.
    in = new MleDwpInput;
    MLE_ASSERT(in);
    
    // Open the DWP.
    //__asm int 3h
    if (in->openFile(args.workprint) > 0)
    {
        fprintf(stderr,"%s : %s\n",args.commandName,
            "Unable to open Digital Workprint");
        exit(1);
    }
    
    // Build the entire DWP, instantiating each MleDwpItem.
    root = MleDwpItem::readAll(in);

    // Redirect the generated output.
    if (args.outputDir != NULL)
        MleDppSceneOutput::setOutputDirectory(args.outputDir);
    
    // Create an Scene File output object.
    out = new MleDppSceneOutput(root,args.fixedPt,args.byteOrder,
        args.language,args.package);
    if (! out->init(args.tags, args.sceneid))
    {
        fprintf(stderr,"%s : %s\n",args.commandName,
                "Unable to initialize Scene output");
        exit(1);
    }
    
    if (args.fixedPt)
      out->setScalarFormat(ML_SCALAR_FIXED_16_16);
    else
      out->setScalarFormat(ML_SCALAR_FLOAT);
    
    MLE_ASSERT(out);
    
    // Verify digital workprint.
    if (! mlVerifyTargetWorkprint(root,args.tags))
    {
        fprintf(stderr,"%s : %s\n",args.commandName,
                "Unable to verify Digital Workprint");
        exit(1);
    }
    
    // Find all the MleDwpScene items
    MleDwpFinder sceneFinder(MleDwpScene::typeId,NULL,1);
    sceneFinder.find(root);
    
    // Get the results of the find.
    int nScenes = sceneFinder.getNumItems();
    MleDwpItem **scenes = sceneFinder.getItems();

    // Write the scene and its chunk to output.
    for ( i = 0; i < nScenes; i++ )
    {
        ((MleDppScene *)scenes[i])->write(out);

        if (out->closeFile() != 0)
        {
            return -1;
        }

        // Create the scene chunk.
        char sceneChunkFile[FILENAME_MAX*2];
        if ( args.outputDir != NULL )
        {
#ifdef WIN32
            sprintf(sceneChunkFile, "%s\\%s.chk", args.outputDir, scenes[i]->getName());
#else /* ! WIN32 */
            sprintf(sceneChunkFile, "%s/%s.chk", args.outputDir, scenes[i]->getName());
#endif /* WIN32 */
        } else
        {
            sprintf(sceneChunkFile, "%s.chk", scenes[i]->getName());
        }
        MleDppSceneChunk chunk(sceneChunkFile,args.byteOrder);
    }

    // Clean-up and return.
    delete out;
    return 0;
}
