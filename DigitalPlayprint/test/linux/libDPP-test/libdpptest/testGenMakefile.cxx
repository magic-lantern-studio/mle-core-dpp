// COPYRIGHT_BEGIN
//
// The MIT License (MIT)
//
// Copyright (c) 2025 Wizzer Works
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


// A test of genmakefile: a template that cannot be read is an error.

// Include system header files.
#include <unistd.h>
#include <string>

// Include DPP test header files.
#include "libdpptest.h"

using namespace dpptest;

class GenMakefileTest : public ScratchFixture {};

// The Makefile template ($MLE_ROOT/lib64/TitleMakefile.tmpl) is not part of
// the source tree. genmakefile used to say so, write an empty Makefile.target
// and exit with 0.
TEST_F(GenMakefileTest, FailsWhenTheTemplateCannotBeRead) {
    std::string tool = toolPath("genmakefile");
    if (tool.empty())
        GTEST_SKIP() << "genmakefile is not installed (it is not in the default build)";

    // The root is the directory above bin, from the path of the tool.
    std::string root = tool.substr(0, tool.rfind("/bin/"));
    if (access((root + "/lib64/TitleMakefile.tmpl").c_str(), R_OK) == 0)
        GTEST_SKIP() << "a Makefile template is installed";

    ToolResult result = runTool("genmakefile", { workprintPath("HelloCube.dwp") }, scratch());
    EXPECT_EQ(result.status, 1) << result.output;
    EXPECT_EQ(result.signal, 0);
    EXPECT_NE(result.output.find("TitleMakefile.tmpl"), std::string::npos)
        << "the error names the template: " << result.output;
    EXPECT_NE(access(scratchFile("Makefile.target").c_str(), F_OK), 0)
        << "no Makefile.target is written";
}
