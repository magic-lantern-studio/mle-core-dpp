/** @defgroup MleDPPMaster Magic Lantern Digital Playprint Library API - Master */

/**
 * @file DppGenCode.cpp
 * @ingroup MleDPPMaster
 *
 * Magic Lantern Digital Playprint Library API.
 */


// COPYRIGHT_BEGIN
//
// Copyright (c) 2015-2025 Wizzer Works
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
#include <sys/stat.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

// Include Magic Lantern header files.
#include <mle/mlTypes.h>
#include <mle/mlAssert.h>
#include <mle/mlErrno.h>
#include <mle/mlMalloc.h>
#include <mle/mlFileio.h>

#include <mle/mlExpandFilename.h>
#include <mle/mlConfig.h>
#include <mle/MleTemplate.h>
#include <mle/MlePath.h>
#ifdef _WINDOWS
#include <mle/MleWin32PAth.h>
#else
#include <mle/MleLinuxPath.h>
#endif /* _WINDOWS */

// Include Magic Lantern Digital Workprint header files.
#include "mle/DwpInput.h"
#include "mle/DwpItem.h"
#include "mle/DwpChecker.h"

// Include Magic Lantern Digital Playprint header files.
#include "mle/Dpp.h"
#include "mle/DppTblMgr.h"
#include "mle/DppGenCode.h"


// The global directory for generated output.
MlePath *MleDppGenCode::g_outputDir = NULL;


MleDppGenCode::MleDppGenCode(char *filename,char language,char *package)
  : m_language(language), m_package(package), m_verbose(FALSE)
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

static MlBoolean _javaTemplate;

//
// readTitleTemplate
//
// This code reads in the mastering code template, wherever it might be.
//

void MleDppGenCode::readTitleTemplate(MleTemplate &pTemplate)
{
    char *filename;

    if (m_language)
    {
        // Use template for Java code generation.
        filename = mlFilenameExpand(MLE_TITLE_JAVA_TEMPLATE);
        _javaTemplate = TRUE;
    } else
    {
        // Use template for C++ code generation.
        filename = mlFilenameExpand(MLE_TITLE_CPP_TEMPLATE);
        _javaTemplate = FALSE;
    }

    if (m_verbose) {
        char lang[5];
        if (_javaTemplate == TRUE) strcpy(lang, "Java");
        else strcpy(lang, "C++");
        fprintf(stdout, "*** MleDppGenCode: Template file name is %s.\n", filename);
        fprintf(stdout, "*** MleDppGenCode: Target language is %s.\n", lang);
    }

    if (pTemplate.read(filename) < 0)
    {
        perror(filename);
    }

    mlFree(filename);

    return;
}


#if 0
MlBoolean mlVerifyWorkprint(char *tags)
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
    mlFree(processTags);

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
        tblMgr.getRoleClassTableSize() +
        tblMgr.getSetClassTableSize() +
        tblMgr.getActorPropTableSize() +
        tblMgr.getSetPropTableSize() +
        tblMgr.getSetTableSize() +
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

    MleTemplateBindingCallback doGenCodeActorClass;
    MleTemplateBindingCallback doGenCodeProperties;
    MleTemplateBindingCallback doGenCodeRoleClass;
    MleTemplateBindingCallback doGenCodeSets;
    MleTemplateBindingCallback doGenCodeSetClass;
    MleTemplateBindingCallback doGenCodeGroupClass;
    MleTemplateBindingCallback doGenCodeSceneClass;
    MleTemplateBindingCallback doGenCodeMediaRefClass;
    pGBindings->defineCallback("DO_ACTORCLASS",doGenCodeActorClass,
        (void *)tblMgr.getActorClassTable());
    pGBindings->defineCallback("DO_ACTORPROPERTIES",doGenCodeProperties,
        (void *)tblMgr.getActorPropTable());
    pGBindings->defineCallback("DO_ROLECLASS",doGenCodeRoleClass,
        (void *)tblMgr.getRoleClassTable());
    pGBindings->defineCallback("DO_SETS",doGenCodeSets,
        (void *)tblMgr.getSetTable());
    pGBindings->defineCallback("DO_SETCLASS",doGenCodeSetClass,
        (void *)tblMgr.getSetClassTable());
    pGBindings->defineCallback("DO_SETPROPERTIES",doGenCodeProperties,
        (void *)tblMgr.getSetPropTable());
    pGBindings->defineCallback("DO_GROUPCLASS",doGenCodeGroupClass,
        (void *)tblMgr.getGroupClassTable());
    pGBindings->defineCallback("DO_SCENECLASS",doGenCodeSceneClass,
        (void *)tblMgr.getSceneClassTable());
    pGBindings->defineCallback("DO_MEDIAREFS",doGenCodeMediaRefClass,
        (void *)tblMgr.getMediaRefTable());

    // Generate quark to string mapping utilities.
    MleTemplateProcess *pProcess;
    const char *pfilename;
    if (m_language)
        pfilename = "MleRuntimeTables.java";
    else
        pfilename = "mltables.cxx";
    FILE *fd;

    // Expand directory path, if necessary.
    char *tmpPath;
    if (g_outputDir != NULL)
    {
        tmpPath = (char *)mlMalloc(strlen((char *)g_outputDir->getPlatformPath()) + strlen(pfilename) + 2);
        strcpy(tmpPath,(char *)g_outputDir->getPlatformPath());
#ifdef _WINDOWS
        strcat(tmpPath,"\\");
#else /* ! _WINDOWS */
        strcat(tmpPath,"/");
#endif /* _WINDOWS */
        strcat(tmpPath,pfilename);
    } else
        tmpPath = (char *)pfilename;

    // Open the generated tables file.
    if ( (fd = mlFOpen(tmpPath, "w")) == NULL )
    {
        perror(pfilename);
        if (g_outputDir != NULL)
            mlFree(tmpPath);
        return FALSE;
    }

    // Add language specific bindings.
    if (m_language)
    {
        // Generating Java output.
        pGBindings->defineConstant("PACKAGE",m_package);
    }

    // Generate the tables.
    pGBindings->defineConstant("FILENAME", pfilename);
    pProcess = new MleTemplateProcess("RUNTIME_INDEX_TO_STRING", &pTemplate, NULL, fd);
    pProcess->go();

    // Close the generated tables file.
    mlFClose(fd);

    // Clean up.
    delete pProcess;
    if (g_outputDir != NULL)
        mlFree(tmpPath);

    return TRUE;
}


void doGenCodeIndexStrings(MleTemplateProcess *ptp, char *section, void *data)
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


void doGenCodeActorClass(MleTemplateProcess *ptp, char *section, void *data)
{
    // Decare local variables.
    TblMgrItems *actorTable = (TblMgrItems *)data;
    MleTemplateBindings pLBindings;
    MleTemplateProcess pProcess(section,
        ptp->getTemplate(), &pLBindings, ptp->getFileDescriptor());
    char *className,*headerFile,*package,*lastDot;
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
            // Set Java PACKAGE if generatinog code for Java target.
            if (_javaTemplate)
            {
                // Generate Java imports.
                if (((ActorClassTableItem *)actorTable->items[i])->package)
                {
                    package = ((ActorClassTableItem *)actorTable->items[i])->package;
                    pLBindings.defineConstant("PACKAGE",package);
                }

                headerFile = ((ActorClassTableItem *)actorTable->items[i])->name;
                pLBindings.defineConstant("NAME_STRING",headerFile);
            } else
            {
                // Process C/C++ header files.
                if (((ActorClassTableItem *)actorTable->items[i])->headerFile)
                {
                    headerFile = ((ActorClassTableItem *)actorTable->items[i])->headerFile;
                    lastDot = strrchr(headerFile, '.');
                    if (lastDot)
                        *lastDot = '\0';
                    pLBindings.defineConstant("NAME_STRING",headerFile);
                    if (lastDot)
                        *lastDot = '.';
                } else
                {
                    headerFile = ((ActorClassTableItem *)actorTable->items[i])->name;
                    pLBindings.defineConstant("NAME_STRING",headerFile);
                }
            }
        }
        else
        {
            // Set Java PACKAGE if generatinog code for Java target.
            if (_javaTemplate)
            {
                if (((ActorClassTableItem *)actorTable->items[i])->package)
                {
                    package = ((ActorClassTableItem *)actorTable->items[i])->package;
                    pLBindings.defineConstant("PACKAGE",package);
                }
            }
            
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


void doGenCodeProperties(MleTemplateProcess *ptp, char *section, void *data)
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
#if defined(_WINDOWS)
        className = _strdup((char *)strTable->items[i]);
#else
        className = strdup((char *)strTable->items[i]);
#endif
        underbar = strchr(className,'_');
        *underbar = '\0';
        pLBindings.defineConstant("NAME_STRING",className);

        underbar++;
#if defined(_WINDOWS)
        propName = _strdup(underbar);
#else
        propName = strdup(underbar);
#endif
        pLBindings.defineConstant("PROPERTY_NAME",propName);

        pProcess.go();

        mlFree(className);
        mlFree(propName);
    }
}


void doGenCodeRoleClass(MleTemplateProcess *ptp, char *section, void *data)
{
    // Decare local variables.
    TblMgrItems *roleTable = (TblMgrItems *)data;
    MleTemplateBindings pLBindings;
    MleTemplateProcess pProcess(section,
        ptp->getTemplate(), &pLBindings, ptp->getFileDescriptor());
    char *className,*package;

    // Output Role Class entries.
    for (int i=0; i < roleTable->used; i++)
    {
        // Set Java PACKAGE if generatinog code for Java target.
        if (_javaTemplate)
        {
            if (((RoleClassTableItem *)roleTable->items[i])->package)
            {
                package = ((RoleClassTableItem *)roleTable->items[i])->package;
                pLBindings.defineConstant("PACKAGE",package);
            }
        }

        className = ((RoleClassTableItem *)roleTable->items[i])->name;
        pLBindings.defineConstant("NAME_STRING",className);
        pProcess.go();
    }
}



void doGenCodeSetClass(MleTemplateProcess *ptp, char *section, void *data)
{
    // Decare local variables.
    TblMgrItems *setTable = (TblMgrItems *)data;
    MleTemplateBindings pLBindings;
    MleTemplateProcess pProcess(section,
        ptp->getTemplate(), &pLBindings, ptp->getFileDescriptor());
    char *className,*headerFile,*package,*lastDot;
    int offset = 0;
    MlBoolean isTypeInclude;

    if (strcmp(section, "TYPE_INCLUDES") == 0)
        isTypeInclude = TRUE;
    else
        isTypeInclude = FALSE;


    // Output Set Class entries.
    for (int i=0; i<setTable->used; i++)
    {
        if (isTypeInclude)
        {
            if (_javaTemplate)
            {
                // Generate Java imports.
                if (((SetClassTableItem *)setTable->items[i])->package)
                {
                    package = ((SetClassTableItem *)setTable->items[i])->package;
                    pLBindings.defineConstant("PACKAGE",package);
                }

                headerFile = ((SetClassTableItem *)setTable->items[i])->name;
                pLBindings.defineConstant("NAME_STRING",headerFile);
            } else
            {
                // Process C/C++ header files.
                if (((SetClassTableItem *)setTable->items[i])->headerFile)
                {
                    headerFile = ((SetClassTableItem *)setTable->items[i])->headerFile;
                    lastDot = strrchr(headerFile, '.');
                    if(lastDot)
                        *lastDot = '\0';
                    pLBindings.defineConstant("NAME_STRING",headerFile);
                    if(lastDot)
                        *lastDot = '.';
                }
                else
                {
                    headerFile = ((SetClassTableItem *)setTable->items[i])->name;
                    pLBindings.defineConstant("NAME_STRING",headerFile);
                }
            }
        }
        else
        {
            // Set Java PACKAGE if generatinog code for Java target.
            if (_javaTemplate)
            {
                if (((SetClassTableItem *)setTable->items[i])->package)
                {
                    package = ((SetClassTableItem *)setTable->items[i])->package;
                    pLBindings.defineConstant("PACKAGE",package);
                }
            }

            className = ((SetClassTableItem *)setTable->items[i])->name;
            pLBindings.defineConstant("NAME_STRING",className);
        }

        pLBindings.defineConstant("PROPTBL_OFFSET",offset);
        pProcess.go();

        // Iterate over the Set Class and increment offset
        // into Property table.
        for (int j = 0;
             j<((SetClassTableItem *)setTable->items[i])->numProperties;
             j++)
        {
                offset++;
        }
    }
}


void doGenCodeSets(MleTemplateProcess *ptp, char *section, void *data)
{
    // Decare local variables.
    TblMgrItems *setTable = (TblMgrItems *)data;
    MleTemplateBindings pLBindings;
    MleTemplateProcess pProcess(section,
        ptp->getTemplate(),  &pLBindings, ptp->getFileDescriptor());
    char *setType;

    // Output Set Type entries.
    for (int i=0; i<setTable->used; i++)
    {
        setType = ((SetTableItem *)setTable->items[i])->type;
        pLBindings.defineConstant("NAME_STRING",setType);
        pProcess.go();
    }
}


void doGenCodeGroupClass(MleTemplateProcess *ptp, char *section, void *data)
{
    // decare local variables.
    TblMgrItems *groupTable = (TblMgrItems *)data;
    MleTemplateBindings pLBindings;
    MleTemplateProcess pProcess(section,
        ptp->getTemplate(), &pLBindings, ptp->getFileDescriptor());
    char *className,*headerFile,*package,*lastDot;
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
            if (_javaTemplate)
            {
                // Generate Java imports.
                if (((GroupClassTableItem *)groupTable->items[i])->package)
                {
                    package = ((GroupClassTableItem *)groupTable->items[i])->package;
                    pLBindings.defineConstant("PACKAGE",package);
                }

                headerFile = ((GroupClassTableItem *)groupTable->items[i])->name;
                pLBindings.defineConstant("NAME_STRING",headerFile);
            } else
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
        }
        else 
        {
            // Set Java PACKAGE if generatinog code for Java target.
            if (_javaTemplate)
            {
                if (((GroupClassTableItem *)groupTable->items[i])->package)
                {
                    package = ((GroupClassTableItem *)groupTable->items[i])->package;
                    pLBindings.defineConstant("PACKAGE",package);
                }

            }

            className = ((GroupClassTableItem *)groupTable->items[i])->name;
            pLBindings.defineConstant("NAME_STRING",className);
        }

        pProcess.go();

    }
}


void doGenCodeSceneClass(MleTemplateProcess *ptp, char *section, void *data)
{
    // Decare local variables.
    TblMgrItems *sceneTable = (TblMgrItems *)data;
    MleTemplateBindings pLBindings;
    MleTemplateProcess pProcess(section,
        ptp->getTemplate(), &pLBindings, ptp->getFileDescriptor());
    char *className,*headerFile,*package,*lastDot;
    MlBoolean isTypeInclude;

    if(strcmp(section, "TYPE_INCLUDES") == 0)
        isTypeInclude = TRUE;
    else
        isTypeInclude = FALSE;

    // Output Scene Class entries.
    for (int i=0; i<sceneTable->used; i++)
    {
        if (isTypeInclude)
        {
            if (_javaTemplate)
            {
                // Generate Java imports.
                if (((SceneClassTableItem *)sceneTable->items[i])->package)
                {
                    package = ((SceneClassTableItem *)sceneTable->items[i])->package;
                    pLBindings.defineConstant("PACKAGE",package);
                }

                headerFile = ((SceneClassTableItem *)sceneTable->items[i])->name;
                pLBindings.defineConstant("NAME_STRING",headerFile);
            } else
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
        }
        else
        {
            // Set Java PACKAGE if generatinog code for Java target.
            if (_javaTemplate)
            {
                if (((SceneClassTableItem *)sceneTable->items[i])->package)
                {
                    package = ((SceneClassTableItem *)sceneTable->items[i])->package;
                    pLBindings.defineConstant("PACKAGE",package);
                }
            }

            className = ((SceneClassTableItem *)sceneTable->items[i])->name;
            pLBindings.defineConstant("NAME_STRING",className);
        }

        pProcess.go();

    }
}


void doGenCodeMediaRefClass(MleTemplateProcess *ptp, char *section, void *data)
{
    // Decare local variables.
    TblMgrItems *mrefTable = (TblMgrItems *)data;
    MleTemplateBindings pLBindings;
    MleTemplateProcess pProcess(section,
        ptp->getTemplate(), &pLBindings,  ptp->getFileDescriptor());
    char *className,*package;

    // Output MediaRef Class entries.
    for (int i=0; i<mrefTable->used; i++)
    {
        // Set Java PACKAGE if generatinog code for Java target.
        if (_javaTemplate)
        {
            if (((MediaRefClassTableItem *)mrefTable->items[i])->package)
            {
                package = ((MediaRefClassTableItem *)mrefTable->items[i])->package;
                pLBindings.defineConstant("PACKAGE",package);
            }
        }

        className = ((MediaRefClassTableItem *)mrefTable->items[i])->name;
        pLBindings.defineConstant("NAME_STRING",className);
        pProcess.go();
    }
}


void MleDppGenCode::setOutputDirectory(const char *path)
{
    // Clear the output directory path, if path is NULL.
    if (path == NULL)
    {
        delete g_outputDir;
        g_outputDir = NULL;
        return;
    }

    if (g_outputDir == NULL)
#if _WINDOWS
        g_outputDir = new MleWin32Path((MlChar *)path, TRUE);
#else
        g_outputDir = new MleLinuxPath((MlChar *)path, TRUE);
#endif /* _WINDOWS */
    else
        g_outputDir->setPlatformPath((MlChar *)path);
}


inline MlePath *
MleDppGenCode::getOutputDirectory()
{
    return g_outputDir;
}


void *
MleDppGenCode::operator new(size_t tSize)
{
    void *p = mlMalloc(tSize);
    return p;
}


void
MleDppGenCode::operator delete(void *p)
{
    mlFree(p);
}


void*
MleDppGenCode::operator new[](size_t tSize)
{
    void* p = mlMalloc(tSize);
    return p;
}


void
MleDppGenCode::operator delete[](void* p)
{
    mlFree(p);
}
