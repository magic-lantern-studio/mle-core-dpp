/** @defgroup MleDPPMaster Magic Lantern Digital Playprint Library API - Master */

/**
 * @file DppGenCode.h
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

#ifndef __MLE_DPP_GENCODE_H_
#define __MLE_DPP_GENCODE_H_


// Include Magic Lantern header files.
#include <mle/mlTypes.h>
#include <mle/MlePath.h>
#include <mle/MleTemplate.h>

// Include Digital Playprint header files.
#include "mle/Dpp.h"

// Declare external references.
class MleDwpInput;
class MleDwpItem;


/**
 * This class is a utility for generating application code based on
 * a generic templating capability.
 */
class MleDppGenCode
{
    // Declare member variables.
    private:

        MleDwpInput *m_wp;        // DWP input object for reader/writer.
        MleDwpItem  *m_root;      // Root of instantiated DWP.
		char         m_language;  // The programming language to generate.
		char        *m_package;   // Packaging information for Java.
		MlBoolean    m_verbose;    // Verbosity flag.

    // Declare member functions.
    public:

        MleDppGenCode(char *filename,char language,char *package);

        /**
		 * @brief The destructor.
		 */
		virtual ~MleDppGenCode(void);

		/**
		 * @brief Get the root of the Digital Workprint.
		 *
		 * @return A pointer to the instantiated DWP is returned.
		 */
        MleDwpItem *getRoot(void)
		{ return(m_root); };

        // MlBoolean verifyWorkprint(char *tags);

        MlBoolean generateTitle(char *tags);
	   
		/**
		 * @brief Set the path for the generated output.
		 *
		 * @param path A character array identifying the output directory
		 * for the generated assets. This path is platform specific.
		 */
		static void setOutputDirectory(const char *path);

		/**
		 * @brief Get the path for the generated output.
		 *
		 * @return A path object is returned from which the platform
		 * specific representation of the path can be obtained.
		 */
		static MlePath *getOutputDirectory();

		/**
		 * Set verbosity.
		 *
		 * @param flag If <code>TRUE</code> then code generation will output
		 * debugging information. Otherwise, if set to <code>FALSE</code>, then
		 * the code generation will execute silently.
		 */
		void setVerbose(MlBoolean flag)
		{ m_verbose = flag; }

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
        MleDppGenCode(void) {};

		// Read the template for the title.
        void readTitleTemplate(MleTemplate &pTemplate);

		// The output directory where generated code will be placed.
		static MlePath *g_outputDir;
};


// Declare function prototypes.
void doGenCodeIndexStrings(MleTemplateProcess *, char *, void *);
void doGenCodeActorClass(MleTemplateProcess *, char *, void *);
void doGenCodeProperties(MleTemplateProcess *, char *, void *);
void doGenCodeRoleClass(MleTemplateProcess *, char *, void *);
void doGenCodeSets(MleTemplateProcess *, char *, void *);
void doGenCodeGroupClass(MleTemplateProcess *, char *, void *);
void doGenCodeSceneClass(MleTemplateProcess *, char *, void *);
void doGenCodeMediaRefClass(MleTemplateProcess *, char *, void *);


#endif /* __MLE_DPP_GENCODE_H_ */
