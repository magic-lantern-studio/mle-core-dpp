/** @defgroup MleDPPBase Magic Lantern Digital Workprint Library API - Base */

/**
 * @file DppTblMgr.cpp
 * @ingroup MleDPPBase
 *
 * This file implements the table manager used by the Magic Lantern Digital
 * Playprint Library API.
 *
 * @author Mark S. Millard
 * @date July 17, 2004
 */

// COPYRIGHT_BEGIN
//
// The MIT License (MIT)
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

// Include Magic Lantern header files.
#include "mle/mlUnique.h"
#include "mle/mlErrno.h"
#include "mle/mlMalloc.h"
#include "mle/mlAssert.h"

// Include Magic Lantern DPP header files.
#include "mle/DppTblMgr.h"

// Include Magic Lantern DWP header files.
#include "mle/DwpItem.h"
#include "mle/DwpActor.h"
#include "mle/DwpActorTemplate.h"
#include "mle/DwpProperty.h"
#include "mle/DwpDatatype.h"
#include "mle/DwpActorDef.h"
#include "mle/DwpDelegateBinding.h"
#include "mle/DwpDelegateDef.h"
#include "mle/DwpRoleBinding.h"
#include "mle/DwpRoleDef.h"
#include "mle/DwpGroup.h"
#include "mle/DwpScene.h"
#include "mle/DwpForum.h"
#include "mle/DwpForumDef.h"
#include "mle/DwpSet.h"
#include "mle/DwpSetDef.h"
#include "mle/DwpMediaRef.h"
#include "mle/DwpFinder.h"
#include "mle/DwpStrKeyDict.h"
#include "mle/DwpHeaderFile.h"
#include "mle/DwpPackage.h"

// Include Magic Lantern DWP header files.


MleDppTblMgr::MleDppTblMgr()
    :m_discriminator(NULL)
{
    m_actorClassTable.used = m_actorClassTable.allocated = 0;
	m_actorClassTable.items = NULL;
    m_roleClassTable.used = m_roleClassTable.allocated = 0;
	m_roleClassTable.items = NULL;
    m_setClassTable.used = m_setClassTable.allocated = 0;
	m_setClassTable.items = NULL;
    m_actorPropTable.used = m_actorPropTable.allocated = 0;
	m_actorPropTable.items = NULL;
    m_setPropTable.used = m_setPropTable.allocated = 0;
	m_setPropTable.items = NULL;
    m_groupClassTable.used = m_groupClassTable.allocated = 0;
	m_groupClassTable.items = NULL;
    m_groupInstanceTable.used = m_groupInstanceTable.allocated = 0;
	m_groupInstanceTable.items = NULL;
    m_sceneClassTable.used = m_sceneClassTable.allocated = 0;
	m_sceneClassTable.items = NULL;
    m_setTable.used = m_setTable.allocated = 0;
	m_setTable.items = NULL;
    m_mediarefTable.used = m_mediarefTable.allocated = 0;
	m_mediarefTable.items = NULL;
}


MleDppTblMgr::~MleDppTblMgr(void)
{
    // Do nothing extra.
}


#define ALLOC_INC 32

void MleDppTblMgr::saveString(
	const char *pstr,
	TblMgrItems *strTable)
{
    if ( strTable->used >= strTable->allocated )
	{
        if ( strTable->allocated == 0 )
		{
            strTable->allocated = ALLOC_INC;
            strTable->items = (void **)mlMalloc(strTable->allocated*sizeof(char *));
        } else
		{
            strTable->allocated += ALLOC_INC;
            strTable->items = (void **)mlRealloc((void *)strTable->items,
                strTable->allocated*sizeof(char *));
        }
    }
    strTable->items[strTable->used++] = (void *)pstr;
}


void MleDppTblMgr::saveActorClass(
  char *name,
  int numProperties,
  char *headerFile,
  char *package,
  TblMgrItems *actorTable)
{
    int i;

    if ( actorTable->used >= actorTable->allocated )
	{
        if ( actorTable->allocated == 0 )
		{
            actorTable->allocated = ALLOC_INC;
            actorTable->items =
                (void **)mlMalloc(actorTable->allocated*sizeof(ActorClassTableItem *));

            for (i = 0; i < ALLOC_INC; i++)
			{
                actorTable->items[i] =
                    (void *)mlMalloc(sizeof(ActorClassTableItem));
            }
        } else
		{
            actorTable->allocated += ALLOC_INC;
            actorTable->items = (void **)mlRealloc((void *)actorTable->items,
                actorTable->allocated*sizeof(ActorClassTableItem *));

            for (i = ALLOC_INC; i > 0; i--)
			{
                actorTable->items[actorTable->allocated - i] =
                    (void *)mlMalloc(sizeof(ActorClassTableItem));
            }
        }
    }
    ((ActorClassTableItem *)actorTable->items[actorTable->used])->name = name;
    ((ActorClassTableItem *)actorTable->items[actorTable->used])->numProperties = numProperties;
    ((ActorClassTableItem *)actorTable->items[actorTable->used])->headerFile = headerFile;
	((ActorClassTableItem *)actorTable->items[actorTable->used])->package = package;

	actorTable->used++;
}


void MleDppTblMgr::saveRoleClass(
  char *name,
  char *headerFile,
  char *package,
  TblMgrItems *roleTable)
{
    int i;

    if ( roleTable->used >= roleTable->allocated )
	{
        if ( roleTable->allocated == 0 )
		{
            roleTable->allocated = ALLOC_INC;
            roleTable->items =
                (void **)mlMalloc(roleTable->allocated*sizeof(RoleClassTableItem *));

            for (i = 0; i < ALLOC_INC; i++)
			{
                roleTable->items[i] =
                    (void *)mlMalloc(sizeof(RoleClassTableItem));
            }
        } else
		{
            roleTable->allocated += ALLOC_INC;
            roleTable->items = (void **)mlRealloc((void *)roleTable->items,
                roleTable->allocated*sizeof(RoleClassTableItem *));

            for (i = ALLOC_INC; i > 0; i--)
			{
                roleTable->items[roleTable->allocated - i] =
                    (void *)mlMalloc(sizeof(RoleClassTableItem));
            }
        }
    }
    ((RoleClassTableItem *)roleTable->items[roleTable->used])->name = name;
    ((RoleClassTableItem *)roleTable->items[roleTable->used])->headerFile = headerFile;
	((RoleClassTableItem *)roleTable->items[roleTable->used])->package = package;

    roleTable->used++;
}


void MleDppTblMgr::saveGroupClass(
  char *name,
  char *headerFile,
  char *package,
  TblMgrItems *groupTable)
{
    int i;

    if ( groupTable->used >= groupTable->allocated )
	{
        if ( groupTable->allocated == 0 )
		{
            groupTable->allocated = ALLOC_INC;
            groupTable->items =
                (void **)mlMalloc(groupTable->allocated*sizeof(GroupClassTableItem *));

            for (i = 0; i < ALLOC_INC; i++)
			{
                groupTable->items[i] =
                    (void *)mlMalloc(sizeof(GroupClassTableItem));
            }
        } else
		{
            groupTable->allocated += ALLOC_INC;
            groupTable->items = (void **)mlRealloc((void *)groupTable->items,
                groupTable->allocated*sizeof(GroupClassTableItem *));

            for (i = ALLOC_INC; i > 0; i--)
			{
                groupTable->items[groupTable->allocated - i] =
                    (void *)mlMalloc(sizeof(GroupClassTableItem));
            }
        }
    }
    ((GroupClassTableItem *)groupTable->items[groupTable->used])->name = name;
    ((GroupClassTableItem *)groupTable->items[groupTable->used])->headerFile = headerFile;
	((GroupClassTableItem *)groupTable->items[groupTable->used])->package = package;

    groupTable->used++;
}


void MleDppTblMgr::saveGroupInstance(
  char *name,
  TblMgrItems *groupTable)
{
    int i;

    if ( groupTable->used >= groupTable->allocated )
	{
        if ( groupTable->allocated == 0 )
		{
            groupTable->allocated = ALLOC_INC;
            groupTable->items =
                (void **)mlMalloc(groupTable->allocated*sizeof(GroupInstanceTableItem *));

            for (i = 0; i < ALLOC_INC; i++)
			{
                groupTable->items[i] =
                    (void *)mlMalloc(sizeof(GroupInstanceTableItem));
            }
        } else
		{
            groupTable->allocated += ALLOC_INC;
            groupTable->items = (void **)mlRealloc((void *)groupTable->items,
                groupTable->allocated*sizeof(GroupInstanceTableItem *));

            for (i = ALLOC_INC; i > 0; i--)
			{
                groupTable->items[groupTable->allocated - i] =
                    (void *)mlMalloc(sizeof(GroupInstanceTableItem));
            }
        }
    }

    ((GroupInstanceTableItem *)groupTable->items[groupTable->used])->name = name;

    groupTable->used++;
}


void MleDppTblMgr::saveSceneClass(
  char *name,
  char *headerFile,
  char *package,
  TblMgrItems *sceneTable)
{
    int i;

    if ( sceneTable->used >= sceneTable->allocated )
	{
        if ( sceneTable->allocated == 0 )
		{
            sceneTable->allocated = ALLOC_INC;
            sceneTable->items =
                (void **)mlMalloc(sceneTable->allocated*sizeof(SceneClassTableItem *));

            for (i = 0; i < ALLOC_INC; i++)
			{
                sceneTable->items[i] =
                    (void *)mlMalloc(sizeof(SceneClassTableItem));
            }
        } else
		{
            sceneTable->allocated += ALLOC_INC;
            sceneTable->items = (void **)mlRealloc((void *)sceneTable->items,
                sceneTable->allocated*sizeof(SceneClassTableItem *));

            for (i = ALLOC_INC; i > 0; i--)
			{
                sceneTable->items[sceneTable->allocated - i] =
                    (void *)mlMalloc(sizeof(SceneClassTableItem));
            }
        }
    }

    ((SceneClassTableItem *)sceneTable->items[sceneTable->used])->name = name;
    ((SceneClassTableItem *)sceneTable->items[sceneTable->used])->headerFile = headerFile;
	((SceneClassTableItem *)sceneTable->items[sceneTable->used])->package = package;

    sceneTable->used++;
}


void MleDppTblMgr::saveSet(
	char *name,
	char *type,
	TblMgrItems *setTable)
{
    int i;

    if ( setTable->used >= setTable->allocated )
	{
        if ( setTable->allocated == 0 )
		{
            setTable->allocated = ALLOC_INC;
            setTable->items =
                (void **)mlMalloc(setTable->allocated*sizeof(SetTableItem *));

            for (i = 0; i < ALLOC_INC; i++)
			{
                setTable->items[i] =
                    (void *)mlMalloc(sizeof(SetTableItem));
            }
        } else
		{
            setTable->allocated += ALLOC_INC;
            setTable->items = (void **)mlRealloc((void *)setTable->items,
                setTable->allocated*sizeof(SetTableItem *));

            for (i = ALLOC_INC; i > 0; i--)
			{
                setTable->items[setTable->allocated - i] =
                    (void *)mlMalloc(sizeof(SetTableItem));
            }
        }
    }

    ((SetTableItem *)setTable->items[setTable->used])->name = name;
    ((SetTableItem *)setTable->items[setTable->used])->type = type;

	setTable->used++;
}


void MleDppTblMgr::saveSetClass(
	char *name,
	int numProperties,
	char *headerFile,
	char *package,
    TblMgrItems *setTable)
{
    int i;

    if ( setTable->used >= setTable->allocated )
	{
        if ( setTable->allocated == 0 )
		{
            setTable->allocated = ALLOC_INC;
            setTable->items =
                (void **)mlMalloc(setTable->allocated*sizeof(SetClassTableItem *));

            for (i = 0; i < ALLOC_INC; i++)
			{
                setTable->items[i] =
                    (void *)mlMalloc(sizeof(SetClassTableItem));
            }
        } else
		{
            setTable->allocated += ALLOC_INC;
            setTable->items = (void **)mlRealloc((void *)setTable->items,
                setTable->allocated*sizeof(SetClassTableItem *));

            for (i = ALLOC_INC; i > 0; i--)
			{
                setTable->items[setTable->allocated - i] =
                    (void *)mlMalloc(sizeof(SetClassTableItem));
            }
        }
    }

    ((SetClassTableItem *)setTable->items[setTable->used])->name = name;
    ((SetClassTableItem *)setTable->items[setTable->used])->numProperties = numProperties;
    ((SetClassTableItem *)setTable->items[setTable->used])->headerFile = headerFile;
	((SetClassTableItem *)setTable->items[setTable->used])->package = package;

	setTable->used++;
}


void MleDppTblMgr::saveMediaRefClass(
  char *name,
  char *headerFile,
  char *package,
  TblMgrItems *mrefTable)
{
    int i;

    if ( mrefTable->used >= mrefTable->allocated )
	{
        if ( mrefTable->allocated == 0 )
		{
            mrefTable->allocated = ALLOC_INC;
            mrefTable->items =
                (void **)mlMalloc(mrefTable->allocated*sizeof(MediaRefClassTableItem *));

            for (i = 0; i < ALLOC_INC; i++)
			{
                mrefTable->items[i] =
                    (void *)mlMalloc(sizeof(MediaRefClassTableItem));
            }
        } else
		{
            mrefTable->allocated += ALLOC_INC;
            mrefTable->items = (void **)mlRealloc((void *)mrefTable->items,
                mrefTable->allocated*sizeof(MediaRefClassTableItem *));

            for (i = ALLOC_INC; i > 0; i--)
			{
                mrefTable->items[mrefTable->allocated - i] =
                    (void *)mlMalloc(sizeof(MediaRefClassTableItem));
            }
        }
    }
    ((MediaRefClassTableItem *)mrefTable->items[mrefTable->used])->name = name;
    ((MediaRefClassTableItem *)mrefTable->items[mrefTable->used])->headerFile = headerFile;
	((MediaRefClassTableItem *)mrefTable->items[mrefTable->used])->package = package;

    mrefTable->used++;
}


static int StrMapCompare( const void *s1, const void *s2 )
{
    return strcmp(*(char **)s1, *(char **)s2);
}


static int ActorClassCompare( const void *s1, const void *s2 )
{
    return strcmp((*(ActorClassTableItem **)s1)->name,
                  (*(ActorClassTableItem **)s2)->name);
}


static int RoleClassCompare( const void *s1, const void *s2 )
{
    return strcmp((*(RoleClassTableItem **)s1)->name,
		  (*(RoleClassTableItem **)s2)->name);
}


static int SetClassCompare( const void *s1, const void *s2 )
{
    return strcmp((*(SetClassTableItem **)s1)->name,
                  (*(SetClassTableItem **)s2)->name);
}


static int SetNameCompare( const void *s1, const void *s2 )
{
    return strcmp((*(SetTableItem **)s1)->name,(*(SetTableItem **)s2)->name);
}


static int GroupClassCompare( const void *s1, const void *s2 )
{
    return strcmp((*(GroupClassTableItem **)s1)->name,
		  (*(GroupClassTableItem **)s2)->name);
}


static int SceneClassCompare( const void *s1, const void *s2 )
{
    return strcmp((*(SceneClassTableItem **)s1)->name,
		  (*(SceneClassTableItem **)s2)->name);
}


static int MediaRefClassCompare( const void *s1, const void *s2 )
{
    return strcmp((*(MediaRefClassTableItem **)s1)->name,
		  (*(MediaRefClassTableItem **)s2)->name);
}


MlBoolean MleDppTblMgr::buildIndexTables(MleDwpItem *root)
{
    // Declare local variables.
    MleDwpFinder roleClassFinder;
    MleDwpFinder mediarefClassFinder;
    MleDwpRoleBinding **roleClassItems;
    MleDwpMediaRef **mediarefClassItems;
    char buf[128];
    int numItems,i,j;
    
    // PROCESS ACTOR CLASS and PROPERTY TABLES
    
    // Find all actors in the workprint.
    MleDwpFinder actorFinder(MleDwpActor::typeId,NULL,1);
    actorFinder.find(root);
    
    // Create a dictionary of actor classes.
    //   This dictionary is used to hold property dictionaries for each
    //   actor class.  That is, the keys to this dictionary are actor
    //   class names, and the value returned is another dictionary that
    //   indexes by property names (to property items).
    MleDwpStrKeyDict actorClassDict;
    
    // Loop over all the actors.
    //   What we are going to do is find all the properties used for each
    //   actor type.  We do this by looking at each actor instance and take
    //   the union of all the properties (checking to make sure that types
    //   are consistent).
    //
    //   The name of the actor type is found by searching for ActorTemplates
    //   until none exist.  The properties used in the ActorTemplates must
    //   also be added to the list.
    //
    //   The set of discriminators should already be set before calling this
    //   method. Do this by calling the setDiscriminators() member function.

    int nActors = actorFinder.getNumItems();
    MleDwpActor **actor = (MleDwpActor **)actorFinder.getItems();
    for ( i = 0; i < nActors; i++ )
    {
	  // Find the actor class, possibly looking through multiple templates.
	  MleDwpFinder tmplFinder(
				MleDwpActorTemplate::typeId,
				actor[i]->getActorClass());
	  MleDwpActorTemplate *tmpl = NULL;
	  while ( tmplFinder.find(root) )
	  {
		tmpl = (MleDwpActorTemplate *)tmplFinder.getItem();
		tmplFinder.setName(tmpl->getActorClass());
	  }
	  
	  // Find the property dictionary to use, based on actor class.
	  MleDwpStrKeyDict *propDict =
	    (MleDwpStrKeyDict *)actorClassDict.find(
						  tmpl ? tmpl->getActorClass() : actor[i]->getActorClass());
	  
	  // If there isn't one, create it and add it to the dictionary.
	  //   XXX These dictionaries are leaked.  This is sloppy, but
	  //   harmless.  Avoiding leaking these objects requires creating
	  //   a dictionary subclass that knows how to delete its values.
	  if ( propDict == NULL )
	  {
		propDict = new MleDwpStrKeyDict;
		actorClassDict.set(actor[i]->getActorClass(),propDict);
	  }
	  
	  // Look for properties and traverse to find the real class type
	  //   which may be hiding behind one or more templates.
	  MleDwpItem *propCont = actor[i];
	  const char *actorClassName = actor[i]->getActorClass();
	  do
	  {
		// Find all subordinate properties.
		MleDwpFinder propFinder(MleDwpProperty::typeId,NULL,1);
		propFinder.find(propCont);
		
		// Loop over all the properties.
		//   Add each one to the property dictionary.  The dictionary
		//   will only remember the last one entered under a given
		//   name, but that's okay.  We'll check to see that the
		//   types are consistent.
		int nProps = propFinder.getNumItems();
		MleDwpProperty **prop = (MleDwpProperty **)propFinder.getItems();
		for ( j = 0; j < nProps; j++ )
		{
		    // See if the dictionary already has one.
		    MleDwpProperty *currentProp =
			    (MleDwpProperty *)propDict->find(prop[j]->getName());
		      
		    if ( currentProp )
			{
			    // Verify the types match.
			    if ( currentProp->getDatatype() != prop[j]->getDatatype() )
			    {
				  // Complain.
				  //   XXX use real error reporting.
				  fprintf(stderr,"actor class %s property %s inconsistently used as %s and %s.\n",
					  actor[i]->getActorClass(),
					  currentProp->getName(),
					  currentProp->getDatatype()->getName(),
					  prop[j]->getDatatype()->getName());
				}
			}
		    else
			{
			    // Add the property to the dictionary.
			    propDict->set(prop[j]->getName(),prop[j]);
			}
		}
		
		// Look for a template.
		MleDwpFinder tmplFinder(MleDwpActorTemplate::typeId);
		propCont = (MleDwpItem *)tmplFinder.find(root);
	  } while ( propCont );
    }
    
    // Iterate through the actor class dictionary.
    MleDwpDictIter actorClassDictIter(actorClassDict);
    while ( actorClassDictIter.getKey() )
    {
	  MleDwpStrKeyDict *propDict =
	    (MleDwpStrKeyDict *)actorClassDictIter.getValue();
	  
	  // printf("actor class %s:\n",actorClassDictIter.getKey());
	  
	  // Iterate through the list of properties.
	  MleDwpDictIter propDictIter(*propDict);
	  int nProps = 0;
	  while ( propDictIter.getKey() )
	  {
		nProps++;
		
		buf[0] = '\0';
		strcat(buf,(char *)actorClassDictIter.getKey());
		strcat(buf,"_");
		strcat(buf,(char *)propDictIter.getKey());
		saveString(strdup(buf),&m_actorPropTable);
		
		//	    printf("\t%s (%s)\n",
		//		propDictIter.getKey(),
		//		((MleDwpProperty *)propDictIter.getValue())->getDatatype()->getName());
		
		propDictIter.next();
	  }
	  
      char *headerFile = NULL;
	  char *package = NULL;
	  
	  // Find the ActorDef item if it exists.
	  MleDwpFinder actorDefFinder(
				    MleDwpActorDef::typeId,(char *)actorClassDictIter.getKey());
	  MleDwpActorDef *actorDef = (MleDwpActorDef *)actorDefFinder.find(root);
	  
	  if ( actorDef == NULL )
	  {
        fprintf(stderr,"warning: ActorDef for %p not found.\n",
		  actorClassDictIter.getKey());
	  } else
	  {
		// Find the associated header file for this actor class.
		MleDwpFinder headerFileFinder(MleDwpHeaderFile::typeId);
		MleDwpHeaderFile *headerFileItem =
		  (MleDwpHeaderFile *)headerFileFinder.find(actorDef);
		
		if (headerFileItem)
		{
		  if (headerFile != NULL) mlFree(headerFile);
		  headerFile = strdup(headerFileItem->getName());
		} else
		  headerFile = NULL;

		// Find the associated package for this actor class.
		MleDwpFinder packageFinder(MleDwpPackage::typeId);
		MleDwpPackage *packageItem =
		  (MleDwpPackage *)packageFinder.find(actorDef);
		
		if (packageItem)
		{
		  if (package != NULL) mlFree(package);
		  package = strdup(packageItem->getName());
		} else
		  package = NULL;
	  }
	  
	  saveActorClass(strdup((char *)actorClassDictIter.getKey()),nProps,
			 headerFile,package,&m_actorClassTable);
	  
	  actorClassDictIter.next();
    }
    
    // RTH shouldn't need to uniquify these any more
    // MSM however, they still need to be alphabetized in alphanumeric order
	if (m_actorPropTable.used > 0)
	{
		qsort((void *)m_actorClassTable.items,m_actorClassTable.used,sizeof(char *),
			  ActorClassCompare);
		qsort((void *)m_actorPropTable.items,m_actorPropTable.used,
			  sizeof(char *),StrMapCompare);
    
		for ( i = 1, j = 0 ; i<m_actorClassTable.used ; i++ )
		{
		  if (strcmp(((ActorClassTableItem *)m_actorClassTable.items[i])->name,
					 ((ActorClassTableItem *)m_actorClassTable.items[j])->name) != 0)
		  {
			  m_actorClassTable.items[++j] = m_actorClassTable.items[i];
		  }
		}
		m_actorClassTable.used = j+1;
    
		for ( i = 1, j = 0 ; i < m_actorPropTable.used ; i++ )
		{
			if ( strcmp((char *)m_actorPropTable.items[i],
						(char *)m_actorPropTable.items[j]) != 0 ) {
				m_actorPropTable.items[++j] = m_actorPropTable.items[i];
			}
		}
		m_actorPropTable.used = j+1;
	}
    
    // PROCESS ROLE CLASS TABLE
    
    // Retrieve all role classes in the DWP.
    roleClassFinder.setType(MleDwpRoleBinding::typeId);
    roleClassFinder.setFindAll(1);
    roleClassFinder.find(root);
    roleClassItems = (MleDwpRoleBinding **)roleClassFinder.getItems();
    numItems = roleClassFinder.getNumItems();
    
    //
    // This error checking assumes that a Digital Workprint will
    // contain at least one role class.
    //
    if (roleClassItems == 0)
	{
	  m_roleClassTable.used = 0;
/*
        mlSetErrno(MLERR_MASTERLIB_NODELEGATECLASS);
        return FALSE;
*/
    } else
    {
	  
	  // Iterate over the MleDwpRoleBinding items, extracting class names
	  for (i = 0; i < numItems; i++)
	  {
	      const char *className = roleClassItems[i]->getName();

		  // Find the DeledageDef item if it exists.
	      MleDwpFinder roleDefFinder(
		      MleDwpRoleDef::typeId,(char *)className);
	      MleDwpRoleDef *roleDef = (MleDwpRoleDef *)roleDefFinder.find(root);

		  char *headerFile = NULL;
		  char *package = NULL;
		  
		  if ( roleDef == NULL )
	          fprintf(stderr,"warning: RoleDef for %s not found.\n", className);
	      else
		  {
			  // Find the associated header file for this role class.
			  MleDwpFinder headerFileFinder(MleDwpHeaderFile::typeId);
			  MleDwpHeaderFile *headerFileItem =
				  (MleDwpHeaderFile *)headerFileFinder.find(roleDef);
			  
			  if (headerFileItem)
				  headerFile = strdup(headerFileItem->getName());
			  else
				  headerFile = NULL;

			  // Find the associated package for this role class.
			  MleDwpFinder packageFinder(MleDwpPackage::typeId);
			  MleDwpPackage *packageItem =
				  (MleDwpPackage *)packageFinder.find(roleDef);
			
			  if (packageItem)
				  package = strdup(packageItem->getName());
			  else
				  package = NULL;
			  
			  // Skip NULL bindings.
			  //   Sometimes a NULL binding may be specified in order to
			  //   have MleSet::currentSet available to an actor.  In
			  //   this case, we need to avoid declaring a creation function.
			  if ( className && strcmp(className,"NULL") != 0 )
				  saveRoleClass((char *)className,headerFile,package,&m_roleClassTable);
		  }
	  }
	  
	  qsort((void *)m_roleClassTable.items,m_roleClassTable.used,
		sizeof(char *),RoleClassCompare);
	  
	  for ( i = 1, j = 0 ; i < m_roleClassTable.used ; i++ )
	  {
	      if ( strcmp(((RoleClassTableItem *)m_roleClassTable.items[i])->name,
			  ((RoleClassTableItem *)m_roleClassTable.items[j])->name) != 0 )
		  {
		      m_roleClassTable.items[++j] = m_roleClassTable.items[i];
	      }
	  }
	  m_roleClassTable.used = j+1;
	  
    }
    
    // PROCESS SET TYPE TABLE
    
    // Find all sets in the workprint.
    MleDwpFinder setFinder(MleDwpSet::typeId,NULL,1);
    setFinder.find(root);
    
    // Create a dictionary of set classes.
    //   This dictionary is used to hold property dictionaries for each
    //   set class.  That is, the keys to this dictionary are set
    //   class names, and the value returned is another dictionary that
    //   indexes by property names (to property items).
    MleDwpStrKeyDict setClassDict;
    
    // Loop over all the sets.
    //   What we are going to do is find all the properties used for each
    //   set type.  We do this by looking at each set instance and
    //   take the union of all the properties (checking to make sure that
    //   types are consistent).
    int nSets = setFinder.getNumItems();
    MleDwpSet **set = (MleDwpSet **)setFinder.getItems();
    for (i = 0; i < nSets; i++)
    {
	  saveSet((char *)set[i]->getName(),(char *)set[i]->getType(),
	      &m_setTable);
	  
	  // Find the property dictionary to use, based on set type.
	  MleDwpStrKeyDict *propDict =
	    (MleDwpStrKeyDict *)setClassDict.find(set[i]->getType());
	  
	  // If there isn't one, create it and add it to the dictionary.
	  //   XXX These dictionaries are leaked.  This is sloppy, but
	  //   harmless.  Avoiding leaking these objects requires creating
	  //   a dictionary subclass that knows how to delete its values.
	  if ( propDict == NULL )
	  {
		propDict = new MleDwpStrKeyDict;
		setClassDict.set(set[i]->getType(),propDict);
	  }
	  
	  // Look for properties.
	  MleDwpFinder propFinder(MleDwpProperty::typeId,NULL,1);
	  propFinder.find(set[i]);
	  
	  // Loop over all the properties.
	  //   Add each one to the property dictionary.  The dictionary
	  //   will only remember the last one entered under a given
	  //   name, but that's okay.  We'll check to see that the
	  //   types are consistent.
	  int nProps = propFinder.getNumItems();
	  MleDwpProperty **prop = (MleDwpProperty **)propFinder.getItems();
	  for ( j = 0; j < nProps; j++ )
	  {
		// See if the dictionary already has one.
		MleDwpProperty *currentProp =
		  (MleDwpProperty *)propDict->find(prop[j]->getName());
		
		if ( currentProp )
		{
		      // Verify the types match.
		      if ( currentProp->getDatatype() != prop[j]->getDatatype() )
			{
			    // Complain.
			    //   XXX use real error reporting.
			    fprintf(stderr,"set class %s property %s inconsistently used as %s and %s.\n",
				    set[i]->getType(),
				    currentProp->getName(),
				    currentProp->getDatatype()->getName(),
				    prop[j]->getDatatype()->getName());
			}
		}
		else
		{
		    // Add the property to the dictionary.
		    propDict->set(prop[j]->getName(),prop[j]);
		}
	  }
    }
    
    if ( m_setTable.used > 0 )
	{
		m_setTable.used = mlUnique((void *)m_setTable.items, m_setTable.used,
			sizeof(SetTableItem *), SetNameCompare);
	
#if 0
		// Don't sort the set. The order of the set in the DWP is important.
		qsort((void *)setTable.items,m_setTable.used,sizeof(SetTableItem *),
			  SetNameCompare);
		
		for ( i = 1, j = 0 ; i < m_setTable.used ; i++ )
		{
			if ( strcmp(((SetTableItem *)m_setTable.items[i])->name,
				((SetTableItem *)m_setTable.items[j])->name) != 0 )
			{
			  m_setTable.items[++j] = m_setTable.items[i];
			}
		}
		m_setTable.used = j+1;
#endif /* 0 */
    }
    
    // Iterate through the set class dictionary.
    MleDwpDictIter setClassDictIter(setClassDict);
    while ( setClassDictIter.getKey() )
    {
	  // Get the property dictionary.
	  MleDwpStrKeyDict *propDict =
	    (MleDwpStrKeyDict *)setClassDictIter.getValue();
	  
	  // Iterate through the list of properties.
	  MleDwpDictIter propDictIter(*propDict);
	  int nProps = 0;
	  while ( propDictIter.getKey() )
	  {
		nProps++;
		
		buf[0] = '\0';
		strcat(buf,(char *)setClassDictIter.getKey());
		strcat(buf,"_");
		strcat(buf,(char *)propDictIter.getKey());
		saveString(strdup(buf),&m_setPropTable);
		
		propDictIter.next();
	  }
	  
	  // Find the SetDef item if it exists.
	  MleDwpFinder setDefFinder(
          MleDwpSetDef::typeId,(char *)setClassDictIter.getKey());
	  MleDwpSetDef *setDef = (MleDwpSetDef *)setDefFinder.find(root);
	  
	  char *headerFile = NULL;
	  char *package = NULL;
	  
	  if ( setDef == NULL )
	    fprintf(stderr,"warning: SetDef for %p not found.\n",
		    setClassDictIter.getKey());
	  else
	  {
		// Find the associated header file for this set class.
		MleDwpFinder headerFileFinder(MleDwpHeaderFile::typeId);
		headerFileFinder.find(setDef);
		MleDwpHeaderFile *headerFileItem =
		  (MleDwpHeaderFile *) headerFileFinder.getItem();
		
		if (headerFileItem)
		  headerFile = strdup(headerFileItem->getName());

		// Find the associated package for this actor class.
		MleDwpFinder packageFinder(MleDwpPackage::typeId);
		MleDwpPackage *packageItem =
		  (MleDwpPackage *)packageFinder.find(setDef);
		
		if (packageItem)
		  package = strdup(packageItem->getName());
		else
		  package = NULL;
	  }
	  
	  saveSetClass(strdup((char *)setClassDictIter.getKey()), nProps,
	      headerFile,package,&m_setClassTable);
	  
	  setClassDictIter.next();
    }
    
    if (m_setPropTable.used > 0)
	{
        qsort((void *)m_setClassTable.items,m_setClassTable.used,
              sizeof(char *),SetClassCompare);
        qsort((void *)m_setPropTable.items,m_setPropTable.used,
              sizeof(char *),StrMapCompare);
	
        for ( i = 1, j = 0 ; i<m_setClassTable.used ; i++ )
		{
            if (strcmp(((SetClassTableItem *)m_setClassTable.items[i])->name,
		               ((SetClassTableItem *)m_setClassTable.items[j])->name) != 0)
			{
		        m_setClassTable.items[++j] = m_setClassTable.items[i];
			}
        }
        m_setClassTable.used = j+1;
	
        for ( i = 1, j = 0 ; i < m_setPropTable.used ; i++ )
		{
            if ( strcmp((char *)m_setPropTable.items[i],
                        (char *)m_setPropTable.items[j]) != 0 ) {
                m_setPropTable.items[++j] = m_setPropTable.items[i];
            }
        }
        m_setPropTable.used = j+1;
    }
    
    // PROCESS MEDIAREF CLASS TABLE
    
    // Retrieve all mediaref classes in the DWP.
    mediarefClassFinder.setType(MleDwpMediaRef::typeId);
    mediarefClassFinder.setFindAll(1);
    mediarefClassFinder.find(root);
    mediarefClassItems = (MleDwpMediaRef **)mediarefClassFinder.getItems();
    numItems = mediarefClassFinder.getNumItems();
    
    // Iterate over the MleDwpMediaRef items, extracting class names.
    for (i = 0; i < numItems; i++)
	{
		char *headerFile = NULL;
		char *package = NULL;
		  
		// Find the associated header file for this role class.
		MleDwpFinder headerFileFinder(MleDwpHeaderFile::typeId);
		MleDwpHeaderFile *headerFileItem =
			(MleDwpHeaderFile *)headerFileFinder.find(mediarefClassItems[i]);
			  
		if (headerFileItem)
			headerFile = strdup(headerFileItem->getName());
		else
			headerFile = NULL;

		// Find the associated package for this role class.
		MleDwpFinder packageFinder(MleDwpPackage::typeId);
		MleDwpPackage *packageItem =
			(MleDwpPackage *)packageFinder.find(mediarefClassItems[i]);
			
		if (packageItem)
			package = strdup(packageItem->getName());
		else
			package = NULL;
		
		const char *className = mediarefClassItems[i]->getMediaRefClass();
		saveMediaRefClass((char *)className,headerFile,package,&m_mediarefTable);
    }
    
    if (m_mediarefTable.used > 0)
	{
        qsort((void *)m_mediarefTable.items,m_mediarefTable.used,
              sizeof(char *),MediaRefClassCompare);
	
        for ( i = 1, j = 0 ; i < m_mediarefTable.used ; i++ )
		{
            if ( strcmp(((MediaRefClassTableItem *)m_mediarefTable.items[i])->name,
                        ((MediaRefClassTableItem *)m_mediarefTable.items[j])->name) != 0 )
			{
				m_mediarefTable.items[++j] = m_mediarefTable.items[i];
            }
        }
        m_mediarefTable.used = j+1;
    }
    
    // PROCESS GROUP TYPE TABLE
    
    // find all groups in the workprint
    MleDwpFinder groupFinder(MleDwpGroup::typeId,NULL,1);
    groupFinder.find(root);
    
    // Create a dictionary of group classes.
    //   This dictionary is used to hold property dictionaries for each
    //   actor class.  That is, the keys to this dictionary are actor
    //   class names, and the value returned is another dictionary that
    //   indexes by property names (to property items).
    MleDwpStrKeyDict groupClassDict;
    
    // Loop over all the groups.
    //   What we are going to do is find all the properties used for each
    //   group type.  We do this by looking at each group instance and
    //   take the union of all the properties (checking to make sure that
    //   types are consistent).
    int nGroups = groupFinder.getNumItems();
    MleDwpGroup **group = (MleDwpGroup **)groupFinder.getItems();
    for (i = 0; i < nGroups; i++)
    {
	  char *headerFile = NULL;
	  char *package = NULL;
	  
	  // Find the associated header file for this group class.
	  MleDwpFinder headerFileFinder(MleDwpHeaderFile::typeId);
	  MleDwpHeaderFile *headerFileItem =
	    (MleDwpHeaderFile *)headerFileFinder.find(group[i]);
	  
	  if (headerFileItem)
	    headerFile = strdup(headerFileItem->getName());
	  else
	    headerFile = NULL;

		// Find the associated package for this actor class.
		MleDwpFinder packageFinder(MleDwpPackage::typeId);
		MleDwpPackage *packageItem =
		  (MleDwpPackage *)packageFinder.find(group[i]);
		
		if (packageItem)
		  package = strdup(packageItem->getName());
		else
		  package = NULL;
	  
	  // Save the class and the uniquely named instance for this group
	  saveGroupClass((char *)group[i]->getGroupClass(),headerFile,package,&m_groupClassTable);
	  saveGroupInstance((char *)group[i]->getName(),&m_groupInstanceTable);
    }
    
    // Uniqify the class names.  The instance names are already expected to
    // be unique, so don\'t have to sort|unique them.
    //
	if (m_groupClassTable.used > 0)
	{
		qsort((void *)m_groupClassTable.items,m_groupClassTable.used,sizeof(GroupClassTableItem *),
			  GroupClassCompare);
    
		for ( i = 1, j = 0 ; i < m_groupClassTable.used ; i++ )
		{
			if ( strcmp(((GroupClassTableItem *)m_groupClassTable.items[i])->name,
						((GroupClassTableItem *)m_groupClassTable.items[j])->name) != 0 )
		  {
			  m_groupClassTable.items[++j] = m_groupClassTable.items[i];
		  }
		}
		m_groupClassTable.used = j+1;
	}
    
    // PROCESS SCENE TYPE TABLE
    
    // Find all scenes in the workprint.
    MleDwpFinder sceneFinder(MleDwpScene::typeId,NULL,1);
    sceneFinder.find(root);
    
    // Create a dictionary of scene classes.
    //   This dictionary is used to hold property dictionaries for each
    //   actor class.  That is, the keys to this dictionary are actor
    //   class names, and the value returned is another dictionary that
    //   indexes by property names (to property items).
    MleDwpStrKeyDict sceneClassDict;
    
    // Loop over all the scenes.
    //   What we are going to do is find all the properties used for each
    //   scene type.  We do this by looking at each scene instance and
    //   take the union of all the properties (checking to make sure that
    //   types are consistent).
    int nScenes = sceneFinder.getNumItems();
    MleDwpScene **scene = (MleDwpScene **)sceneFinder.getItems();
    for (i = 0; i < nScenes; i++)
    {
	  char *headerFile = NULL;
	  char *package = NULL;
	  
	  // Find the associated header file for this scene class.
	  MleDwpFinder headerFileFinder(MleDwpHeaderFile::typeId);
	  MleDwpHeaderFile *headerFileItem =
	    (MleDwpHeaderFile *)headerFileFinder.find(scene[i]);
	  
	  if (headerFileItem)
	    headerFile = strdup(headerFileItem->getName());
	  else
	    headerFile = NULL;

		// Find the associated package for this actor class.
		MleDwpFinder packageFinder(MleDwpPackage::typeId);
		MleDwpPackage *packageItem =
		  (MleDwpPackage *)packageFinder.find(scene[i]);
		
		if (packageItem)
		  package = strdup(packageItem->getName());
		else
		  package = NULL;
	  
	  saveSceneClass((char *)scene[i]->getSceneClass(),headerFile,package,&m_sceneClassTable);
    }
    
	if (m_sceneClassTable.used > 0)
	{
		qsort((void *)m_sceneClassTable.items,m_sceneClassTable.used,sizeof(SceneClassTableItem *),
			  SceneClassCompare);
    
		for ( i = 1, j = 0 ; i < m_sceneClassTable.used ; i++ )
		{
			if ( strcmp(((SceneClassTableItem *)m_sceneClassTable.items[i])->name,
						((SceneClassTableItem *)m_sceneClassTable.items[j])->name) != 0 )
			{
				m_sceneClassTable.items[++j] = m_sceneClassTable.items[i];
			}
		}
		m_sceneClassTable.used = j+1;
	}

    return TRUE;
}


/*
 * This function may be used during the mastering process to retrieve
 * the mastered index value for a known string.
 */
int MleDppTblMgr::mapStringToIndex(
	const char *name,
	TblMgrItems *strTable)
{
    /* Declare local variables. */
    int i;

    for (i = 0; i < strTable->used; i++)
	{
        if (! strcmp((char *)strTable->items[i],name))
            return i;
    }
    return MLE_DPP_BADTBL_INDEX;
}



/*
 * This function may be used during the mastering process to retrieve
 * the mastered index value for a known actor class.
 */
int MleDppTblMgr::mapActorClassToIndex(
	const char *name,
	TblMgrItems *actorTable)
{
    /* Declare local variables. */
    int i;

    for (i = 0; i < actorTable->used; i++) {
        if (! strcmp(((ActorClassTableItem *)actorTable->items[i])->name,name))
            return i;
    }
    return MLE_DPP_BADTBL_INDEX;
}


/*
 * This routine is used to access the MleRTActorClass table.
 */
int MleDppTblMgr::actorClassRuntimeName(const char *actorClass)
{
    /* Declare local variables. */
    int index;

    index = mapActorClassToIndex(actorClass,&m_actorClassTable);
    if (index == MLE_DPP_BADTBL_INDEX)
        mlSetErrno(MLE_E_MASTERLIB_BADTBLINDEX);
    return index;
}


/*
 * This function may be used during the mastering process to retrieve
 * the mastered index value for a known group class.
 */
int MleDppTblMgr::mapGroupClassToIndex(
	const char *name,
	TblMgrItems *groupTable)
{
    /* Declare local variables. */
    int i;

    for (i = 0; i < groupTable->used; i++)
	{
        if (! strcmp(((GroupClassTableItem *)groupTable->items[i])->name,name))
            return i;
    }
    return MLE_DPP_BADTBL_INDEX;
}


/*
 * This function may be used during the mastering process to retrieve
 * the mastered index value for a known group instance.
 */
int MleDppTblMgr::mapGroupInstanceToIndex(
	const char *name,
	TblMgrItems *groupTable)
{
    /* Declare local variables. */
    int i;

    for (i = 0; i < groupTable->used; i++)
	{
        if (! strcmp(((GroupInstanceTableItem *)groupTable->items[i])->name,name))
            return i;
    }
    return MLE_DPP_BADTBL_INDEX;
}


/*
 * This routine is used to access the MleRTGroupClass table.
 */
int MleDppTblMgr::groupClassRuntimeName(const char *groupClass)
{
    /* Declare local variables. */
    int index;

    index = mapGroupClassToIndex(groupClass,&m_groupClassTable);
    if (index == MLE_DPP_BADTBL_INDEX)
        mlSetErrno(MLE_E_MASTERLIB_BADTBLINDEX);
    return index;
}


/*
 * This routine is used to access the MleRTGroupInstance table.
 */
int MleDppTblMgr::groupInstanceRuntimeName(const char *groupInstance)
{
    /* Declare local variables. */
    int index;

    index = mapGroupInstanceToIndex(groupInstance,&m_groupInstanceTable);
    if (index == MLE_DPP_BADTBL_INDEX)
        mlSetErrno(MLE_E_MASTERLIB_BADTBLINDEX);
    return index;
}


/*
 * This function may be used during the mastering process to retrieve
 * the mastered index value for a known scene class.
 */
int MleDppTblMgr::mapSceneClassToIndex(
	const char *name,
	TblMgrItems *sceneTable)
{
    /* Declare local variables. */
    int i;

    for (i = 0; i < sceneTable->used; i++)
	{
        if (! strcmp(((SceneClassTableItem *)sceneTable->items[i])->name,name))
            return i;
    }
    return MLE_DPP_BADTBL_INDEX;
}


/*
 * This routine is used to access the MleRTSceneClass table.
 */
int MleDppTblMgr::sceneClassRuntimeName(const char *sceneClass)
{
    /* Declare local variables. */
    int index;

    index = mapSceneClassToIndex(sceneClass,&m_sceneClassTable);
    if (index == MLE_DPP_BADTBL_INDEX)
        mlSetErrno(MLE_E_MASTERLIB_BADTBLINDEX);
    return index;
}


/*
 * This function may be used during the mastering process to retrieve
 * the mastered index value for a known role class.
 */
int MleDppTblMgr::mapRoleClassToIndex(
	const char *name,
	TblMgrItems *roleTable)
{
    /* Declare local variables. */
    int i;

    for (i = 0; i < roleTable->used; i++)
	{
        if (! strcmp(((RoleClassTableItem *)roleTable->items[i])->name,name))
            return i;
    }
    return MLE_DPP_BADTBL_INDEX;
}

/*
 * This routine is used to access the MleRTRoleClass table.
 */
#if 0
/*** OBSOLETE ***/
int MleDppTblMgr::delegateClassRuntimeName(const char *delegateClass)
{
    /* Declare local variables. */
    int index;

    index = mapDelegateClassToIndex(delegateClass,&m_roleClassTable);
    if (index == MLE_DPP_BADTBL_INDEX)
        mlSetErrno(MLE_E_MASTERLIB_BADTBLINDEX);
    return index;
}
#endif /* 0 */

int MleDppTblMgr::roleClassRuntimeName(const char *roleClass)
{
    /* Declare local variables. */
    int index;

    index = mapRoleClassToIndex(roleClass,&m_roleClassTable);
    if (index == MLE_DPP_BADTBL_INDEX)
        mlSetErrno(MLE_E_MASTERLIB_BADTBLINDEX);
    return index;
}

/*
 * This routine is used to access the MleRTPropertyDesc table.
 */
int MleDppTblMgr::actorPropRuntimeName(
	const char *actorClass,
	const char *propertyName)
{
    /* Declare local variables. */
    int index;
    char *buf;

    /* Build name. */
    buf = (char *)mlMalloc(strlen(actorClass)+1+strlen(propertyName)+1);
    strcpy(buf,actorClass);
    strcat(buf,"_");
    strcat(buf,propertyName);

    index = mapStringToIndex(buf,&m_actorPropTable);
    mlFree(buf);
    if (index == MLE_DPP_BADTBL_INDEX)
        mlSetErrno(MLE_E_MASTERLIB_BADTBLINDEX);
    return index;
}


/*
 * This function may be used during the mastering process to retrieve
 * the mastered index value for a known set.
 */
int MleDppTblMgr::mapSetToIndex(
	const char *name,
	TblMgrItems *setTable)
{
    /* Declare local variables. */
    int i;

    for (i = 0; i < setTable->used; i++)
	{
        if (! strcmp(((SetTableItem *)setTable->items[i])->name,name))
            return i;
    }
    return MLE_DPP_BADTBL_INDEX;
}


/*
 * This function may be used during the mastering process to retrieve
 * the mastered index value for a known set.
 */
int MleDppTblMgr::mapSetClassToIndex(
	const char *name,
	TblMgrItems *setTable)
{
    /* Declare local variables. */
    int i;

    for (i = 0; i < setTable->used; i++)
	{
        if (! strcmp(((SetClassTableItem *)setTable->items[i])->name,name))
            return i;
    }
    return MLE_DPP_BADTBL_INDEX;
}


/*
 * This routine is used to access the MleRTSetDesc table.
 */
#if 0
/*** OBSOLETE ***/
int MleDppTblMgr::forumRuntimeName(const char *forumName)
{
    /* Declare local variables. */
    int index;

    index = mapForumToIndex(forumName,&m_setTable);
    if (index == MLE_DPP_BADTBL_INDEX)
        mlSetErrno(MLE_E_MASTERLIB_BADTBLINDEX);
    return index;
}
#endif /* 0 */

int MleDppTblMgr::setRuntimeName(const char *setName)
{
    /* Declare local variables. */
    int index;

    index = mapSetToIndex(setName,&m_setTable);
    if (index == MLE_DPP_BADTBL_INDEX)
        mlSetErrno(MLE_E_MASTERLIB_BADTBLINDEX);
    return index;
}

/*
 * This routine is used to access the MleRTSetDesc table.
 */
#if 0
/*** OBSOLETE ***/
int MleDppTblMgr::forumClassRuntimeName(const char *forumName)
{
    /* Declare local variables. */
    int index;

    index = mapForumClassToIndex(forumName,&m_setClassTable);
    if (index == MLE_DPP_BADTBL_INDEX)
        mlSetErrno(MLE_E_MASTERLIB_BADTBLINDEX);
    return index;
}
#endif /* 0 */

int MleDppTblMgr::setClassRuntimeName(const char *setName)
{
    /* Declare local variables. */
    int index;

    index = mapSetClassToIndex(setName,&m_setClassTable);
    if (index == MLE_DPP_BADTBL_INDEX)
        mlSetErrno(MLE_E_MASTERLIB_BADTBLINDEX);
    return index;
}


/*
 * This routine is used to access the MleRTSetProperties table.
 */
#if 0
/*** OBSOLETE ***/
int MleDppTblMgr::forumPropRuntimeName(
	const char *forumClass,
	const char *propertyName)
{
    /* Declare local variables. */
    int index;
    char *buf;

    /* Build name. */
    buf = (char *)mlMalloc(strlen(forumClass)+1+strlen(propertyName)+1);
    strcpy(buf,forumClass);
    strcat(buf,"_");
    strcat(buf,propertyName);

    index = mapStringToIndex(buf,&m_setPropTable);
    mlFree(buf);
    if (index == MLE_DPP_BADTBL_INDEX)
        mlSetErrno(MLE_E_MASTERLIB_BADTBLINDEX);
    return index;
}
#endif /* 0 */

int MleDppTblMgr::setPropRuntimeName(
	const char *setClass,
	const char *propertyName)
{
    /* Declare local variables. */
    int index;
    char *buf;

    /* Build name. */
    buf = (char *)mlMalloc(strlen(setClass)+1+strlen(propertyName)+1);
    strcpy(buf,setClass);
    strcat(buf,"_");
    strcat(buf,propertyName);

    index = mapStringToIndex(buf,&m_setPropTable);
    mlFree(buf);
    if (index == MLE_DPP_BADTBL_INDEX)
        mlSetErrno(MLE_E_MASTERLIB_BADTBLINDEX);
    return index;

}

/*
 * This function may be used during the mastering process to retrieve
 * the mastered index value for a known media reference class.
 */
int MleDppTblMgr::mapMediaRefClassToIndex(
	const char *name,
	TblMgrItems *mrefTable)
{
    /* Declare local variables. */
    int i;

    for (i = 0; i < mrefTable->used; i++)
	{
        if (! strcmp(((MediaRefClassTableItem *)mrefTable->items[i])->name,name))
            return i;
    }
    return MLE_DPP_BADTBL_INDEX;
}

/*
 * This routine is used to access the MleRTMediaRef table.
 */
int MleDppTblMgr::mediarefClassRuntimeName(const char *mediarefClass)
{
    /* Declare local variables */
    int index;

    index = mapMediaRefClassToIndex(mediarefClass,&m_mediarefTable);
    if (index == MLE_DPP_BADTBL_INDEX)
        mlSetErrno(MLE_E_MASTERLIB_BADTBLINDEX);
    return index;
}


/*
 * This routine is used to set the Digital Workprint discriminators.
 * It expects a ':' deliminated string of tags to set multiple
 * discriminators. For example:
 *
 * tags = "brender:win32"
 */
MlBoolean MleDppTblMgr::setDiscriminators(MleDwpItem *tree, char *tags)
{
    /* Declare local variables. */
    char *processTags;
    char *tag,*nextTag;
    MleDwpTagAllDiscriminator *newDiscriminator = new MleDwpTagAllDiscriminator;

    if (! tags)
	{
        if (m_discriminator)
		{
            tree->setDiscriminator(NULL);
            delete m_discriminator;
        }
        return TRUE;
    } else
	{
        processTags = strdup(tags);
    }

    // Remove old discriminator.
    if (m_discriminator)
	{
        tree->setDiscriminator(NULL);
        delete m_discriminator;
    }

    tag = processTags;
    while (tag) {
        nextTag = strchr(tag,':');
        if (nextTag) *nextTag++ = '\0';
        newDiscriminator->addTag(tag);
        tag = nextTag;
    }

    tree->setDiscriminator(newDiscriminator);
    m_discriminator = newDiscriminator;

    mlFree(processTags);
    return TRUE;
}

void *
MleDppTblMgr::operator new(size_t tSize)
{
	void *p = mlMalloc(tSize);
	return p;
}

void
MleDppTblMgr::operator delete(void *p)
{
	mlFree(p);
}
