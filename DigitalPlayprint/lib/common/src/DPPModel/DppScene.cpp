/** @defgroup MleDPPModel Magic Lantern Digital Playprint Library API - Model */

/**
 * @file DppScene.pp
 * @ingroup MleDPPModel
 *
 * Magic Lantern Digital Playprint Library API.
 *
 * @author Mark S. Millard
 * @date September 14, 2004
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
#include <string.h>
#include <stdio.h>

// Include Magic Lantern header files.
#include "mle/mlMalloc.h"

// Include Digital Workprint header files.
#include "mle/DwpFinder.h"
#include "mle/DwpGroup.h"
#include "mle/DwpGroupRef.h"
#include "mle/DwpInput.h"
#include "mle/DwpOutput.h"
#include "mle/DwpScene.h"

// Include Digital Playprint header files.
#include "mle/DppGroup.h"
#include "mle/DppScene.h"
#include "mle/DppSceneOutput.h"
#include "mle/DppTblMgr.h"


MLE_DWP_SOURCE(MleDppScene,MleDwpScene);


MleDppScene::MleDppScene(char *sc)
  : MleDwpScene(sc)
{
	m_dwpItem = this;
}

MleDppScene::~MleDppScene(void)
{
    // Do nothing for now.
}

int
MleDppScene::writeContents(MleDppSceneOutput *out)
{
	// find the scene class
	const char *className = getSceneClass();

	int sceneClassNumber = out->m_table->sceneClassRuntimeName(className); 
	out->writeInt(sceneClassNumber);
	out->m_currentSceneClass = className;

	return 0;
}

int
MleDppScene::write(MleDppSceneOutput *out)
{
    int i;	// Loop variables.

    // Open output file
    char sceneChunkFile[FILENAME_MAX];
    sprintf(sceneChunkFile, "%s.chk", getName());
    out->openFile(sceneChunkFile);

    // Find all the groups and group references in this scene.
    MleDwpFinder groupFinder;
    //groupFinder.setCallback(&MleDwpGroup::groupOrGroupRefFinderCB, NULL);
	groupFinder.setCallback(&MleDppGroup::groupOrGroupRefFinderCB, NULL);
    groupFinder.setFindAll(1);
    groupFinder.find(this);
    const int numGroups = groupFinder.getNumItems();
    MleDwpItem **groups = (MleDwpItem **)groupFinder.getItems();

    // Write out the index of this scene\'s class in the runtime
    // table of constructors.
    MleDwpItem *root = this;
    while ( root->getParent() )
	    root = root->getParent();

    MleDppTblMgr tbl;
    int status = tbl.buildIndexTables(root);
    MLE_ASSERT(status == TRUE);
    int index = tbl.sceneClassRuntimeName( this->getSceneClass() );
    out->writeInt(index);

    // Write out the number of groups in the scene to the chunk.
    out->writeInt(numGroups);

    // Output each group
    //   Groups are written out in the same order they were created.
    for ( i = 0; i < numGroups; i++ )
	{

	    // If this item is a groupRef, then we ask for its name,
	    // which is the unique name of a group instance.  Then we
	    // look up the group instance in the TblMgr and get the 
	    // group\'s index from there.
	    // It\'s pretty handy that if this item is a group, then
	    // it\'s name is the same as that of a valid group ref,
	    // so we can use the same code for both types of items.
	    int index = tbl.groupInstanceRuntimeName(groups[i]->getName());
	    
	    // Write the group\'s global index to the chunk.
	    out->writeInt(index); 

    }

    // The scene\'s part of the sceneid.h file.
    // We don\'t know our number, so instead count on the sceneCount
    // member to hold it for us, and on out to traverse the set of 
    // scenes in order. XXX
    if (out->m_sceneid)
	{
		if (out->m_language)
		{
			// Output Java code.
			fprintf(out->m_sceneid,"\n    /*\n     * Global scene index. \n     */\n");
			fprintf(out->m_sceneid, "    public static final int MleDPPSceneID_%s = %d;\n", 
				getName(), out->m_sceneCount);

			// The group\'s relative index in the scene.
			fprintf(out->m_sceneid,"\n    /*\n     * Group indices within the scene. \n     */\n");
			for ( i = 0; i < numGroups; i++ )
			{
				fprintf(out->m_sceneid,"    public static final int MleDPPSceneGroupID_%s_%s = %d;\n",
					getName(), groups[i]->getName(), i);
			}
		} else
		{
			// Output C/C++ code.
			fprintf(out->m_sceneid,"\n/*\n * Global scene index. \n */\n");
			fprintf(out->m_sceneid, "#define MleDPPSceneID_%s %d\n", 
				getName(), out->m_sceneCount);

			// The group\'s relative index in the scene.
			fprintf(out->m_sceneid,"\n/*\n * Group indices within the scene. \n */\n");
			for ( i = 0; i < numGroups; i++ )
			{
				fprintf(out->m_sceneid,"#define MleDPPSceneGroupID_%s_%s %d\n",
					getName(), groups[i]->getName(), i);
			}
		}
    }
    out->m_sceneCount++;

    // Don't need to delete the groups here because the MleDwpFinder's
	// destructor will take care of it.

    return 0;
}

void *
MleDppScene::operator new(size_t tSize)
{
	void *p = mlMalloc(tSize);
	return p;
}

void
MleDppScene::operator delete(void *p)
{
	mlFree(p);
}
