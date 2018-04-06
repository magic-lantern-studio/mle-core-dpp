/** @defgroup MleDPPMaster Magic Lantern Digital Playprint Library API - Master */

/**
 * @file gencode.cpp
 * @ingroup MleDPPMaster
 *
 * Code generation phase of Digital Playprint construction.
 *
 * This module generates the statically initialized global title symbols
 * and map chunk names into TOC indices.
 *
 * @author Mark S. Millard
 * @date September 15, 2004
 */


// COPYRIGHT_BEGIN
//
// Copyright (c) 2015-2018 Wizzer Works
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
#include <stdlib.h>
#include <time.h>
#include <string.h>

// Include Magic Lantern header files.
#include <mle/mlExpandFilename.h>
#include <mle/MleTemplate.h>
#include <mle/mlConfig.h>
#include <mle/mlMalloc.h>
#include <mle/mlAssert.h>
#include <mle/MlePath.h>
#ifdef WIN32
#include <mle/MleWin32Path.h>
#else
#include <mle/MleLinuxPath.h>
#endif

// Include Digital Workprint header files.
#include "mle/Dwp.h"
#include "mle/DwpInput.h"
#include "mle/DwpItem.h"

// Include Digital Playprint header files.
#include "mle/DppTblMgr.h"
#include "pplayout.h"


int gencodeStart(LayoutState *state, char *headerfilename, char *codefilename)
{
    char *filename;
    MleTemplateProcess *process;

    // Do the work necessary to create an in-memory workprint we can
    // pass to the subsidiary routines.
    MLE_ASSERT( NULL == state->m_root );

	char *wpFile;
	if (state->m_workprint == NULL)
	{
		/*
		 * Determine the workprint file name based on the playprint
		 * script file name.
		 *
		 * This will not work if workprint and playprint do not
		 * have the same name.
		 */
		char *sourceName = state->m_scriptfile;
		char *lastDot = strrchr(sourceName, '.');
		if (lastDot)
			*lastDot = '\0';
		wpFile = new char[strlen(sourceName) + strlen(".dwp") + 1];
		sprintf(wpFile, "%s.dwp", sourceName);
		if (lastDot)
		 	*lastDot = '.';
	} else
	{
//		MlePath *wpPath;
//#if WIN32
//		wpPath = new MleWin32Path((MlChar *)state->m_workprint,true);
//		wpFile = (char *)wpPath->getPlatformPath();
//#else /* ! WIN32 */
		wpFile = state->m_workprint;
//#endif
	}

    /*
     * Initialize the workprint toolkit.
     */
    mleDwpInit();

    MleDwpInput *wp = new MleDwpInput;
    MLE_ASSERT(wp);

	// Open the workprint.
    int status = wp->openFile(wpFile);
    MLE_ASSERT(status == 0);

    /*
     * Read in the workprint.
     */
    state->m_root = MleDwpItem::readAll(wp);
    MLE_ASSERT(state->m_root);

	if (state->m_workprint == NULL)
		delete [] wpFile;

    wp->closeFile();
    delete wp;

    // Left with a new state->m_root here.

    // Build the file name path for the code generation template.
	if (state->m_language)
	{
		// Use Java template.
		filename = mlFilenameExpand(MLE_TOC_JAVA_GLOBALS_TEMPLATE);
	} else
	{
		// Use C/C++ template.
		filename = mlFilenameExpand(MLE_TOC_CPP_GLOBALS_TEMPLATE);
	}

	// Read in the template.
    state->m_tmplate = new MleTemplate;
    if (state->m_tmplate->read(filename) < 0)
	{
        perror(filename);
		mlFree(filename);
		return 1;
    }

    /*
     * Create the global bindings for this template...
     */

    mlFree(filename);
    MleTemplateBindings *gbindings = new MleTemplateBindings();

    time_t clock = time(0);
    char *ptime, *pnl;
    ptime = ctime(&clock);
    if ((pnl=strrchr(ptime, '\n')) != NULL )
	{
        *pnl = 0;
    }
    gbindings->defineConstant("DATE", ptime);
    //gbindings->defineConstant("HEADER_FILENAME", headerfilename );
    gbindings->defineConstant("CODE_FILENAME", codefilename );
    MleTemplateBindingCallback doTOCSymbols, doTab;
    MleTemplateBindingCallback doSetToChunk;
    gbindings->defineCallback("DO_TOC_SYMBOLS",
			      doTOCSymbols,
			      (void *)state);
    gbindings->defineCallback("TAB",
			      doTab,
			      (void *)state);
    gbindings->defineCallback("DO_SET_TO_CHUNK",
			      doSetToChunk,
			      (void *)state);
	if (state->m_language)
		gbindings->defineConstant("PACKAGE", state->m_package);
	else
		gbindings->defineConstant("HEADER_FILENAME", headerfilename );

    state->m_tmplate->setGlobalBindings(gbindings);

    /*
     * Put out the top of the header file and the source file.
     */

	if (state->m_language != TRUE)
	{
		// Only necessary for C/C++ code generation.
		process = new MleTemplateProcess("HEADER_TOP",
					   state->m_tmplate,
					   NULL,
					   state->m_headerfd);
		process->go();
		delete process;
	}

    process = new MleTemplateProcess("CODE_TOP",
				   state->m_tmplate,
				   NULL,
				   state->m_codefd);
    process->go();
    delete process;

    return 0;
}

void gencodeTables(LayoutState *state)
{
    MleTemplateProcess *process;
    MleTemplateBindings lbindings;
    
    lbindings.defineConstant("PLAYPRINT_FILENAME", state->m_playprint);

	if (state->m_language != TRUE)
	{
		// Only necessary for C/C++ code generation.
		process = new MleTemplateProcess("HEADER_BODY",
					   state->m_tmplate,
					   &lbindings,
					   state->m_headerfd);
		process->go();
		delete process;
	}

    process = new MleTemplateProcess("CODE_BODY",
				   state->m_tmplate,
				   &lbindings,
				   state->m_codefd);
    process->go();

    delete process;
}

void doTOCSymbols(MleTemplateProcess *ptp, char *section, void *data)
{
    MleTemplateBindings lbindings;
    char groupSection[100], mediaSection[100], setSection[100], sceneSection[100], bootSceneSection[100];
    LayoutState *state = (LayoutState *)data;

    sprintf(groupSection, section, "GROUP");
    sprintf(mediaSection, section, "MEDIA");
    sprintf(setSection, section, "SET");
    sprintf(sceneSection, section, "SCENE");
    sprintf(bootSceneSection, section, "BOOT_SCENE");
    MleTemplateProcess processgroup(groupSection,
				 ptp->getTemplate(),
				 &lbindings,
				 ptp->getFileDescriptor());
    MleTemplateProcess processmedia(mediaSection,
				 ptp->getTemplate(),
				 &lbindings,
				 ptp->getFileDescriptor());
    MleTemplateProcess processset(setSection,
				 ptp->getTemplate(),
				 &lbindings,
				 ptp->getFileDescriptor());
    MleTemplateProcess processscene(sceneSection,
				 ptp->getTemplate(),
				 &lbindings,
				 ptp->getFileDescriptor());
    MleTemplateProcess processbootscene(bootSceneSection,
				 ptp->getTemplate(),
				 &lbindings,
				 ptp->getFileDescriptor());

    /*
     * Only write the distinguished boot scene symbol once,
     * at the first boot scene we find.
     */
    int bootSceneProcessed = 0;

    for ( int i=0 ; i<state->m_chunks->getUsed() ; i++ )
	{
		lbindings.defineConstant("NAME", state->m_chunks->getName(i));
		lbindings.defineConstant("VALUE", i);
		switch ( state->m_chunks->getType(i) )
		{
		  case CHUNK_GROUP:
			processgroup.go();
			break;
		  case CHUNK_MEDIA:
			processmedia.go();
			break;
		  case CHUNK_SET:
			processset.go();
			break;
		  case CHUNK_SCENE:
			if (0 == bootSceneProcessed++)
			{
				processbootscene.go();
			}		    
			processscene.go();
			break;
		}
    }

	// Add a default boot scene for Java if missing in Digital Workprint.
	if (state->m_language)
	{
		// Processing Java template.
		if (bootSceneProcessed == 0)
		{
			lbindings.defineConstant("VALUE", -1);
			processbootscene.go();
		}
	}
}

void doTab(MleTemplateProcess *ptp, char *section, void *)
{
    int col;

    col = atoi(section);
    ptp->tabTo(col);
}

void
doSetToChunk(MleTemplateProcess *ptp, char *section, void *data)
{
    char *setName;
    char setChunkBeginSection[100];
    char setChunkEntrySection[100], setChunkEndSection[100];
    int i, j, numChunk, numSetChunk;
    LayoutState *state = (LayoutState *) data;
    MleDppChunkTable *chunks = state->m_chunks;
    MleTemplateBindings localBinding;
    MleDppTblMgr *tblMgr;
    TblMgrItems *setTable;

    /*
     * Prepare for code generation.
     */
    sprintf(setChunkBeginSection, section, "BEGIN");
    sprintf(setChunkEntrySection, section, "ENTRY");
    sprintf(setChunkEndSection, section, "END");
    MleTemplateProcess processFCBegin(setChunkBeginSection, ptp->getTemplate(),
				   &localBinding, ptp->getFileDescriptor());
    MleTemplateProcess processFCEntry(setChunkEntrySection, ptp->getTemplate(),
				   &localBinding, ptp->getFileDescriptor());
    MleTemplateProcess processFCEnd(setChunkEndSection, ptp->getTemplate(),
				 &localBinding, ptp->getFileDescriptor());

    /*
     * Find the total number of set chunks.
     */
    for (i = 0, numSetChunk = 0, numChunk = chunks->getUsed();
	     i < numChunk;
	     i++)
	{
		if (chunks->getType(i) == CHUNK_SET)
			numSetChunk++;
    }

    /*
     * Fill in the table heading.
     */
    if (numSetChunk > 0)
		localBinding.defineConstant("VALUE", "[] = {");
    else
		localBinding.defineConstant("VALUE", "[] = {0};");
    processFCBegin.go();

    /*
     * Fill in the entries.
     */
    if (numSetChunk > 0)
	{
    	int status;

		/*
		 * Build up the runtime table.
		 */
		tblMgr = new MleDppTblMgr();
		MLE_ASSERT(tblMgr);
		status = tblMgr->buildIndexTables(state->m_root);
		MLE_ASSERT(status == TRUE);
		status = tblMgr->setDiscriminators(state->m_root, state->m_tags);
		MLE_ASSERT(status == TRUE);

		/*
		 * Find the set chunk for each member of the set runtime table,
		 * and write out the entries.
		 */
		setTable = tblMgr->getSetTable();
		numChunk = chunks->getUsed();
		for(i = 0; i < setTable->used; i++)
		{
			setName = (((SetTableItem **) (setTable->items))[i])->name;

			for(j = 0; j < numChunk; j++)
			{
				if (chunks->getType(j) == CHUNK_SET &&
					strcmp(setName, chunks->getName(j)) == 0)
				{
					localBinding.defineConstant("NAME", chunks->getName(j));
					processFCEntry.go();
					break;
				}
			}
		}

		/*
		 * Fill in the table ending.
		 */
		processFCEnd.go();

		delete tblMgr;
    }
}
