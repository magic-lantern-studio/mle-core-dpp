/** @defgroup MleDPPMaster Magic Lantern Digital Playprint Library API - Master */

/**
 * @file pymain.cpp
 * @ingroup MleDPPMaster
 *
 * Playprint layout script interpreter
 *
 * This is a Python-based  interpreter for playprint layout commands.
 * It operates in two passes - first collecting the names of the chunks
 * to place into the playprint and building the TOC, then writing the
 * chunks to the playprint.
 *
 * This code is structured so that it can write multiple playprints from
 * a single script.
 */

// COPYRIGHT_BEGIN
//
// Copyright (c) 2021-2024 Wizzer Works
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


// Include Python header files.
#include <Python.h>

// Include system header files.
#ifdef WIN32
#include <windows.h>
#else
#include <unistd.h>
#endif
#include <stdio.h>
#include <string.h>
#include <errno.h>

// Include Magic Lantern header files.
#ifdef WIN32
#include <mle/mlGetOpt.h>
#include <mle/MleWin32Path.h>
#else
#include <unistd.h>
#include <mle/MleLinuxPath.h>
#endif
#include <mle/mlMalloc.h>
#include <mle/mlFileio.h>
#include <mle/mlConfig.h>
#include <mle/MlePath.h>

#include "pplayout.h"
#include "DppLayoutManager.h"

const char *usage_str = "\
Syntax:   gendpp  [-b|-l] [-j <package> | -c] [-d <dir>] [-s <dir>]\n\
                  <tags> <workprint> <script>\n\
\n\
          -b            Use Big Endian byte ordering\n\
          -l            Use Little Endian byte ordering\n\
          -c            Generate C++ compliant files (default)\n\
          -j <package>  Generate Java compliant files\n\
          -d <dir>      Directory where ouput is generated\n\
          -s <dir>      Directory where resources can be found,\n\
                        relative to script commands\n\
          <tags>        Digital Workprint tags\n\
          <workprint>   Digital Workprint\n\
          <script>      Playprint Layout Script\n\
\n\
Function: Generate Magic Lantern Digital Playprint.\n\
\n\
In addition to generating the DPP, gendpp will generate the code\n\
necessary for locating the location of each chunk in the playprint.\n";

// Declare forward references to local procedures.
static char *readFileToMemory(char *, int *);

// Declare external references.
extern "C" PyObject *PyInit_dpp(void);
extern void endPlayprint();

//
// Get a canonical path definition for the specified input.
//
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

//
// Parse command line arguments
//
static int parseArgs(int argc, char *argv[], LayoutState *state)
{
    int c, errflg;
    extern char *optarg;
    extern int optind;

    errflg = 0;
    while ((c = getopt(argc, argv, "blcj:d:s:")) != EOF ) {
        switch (c) {
          case 'b':
            // Big Endian.
            state->m_byteOrder = FALSE;
            break;
          case 'l':
            // Little Endian
            state->m_byteOrder = TRUE;
            break;
          case 'j':
            // Generate code for Java programming language.
            state->m_language = TRUE;
            state->m_package = optarg;
            break;
          case 'c':
            // Generate code for C++ programming language.
            state->m_language = FALSE;
            break;
          case 'd':
            state->m_outputDir = optarg;
            break;
          case 's':
            state->m_inputDir = optarg;
            break;
          case '?':
            errflg++;
        }
    }

    for ( ; optind < argc; optind++) {
        if (! state->m_tags) {
            state->m_tags = strdup(argv[optind]);
        } else if (! state->m_workprint) {
            state->m_workprint = getCanonicalPath(argv[optind]);
        } else if (! state->m_scriptfile) {
            state->m_scriptfile = strdup(argv[optind]);
        } else {
            fprintf(stderr,"%s\n",usage_str);
            return FALSE;
        }
    }

    // If there is no specified tags or scriptfile, complain.
    if (state->m_tags == nullptr ||
        state->m_workprint == nullptr ||
        state->m_scriptfile == nullptr ||
        ((state->m_language == TRUE) && (state->m_package == nullptr)))
    {
        fprintf(stderr,"%s\n", usage_str);
        return FALSE;
    }

    return TRUE;
}

//
// Application initialization and script interpretation.
//
int main(int argc, char *argv[])
{
    LayoutState *state;
    char *script;

    // Initialize the playprint layout state.
    state = (LayoutState *)mlMalloc(sizeof(LayoutState));
    state->m_commandName = argv[0];
    state->m_byteOrder = FALSE;
    state->m_workprint = nullptr;
    state->m_playprint = nullptr;
    state->m_outputDir = nullptr;
    state->m_inputDir = nullptr;
    state->m_codefile = nullptr;
    state->m_chunks = new MleDppChunkTable();
    state->m_scriptfile = nullptr;
    state->m_root = nullptr;
    state->m_tags = nullptr;
    state->m_language = FALSE;
    state->m_package = nullptr;

    DppLayoutManager *mgr = DppLayoutManager::getInstance();
    mgr->setState(state);

    // Parse arguments.
    if (! parseArgs(argc, argv, state) ) {
        exit(1);
    }
    if (state->m_outputDir == nullptr)
        state->m_outputDir = strdup(".");

    //__asm int 3h

    // Read in the script file...
    MlePath *scriptFile;
#if WIN32
    scriptFile = new MleWin32Path((MlChar *)(state->m_scriptfile), true);
#else /* ! WIN32 */
    scriptFile = new MleLinuxPath((MlChar *)state->m_scriptfile, true);;
#endif

    if ((script = readFileToMemory((char *)scriptFile->getPlatformPath(), NULL)) == NULL ) {
        perror(state->m_scriptfile);
        exit(2);
    }
    if (scriptFile != NULL)
        delete scriptFile;

    // Setup Python and execute sript...
    // Add the built-in module, before Py_Initialize.
    if (PyImport_AppendInittab("dpp", PyInit_dpp) == -1) {
        fprintf(stderr, "Error: could not extend in-built modules table.\n");
        exit(1);
    }

    // Initialize Python interpreter.
    Py_Initialize();

    // Call python command script.
    //PyRun_SimpleString("print('Hello World from Embedded Python!!!')");
    //FILE *fp = _Py_fopen(state->m_scriptfile, "r");
    FILE *fp = mlFOpen(state->m_scriptfile, "r");
    PyRun_SimpleFile(fp, state->m_scriptfile);
    
    // Terminate Python interpreter.
    Py_Finalize();

    // Todo: validate python script completion.

    // Check for more chunks cached.  This would happen if the script
    // ommitted a final endplayprint command, or simply used the -o
    // option to specify the output file.

    if (state->m_chunks->getUsed() > 0) {
        endPlayprint();
    }

    // Clean up state.
    if (state->m_root != nullptr) delete state->m_root;
    if (state->m_tags != nullptr) mlFree(state->m_tags);
    if (state->m_workprint != nullptr) mlFree(state->m_workprint);
    if (state->m_scriptfile != nullptr) mlFree(state->m_scriptfile);
    if (state != nullptr) mlFree(state);
    mlFree(script);
    
    return 0;
}

//
// Read the entire script file into memory.
//    Input parameter:  filename == NULL, read from stdin
//                      filename == "-", read from stdin
//    Output Parameter: size will contain the number of bytes read.
//

#define READ_CHUNK_SIZE 4096

static char *readFileToMemory(char *filename, int *size)
{
    FILE *ifd;
    int closeMe;
    char *data;
    int length, used, allocked;

    if (filename == NULL || strcmp(filename,"-") == 0 ) {
        ifd = stdin;
        closeMe = 0;
    } else {
        if ((ifd = mlFOpen(filename, "r")) == NULL ) {
            return NULL;
        }
        closeMe = 1;
    }

    data = (char *) mlMalloc(READ_CHUNK_SIZE);
    allocked = READ_CHUNK_SIZE;
    used = 0;
    while ((length = mlFRead(data+used, sizeof(char), READ_CHUNK_SIZE, ifd)) > 0) {
        used += length;
        allocked += READ_CHUNK_SIZE;
        data = (char *) mlRealloc(data, allocked);
    }

    if (closeMe) {
        mlFClose(ifd);
    }

    if (used != allocked) {
        data = (char *) mlRealloc(data, used);
    }

    if (size != NULL) {
        *size = used;
    }

    return data;
}
