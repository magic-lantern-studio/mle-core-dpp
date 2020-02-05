/** @defgroup MleDPPMaster Magic Lantern Digital Playprint Library API - Master */

/**
 * @file DppGenMakefile.cpp
 * @ingroup MleDPPMaster
 *
 * Magic Lantern Digital Playprint Library API.
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
#include <sys/stat.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

// Include Magic Lantern header files.
#include <mle/mlConfig.h>
#include <mle/mlExpandFilename.h>
#include <mle/MleTemplate.h>
#include <mle/mlMalloc.h>
#include <mle/mlAssert.h>

// Include Magic Lantern Digital Workprint header files.
#include "mle/DwpInput.h"
#include "mle/DwpItem.h"
#include "mle/DwpActorDef.h"
#include "mle/DwpDelegateBinding.h"
#include "mle/DwpForumDef.h"
#include "mle/DwpFinder.h"

// Include Magic Lantern Digital Playprint header files.
#include "mle/Dpp.h"

// Include Magic Lantern Mastering Library header files.
#include "mle/DppGenMakefile.h"


MleDppGenMakefile::MleDppGenMakefile(char *filename)
{
    // Initialize Digital Eorkprint Playprint toolkit.
    mleDppInit();

    // Create a new DWP input object.
    m_wp = new MleDwpInput;
    MLE_ASSERT(m_wp);

    // Open the DWP
    if ( m_wp->openFile(filename) )
	{
		m_root = NULL;
		return;
    }

    // Read in the entire DWP.
    m_root = MleDwpItem::readAll(m_wp);
    MLE_ASSERT(m_root);
}

MleDppGenMakefile::~MleDppGenMakefile()
{
    // XXX -- should close the DWP here; however, there is no API for this.
}

/*
 * readTitleTemplate
 *
 * This code reads in the mastering Makefile template, wherever it might be.
 */

void MleDppGenMakefile::readMakefileTemplate(MleTemplate &pTemplate)
{
    char *filename;

    filename = mlFilenameExpand(MLE_TITLE_MAKEFILE_TEMPLATE);

    if (pTemplate.read(filename) < 0)
	{
        perror(filename);
    }

    mlFree(filename);

    return;
}


/*
 * generateMakefile
 */
MlBoolean MleDppGenMakefile::generateMakefile()
{
    // Read code generation template.
    MleTemplate pTemplate;
    readMakefileTemplate(pTemplate);

    MleTemplateBindings *pGBindings = new MleTemplateBindings();
    time_t clock = time(0);
    char *ptime, *pnl;
    ptime = ctime(&clock);
    if ((pnl=strrchr(ptime, '\n')) != NULL ) {
        *pnl = 0;
    }
    pGBindings->defineConstant("DATE", ptime);
    pTemplate.setGlobalBindings(pGBindings);

    MleTemplateBindingCallback doGenMakefileActors;
    MleTemplateBindingCallback doGenMakefileDelegates;
    MleTemplateBindingCallback doGenMakefileForums;
    pGBindings->defineCallback("DO_ACTORS", doGenMakefileActors, (void *)m_root);
    pGBindings->defineCallback("DO_DELEGATES", doGenMakefileDelegates, (void *)m_root);
    pGBindings->defineCallback("DO_FORUMS", doGenMakefileForums, (void *)m_root);

    // Generate quark to string mapping utilities.
    MleTemplateProcess *pProcess;
    char *pfilename = "Makefile.target";
    FILE *fd;
    if ( (fd = fopen(pfilename, "w")) == NULL )
	{
        perror(pfilename);
        return FALSE;
    }
    pGBindings->defineConstant("FILENAME", pfilename);
    pProcess = new MleTemplateProcess("MASTERING_MAKEFILE", &pTemplate, NULL, fd);

	// Process the template.
    pProcess->go();

	// Clean up.
    fclose(fd);
    delete pProcess;

    return TRUE;
}


void *
MleDppGenMakefile::operator new(size_t tSize)
{
	void *p = mlMalloc(tSize);
	return p;
}


void
MleDppGenMakefile::operator delete(void *p)
{
	mlFree(p);
}


void doGenMakefileActors(MleTemplateProcess *ptp, char *section, void *data)
{
    MleDwpItem *root = (MleDwpItem *)data;
    MleDwpFinder finder;
    MleTemplateBindings pLBindings;

    finder.setType(MleDwpActorDef::typeId);
    finder.setFindAll(1);
    finder.find(root);
    int numItems = finder.getNumItems();
    MleDwpActorDef **items = (MleDwpActorDef **)finder.getItems();

    MleTemplateProcess pProcess(
		section, ptp->getTemplate(), &pLBindings, ptp->getFileDescriptor());
    for ( int i=0 ; i<numItems ; i++ )
	{
		MleDwpActorDef *actorDef = items[i];
        pLBindings.defineConstant("NAME_STRING", (char *)(actorDef->getName()));
        pProcess.go();
    }
}


void doGenMakefileDelegates(MleTemplateProcess *ptp, char *section, void *data)
{
    MleDwpItem *root = (MleDwpItem *)data;
    MleDwpFinder finder;
    MleTemplateBindings pLBindings;
    MleTemplateProcess pProcess(
	    section, ptp->getTemplate(), &pLBindings, ptp->getFileDescriptor());
}



void doGenMakefileForums(MleTemplateProcess *ptp, char *section, void *data)
{
    MleDwpItem *root = (MleDwpItem *)data;
    MleTemplateBindings pLBindings;
    MleTemplateProcess pProcess(
		section, ptp->getTemplate(), &pLBindings, ptp->getFileDescriptor());
}
