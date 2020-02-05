/** @defgroup MleDPPAccess Magic Lantern Digital Playprint Library API */

/**
 * @file DppGenCode.cpp
 * @ingroup MleDPPAccess
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
#include <mle/mlTypes.h>
#include <mle/mlAssert.h>
#include <mle/mlErrno.h>

#include <mle/mlExpandFilename.h>
#include <mle/mlConfig.h>
#include <mle/MleTemplate.h>
//#include <mle/expandfn.h>

// Include Magic Lantern Digital Workprint header files.
#include "mle/DwpInput.h"
#include "mle/DwpItem.h"
#include "mle/DwpChecker.h"

// Include Magic Lantern Digital Playprint header files.
#include "mle/Dpp.h"
#include "mle/DppTblMgr.h"
#include "mle/DppGenCode.h"


MleDppGenCode::MleDppGenCode(char *filename)
{
    // Initialize Digital Workprint/Playprint toolkit.
    mleDppInit();

    // Create a new DWP input object.
    m_wp = new MleDwpInput;
    MLE_ASSERT(m_wp);

    // Open the DWP.
    if ( m_wp->openFile(filename) )
	{
		m_root = NULL;
		return;
    }

    // Read in the entire DWP.
    m_root = MleDwpItem::readAll(m_wp);
    MLE_ASSERT(m_root);
}


MleDppGenCode::~MleDppGenCode()
{
    // XXX -- should close the DWP here; however, there is no API for this.
}


//
// readTitleTemplate
//
// This code reads in the mastering code template, wherever it might be.
//

void MleDppGenCode::readTitleTemplate(MleTemplate &pTemplate)
{
    char *filename;

    filename = mleFilenameExpand(MLE_TITLE_CODE_TEMPLATE_PATH);

    if (pTemplate.read(filename) < 0)
	{
        perror(filename);
    }

    free(filename);

    return;
}


#if 0
MlBoolean MleDppGenCode::verifyWorkprint(char *tags)
{
    // Declare local variables.
    MleDwpChecker wpChecker;
    MleErr status;
    char *processTags;
    char *tag,*nextTag;
    MleDwpTagAllDiscriminator *discriminator = new MleDwpTagAllDiscriminator;

    // Initialize DWP checker.
    if (m_root == NULL)
	{
        return FALSE;
    } else {
        wpChecker.setWorkprint(m_root);
        wpChecker.setFlags(MLE_WPCHK_TARGET_SGI,MLE_WPCHK_TARGETMASK);
    }

    // Set discriminators.
    processTags = strdup(tags);
    tag = processTags;
    while (tag)
	{
        nextTag = strchr(tag,':');
        if (nextTag) *nextTag++ = '\0';
        discriminator->addTag(tag);
        tag = nextTag;
    }

    m_root->setDiscriminator(discriminator);

    // Verify DWP.
    status = wpChecker.check();

    // Clear discriminators and cleanup.
    m_root->setDiscriminator(NULL);
    delete discriminator;
    free(processTags);

    if (status == mlNoErr)
        return TRUE;
    else
        return FALSE;
}
#endif /* 0 */


//
// generateTitle
//
//  This produces the static quarks for actors, properties, methods,
//  and messages. It also will create a main() entry-point for the title.
//

MlBoolean MleDppGenCode::generateTitle(char *tags)
{
    // Build tables for table manager.
    MleDppTblMgr tblMgr;
    tblMgr.setDiscriminators(m_root, tags);
    if (! tblMgr.buildIndexTables(m_root))
	{
        // XXX - should probably print mlErrno message here.
        return(FALSE);
    }

    // Read code generation template.
    MleTemplate pTemplate;
    readTitleTemplate(pTemplate);

    MleTemplateBindings *pGBindings = new MleTemplateBindings();
    pGBindings->defineConstant("ELEMENTS",
        tblMgr.getActorClassTableSize() +
        tblMgr.getDelegateClassTableSize() +
        tblMgr.getForumClassTableSize() +
        tblMgr.getActorPropTableSize() +
        tblMgr.getForumPropTableSize() +
        tblMgr.getForumTableSize() +
        tblMgr.getGroupClassTableSize() +
        tblMgr.getSceneClassTableSize() +
        tblMgr.getMediaRefTableSize());
    time_t clock = time(0);
    char *ptime, *pnl;
    ptime = ctime(&clock);
    if ((pnl=strrchr(ptime, '\n')) != NULL )
	{
        *pnl = 0;
    }
    pGBindings->defineConstant("DATE", ptime);
    pTemplate.setGlobalBindings(pGBindings);

    MleTemplateBindingCallback doActorClass;
    MleTemplateBindingCallback doProperties;
    MleTemplateBindingCallback doDelegateClass;
    MleTemplateBindingCallback doForums;
    MleTemplateBindingCallback doForumClass;
    MleTemplateBindingCallback doGroupClass;
    MleTemplateBindingCallback doSceneClass;
    MleTemplateBindingCallback doMediaRefClass;
    pGBindings->defineCallback("DO_ACTORCLASS",doActorClass,
        (void *)tblMgr.getActorClassTable());
    pGBindings->defineCallback("DO_ACTORPROPERTIES",doProperties,
        (void *)tblMgr.getActorPropTable());
    pGBindings->defineCallback("DO_DELEGATECLASS",doDelegateClass,
        (void *)tblMgr.getDelegateClassTable());
    pGBindings->defineCallback("DO_FORUMS",doForums,
        (void *)tblMgr.getForumTable());
    pGBindings->defineCallback("DO_FORUMCLASS",doForumClass,
        (void *)tblMgr.getForumClassTable());
    pGBindings->defineCallback("DO_FORUMPROPERTIES",doProperties,
        (void *)tblMgr.getForumPropTable());
    pGBindings->defineCallback("DO_GROUPCLASS",doGroupClass,
        (void *)tblMgr.getGroupClassTable());
    pGBindings->defineCallback("DO_SCENECLASS",doSceneClass,
        (void *)tblMgr.getSceneClassTable());
    pGBindings->defineCallback("DO_MEDIAREFS",doMediaRefClass,
        (void *)tblMgr.getMediaRefTable());

    // Generate quark to string mapping utilities.
    MleTemplateProcess *pProcess;
    char *pfilename = "mltables.cxx";
    FILE *fd;
    if ( (fd = fopen(pfilename, "w")) == NULL )
	{
        perror(pfilename);
        return FALSE;
    }
    pGBindings->defineConstant("FILENAME", pfilename);
    pProcess = new MleTemplateProcess("RUNTIME_INDEX_TO_STRING", &pTemplate, NULL, fd);
    pProcess->go();
    fclose(fd);
    delete pProcess;

    return TRUE;
}


void doIndexStrings(MleTemplateProcess *ptp, char *section, void *data)
{
    TblMgrItems *strTable = (TblMgrItems *)data;
    MleTemplateBindings pLBindings;
    MleTemplateProcess pProcess(section,
		ptp->getTemplate(), &pLBindings, ptp->getFileDescriptor());

    for (int i = 0; i < strTable->used; i++)
	{
        pLBindings.defineConstant("INDEX_STRING", (char *)strTable->items[i]);
        pLBindings.defineConstant("INDEX_VALUE", i);
        pProcess.go();
    }
}


void doActorClass(MleTemplateProcess *ptp, char *section, void *data)
{
    // Decare local variables.
    TblMgrItems *actorTable = (TblMgrItems *)data;
    MleTemplateBindings pLBindings;
    MleTemplateProcess pProcess(section,
		ptp->getTemplate(), &pLBindings, ptp->getFileDescriptor());
    char *className,*headerFile,*lastDot;
    int offset = 0;
    MlBoolean isTypeInclude;

    if(strcmp(section, "TYPE_INCLUDES") == 0)
		isTypeInclude = TRUE;
    else
		isTypeInclude = FALSE;

    // Output Actor Class entries.
    for (int i=0; i<actorTable->used; i++)
	{
		if (isTypeInclude)
		{
			if (((ActorClassTableItem *)actorTable->items[i])->headerFile) {
				headerFile = ((ActorClassTableItem *)actorTable->items[i])->headerFile;
				lastDot = strrchr(headerFile, '.');
				if(lastDot)
					*lastDot = '\0';
				pLBindings.defineConstant("NAME_STRING",headerFile);
				if(lastDot)
					*lastDot = '.';
			}
			else {
				headerFile = ((ActorClassTableItem *)actorTable->items[i])->name;
				pLBindings.defineConstant("NAME_STRING",headerFile);
			}
		}
		else
		{
			className = ((ActorClassTableItem *)actorTable->items[i])->name;
			pLBindings.defineConstant("NAME_STRING",className);
		}

        pLBindings.defineConstant("PROPTBL_OFFSET",offset);
        pProcess.go();

        // Iterate over the Actor Class and increment offset
        // into Property table.
        for (int j=0;
             j<((ActorClassTableItem *)actorTable->items[i])->numProperties;
             j++)
		{
            offset++;
		}
    }
}


void doProperties(MleTemplateProcess *ptp, char *section, void *data)
{
    // Decare local variables.
    TblMgrItems *strTable = (TblMgrItems *)data;
    MleTemplateBindings pLBindings;
    MleTemplateProcess pProcess(section,
		ptp->getTemplate(), &pLBindings, ptp->getFileDescriptor());
    char *className,*propName,*underbar;

    // Iterate over the MleDwpActor items, extracting class name and properties.
    for (int i=0; i<strTable->used; i++)
	{
        className = strdup((char *)strTable->items[i]);
		underbar = strchr(className,'_');
        *underbar = '\0';
        pLBindings.defineConstant("NAME_STRING",className);

        underbar++;
        propName = strdup(underbar);
        pLBindings.defineConstant("PROPERTY_NAME",propName);

        pProcess.go();

        free(className);
        free(propName);
    }
}


void doDelegateClass(MleTemplateProcess *ptp, char *section, void *data)
{
    // Decare local variables.
    TblMgrItems *strTable = (TblMgrItems *)data;
    MleTemplateBindings pLBindings;
    MleTemplateProcess pProcess(section,
		ptp->getTemplate(), &pLBindings, ptp->getFileDescriptor());
    char *className;

    // Output Delegate Class entries.
    for (int i=0; i<strTable->used; i++)
	{
        className = (char *)strTable->items[i];
        pLBindings.defineConstant("NAME_STRING",className);
        pProcess.go();
    }
}



void doForumClass(MleTemplateProcess *ptp, char *section, void *data)
{
    // Decare local variables.
    TblMgrItems *forumTable = (TblMgrItems *)data;
    MleTemplateBindings pLBindings;
    MleTemplateProcess pProcess(section,
		ptp->getTemplate(), &pLBindings, ptp->getFileDescriptor());
    char *className,*headerFile,*lastDot;
    int offset = 0;
    MlBoolean isTypeInclude;

    if (strcmp(section, "TYPE_INCLUDES") == 0)
		isTypeInclude = TRUE;
    else
		isTypeInclude = FALSE;


    // Output Forum Class entries.
    for (int i=0; i<forumTable->used; i++)
	{
		if (isTypeInclude)
		{
			if (((ForumClassTableItem *)forumTable->items[i])->headerFile)
			{
				headerFile = ((ForumClassTableItem *)forumTable->items[i])->headerFile;
				lastDot = strrchr(headerFile, '.');
				if(lastDot)
					*lastDot = '\0';
				pLBindings.defineConstant("NAME_STRING",headerFile);
				if(lastDot)
					*lastDot = '.';
			}
			else
			{
				headerFile = ((ForumClassTableItem *)forumTable->items[i])->name;
				pLBindings.defineConstant("NAME_STRING",headerFile);
			}
		}
		else
		{
			className = ((ForumClassTableItem *)forumTable->items[i])->name;
			pLBindings.defineConstant("NAME_STRING",className);
		}

		pLBindings.defineConstant("PROPTBL_OFFSET",offset);
		pProcess.go();

		// Iterate over the Forum Class and increment offset
		// into Property table.
		for (int j = 0;
			 j<((ForumClassTableItem *)forumTable->items[i])->numProperties;
			 j++)
		{
				offset++;
		}
    }
}


void doForums(MleTemplateProcess *ptp, char *section, void *data)
{
    // Decare local variables.
    TblMgrItems *forumTable = (TblMgrItems *)data;
    MleTemplateBindings pLBindings;
    MleTemplateProcess pProcess(section,
		ptp->getTemplate(),  &pLBindings, ptp->getFileDescriptor());
    char *forumType;

    // Output Forum Type entries.
    for (int i=0; i<forumTable->used; i++)
	{
        forumType = ((ForumTableItem *)forumTable->items[i])->type;
        pLBindings.defineConstant("NAME_STRING",forumType);
        pProcess.go();
    }
}


void doGroupClass(MleTemplateProcess *ptp, char *section, void *data)
{
    // decare local variables.
    TblMgrItems *groupTable = (TblMgrItems *)data;
    MleTemplateBindings pLBindings;
    MleTemplateProcess pProcess(section,
		ptp->getTemplate(), &pLBindings, ptp->getFileDescriptor());
    char *className,*headerFile,*lastDot;
    MlBoolean isTypeInclude;

    if (strcmp(section, "TYPE_INCLUDES") == 0)
		isTypeInclude = TRUE;
    else
		isTypeInclude = FALSE;

    // output Group Class entries.
    for (int i=0; i<groupTable->used; i++)
	{
		if (isTypeInclude)
		{
			if (((GroupClassTableItem *)groupTable->items[i])->headerFile)
			{
				headerFile = ((GroupClassTableItem *)groupTable->items[i])->headerFile;
				lastDot = strrchr(headerFile, '.');
				if(lastDot)
					*lastDot = '\0';
				pLBindings.defineConstant("NAME_STRING",headerFile);
				if(lastDot)
					*lastDot = '.';
			}
			else
			{
				headerFile = ((GroupClassTableItem *)groupTable->items[i])->name;
				pLBindings.defineConstant("NAME_STRING",headerFile);
			}
		}
		else 
		{
			className = ((GroupClassTableItem *)groupTable->items[i])->name;
			pLBindings.defineConstant("NAME_STRING",className);
		}

        pProcess.go();

    }
}


void doSceneClass(MleTemplateProcess *ptp, char *section, void *data)
{
    // Decare local variables.
    TblMgrItems *sceneTable = (TblMgrItems *)data;
    MleTemplateBindings pLBindings;
    MleTemplateProcess pProcess(section,
		ptp->getTemplate(), &pLBindings, ptp->getFileDescriptor());
    char *className,*headerFile,*lastDot;
    MlBoolean isTypeInclude;

    if(strcmp(section, "TYPE_INCLUDES") == 0)
		isTypeInclude = TRUE;
    else
		isTypeInclude = FALSE;

    // Output Scene Class entries.
    for (int i=0; i<sceneTable->used; i++)
	{
		if(isTypeInclude)
		{
			if (((SceneClassTableItem *)sceneTable->items[i])->headerFile)
			{
				headerFile = ((SceneClassTableItem *)sceneTable->items[i])->headerFile;
				lastDot = strrchr(headerFile, '.');
				if(lastDot)
					*lastDot = '\0';
				pLBindings.defineConstant("NAME_STRING",headerFile);
				if(lastDot)
					*lastDot = '.';
			}
			else
			{
				headerFile = ((SceneClassTableItem *)sceneTable->items[i])->name;
				pLBindings.defineConstant("NAME_STRING",headerFile);
			}
		}
		else
		{
			className = ((SceneClassTableItem *)sceneTable->items[i])->name;
			pLBindings.defineConstant("NAME_STRING",className);
		}

        pProcess.go();

    }
}


void doMediaRefClass(MleTemplateProcess *ptp, char *section, void *data)
{
    // Decare local variables.
    TblMgrItems *strTable = (TblMgrItems *)data;
    MleTemplateBindings pLBindings;
    MleTemplateProcess pProcess(section,
		ptp->getTemplate(), &pLBindings,  ptp->getFileDescriptor());
    char *className;

    // Output MediaRef Class entries.
    for (int i=0; i<strTable->used; i++)
	{
        className = (char *)strTable->items[i];
        pLBindings.defineConstant("NAME_STRING",className);
        pProcess.go();
    }
}
