/** @defgroup MleDPPMaster Magic Lantern Digital Playprint Library API - Master */

/**
 * @file main.cpp
 * @ingroup MleDPPMaster
 *
 * Playprint layout script interpreter
 *
 * This is a TCL-based  interpreter for playprint layout commands.
 * It operates in two passes - first collecting the names of the chunks
 * to place into the playprint and building the TOC, then writing the
 * chunks to the playprint.
 *
 * This code is structured so that it can write multiple playprints from
 * a single script.
 *
 * @author Mark S. Millard
 * @date September 15, 2004
 */


// COPYRIGHT_BEGIN
//
// Copyright (c) 2015-2020 Wizzer Works
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
#else
#include <unistd.h>
#endif
#include <stdio.h>
#include <string.h>
#include <errno.h>

// Include TCL header files.
//#define STATIC_BUILD
#include <tcl.h>

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

const char *usage_str = "\
Syntax:   gendpp  [-b|-l] [-j <package> | -c] [-d <dir>] [-s <dir>]\n\
                  <tags> <workprint> <script>\n\
\n\
          -b            Use Big Endian byte ordering\n\
          -l            Use Little Endian byte ordering\n\
          -c            Generate C++ compliant files (default)\n\
          -j <package>  Gerneate Java compliant files\n\
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

/*
 * Declare forward references to local procedures:
 */

static int parseArgs(int argc, char *argv[], LayoutState *);
static char *readFileToMemory(char *, int *);
int gencodeStart(LayoutState *, char *, char *);
void gencodeTables(LayoutState *);

Tcl_CmdProc codeFile;
Tcl_CmdProc beginPlayPrint;
Tcl_CmdProc endPlayPrint;
Tcl_CmdProc addGroup;
Tcl_CmdProc addMedia;
Tcl_CmdProc addSet;
Tcl_CmdProc addScene;
Tcl_CmdProc beginList;
Tcl_CmdProc endList;


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

/*
 * main
 *
 * Application initialization and script interpretation.
 */

int main(int argc, char *argv[])
{
    Tcl_Interp *interp;
    int code;
    char *script;
    LayoutState state;

    state.m_commandName = argv[0];
    state.m_byteOrder = FALSE;
	state.m_workprint = NULL;
    state.m_playprint = NULL;
	state.m_outputDir = NULL;
	state.m_inputDir = NULL;
    state.m_codefile = NULL;
    state.m_chunks = new MleDppChunkTable();
    state.m_scriptfile = NULL;
    state.m_root = NULL;
    state.m_tags = NULL;
	state.m_language = FALSE;
	state.m_package = NULL;

    /* Parse arguments. */
    if ( !parseArgs(argc, argv, &state) )
	{
		exit(1);
    }

    //__asm int 3h

    /*
     * Read in the script file...
     */
	MlePath *scriptFile;
#if WIN32
	scriptFile = new MleWin32Path((MlChar *)state.m_scriptfile, true);
#else /* ! WIN32 */
	scriptFile = new MleLinuxPath((MlChar *)state.m_scriptfile, true);;
#endif

    if ((script = readFileToMemory((char *)scriptFile->getPlatformPath(), NULL)) == NULL )
	{
		perror(state.m_scriptfile);
		exit(2);
    }
	if (scriptFile != NULL)
		delete scriptFile;

    /*
     * Start interpreting the script...
     */

    interp = Tcl_CreateInterp();
    Tcl_CreateCommand( interp, "codefile", codeFile,
		      (ClientData)&state, (Tcl_CmdDeleteProc *)NULL);
    Tcl_CreateCommand( interp, "beginplayprint", beginPlayPrint,
		      (ClientData)&state, (Tcl_CmdDeleteProc *)NULL);
    Tcl_CreateCommand( interp, "endplayprint", endPlayPrint,
		      (ClientData)&state, (Tcl_CmdDeleteProc *)NULL);
    Tcl_CreateCommand( interp, "addgroup", addGroup,
		      (ClientData)&state, (Tcl_CmdDeleteProc *)NULL);
    Tcl_CreateCommand( interp, "addmedia", addMedia,
		      (ClientData)&state, (Tcl_CmdDeleteProc *)NULL);
    Tcl_CreateCommand( interp, "addset", addSet,
		      (ClientData)&state, (Tcl_CmdDeleteProc *)NULL);
    Tcl_CreateCommand( interp, "addscene", addScene,
		      (ClientData)&state, (Tcl_CmdDeleteProc *)NULL);
    Tcl_CreateCommand( interp, "beginlist", beginList,
		      (ClientData)&state, (Tcl_CmdDeleteProc *)NULL);
    Tcl_CreateCommand( interp, "endlist", endList,
		      (ClientData)&state, (Tcl_CmdDeleteProc *)NULL);
    code = Tcl_Eval(interp, script);

    /*
     * Check for successful script interpretation...
     */

    //if( *interp->result != 0)
	if( Tcl_GetStringResult(interp) != 0 )
	{
		//fprintf(stderr, "%s\n", interp->result);
		fprintf(stderr, "%s\n", Tcl_GetStringResult(interp)); 
    }
    if ( code != TCL_OK )
	{
		exit(1);
    }

    /*
     * Check for more chunks cached.  This would happen if the script
     * ommitted a final endplayprint command, or simply used the -o
     * option to specify the output file.
     */

    if ( state.m_chunks->getUsed() > 0 )
	{
		endPlayPrint((ClientData)&state, interp, 1, NULL);
    }

    /*
     * Clean up workprint part of state
     */
    delete state.m_root;

    return 0;
}


/*
 * Parse command line arguments
 */

static int parseArgs(int argc, char *argv[], LayoutState *state)
{
    int c, errflg;
    extern char *optarg;
    extern int optind;

    errflg = 0;
    while ((c = getopt(argc, argv, "blcj:d:s:")) != EOF )
	{
		switch (c)
		{
		  case 'b':
			state->m_byteOrder = FALSE;
			break;
		  case 'l':
			state->m_byteOrder = TRUE;
			break;
		  case 'j':
			/* Generate code for Java programming language. */
			state->m_language = TRUE;
			state->m_package = optarg;
			break;
		  case 'c':
			/* Generate code for C++ programming language. */
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

    for ( ; optind < argc; optind++)
	{
        if (! state->m_tags)
		{
            state->m_tags = strdup(argv[optind]);
        } else if (! state->m_workprint)
		{
			state->m_workprint = getCanonicalPath(argv[optind]);
		} else if (! state->m_scriptfile)
		{
            state->m_scriptfile = strdup(argv[optind]);
        } else
		{
            fprintf(stderr,"%s\n",usage_str);
            return FALSE;
        }
    }
    
    /* If there is no specified tags or scriptfile, complain. */
    if (state->m_tags == NULL ||
		state->m_workprint == NULL ||
        state->m_scriptfile == NULL ||
		((state->m_language == TRUE) && (state->m_package == NULL)))
	{
        fprintf(stderr,"%s\n",usage_str);
        return FALSE;
    }

    return TRUE;
}


/*
 * Read the entire script file into memory.
 *    filename == NULL read from stdin
 *    filename == "-" read from stdin
 */

#define READ_CHUNK_SIZE 4096

static char *readFileToMemory(char *filename, int *size)
{
    FILE *ifd;
    int closeMe;
    char *data;
    int length, used, allocked;

    if ( filename==NULL || strcmp(filename,"-")==0 )
	{
		ifd = stdin;
		closeMe = 0;
    } else
	{
		if ( (ifd = mlFOpen(filename, "r")) == NULL )
		{
			return NULL;
		}
		closeMe = 1;
    }

    data = (char *)mlMalloc(READ_CHUNK_SIZE);
    allocked = READ_CHUNK_SIZE;
    used = 0;
    while((length = mlFRead(data+used,sizeof(char),READ_CHUNK_SIZE,ifd)) > 0)
	{
		used += length;
		allocked += READ_CHUNK_SIZE;
		data = (char *)mlRealloc(data, allocked);
    }

    if ( closeMe )
	{
		mlFClose(ifd);
    }

    if ( used != allocked )
	{
		data = (char *)mlRealloc(data, used);
    }

    if ( size != NULL )
	{
		*size = used;
    }

    return data;
}
    
/*
 * TCL command "addgroup filename name"
 */

int addGroup(ClientData cd, Tcl_Interp *interp, int argc, CONST84 char *argv[])
{
    LayoutState *state = (LayoutState *)cd;

    if ( argc != 3 )
	{
		//interp->result = "wrong # args";
		Tcl_SetResult(interp, "wrong # args", TCL_STATIC);
		return TCL_ERROR;
    }

	char *chunkfile;
	if (state->m_inputDir != NULL)
	{
#if WIN32
		char *tmpFile = (char *)mlMalloc(strlen(state->m_inputDir) + strlen(argv[1]) + 2);
		strcpy(tmpFile,state->m_inputDir);
		strcat(tmpFile,"\\");
		strcat(tmpFile,argv[1]);

		MlePath *chunkpath;
		chunkpath = new MleWin32Path((MlChar *)tmpFile,true);
		chunkfile = (char *)chunkpath->getPlatformPath();

		mlFree(tmpFile);
#else /* ! WIN32 */
		char *tmpFile = (char *)mlMalloc(strlen(state->m_inputDir) + strlen(argv[1]) + 2);
		strcpy(tmpFile,state->m_inputDir);
		strcat(tmpFile,"/");
		strcat(tmpFile,argv[1]);

		MlePath *chunkpath;
		chunkpath = new MleLinuxPath((MlChar *)tmpFile,true);
		chunkfile = (char *)chunkpath->getPlatformPath();

		mlFree(tmpFile);
#endif
	} else
	{
		chunkfile = (char *)argv[1];
	}

    if ( access(chunkfile, R_OK) < 0 )
	{
		char errmsg[BUFSIZ];
		sprintf(errmsg, "%s: %s", argv[1], strerror(errno));
		//interp->result = errmsg;
		Tcl_SetResult(interp, errmsg, TCL_STATIC);
		return TCL_ERROR;
    }

    /*
     * Add this entry to the chunks table, to be processed during
     * the endplayprint command.
     * Check for sufficient storage first.
     */

    state->m_chunks->addEntry(CHUNK_GROUP, chunkfile, (char *)argv[2]);

    return TCL_OK;
}

/*
 * TCL command "addmedia filename name"
 */

int addMedia(ClientData cd, Tcl_Interp *interp, int argc, CONST84 char *argv[])
{
    LayoutState *state = (LayoutState *)cd;

    if ( argc != 3 )
	{
		//interp->result = "wrong # args";
		Tcl_SetResult(interp, "wrong # args", TCL_STATIC);
		return TCL_ERROR;
    }

	char *chunkfile;
	if (state->m_inputDir != NULL)
	{
#if WIN32
		char *tmpFile = (char *)mlMalloc(strlen(state->m_inputDir) + strlen(argv[1]) + 2);
		strcpy(tmpFile,state->m_inputDir);
		strcat(tmpFile,"\\");
		strcat(tmpFile,argv[1]);

		MlePath *chunkpath;
		chunkpath = new MleWin32Path((MlChar *)tmpFile,true);
		chunkfile = (char *)chunkpath->getPlatformPath();

		mlFree(tmpFile);
#else /* ! WIN32 */
		char *tmpFile = (char *)mlMalloc(strlen(state->m_inputDir) + strlen(argv[1]) + 2);
		strcpy(tmpFile,state->m_inputDir);
		strcat(tmpFile,"/");
		strcat(tmpFile,argv[1]);

		MlePath *chunkpath;
		chunkpath = new MleLinuxPath((MlChar *)tmpFile,true);
		chunkfile = (char *)chunkpath->getPlatformPath();

		mlFree(tmpFile);
#endif
	} else
	{
		chunkfile = (char *)argv[1];
	}

    if ( access(chunkfile, R_OK) < 0 )
	{
		char errmsg[BUFSIZ];
		sprintf(errmsg, "%s: %s", argv[1], strerror(errno));
		//interp->result = errmsg;
		Tcl_SetResult(interp, errmsg, TCL_STATIC);
		return TCL_ERROR;
    }

    /*
     * Add this entry to the chunks table, to be processed during
     * the endplayprint command.
     */

    state->m_chunks->addEntry(CHUNK_MEDIA, chunkfile, (char *)argv[2]);

    return TCL_OK;
}

/*
 * TCL command "addset filename name"
 */

int addSet(ClientData cd, Tcl_Interp *interp, int argc, CONST84 char *argv[])
{
    LayoutState *state = (LayoutState *)cd;

    if ( argc != 3 )
	{
		//interp->result = "wrong # args";
		Tcl_SetResult(interp, "wrong # args", TCL_STATIC);
		return TCL_ERROR;
    }

	char *chunkfile;
	if (state->m_inputDir != NULL)
	{
#if WIN32
		char *tmpFile = (char *)mlMalloc(strlen(state->m_inputDir) + strlen(argv[1]) + 2);
		strcpy(tmpFile,state->m_inputDir);
		strcat(tmpFile,"\\");
		strcat(tmpFile,argv[1]);

		MlePath *chunkpath;
		chunkpath = new MleWin32Path((MlChar *)tmpFile,true);
		chunkfile = (char *)chunkpath->getPlatformPath();

		mlFree(tmpFile);
#else /* ! WIN32 */
		char *tmpFile = (char *)mlMalloc(strlen(state->m_inputDir) + strlen(argv[1]) + 2);
		strcpy(tmpFile,state->m_inputDir);
		strcat(tmpFile,"/");
		strcat(tmpFile,argv[1]);

		MlePath *chunkpath;
		chunkpath = new MleLinuxPath((MlChar *)tmpFile,true);
		chunkfile = (char *)chunkpath->getPlatformPath();

		mlFree(tmpFile);
#endif
	} else
	{
		chunkfile = (char *)argv[1];
	}

    if ( access(chunkfile, R_OK) < 0 )
	{
		char errmsg[BUFSIZ];
		sprintf(errmsg, "%s: %s", argv[1], strerror(errno));
		//interp->result = errmsg;
		Tcl_SetResult(interp, errmsg, TCL_STATIC);
		return TCL_ERROR;
    }

    /*
     * Add this entry to the chunks table, to be processed during
     * the endplayprint command.
     * Check for sufficient storage first.
     */

    state->m_chunks->addEntry(CHUNK_SET, chunkfile, (char *)argv[2]);
    return TCL_OK;
}

/*
 * TCL command "addscene filename name"
 */

int addScene(ClientData cd, Tcl_Interp *interp, int argc, CONST84 char *argv[])
{
    LayoutState *state = (LayoutState *)cd;

    if ( argc != 3 )
	{
		//interp->result = "wrong # args";
		Tcl_SetResult(interp, "wrong # args", TCL_STATIC);
		return TCL_ERROR;
    }

	char *chunkfile;
	if (state->m_inputDir != NULL)
	{
#if WIN32
		char *tmpFile = (char *)mlMalloc(strlen(state->m_inputDir) + strlen(argv[1]) + 2);
		strcpy(tmpFile,state->m_inputDir);
		strcat(tmpFile,"\\");
		strcat(tmpFile,argv[1]);

		MlePath *chunkpath;
		chunkpath = new MleWin32Path((MlChar *)tmpFile,true);
		chunkfile = (char *)chunkpath->getPlatformPath();

		mlFree(tmpFile);
#else /* ! WIN32 */
		char *tmpFile = (char *)mlMalloc(strlen(state->m_inputDir) + strlen(argv[1]) + 2);
		strcpy(tmpFile,state->m_inputDir);
		strcat(tmpFile,"/");
		strcat(tmpFile,argv[1]);

		MlePath *chunkpath;
		chunkpath = new MleLinuxPath((MlChar *)tmpFile,true);
		chunkfile = (char *)chunkpath->getPlatformPath();

		mlFree(tmpFile);
#endif
	} else
	{
		chunkfile = (char *)argv[1];
	}

    if ( access(chunkfile, R_OK) < 0 )
	{
		char errmsg[BUFSIZ];
		sprintf(errmsg, "%s: %s", argv[1], strerror(errno));
		//interp->result = errmsg;
		Tcl_SetResult(interp, errmsg, TCL_STATIC);
		return TCL_ERROR;
    }

    /*
     * Add this entry to the chunks table, to be processed during
     * the endplayprint command.
     * Check for sufficient storage first.
     */

    state->m_chunks->addEntry(CHUNK_SCENE, chunkfile, (char *)argv[2]);

    return TCL_OK;
}

/*
 * TCL command "beginlist"
 */

int beginList(ClientData /*cd*/, Tcl_Interp *interp, int argc, CONST84 char *argv[])
{
//    LayoutState *state = (LayoutState *)cd;
    char *listName;

    if ( argc != 2 )
	{
		//interp->result = "wrong # args";
		Tcl_SetResult(interp, "wrong # args", TCL_STATIC);
		return TCL_ERROR;
    }
    listName = (char *)argv[1];

    //interp->result = "Command not implemented yet";
	Tcl_SetResult(interp,"Command not implemented yet", TCL_STATIC);
    return TCL_ERROR;
}

/*
 * TCL command "endlist"
 */

int endList(ClientData cd, Tcl_Interp *interp, int argc, CONST84 char *argv[])
{
    LayoutState *state = (LayoutState *)cd;
    char *listName;

    if ( argc != 2 )
	{
		//interp->result = "wrong # args";
		Tcl_SetResult(interp, "wrong # args", TCL_STATIC);
		return TCL_ERROR;
    }
    listName = (char *)argv[1];

    state->m_playprint = NULL;
    //interp->result = "Command not implemented yet";
	Tcl_SetResult(interp, "Command not implemented yet", TCL_STATIC);
    return TCL_ERROR;
}

/*
 * TCL command "beginplayprint" - start a new playprint file.
 */

int beginPlayPrint(ClientData cd, Tcl_Interp *interp, int argc, CONST84 char *argv[])
{
    LayoutState *state = (LayoutState *)cd;

    if ( argc != 2 )
	{
		//interp->result = "wrong # args";
		Tcl_SetResult(interp, "wrong # args", TCL_STATIC);
		return TCL_ERROR;
    }

    /*
     * The "playprint" state variable is kept NULL when there is no
     * active playprint; can't have more than one active at a time.
     */

    if ( state->m_playprint != NULL )
	{
		//interp->result = "playprint file has already been specified.";
		Tcl_SetResult(interp, "playprint file has already been specified.", TCL_STATIC);
		return TCL_ERROR;
    }
#if WIN32
	char *tmpFile = (char *)mlMalloc(strlen(state->m_outputDir) + strlen(argv[1]) + 2);
	strcpy(tmpFile,state->m_outputDir);
	strcat(tmpFile,"\\");
	strcat(tmpFile,argv[1]);

	MlePath *dpppath;
	dpppath = new MleWin32Path((MlChar *)tmpFile,true);
	state->m_playprint = (char *)dpppath->getPlatformPath();

	mlFree(tmpFile);
#else /* ! WIN32 */
	//state->m_playprint = strdup(argv[1]);
	char *tmpFile = (char *)mlMalloc(strlen(state->m_outputDir) + strlen(argv[1]) + 2);
	strcpy(tmpFile,state->m_outputDir);
	strcat(tmpFile,"/");
	strcat(tmpFile,argv[1]);

	MlePath *dpppath;
	dpppath = new MleLinuxPath((MlChar *)tmpFile,true);
	state->m_playprint = (char *)dpppath->getPlatformPath();

	mlFree(tmpFile);
#endif

    /*
     * Create the playprint file...
     */

    if (state->m_byteOrder)
	{
		// Little Endian format.
        state->m_dpp = new MleDppOutput(state->m_playprint);
#if BYTE_ORDER != LITTLE_ENDIAN
        state->m_dpp->setSwap(TRUE);
#endif
    } else
	{
		// Big Endian format.
        state->m_dpp = new MleDppOutput(state->m_playprint);
#if BYTE_ORDER != BIG_ENDIAN
        state->m_dpp->setSwap(TRUE);
#endif

    }
    MLE_ASSERT(state->m_dpp);

    state->m_dpp->begin(state->m_byteOrder? MLE_DPP_CREATEPPLE: MLE_DPP_CREATEPPBE);
    state->m_dpp->writeHeader();

    return TCL_OK;
}

/*
 * TCL command "endplayprint" - this is where the chunks actually get written.
 *   generate the table of contents.
 *   write out the TOC.
 *   write out the chunks.
 *   wrap up the playprint.
 */
int endPlayPrint(ClientData cd, Tcl_Interp *interp, int argc, CONST84 char *argv[])
{
    LayoutState *state = (LayoutState *)cd;
    //char *contents;
    //int i, size;
    int i;

    if ( argc != 1 )
	{
		//interp->result = "wrong # args";
		Tcl_SetResult(interp, "wrong # args", TCL_STATIC);
		return TCL_ERROR;
    }

    /*
     * Generate the TOC...
     */

    state->m_dpp->beginTOC(state->m_chunks->getUsed());

    /*
     * Write out the chunks...
     */

    for ( i=0 ; i<state->m_chunks->getUsed() ; i++ )
	{
		switch ( state->m_chunks->getType(i) )
		{
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

    /*
     * Seal up the playprint file...
     */

    state->m_dpp->endTOC();
    state->m_dpp->end();
    delete state->m_dpp;

    /*
     * Generate the table of global symbols for this set...
     */

    gencodeTables(state);

    //delete state->m_chunks;
    //state->m_chunks = new MleDppChunkTable();
    //state->m_playprint = NULL;

	/*
	 * Close the generated files.
	 */
	mlFClose(state->m_codefd);
	if (state->m_language != TRUE)
	    mlFClose(state->m_headerfd);

    return TCL_OK;
}

/*
 * TCL command "codefile"
 */

int codeFile(ClientData cd, Tcl_Interp *interp, int argc, CONST84 char *argv[])
{
    LayoutState *state = (LayoutState *)cd;

    if ( argc != 2 )
	{
		//interp->result = "wrong # args";
		Tcl_SetResult(interp, "wrong # args", TCL_STATIC);
		return TCL_ERROR;
    }

    if (state->m_codefile != NULL)
	{
		if (state->m_language != TRUE)
		{
			// Closing C/C++ files.
			mlFClose(state->m_codefd);
			mlFClose(state->m_headerfd);
		} else
		{
			// Closing Java file.
			mlFClose(state->m_codefd);
		}
	}

    char *codefilename, *headerfilename;
	if (state->m_language)
	{
		// Java code generation.
		state->m_codefile = strdup(argv[1]);
		codefilename = new char[strlen(state->m_codefile) + strlen(JAVA_CODE_FILENAME_EXT) + 1];
		strcpy(codefilename, state->m_codefile); strcat(codefilename, JAVA_CODE_FILENAME_EXT);
	} else
	{
		// C/C++ code generation.
		state->m_codefile = strdup(argv[1]);
		codefilename = new char[strlen(state->m_codefile) + strlen(CPP_CODE_FILENAME_EXT) + 1];
		strcpy(codefilename, state->m_codefile); strcat(codefilename, CPP_CODE_FILENAME_EXT);
		headerfilename = new char[strlen(state->m_codefile) + strlen(CPP_HEADER_FILENAME_EXT) + 1];
		strcpy(headerfilename, state->m_codefile); strcat(headerfilename, CPP_HEADER_FILENAME_EXT);
	}

	if (state->m_outputDir != NULL)
	{
#ifdef WIN32
		char *tmpFilename = new char[strlen(state->m_outputDir) + strlen(codefilename) + 2];
		strcpy(tmpFilename,state->m_outputDir);
		strcat(tmpFilename,"\\");
		strcat(tmpFilename,codefilename);

		MlePath *tmpSourcePath;
		tmpSourcePath = new MleWin32Path((MlChar *)tmpFilename,true);
		delete [] codefilename;
		codefilename = (char *)tmpSourcePath->getPlatformPath();

		delete [] tmpFilename;

		if (state->m_language == FALSE)
		{
			// Process header file name for C/C++ generation.
			tmpFilename = new char[strlen(state->m_outputDir) + strlen(headerfilename) + 2];
			strcpy(tmpFilename,state->m_outputDir);
			strcat(tmpFilename,"\\");
			strcat(tmpFilename,headerfilename);

			MlePath *tmpHeaderPath;
			tmpHeaderPath = new MleWin32Path((MlChar *)tmpFilename,true);
			delete [] headerfilename;
			headerfilename = (char *)tmpHeaderPath->getPlatformPath();
		} else
		{
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

		if (state->m_language == FALSE)
		{
			// Process header file name for C/C++ generation.
			tmpFilename = new char[strlen(state->m_outputDir) + strlen(headerfilename) + 2];
			strcpy(tmpFilename,state->m_outputDir);
			strcat(tmpFilename,"/");
			strcat(tmpFilename,headerfilename);

			MlePath *tmpHeaderPath;
			tmpHeaderPath = new MleLinuxPath((MlChar *)tmpFilename,true);
			delete [] headerfilename;
			headerfilename = (char *)tmpHeaderPath->getPlatformPath();
		} else
		{
			headerfilename = NULL;
		}
#endif
	}

	// Open the file(s) for generation.
	if (state->m_language)
	{
		// Generating code for Java.
		if ((state->m_codefd = mlFOpen(codefilename, "w")) == NULL)
		{
			char errmsg[BUFSIZ];
			sprintf(errmsg, "%s: %s", argv[1], strerror(errno));
			//interp->result = errmsg;
			Tcl_SetResult(interp, errmsg, TCL_STATIC);
			return TCL_ERROR;
		}
		state->m_headerfd = NULL;
	} else
	{
		// Generating code for C/C++.
		if ( (state->m_codefd = mlFOpen(codefilename, "w")) == NULL ||
			 (state->m_headerfd = mlFOpen(headerfilename, "w")) == NULL )
		{
			char errmsg[BUFSIZ];
			sprintf(errmsg, "%s: %s", argv[1], strerror(errno));
			//interp->result = errmsg;
			Tcl_SetResult(interp, errmsg, TCL_STATIC);
			return TCL_ERROR;
		}
	}

	// Start the code generation.
    if ( gencodeStart(state, headerfilename, codefilename) )
	{
		return TCL_ERROR;
    }

	// Return success.
    return TCL_OK;
}
