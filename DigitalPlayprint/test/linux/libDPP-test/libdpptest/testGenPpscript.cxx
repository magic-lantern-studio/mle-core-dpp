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


// Tests of genppscript: it fails, and does not crash or leave a script, when
// it cannot write the script or read the workprint.

// Include system header files.
#include <sys/stat.h>
#include <unistd.h>
#include <string>

// Include DPP test header files.
#include "libdpptest.h"

using namespace dpptest;

class GenPpscriptTest : public ScratchFixture {};

// genppscript used to crash (a segmentation fault) when the directory of its
// output did not exist: the script was not checked after it was opened.
TEST_F(GenPpscriptTest, FailsWhenTheOutputDirectoryDoesNotExist) {
    ASSERT_FALSE(toolPath("genppscript").empty());
    ToolResult result = runTool("genppscript",
        { "-d", "no-such-directory", "rehearsal", workprintPath("HelloCube.dwp"),
          "playprint.dpp", "playprint.py", "toc.idx" }, scratch());
    EXPECT_EQ(result.signal, 0) << "the tool crashed: " << result.output;
    EXPECT_EQ(result.status, 1) << result.output;
    EXPECT_NE(result.output.find("playprint.py"), std::string::npos)
        << "the error names the script: " << result.output;
}

// It used to say nothing, leave an empty script, and exit with 0.
TEST_F(GenPpscriptTest, FailsWhenTheWorkprintCannotBeOpened) {
    ASSERT_FALSE(toolPath("genppscript").empty());
    ASSERT_EQ(mkdir(scratchFile("gen").c_str(), 0755), 0);
    ToolResult result = runTool("genppscript",
        { "-d", "gen", "rehearsal", scratchFile("missing.dwp"),
          "playprint.dpp", "playprint.py", "toc.idx" }, scratch());
    EXPECT_EQ(result.signal, 0) << result.output;
    EXPECT_EQ(result.status, 1) << result.output;
    EXPECT_NE(access(scratchFile("gen/playprint.py").c_str(), F_OK), 0)
        << "no script is left behind";
}

TEST_F(GenPpscriptTest, WritesAScriptThatNamesTheChunkFiles) {
    ASSERT_FALSE(toolPath("genppscript").empty());
    ASSERT_EQ(mkdir(scratchFile("gen").c_str(), 0755), 0);
    ToolResult result = runTool("genppscript",
        { "-d", "gen", "rehearsal", workprintPath("HelloCube.dwp"),
          "playprint.dpp", "playprint.py", "toc.idx" }, scratch());
    ASSERT_EQ(result.status, 0) << result.output;
    std::string script;
    ASSERT_TRUE(readFile(scratchFile("gen/playprint.py"), script));
    for (const char *line : { "dpp.codefile(\"toc.idx\")", "dpp.beginplayprint(\"playprint.dpp\")",
                              "dpp.addgroup(\"group1.chk\", \"group1\")",
                              "dpp.addscene(\"scene1.chk\", \"scene1\")", "dpp.endplayprint()" })
        EXPECT_NE(script.find(line), std::string::npos) << line << " in:\n" << script;
}
