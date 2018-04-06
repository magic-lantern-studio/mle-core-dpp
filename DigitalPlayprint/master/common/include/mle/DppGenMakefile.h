/** @defgroup MleDPPMaster Magic Lantern Digital Playprint Library API - Master */

/**
 * @file DppGenMakefile.h
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
//  For information concerning this header file, contact Mark S. Millard,
//  of Wizzer Works at msm@wizzerworks.com.
//
//  More information concerning Wizzer Works may be found at
//
//      http://www.wizzerworks.com
//
// COPYRIGHT_END

#ifndef __MLE_DPP_GENMAKEFILE_H_
#define __MLE_DPP_GENMAKEFILE_H_


// Include Magic Lantern header files.
#include <mle/mlTypes.h>
#include <mle/MleTemplate.h>

#include "mle/Dpp.h"

class MleDwpInput;
class MleDwpItem;

/**
 * This class is used to generate Makefiles for Magic Lantern titles.
 */
class MleDppGenMakefile
{
    // Declare member variables.

    private:

        MleDwpInput *m_wp;      // DWP input object for reader/writer.
        MleDwpItem  *m_root;    // Root of instantiated DWP.

    // Declare member functions.

    public:

        MleDppGenMakefile(char *filename);

        virtual ~MleDppGenMakefile(void);

		MlBoolean isError()
		{ return m_root == NULL; }

        MleDwpItem *getRoot(void)
		{ return(m_root); };

        MlBoolean generateMakefile(void);

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

		// Hide the default constructor.
        MleDppGenMakefile(void) {};

		// Read the Makefile generator template.
        void readMakefileTemplate(MleTemplate &pTemplate);
};


// Declare function prototypes.
void doGenMakefileActors(MleTemplateProcess *process, char *section, void *clientdata);
void doGenMakefileRoles(MleTemplateProcess *process, char *section, void *clientdata);
void doGenMakefileSets(MleTemplateProcess *process, char *section, void *clientdata);

#endif /* __MLE_DPP_GENMAKEFILE_H_ */
