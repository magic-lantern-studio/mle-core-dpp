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


// A test of TitleAnt.tmpl, the template of the Ant build file for Java titles.
// The template is rendered as the generators render a template (MleTemplate),
// and the result is checked. No Java is built.

// Include system header files.
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <string>

// Include DPP test header files.
#include "libdpptest.h"

// Include Magic Lantern header files.
#include <mle/MleTemplate.h>

using namespace dpptest;

namespace {

// Writes the section once for each Actor, as MleDppGenMakefile does.
void doActors(MleTemplateProcess *ptp, char *section, void *)
{
    MleTemplateBindings local;
    MleTemplateProcess process(section, ptp->getTemplate(), &local, ptp->getFileDescriptor());
    for (const char *name : { "CubeActor", "SecondActor" }) {
        local.defineConstant("NAME_STRING", const_cast<char *>(name));
        process.go();
    }
}

// The text of the <exec> task of a tool in the build file.
std::string execOf(const std::string &xml, const std::string &tool)
{
    size_t at = xml.find("<exec executable=\"${" + tool + "}\"");
    if (at == std::string::npos)
        return "";
    return xml.substr(at, xml.find("</exec>", at) - at);
}

} // anonymous namespace

class TitleAntTest : public ScratchFixture {};

TEST_F(TitleAntTest, RendersABuildFileThatMastersWithTheJavaOptions) {
    std::string tool = toolPath("gengroup");
    if (tool.empty())
        GTEST_SKIP() << "the tools are not installed";
    std::string templatePath = tool.substr(0, tool.rfind("/bin/")) + "/lib64/TitleAnt.tmpl";
    if (access(templatePath.c_str(), R_OK) != 0)
        GTEST_SKIP() << "the Ant template is not installed";

    MleTemplate tmpl;
    ASSERT_GE(tmpl.read(const_cast<char *>(templatePath.c_str())), 0);
    MleTemplateBindings *globals = new MleTemplateBindings();
    globals->defineConstant("DATE", const_cast<char *>("a date"));
    globals->defineConstant("FILENAME", const_cast<char *>("build.xml"));
    globals->defineCallback("DO_ACTORS", doActors, NULL);
    tmpl.setGlobalBindings(globals);
    std::string out = scratchFile("build.xml");
    FILE *fd = fopen(out.c_str(), "w");
    ASSERT_TRUE(fd != NULL);
    MleTemplateProcess process("MASTERING_ANT", &tmpl, NULL, fd);
    ASSERT_EQ(process.go(), 0);
    fclose(fd);

    std::string xml;
    ASSERT_TRUE(readFile(out, xml));

    // The text is an XML document, and the values are in it.
    EXPECT_EQ(xml.compare(0, 5, "<?xml"), 0) << "the XML declaration has to be first";
    EXPECT_NE(xml.find("File: build.xml"), std::string::npos);
    EXPECT_NE(xml.find("<echo message=\"CubeActor\"/>"), std::string::npos);
    EXPECT_NE(xml.find("<echo message=\"SecondActor\"/>"), std::string::npos);

    // The Ant properties come through as ${name}, and what the template
    // escapes does not leave a backslash behind. Nothing is left over from
    // the template language either (a ${NAME} that was not replaced is
    // written as it is, so these are in capitals).
    for (const char *property : { "${workprint}", "${tags}", "${package}", "${endian}", "${gen.dir}", "${out.dir}" })
        EXPECT_NE(xml.find(property), std::string::npos) << property;
    EXPECT_EQ(xml.find("\\$"), std::string::npos) << "an escaped dollar sign was left";
    EXPECT_EQ(xml.find("${NAME_STRING}"), std::string::npos);
    EXPECT_EQ(xml.find("${DATE}"), std::string::npos);
    EXPECT_EQ(xml.find("${FILENAME}"), std::string::npos);
    EXPECT_EQ(xml.find("${DO_ACTORS"), std::string::npos);

    // The tools that write code get -j and the package; the others do not
    // have the option.
    for (const char *name : { "gentables", "gengroup", "genscene", "gendpp" }) {
        std::string task = execOf(xml, name);
        ASSERT_FALSE(task.empty()) << name;
        EXPECT_NE(task.find("<arg value=\"-j\"/>"), std::string::npos) << name;
        EXPECT_NE(task.find("<arg value=\"${package}\"/>"), std::string::npos) << name;
    }
    for (const char *name : { "genmedia", "genppscript" }) {
        std::string task = execOf(xml, name);
        ASSERT_FALSE(task.empty()) << name;
        EXPECT_EQ(task.find("<arg value=\"-j\"/>"), std::string::npos) << name;
    }

    // The text is well formed, if xmllint is there to say so.
    if (access("/usr/bin/xmllint", X_OK) == 0) {
        ToolResult lint = runTool("/usr/bin/xmllint", { "--noout", "build.xml" }, scratch());
        EXPECT_EQ(lint.status, 0) << lint.output;
    }
}
