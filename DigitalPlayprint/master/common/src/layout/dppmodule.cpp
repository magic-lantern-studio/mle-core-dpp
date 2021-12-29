// COPYRIGHT_BEGIN
//
// Copyright (c) 2021 Wizzer Works
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

#define PY_SSIZE_T_CLEAN
#include <Python.h>

// Include system header files.
#if defined(WIN32)
#include <windows.h>
#else
#include <unistd.h>
#endif
#include <stdio.h>
#include <string.h>
#include <errno.h>

// Include Magic Lantern header files.
#if defined(WIN32)
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

// Declare external functions.
extern int gencodeStart(LayoutState *, char *, char *);
extern void gencodeTables(LayoutState *);

/*
 * An Exception object for the dpp module.
 */
static PyObject *DppError;

/*
 * Python method "system" - make a system call.
 */
static PyObject *
dpp_system(PyObject *self, PyObject *args)
{
    const char *command;
    int sts;

    // Parse python command arguments.
    if (! PyArg_ParseTuple(args, "s", &command))
        return NULL;

    // Execute system command
    sts = system(command);
    if (sts < 0) {
        PyErr_SetString(DppError, "System command failed");
        return NULL;
    }

    return PyLong_FromLong(sts);
}

/*
 * Python method "beginplayprint" - start a new Digital Playprint file.
 */
static PyObject *
dpp_beginPlayprint(PyObject *self, PyObject *args)
{
    //LayoutState *state = (LayoutState *)cd;
    LayoutState *state;
    const char *playprint;

    DppLayoutManager *mgr = DppLayoutManager::getInstance();
    state = mgr->getState();

    // Parse python command arguments
    if (! PyArg_ParseTuple(args, "s", &playprint))
        return NULL;

    // The "playprint" state variable is kept NULL when there is no
    // active playprint; can't have more than one active at a time.

    if ( state->m_playprint != NULL ) {
        PyErr_SetString(DppError, "Playprint file has already been specified");
        return NULL;
    }

#if defined(WIN32)
    char *tmpFile = (char *)mlMalloc(strlen(state->m_outputDir) + strlen(playprint) + 2);
    strcpy(tmpFile,state->m_outputDir);
    strcat(tmpFile,"\\");
    strcat(tmpFile, playprint);

    MlePath *dpppath;
    dpppath = new MleWin32Path((MlChar *)tmpFile,true);
    state->m_playprint = (char *)dpppath->getPlatformPath();

    mlFree(tmpFile);
#else /* ! WIN32 */
    char *tmpFile = (char *)mlMalloc(strlen(state->m_outputDir) + strlen(playprint) + 2);
    strcpy(tmpFile,state->m_outputDir);
    strcat(tmpFile,"/");
    strcat(tmpFile, playprint);

    MlePath *dpppath;
    dpppath = new MleLinuxPath((MlChar *)tmpFile,true);
    state->m_playprint = (char *)dpppath->getPlatformPath();

    mlFree(tmpFile);
#endif

    // Create the playprint file...

    if (state->m_byteOrder) {
        // Little Endian format.
        state->m_dpp = new MleDppOutput(state->m_playprint);
#if BYTE_ORDER != LITTLE_ENDIAN
        state->m_dpp->setSwap(TRUE);
#endif
    } else {
        // Big Endian format.
        state->m_dpp = new MleDppOutput(state->m_playprint);
#if BYTE_ORDER != BIG_ENDIAN
        state->m_dpp->setSwap(TRUE);
#endif
    }
    MLE_ASSERT(state->m_dpp);

    state->m_dpp->begin(state->m_byteOrder? MLE_DPP_CREATEPPLE: MLE_DPP_CREATEPPBE);
    state->m_dpp->writeHeader();

    Py_RETURN_NONE;
}

/*
 * Python method "endplayprint" - this is where the chunks actually get written.
 *   generate the table of contents.
 *   write out the TOC.
 *   write out the chunks.
 *   wrap up the playprint.
 */
static PyObject *
dpp_endPlayprint(PyObject *self, PyObject *args)
{
    LayoutState *state;

    DppLayoutManager *mgr = DppLayoutManager::getInstance();
    state = mgr->getState();
    // If m_dpp is null, then the playprint has already been completed.
    if (state->m_dpp == nullptr) Py_RETURN_NONE;

    // Generate the TOC...
    state->m_dpp->beginTOC(state->m_chunks->getUsed());

    // Write out the chunks...
    for (int i = 0; i < state->m_chunks->getUsed() ; i++) {
        switch (state->m_chunks->getType(i)) {
            case CHUNK_GROUP:
                //contents = readFileToMemory(state->m_chunks->getFilename(i), &size);
                //MLE_ASSERT(contents!=NULL);
                state->m_dpp->writeGroup(state->m_chunks, i);
                break;
            case CHUNK_MEDIA:
                state->m_dpp->writeMedia(state->m_chunks->getFilename(i));
                break;
            case CHUNK_SET:
                state->m_dpp->writeSet(state->m_chunks->getFilename(i));
                break;
            case CHUNK_SCENE:
                state->m_dpp->writeScene(state->m_chunks, i);
                break;
        }
    }

    // Seal up the playprint file...
    state->m_dpp->endTOC();
    state->m_dpp->end();
    delete state->m_dpp;
    state->m_dpp = nullptr;

    // Generate the table of global symbols for this set...
    gencodeTables(state);

    //delete state.m_chunks;
    //state.m_chunks = new MleDppChunkTable();
    //state.m_playprint = nullptr;

    // Close the generated files.
    mlFClose(state->m_codefd);
    if (state->m_language != TRUE)
        mlFClose(state->m_headerfd);

    Py_RETURN_NONE;
}

/*
 * Python method "codefile" - initialize code generation. The generated code
 * is index references into the Digital Playprint.
 */
static PyObject *
dpp_codeFile(PyObject *self, PyObject *args)
{
    LayoutState *state;
    const char *codefile;

    DppLayoutManager *mgr = DppLayoutManager::getInstance();
    state = mgr->getState();

    // Parse python command arguments
    if (! PyArg_ParseTuple(args, "s", &codefile))
        return NULL;

    if (state->m_codefile != NULL) {
        if (state->m_language != TRUE) {
            // Closing C/C++ files.
            mlFClose(state->m_codefd);
            mlFClose(state->m_headerfd);
        } else {
            // Closing Java file.
            mlFClose(state->m_codefd);
        }
    }

    char *codefilename = nullptr;
    char *headerfilename = nullptr;
    if (state->m_language) {
        // Java code generation.
        state->m_codefile = strdup(codefile);
        codefilename = new char[strlen(state->m_codefile) + strlen(JAVA_CODE_FILENAME_EXT) + 1];
        strcpy(codefilename, state->m_codefile);
        strcat(codefilename, JAVA_CODE_FILENAME_EXT);
    } else {
        // C/C++ code generation.
        state->m_codefile = strdup(codefile);
        codefilename = new char[strlen(state->m_codefile) + strlen(CPP_CODE_FILENAME_EXT) + 1];
        strcpy(codefilename, state->m_codefile);
        strcat(codefilename, CPP_CODE_FILENAME_EXT);
        headerfilename = new char[strlen(state->m_codefile) + strlen(CPP_HEADER_FILENAME_EXT) + 1];
        strcpy(headerfilename, state->m_codefile);
        strcat(headerfilename, CPP_HEADER_FILENAME_EXT);
    }

    if (state->m_outputDir != NULL) {
#if defined(WIN32)
        char *tmpFilename = new char[strlen(state->m_outputDir) + strlen(codefilename) + 2];
        strcpy(tmpFilename,state->m_outputDir);
        strcat(tmpFilename,"\\");
        strcat(tmpFilename, codefilename);

        MlePath *tmpSourcePath;
        tmpSourcePath = new MleWin32Path((MlChar *)tmpFilename,true);
        delete [] codefilename;
        codefilename = (char *)tmpSourcePath->getPlatformPath();

        delete [] tmpFilename;

        if (state->m_language == FALSE) {
            // Process header file name for C/C++ generation.
            tmpFilename = new char[strlen(state->m_outputDir) + strlen(headerfilename) + 2];
            strcpy(tmpFilename,state->m_outputDir);
            strcat(tmpFilename,"\\");
            strcat(tmpFilename,headerfilename);

            MlePath *tmpHeaderPath;
            tmpHeaderPath = new MleWin32Path((MlChar *)tmpFilename,true);
            delete [] headerfilename;
            headerfilename = (char *)tmpHeaderPath->getPlatformPath();
        } else {
            headerfilename = NULL;
        }
#else /* ! WIN32 */
        char *tmpFilename = new char[strlen(state->m_outputDir) + strlen(codefilename) + 2];
        strcpy(tmpFilename,state->m_outputDir);
        strcat(tmpFilename,"/");
        strcat(tmpFilename,codefilename);

        MlePath *tmpSourcePath;
        tmpSourcePath = new MleLinuxPath((MlChar *)tmpFilename,true);
        delete [] codefilename;
        codefilename = (char *)tmpSourcePath->getPlatformPath();

        delete [] tmpFilename;
        if (state->m_language == FALSE) {
            // Process header file name for C/C++ generation.
            tmpFilename = new char[strlen(state->m_outputDir) + strlen(headerfilename) + 2];
            strcpy(tmpFilename,state->m_outputDir);
            strcat(tmpFilename,"/");
            strcat(tmpFilename,headerfilename);

            MlePath *tmpHeaderPath;
            tmpHeaderPath = new MleLinuxPath((MlChar *)tmpFilename,true);
            delete [] headerfilename;
            headerfilename = (char *)tmpHeaderPath->getPlatformPath();
        } else {
            headerfilename = NULL;
        }
#endif
    }

    // Open the file(s) for generation.
    if (state->m_language) {
        // Generating code for Java.
        if ((state->m_codefd = mlFOpen(codefilename, "w")) == NULL) {
            delete [] codefilename;
            char errmsg[BUFSIZ];
            sprintf(errmsg, "%s: %s", codefile, strerror(errno));
            PyErr_SetString(DppError, errmsg);
            return NULL;
        }
        state->m_headerfd = NULL;
    } else {
         // Generating code for C/C++.
         if ((state->m_codefd = mlFOpen(codefilename, "w")) == NULL ||
             (state->m_headerfd = mlFOpen(headerfilename, "w")) == NULL )
         {
             delete [] codefilename;
             delete [] headerfilename;
             char errmsg[BUFSIZ];
             sprintf(errmsg, "%s: %s", codefile, strerror(errno));
             PyErr_SetString(DppError, errmsg);
             return NULL;
         }
    }

    // Start the code generation.
    if (gencodeStart(state, headerfilename, codefilename)) {
         return NULL;
    }

    // Return success.
    Py_RETURN_NONE;
}

/*
 * Python method "addgroup" - add a Group chunk to the Digital Playprint.
 */
static PyObject *
dpp_addGroup(PyObject *self, PyObject *args)
{
    LayoutState *state;
    const char *filename;
    const char *name;

    DppLayoutManager *mgr = DppLayoutManager::getInstance();
    state = mgr->getState();

    // Parse python command arguments
    if (! PyArg_ParseTuple(args, "ss", &filename, &name))
        return NULL;

    char *chunkfile;
    if (state->m_inputDir != NULL) {
#if defined(WIN32)
        char *tmpFile = (char *)mlMalloc(strlen(state->m_inputDir) + strlen(filename) + 2);
        strcpy(tmpFile,state->m_inputDir);
        strcat(tmpFile,"\\");
        strcat(tmpFile, filename);

        MlePath *chunkpath;
        chunkpath = new MleWin32Path((MlChar *)tmpFile,true);
        chunkfile = (char *)chunkpath->getPlatformPath();

        mlFree(tmpFile);
#else /* ! WIN32 */
        char *tmpFile = (char *)mlMalloc(strlen(state->m_inputDir) + strlen(filename) + 2);
        strcpy(tmpFile,state->m_inputDir);
        strcat(tmpFile,"/");
        strcat(tmpFile, filename);

        MlePath *chunkpath;
        chunkpath = new MleLinuxPath((MlChar *)tmpFile,true);
        chunkfile = (char *)chunkpath->getPlatformPath();

        mlFree(tmpFile);
#endif
    } else {
        chunkfile = (char *)filename;
    }

    if (access(chunkfile, R_OK) < 0) {
        char errmsg[BUFSIZ];
        sprintf(errmsg, "%s: %s", filename, strerror(errno));
        PyErr_SetString(DppError, errmsg);
        return NULL;
    }

    // Add this entry to the chunks table, to be processed during
    // the endplayprint command.
    // Check for sufficient storage first.

    state->m_chunks->addEntry(CHUNK_GROUP, chunkfile, (char *)name);

    // Return success.
    Py_RETURN_NONE;
}

/*
 * Python method "addmedia" - add a Media Reference chunk to the Digital Playprint.
 */
static PyObject *
dpp_addMedia(PyObject *self, PyObject *args)
{
    LayoutState *state;
    const char *filename;
    const char *name;

    DppLayoutManager *mgr = DppLayoutManager::getInstance();
    state = mgr->getState();

    // Parse python command arguments
    if (! PyArg_ParseTuple(args, "ss", &filename, &name))
        return NULL;

    char *chunkfile;
    if (state->m_inputDir != NULL) {
#if defined(WIN32)
        char *tmpFile = (char *)mlMalloc(strlen(state->m_inputDir) + strlen(filename) + 2);
        strcpy(tmpFile,state->m_inputDir);
        strcat(tmpFile,"\\");
        strcat(tmpFile, filename);

        MlePath *chunkpath;
        chunkpath = new MleWin32Path((MlChar *)tmpFile,true);
        chunkfile = (char *)chunkpath->getPlatformPath();

        mlFree(tmpFile);
#else /* ! WIN32 */
        char *tmpFile = (char *)mlMalloc(strlen(state->m_inputDir) + strlen(filename) + 2);
        strcpy(tmpFile,state->m_inputDir);
        strcat(tmpFile,"/");
        strcat(tmpFile, filename);

        MlePath *chunkpath;
        chunkpath = new MleLinuxPath((MlChar *)tmpFile,true);
        chunkfile = (char *)chunkpath->getPlatformPath();

        mlFree(tmpFile);
#endif
    } else {
        chunkfile = (char *)filename;
    }

    if (access(chunkfile, R_OK) < 0) {
        char errmsg[BUFSIZ];
        sprintf(errmsg, "%s: %s", filename, strerror(errno));
        PyErr_SetString(DppError, errmsg);
        return NULL;
    }

    // Add this entry to the chunks table, to be processed during
    // the endplayprint command.
    // Check for sufficient storage first.

    state->m_chunks->addEntry(CHUNK_MEDIA, chunkfile, (char *)name);

    // Return success.
    Py_RETURN_NONE;
}

/*
 * Python method "addset" - add a Set chunk to the Digital Playprint
 */
static PyObject *
dpp_addSet(PyObject *self, PyObject *args)
{
    LayoutState *state;
    const char *filename;
    const char *name;

    DppLayoutManager *mgr = DppLayoutManager::getInstance();
    state = mgr->getState();

    // Parse python command arguments
    if (! PyArg_ParseTuple(args, "ss", &filename, &name))
        return NULL;

    char *chunkfile;
    if (state->m_inputDir != NULL) {
#if defined(WIN32)
        char *tmpFile = (char *)mlMalloc(strlen(state->m_inputDir) + strlen(filename) + 2);
        strcpy(tmpFile,state->m_inputDir);
        strcat(tmpFile,"\\");
        strcat(tmpFile, filename);

        MlePath *chunkpath;
        chunkpath = new MleWin32Path((MlChar *)tmpFile,true);
        chunkfile = (char *)chunkpath->getPlatformPath();

        mlFree(tmpFile);
#else /* ! WIN32 */
        char *tmpFile = (char *)mlMalloc(strlen(state->m_inputDir) + strlen(filename) + 2);
        strcpy(tmpFile,state->m_inputDir);
        strcat(tmpFile,"/");
        strcat(tmpFile, filename);

        MlePath *chunkpath;
        chunkpath = new MleLinuxPath((MlChar *)tmpFile,true);
        chunkfile = (char *)chunkpath->getPlatformPath();

        mlFree(tmpFile);
#endif
    } else {
        chunkfile = (char *)filename;
    }

    if (access(chunkfile, R_OK) < 0) {
        char errmsg[BUFSIZ];
        sprintf(errmsg, "%s: %s", filename, strerror(errno));
        PyErr_SetString(DppError, errmsg);
        return NULL;
    }

    // Add this entry to the chunks table, to be processed during
    // the endplayprint command.
    // Check for sufficient storage first.

    state->m_chunks->addEntry(CHUNK_SET, chunkfile, (char *)name);

    // Return success.
    Py_RETURN_NONE;
}

/*
 * Python method "addscene" - add a Scene chunk to the Digital Playprint
 */
static PyObject *
dpp_addScene(PyObject *self, PyObject *args)
{
    LayoutState *state;
    const char *filename;
    const char *name;

    DppLayoutManager *mgr = DppLayoutManager::getInstance();
    state = mgr->getState();

    // Parse python command arguments
    if (! PyArg_ParseTuple(args, "ss", &filename, &name))
        return NULL;

    char *chunkfile;
    if (state->m_inputDir != NULL) {
#if defined(WIN32)
        char *tmpFile = (char *)mlMalloc(strlen(state->m_inputDir) + strlen(filename) + 2);
        strcpy(tmpFile,state->m_inputDir);
        strcat(tmpFile,"\\");
        strcat(tmpFile, filename);

        MlePath *chunkpath;
        chunkpath = new MleWin32Path((MlChar *)tmpFile,true);
        chunkfile = (char *)chunkpath->getPlatformPath();

        mlFree(tmpFile);
#else /* ! WIN32 */
        char *tmpFile = (char *)mlMalloc(strlen(state->m_inputDir) + strlen(filename) + 2);
        strcpy(tmpFile,state->m_inputDir);
        strcat(tmpFile,"/");
        strcat(tmpFile, filename);

        MlePath *chunkpath;
        chunkpath = new MleLinuxPath((MlChar *)tmpFile,true);
        chunkfile = (char *)chunkpath->getPlatformPath();

        mlFree(tmpFile);
#endif
    } else {
        chunkfile = (char *)filename;
    }

    if (access(chunkfile, R_OK) < 0) {
        char errmsg[BUFSIZ];
        sprintf(errmsg, "%s: %s", filename, strerror(errno));
        PyErr_SetString(DppError, errmsg);
        return NULL;
    }

    // Add this entry to the chunks table, to be processed during
    // the endplayprint command.
    // Check for sufficient storage first.

    state->m_chunks->addEntry(CHUNK_SCENE, chunkfile, (char *)name);

    // Return success.
    Py_RETURN_NONE;
}

static PyObject *
dpp_beginList(PyObject *self, PyObject *args)
{
    //LayoutState *state;
    const char *listname;

    //DppLayoutManager *mgr = DppLayoutManager::getInstance();
    //state = mgr->getState();

    // Parse python command arguments
    if (! PyArg_ParseTuple(args, "s", &listname))
        return NULL;

    PyErr_SetString(DppError, "Command not implemented yet");
    return NULL;
}

static PyObject *
dpp_endList(PyObject *self, PyObject *args)
{
    //LayoutState *state;
    const char *listname;

    //DppLayoutManager *mgr = DppLayoutManager::getInstance();
    //state = mgr->getState();

    // Parse python command arguments
    if (! PyArg_ParseTuple(args, "s", &listname))
        return NULL;

    PyErr_SetString(DppError, "Command not implemented yet");
    return NULL;
}


static PyMethodDef DppMethods[] = {
    {"system",  dpp_system, METH_VARARGS,
     "Execute a shell command."},
    {"beginplayprint",  dpp_beginPlayprint, METH_VARARGS,
     "Begin a Digital Playprint."},
    {"endplayprint",  dpp_endPlayprint, METH_VARARGS,
     "Terminate a Digital Playprint."},
    {"codefile",  dpp_codeFile, METH_VARARGS,
     "Specify the name of the generated code."},
    {"addgroup",  dpp_addGroup, METH_VARARGS,
     "Add a Group to the Digital Playprint."},
    {"addmedia",  dpp_addMedia, METH_VARARGS,
     "Add Media Reference to the Digital Playprint."},
    {"addset",  dpp_addSet, METH_VARARGS,
     "Add a Set to the Digital Playprint."},
    {"addscene",  dpp_addScene, METH_VARARGS,
     "Add a Scene to the Digital Playprint."},
    {NULL, NULL, 0, NULL}        /* Sentinel */
};

static struct PyModuleDef dppmodule = {
    PyModuleDef_HEAD_INIT,
    "dpp",   /* name of module */
    NULL,    /* module documentation, may be NULL */
    -1,      /* size of per-interpreter state of the module,
                or -1 if the module keeps state in global variables. */
    DppMethods,
    NULL, NULL, NULL, NULL
};

PyMODINIT_FUNC
PyInit_dpp(void)
{
    PyObject *module;

    // Create an new dpp module.
    module = PyModule_Create(&dppmodule);
    if (module == NULL)
        return NULL;

    // Initialize the dpp module Exception.
    DppError = PyErr_NewException("dpp.error", NULL, NULL);
    Py_XINCREF(DppError);
    if (PyModule_AddObject(module, "error", DppError) < 0) {
        Py_XDECREF(DppError);
        Py_CLEAR(DppError);
        Py_DECREF(module);
        return NULL;
    }

    return module;
}

void
endPlayprint() {
    dpp_endPlayprint(NULL, NULL);
}
