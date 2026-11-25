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


// Tests of genmakefile: a template that cannot be read is an error, and the
// Makefile that the template makes masters a playprint.

// Include system header files.
#include <unistd.h>
#include <string>
#include <vector>

// Include DPP test header files.
#include "libdpptest.h"

using namespace dpptest;

class GenMakefileTest : public ScratchFixture {};

// A template that cannot be read ($MLE_ROOT/lib64/TitleMakefile.tmpl) used to
// be reported, and then genmakefile wrote an empty Makefile.target and exited
// with 0. The tool runs with a MLE_ROOT that has no template (env sets it
// after runTool did).
TEST_F(GenMakefileTest, FailsWhenTheTemplateCannotBeRead) {
    std::string tool = toolPath("genmakefile");
    if (tool.empty())
        GTEST_SKIP() << "genmakefile is not installed (it is not in the default build)";
    if (access("/usr/bin/env", X_OK) != 0)
        GTEST_SKIP() << "env is not installed";

    ToolResult result = runTool("/usr/bin/env",
        { "MLE_ROOT=" + scratchFile("no-such-root"), tool, workprintPath("HelloCube.dwp") },
        scratch());
    EXPECT_EQ(result.status, 1) << result.output;
    EXPECT_EQ(result.signal, 0);
    EXPECT_NE(result.output.find("TitleMakefile.tmpl"), std::string::npos)
        << "the error names the template: " << result.output;
    EXPECT_NE(access(scratchFile("Makefile.target").c_str(), F_OK), 0)
        << "no Makefile.target is written";
}

// The template installed with the generator makes a Makefile that runs the
// mastering tools. The tools are given by their paths, as make does not find
// them in $MLE_ROOT/bin.
TEST_F(GenMakefileTest, MakefileMastersAPlayprintInEachByteOrder) {
    std::string tool = toolPath("genmakefile");
    if (tool.empty())
        GTEST_SKIP() << "genmakefile is not installed (it is not in the default build)";
    std::string root = tool.substr(0, tool.rfind("/bin/"));
    if (access((root + "/lib64/TitleMakefile.tmpl").c_str(), R_OK) != 0)
        GTEST_SKIP() << "the Makefile template is not installed";
    const std::string make = "/usr/bin/make";
    if (access(make.c_str(), X_OK) != 0)
        GTEST_SKIP() << "make is not installed";
    for (const char *name : { "gentables", "gengroup", "genscene", "genmedia", "genppscript", "gendpp" })
        if (toolPath(name).empty())
            GTEST_SKIP() << name << " is not installed";

    std::string workprint = scratchFile("title.dwp");
    std::string text;
    ASSERT_TRUE(readFile(workprintPath("HelloCube.dwp"), text));
    ASSERT_TRUE(writeFile(workprint, text));
    ToolResult generated = runTool("genmakefile", { "title.dwp" }, scratch());
    ASSERT_EQ(generated.status, 0) << generated.output;
    ASSERT_EQ(access(scratchFile("Makefile.target").c_str(), R_OK), 0);

    std::vector<std::string> common = { "-f", "Makefile.target",
        "GENTABLES=" + toolPath("gentables"), "GENGROUP=" + toolPath("gengroup"),
        "GENSCENE=" + toolPath("genscene"), "GENMEDIA=" + toolPath("genmedia"),
        "GENPPSCRIPT=" + toolPath("genppscript"), "GENDPP=" + toolPath("gendpp") };

    for (bool big : { true, false }) {
        SCOPED_TRACE(big ? "big endian" : "little endian");
        std::vector<std::string> args = common;
        args.push_back(std::string("ENDIAN=") + (big ? "b" : "l"));
        ToolResult built = runTool(make, args, scratch(), 60);
        ASSERT_EQ(built.status, 0) << built.output;

        // The playprint, the tables and the chunk index code are made, in the
        // byte order that was asked for, for the chunk files as well.
        std::vector<uint8_t> playprint, group;
        ASSERT_TRUE(readFile(scratchFile("out/playprint.dpp"), playprint));
        ASSERT_TRUE(readFile(scratchFile("gen/group1.chk"), group));
        ASSERT_GE(playprint.size(), 4u);
        ASSERT_GE(group.size(), 4u);
        EXPECT_EQ(std::string(playprint.begin(), playprint.begin() + 4), big ? "PPBE" : "ELPP");
        EXPECT_EQ(std::string(group.begin(), group.begin() + 4), big ? "MLBE" : "MLLE");
        for (const char *file : { "gen/mltables.cxx", "out/toc.idx.cxx", "out/toc.idx.h" })
            EXPECT_EQ(access(scratchFile(file).c_str(), R_OK), 0) << file;

        // Nothing is made again.
        ToolResult again = runTool(make, args, scratch(), 60);
        EXPECT_EQ(again.status, 0) << again.output;
        EXPECT_NE(again.output.find("Nothing to be done"), std::string::npos) << again.output;

        // The Actors of the workprint are listed.
        std::vector<std::string> listArgs = args;
        listArgs.push_back("actors");
        ToolResult actors = runTool(make, listArgs, scratch(), 60);
        EXPECT_NE(actors.output.find("CubeActor"), std::string::npos) << actors.output;

        ASSERT_EQ(runTool(make, [&] { auto a = args; a.push_back("clean"); return a; }(), scratch(), 60).status, 0);
    }
}
