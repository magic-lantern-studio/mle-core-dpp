/** @defgroup MleDPPModel Magic Lantern Digital Playprint Library API - Model */

/**
 * @file DppGroup.h
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
//  For information concerning this header file, contact Mark S. Millard,
//  of Wizzer Works at msm@wizzerworks.com.
//
//  More information concerning Wizzer Works may be found at
//
//      http://www.wizzerworks.com
//
// COPYRIGHT_END

#ifndef __MLE_DPP_GROUP_H_
#define __MLE_DPP_GROUP_H_

//Include Digital Workprint header files.
#include "mle/DwpGroup.h"

//Include Digital Playprint header files.
#include "mle/DppItem.h"
#include "mle/DppActorGroupOutput.h"

/**
 */
class MleDppGroup : public MleDwpGroup, public MleDppItem
{
	MLE_DWP_HEADER(MleDppGroup);

  public:

	/**
	 * Set a reasonable default value for the runtime class type of the group.
	 * Any subclass should override this.
	 */
	MleDppGroup(char *className = (char *)"MlGroup");

	virtual ~MleDppGroup();

	virtual int write(MleDppActorGroupOutput *out);
	
	/**
	 * Callback that returns true on MleDwpGroup, MleDwpGroupRef,
	 * MleDppGroup, or MleDppGroupRef.
	 */
	static int groupOrGroupRefFinderCB( MleDwpItem *item, void* );

	/**
	 * Override operator new.
	 *
	 * @param tSize The size, in bytes, to allocate.
	 */
	void* operator new(size_t tSize);

	/**
     * Override operator new array.
     *
     * @param tSize The size, in bytes, to allocate.
     */
	void* operator new[](size_t tSize);

	/**
	 * Override operator delete.
	 *
	 * @param p A pointer to the memory to delete.
	 */
    void  operator delete(void *p);

	/**
     * Override operator delete.
     *
     * @param p A pointer to the memory to delete.
     */
	void  operator delete[](void* p);

  protected:

	virtual int writeContents(MleDppActorGroupOutput *out);
};

#endif /* __MLE_DPP_GROUP_H_ */
