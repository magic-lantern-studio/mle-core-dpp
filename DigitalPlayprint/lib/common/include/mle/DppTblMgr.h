/** @defgroup MleDPPBase Magic Lantern Digital Playprint Library API - Base */

/**
 * @file DppActorGroup.h
 * @ingroup MleDPPBase
 *
 * This file defines the int Actor Group Output used by the Magic Lantern Digital
 * Playprint Library API.
 *
 * @author Mark S. Millard
 * @date July 17, 2004
 */

// COPYRIGHT_BEGIN
//
// The MIT License (MIT)
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
//  For information concerning this header file, contact Mark S. Millard,
//  of Wizzer Works at msm@wizzerworks.com.
//
//  More information concerning Wizzer Works may be found at
//
//      http://www.wizzerworks.com
//
// COPYRIGHT_END

#ifndef __MLE_DPP_TBLMGR_H_
#define __MLE_DPP_TBLMGR_H_

// Include Magic Lantern header files
#include "mle/mlTypes.h"
#include "mle/DwpTagAllDiscriminator.h"


class MleDwpItem;

// Support for building tables (i.e. strings, structs, ...).
typedef struct 
{
    int used;
    int allocated;
    void **items;
} TblMgrItems;

typedef struct {
    char *name;
    int numProperties;
    char *headerFile;
	char *package;
} ActorClassTableItem;

typedef struct {
    char *name;
    char *headerFile;
	char *package;
} RoleClassTableItem;

typedef struct {
    char *name;
    char *type;
} SetTableItem;

typedef struct {
    char *name;
    int numProperties;
    char *headerFile;
	char *package;
} SetClassTableItem;

typedef struct {
    char *name;
    char *headerFile;
	char *package;
} GroupClassTableItem;

typedef struct {
    char *name;
} GroupInstanceTableItem;

typedef struct {
    char *name;
    char *headerFile;
	char *package;
} SceneClassTableItem;

typedef struct {
    char *name;
    char *headerFile;
	char *package;
} MediaRefClassTableItem;

class MleDppTblMgr
{
  // Declare member variables.
  protected:

    TblMgrItems m_actorClassTable;
    TblMgrItems m_roleClassTable;
    TblMgrItems m_setClassTable;
    TblMgrItems m_actorPropTable;
    TblMgrItems m_setPropTable;
    TblMgrItems m_setTable;
    TblMgrItems m_groupClassTable;
    TblMgrItems m_groupInstanceTable;
    TblMgrItems m_sceneClassTable;
    TblMgrItems m_mediarefTable;
    
  private:

    MleDwpTagAllDiscriminator *m_discriminator;
    
  // Declare member functions.
  public:

    MleDppTblMgr(void);

    virtual ~MleDppTblMgr(void);
    
    MlBoolean setDiscriminators(MleDwpItem *tree, char *tags);

    MlBoolean buildIndexTables(MleDwpItem *root);
    
    TblMgrItems *getActorClassTable(void) { return(&m_actorClassTable); };

    TblMgrItems *getRoleClassTable(void) { return(&m_roleClassTable); };

    TblMgrItems *getSetClassTable(void) { return(&m_setClassTable); };

    TblMgrItems *getActorPropTable(void) { return(&m_actorPropTable); };

    TblMgrItems *getSetPropTable(void) { return(&m_setPropTable); };

    TblMgrItems *getSetTable(void) { return(&m_setTable); };

    TblMgrItems *getGroupClassTable(void) { return(&m_groupClassTable); };

    TblMgrItems *getGroupInstanceTable(void) { return(&m_groupInstanceTable); };

    TblMgrItems *getSceneClassTable(void) { return(&m_sceneClassTable); };

    TblMgrItems *getMediaRefTable(void) { return(&m_mediarefTable); };

    int getActorClassTableSize(void) { return(m_actorClassTable.used); };

    int getRoleClassTableSize(void) { return(m_roleClassTable.used); };

    int getSetClassTableSize(void) { return(m_setClassTable.used); }

    int getActorPropTableSize(void) { return(m_actorPropTable.used); };

    int getSetPropTableSize(void) { return(m_setPropTable.used); };

    int getSetTableSize(void) { return(m_setTable.used); };

    int getGroupClassTableSize(void) { return(m_groupClassTable.used); };

    int getGroupInstanceTableSize(void) { return(m_groupInstanceTable.used); };

    int getSceneClassTableSize(void) { return(m_sceneClassTable.used); };

    int getMediaRefTableSize(void) { return(m_mediarefTable.used); };
    
	/**
	 * Get the Actor Class runtime name for the specified class.
	 *
	 * @param actorClass The name of the Actor Class.
	 *
	 * @return The Runtime name for the Actor Class is returned as
	 * an index. The index is an unique value used to identify the
	 * Actor Class in the global namespace table.
	 */
    int actorClassRuntimeName(const char *actorClass);

    //int delegateClassRuntimeName(const char *delegateClass); // DEPRECATED

	int roleClassRuntimeName(const char *roleClass);

    //int forumClassRuntimeName(const char *forumClass); // DEPRECATED

	int setClassRuntimeName(const char *setClass);

    int actorPropRuntimeName(const char *actorClass,const char *propertyName);

    //int forumPropRuntimeName(const char *forumClass,const char *propertyName); // DEPRECATED

	int setPropRuntimeName(const char *setClass,const char *propertyName);

    //int forumRuntimeName(const char *forumName); // DEPRECATED

	int setRuntimeName(const char *setName);

    int groupClassRuntimeName(const char *groupClass);

    int groupInstanceRuntimeName(const char *groupInstance);

    int sceneClassRuntimeName(const char *sceneClass);

    int mediarefClassRuntimeName(const char *mediarefClass);

	/**
	 * Override operator new.
	 *
	 * @param tSize The size, in bytes, to allocate.
	 */
	void* operator new(size_t tSize);

	/**
	 * Override operator delete.
	 *
	 * @param p A pointer to the memory to delete.
	 */
    void  operator delete(void *p);
    
  private:

    void saveString(const char *pstr,TblMgrItems *strTable);

    void saveActorClass(char *name,int numProperties,char *headerFile,
			char *package,TblMgrItems *actorTable);

	void saveRoleClass(char *name,char *headerFile,
			char *package,TblMgrItems *roleTable);

    void saveSetClass(char *name,int numProperties,char *headerFile,
			char *package,TblMgrItems *setTable);

    void saveSet(char *name,char *type,TblMgrItems *setTable);

    void saveGroupClass(char *name,char *headerFile,
			char *package,TblMgrItems *groupTable);

    void saveGroupInstance(char *name,TblMgrItems *groupTable);

    void saveSceneClass(char *name,char *headerFile,
			char *package,TblMgrItems *sceneTable);

	void saveMediaRefClass(char *name,char *headerFile,
			char *package,TblMgrItems *mrefTable);

    int mapStringToIndex(const char *name,TblMgrItems *strTable);

    int mapActorClassToIndex(const char *name,TblMgrItems *actorTable);

	int mapRoleClassToIndex(const char *name,TblMgrItems *roleTable);

    int mapSetClassToIndex(const char *name,TblMgrItems *setTable);

    int mapSetToIndex(const char *name,TblMgrItems *setTable);

    int mapGroupClassToIndex(const char *name,TblMgrItems *groupTable);

    int mapGroupInstanceToIndex(const char *name,TblMgrItems *groupTable);

    int mapSceneClassToIndex(const char *name,TblMgrItems *sceneTable);

	int mapMediaRefClassToIndex(const char *name,TblMgrItems *mrefTable);
};


/** A flag indicating a bad table index. */
#define MLE_DPP_BADTBL_INDEX -1


#endif /* __MLE_DPP_TBLMGR_H_ */
