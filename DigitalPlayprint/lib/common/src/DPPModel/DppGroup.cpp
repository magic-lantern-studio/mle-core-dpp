/** @defgroup MleDPPModel Magic Lantern Digital Playprint Library API - Model */

/**
 * @file DppGroup.cpp
 * @ingroup MleDPPModel
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
//  For information concerning this source file, contact Mark S. Millard,
//  of Wizzer Works at msm@wizzerworks.com.
//
//  More information concerning Wizzer Works may be found at
//
//      http://www.wizzerworks.com
//
// COPYRIGHT_END

// Include system header files.
#include <stdlib.h>
#include <string.h>

// Include Magic Lantern header files.
#include "mle/mlMalloc.h"

// Include Digital Workprint header files.
#include "mle/DwpActor.h"
#include "mle/DwpDatatype.h"
#include "mle/DwpDelegateAttachment.h"
#include "mle/DwpFinder.h"
#include "mle/DwpGroup.h"
#include "mle/DwpGroupRef.h"
#include "mle/DwpInput.h"
#include "mle/DwpMediaRef.h"
#include "mle/DwpOutput.h"
#include "mle/DwpProperty.h"

// Include Digital Playprint header files.
#include "mle/DppActor.h"
#include "mle/DppGroup.h"
#include "mle/DppGroupRef.h"
#include "mle/DppActorGroupOutput.h"

//#include "mle/groupclass.h"


MLE_DWP_SOURCE(MleDppGroup,MleDwpGroup);


MleDppGroup::MleDppGroup(char *gc)
  : MleDwpGroup(gc)
{
	m_dwpItem = this;
}


MleDppGroup::~MleDppGroup()
{
    // Do nothing for now.
}


int
MleDppGroup::writeContents(MleDppActorGroupOutput *out)
{
	// Find the group class.
	const char *className = getGroupClass();

	int groupClassNumber = out->m_table->groupClassRuntimeName(className); 
	out->writeInt(groupClassNumber);
	out->m_currentGroupClass = className;

    MleDwpFinder findActors;
    findActors.setType(MleDwpActor::typeId);
    findActors.setFindAll(1);
    findActors.find(this);
    out->writeIndex(findActors.getNumItems());

    return 0;
}


// Used by qsort to sort media references by name.
static int CompareReferences(const void* ref1, const void* ref2)
{
    return strcmp((*(const MleDwpMediaRef**) ref1)->getName(),
        (*(const MleDwpMediaRef**) ref2)->getName());
}


// Used by bsearch in MediaIsReferencedByProperty to find a named property type.
static int FindPropertyData(const void* key, const void* property)
{
    return strcmp((const char*) key, (const char*) (*(MleDwpProperty**) property)->m_data.m_u.v);
}


int
MleDppGroup::write(MleDppActorGroupOutput *out)
{
    int i;	// Loop variables.

    // Open output file.
    char groupChunkFile[FILENAME_MAX];
    sprintf(groupChunkFile, "%s.chk", getName());
    out->openFile(groupChunkFile);

    // Find all the actors in this group.
    //MleDwpFinder actorFinder(MleDwpActor::typeId,NULL,1);
	MleDwpFinder actorFinder(MleDppActor::typeId,NULL,1);
    actorFinder.find(this);
    const int numActors = actorFinder.getNumItems();
    //MleDwpActor **actor = (MleDwpActor **)actorFinder.getItems();
	MleDppActor **actor = (MleDppActor **)actorFinder.getItems();

    // Write out the index of this group\'s class in the runtime
    // table of constructors.
    MleDwpItem *root = this;
    while ( root->getParent() )
	    root = root->getParent();

	MLE_ASSERT(out->m_table != NULL);
	TblMgrItems *groupClassTable = out->m_table->getGroupClassTable();
	int index = out->m_table->groupClassRuntimeName( this->getGroupClass() );
    out->writeInt(index);

    // write out the number of actors in the group to the chunk
    out->writeIndex(numActors);

    // do weird media reference stuff
    root = this->getParent();
    MleDwpItem *parent;
    while ((parent = root->getParent()) != NULL)
        root = parent;

    // find all media references in this actor set
    // reduce the array to exclude media that is not referred to in a
    // property
    MleDwpFinder findMediaReferences;
    findMediaReferences.setType(MleDwpMediaRef::typeId);
    findMediaReferences.setFindAll(1);
    findMediaReferences.find(root);
    out->m_numReferences = findMediaReferences.getNumItems();
    MleDwpMediaRef **references = (MleDwpMediaRef**) findMediaReferences.getItems();
    // sort the media references by name
    if (out->m_numReferences > 0)
	{
            out->m_references = new MleDwpMediaRef*[out->m_numReferences];
            memcpy(out->m_references, references, sizeof(MleDwpItem**) * out->m_numReferences);
            qsort(out->m_references, out->m_numReferences, sizeof(out->m_references[0]), CompareReferences);
    }

    // output each actor
    //   Actors are written out in the same order they were created.
	if (out->m_language)
		// Output Java.
		fprintf(out->m_actorid,"\n    /*\n     * Global actor index. \n     */\n");
	else
		// Output C/C++.
        fprintf(out->m_actorid,"\n/*\n * Global actor index. \n */\n");
    for ( i = 0; i < numActors; i++ )
	{
	    // The actor\'s part of the group chunk file
	    //((MleDppItem *)(actor[i]))->write(out);
		actor[i]->write(out);

	    // The actor\'s part of the actorid.h file.
	    if (out->m_actorid)
		{
			if (out->m_language)
				// Output Java.
		        fprintf(out->m_actorid, "    public static final int MleDPPActor_%s = %d;\n", 
			        actor[i]->getName(), i + out->m_actorCountCumulative);
			else
			    // Output C/C++.
		        fprintf(out->m_actorid, "#define MleDPPActor_%s %d\n", 
			        actor[i]->getName(), i + out->m_actorCountCumulative);
		}
	}
    // Expect the actorCount to contain the running sum of every
    // actor we\'ve seen so far.
    out->m_actorCount += numActors;
    out->m_actorCountCumulative += numActors;

    // The group\'s part of the actorid.h file.
    if (out->m_actorid)
	{
		if (out->m_language)
		{
			// Output Java.

			// The actor\'s relative index in the group.
			fprintf(out->m_actorid,"\n    /*\n     * Actor indices within the group. \n     */\n");
			for ( i = 0; i < numActors; i++ )
			{
				fprintf(out->m_actorid,"    public static final int MleDPPGroupActorID_%s_%s = %d;\n",
					getName(), actor[i]->getName(), i);
			}
		} else
		{
			// Output C/C++.

			// The actor\'s relative index in the group.
			fprintf(out->m_actorid,"\n/*\n * Actor indices within the group. \n */\n");
			for ( i = 0; i < numActors; i++ )
			{
				fprintf(out->m_actorid,"#define MleDPPGroupActorID_%s_%s %d;\n",
					getName(), actor[i]->getName(), i);
			}
		}
    }

    // Write the group\'s global index as part of the groupid.h file.
    // If this is a groupRef, then don\'t write anything.
    if (out->m_groupid)
	{
		if (out->m_language)
		{
			int index = out->m_table->groupInstanceRuntimeName(getName());
			fprintf(out->m_groupid, "    public static final int MleDPPGroupID_%s = %d;\n", 
				getName(), index);
		} else
		{
			int index = out->m_table->groupInstanceRuntimeName(getName());
			fprintf(out->m_groupid, "#define MleDPPGroupID_%s %d\n", 
				getName(), index);
		}
    }

    // Find all delegate attachments.
    MleDwpFinder attachmentFinder(MleDwpDelegateAttachment::typeId,NULL,1);
    attachmentFinder.find(this);
    const int numAttachments = attachmentFinder.getNumItems();
    const MleDwpDelegateAttachment **attachment =
    	(const MleDwpDelegateAttachment **)attachmentFinder.getItems();

    // Loop over the attachments.
    for ( i = 0; i < numAttachments; i++ )
    {
		// Find the parent
		int parentIndex;
		for ( parentIndex = 0; parentIndex < numActors; parentIndex++ )
		{
			if ( !strcmp(actor[parentIndex]->getName(),attachment[i]->getParent()) )
			break;
		}

		// Find the child
		int childIndex;
		for ( childIndex = 0; childIndex < numActors; childIndex++ )
		{
			if ( !strcmp(actor[childIndex]->getName(),attachment[i]->getChild()) )
			break;
		}

		// XXX verification
		//   Should verify here that the actors involved have delegate
		//   bindings in the same forum.  This is a little difficult
		//   because of the possible presence of templates.

		if ( parentIndex == numActors )
		{
			fprintf(stderr,"error generating delegate binding index for %s.\n",attachment[i]->getParent());
			continue;
		}
		if ( childIndex == numActors )
		{
			fprintf(stderr,"error generating delegate binding index for %s.\n",attachment[i]->getChild());
			continue;
		}

		// Write the delegate attachment opcode and the parent and child
		// indices
		out->writeOpcode(parentDelegateOpcode);
		out->writeIndex(parentIndex);
		out->writeOpcode(childDelegateOpcode);
		out->writeIndex(childIndex);
    }

    return 0;
}


int 
MleDppGroup::groupOrGroupRefFinderCB( MleDwpItem *item, void* )
{
	MleDwpType t = item->getTypeId();

	return (t == MleDwpGroup::typeId ||
		    t == MleDwpGroupRef::typeId ||
			t == MleDppGroup::typeId ||
		    t == MleDppGroupRef::typeId);
}

void *
MleDppGroup::operator new(size_t tSize)
{
	void *p = mlMalloc(tSize);
	return p;
}

void
MleDppGroup::operator delete(void *p)
{
	mlFree(p);
}

void*
MleDppGroup::operator new[](size_t tSize)
{
	void* p = mlMalloc(tSize);
	return p;
}

void
MleDppGroup::operator delete[](void* p)
{
	mlFree(p);
}
