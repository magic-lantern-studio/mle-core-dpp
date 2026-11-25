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

// Phase 1 tests: the libraries link and initialize, and the helpers of the
// test project work. The helpers are the base of the tests of the later phases.

// Include system header files.
#include <signal.h>
#include <string.h>
#include <unistd.h>

// Include DPP test header files.
#include "libdpptest.h"

// Include Magic Lantern header files.
#include <mle/DwpFinder.h>
#include <mle/DwpInput.h>
#include <mle/DwpMediaRef.h>
#include <mle/DppGroup.h>
#include <mle/DppGenCode.h>
#include <mle/DppGenMakefile.h>
#include <mle/playprnt.h>

using namespace dpptest;

/*** The libraries link and initialize ***/

TEST(LibrariesTest, PlayprintSwapsBytes) {
    // libplayprint. It has no initialization call.
    EXPECT_EQ(mlDppSwapInt(0x01020304), 0x04030201);
    EXPECT_EQ(mlDppMakeTag('P','P','B','E'), mlDppMakeTag('P','P','B','E'));
    EXPECT_NE(mlDppMakeTag('P','P','B','E'), mlDppMakeTag('P','P','L','E'));
}

TEST(LibrariesTest, GeneratorLinks) {
    // libDPPGen. The classes are not created here: their constructors
    // initialize the library again, and it must be initialized once.
    int (MleDppGenCode::*genCode)(void) = &MleDppGenCode::getErrorCount;
    int (MleDppGenMakefile::*genMakefile)(void) = &MleDppGenMakefile::getErrorCount;
    EXPECT_NE(genCode, nullptr);
    EXPECT_NE(genMakefile, nullptr);
}

TEST(LibrariesTest, ItemsAreRegistered) {
    // libdpp, after mleDppInit(): the item classes can create an item.
    MleDwpItem *group = MleDppGroup::typeId();
    ASSERT_NE(group, nullptr);
    EXPECT_TRUE(group->isa(MleDppGroup::typeId));
    delete group;
}

TEST(LibrariesTest, ReadsWorkprint) {
    // libdwp, libmlmath and libmlutil, through a workprint with Includes and
    // a MediaRef.
    MleDwpInput in;
    std::string path = workprintPath("dppImageTest/DppImageTest.dwp");
    ASSERT_EQ(in.openFile(path.c_str()), 0) << path;

    std::unique_ptr<MleDwpItem> root(MleDwpItem::readAll(&in));
    ASSERT_NE(root, nullptr);
    EXPECT_EQ(in.getErrorCount(), 0);

    MleDwpFinder finder;
    finder.setType(MleDwpMediaRef::typeId);
    finder.setFindAll(1);
    finder.find(root.get());
    EXPECT_EQ(finder.getNumItems(), 1);
}


/*** The test workprints ***/

// Read a workprint and return the number of errors in it. Returns -1 if the
// file cannot be opened.
static int readErrors(const std::string &path) {
    MleDwpInput in;
    if (in.openFile(path.c_str()))
        return -1;
    std::unique_ptr<MleDwpItem> root(MleDwpItem::readAll(&in));
    return in.getErrorCount();
}

TEST(WorkprintsTest, GoodWorkprintsHaveNoErrors) {
    EXPECT_EQ(readErrors(workprintPath("dppImageTest/DppImageTest.dwp")), 0);
    EXPECT_EQ(readErrors(workprintPath("HelloCube.dwp")), 0);
}

TEST(WorkprintsTest, UnresolvedIncludeIsAnError) {
    // HelloWorld.dwp includes a file in $MLE_WORKPRINTS, which the tests
    // do not have. The tools must not use a workprint like this.
    EXPECT_GT(readErrors(workprintPath("HelloWorld.dwp")), 0);
}

TEST(WorkprintsTest, KnownBadInputs) {
    // mediatest.dwp starts with a line of program output, so it is not a
    // workprint: it is refused when it is opened (no magic). A file that is
    // missing is refused too.
    EXPECT_EQ(readErrors(workprintPath("mediatest.dwp")), -1);
    EXPECT_EQ(readErrors(workprintPath("missing-file.dwp")), -1);
}


/*** Helpers: scratch directory and files ***/

class ScratchTest : public ScratchFixture {};

TEST_F(ScratchTest, DirectoryIsNewAndEmpty) {
    EXPECT_EQ(access(scratch().c_str(), W_OK), 0);
    EXPECT_NE(scratch().find("dpptest-"), std::string::npos);
    EXPECT_NE(access(scratchFile("a.bin").c_str(), F_OK), 0);
}

TEST_F(ScratchTest, WritesAndReadsFiles) {
    std::vector<uint8_t> bytes = { 0, 1, 2, 255, 'x' };
    ASSERT_TRUE(writeFile(scratchFile("a.bin"), bytes));
    std::vector<uint8_t> back;
    ASSERT_TRUE(readFile(scratchFile("a.bin"), back));
    EXPECT_EQ(back, bytes);

    std::string text;
    ASSERT_TRUE(writeFile(scratchFile("a.txt"), std::string("one\ntwo\n")));
    ASSERT_TRUE(readFile(scratchFile("a.txt"), text));
    EXPECT_EQ(text, "one\ntwo\n");
    EXPECT_FALSE(readFile(scratchFile("missing"), text));
}


/*** Helpers: running tools ***/

TEST(RunToolTest, ReturnsStatusAndOutput) {
    ToolResult r = runTool("/bin/sh", { "-c", "echo out; echo err 1>&2; exit 3" });
    EXPECT_EQ(r.status, 3);
    EXPECT_FALSE(r.timedOut);
    EXPECT_EQ(r.output, "out\nerr\n");
}

TEST(RunToolTest, StopsATool) {
    ToolResult r = runTool("/bin/sleep", { "30" }, "", 1);
    EXPECT_TRUE(r.timedOut);
    EXPECT_EQ(r.signal, SIGKILL);
}

TEST(RunToolTest, MissingTool) {
    ToolResult r = runTool("no-such-tool", {});
    EXPECT_EQ(r.status, -1);
    EXPECT_NE(r.output.find("not installed"), std::string::npos);
}

TEST_F(ScratchTest, RunsInTheDirectory) {
    ToolResult r = runTool("/bin/pwd", {}, scratch());
    EXPECT_EQ(r.status, 0);
    EXPECT_EQ(r.output, scratch() + "\n");
}

TEST(RunToolTest, DumpdppPrintsUsage) {
    // A tool of this project, run from MLE_ROOT. The usage text is compared
    // with a golden file.
    ASSERT_FALSE(toolPath("dumpdpp").empty()) << "dumpdpp is not installed in $MLE_ROOT/bin";
    ToolResult r = runTool("dumpdpp", {});
    EXPECT_EQ(r.status, 1);
    EXPECT_NE(r.output.find("Syntax:"), std::string::npos);
    EXPECT_TRUE(matchesGolden("dumpdpp-usage.txt", maskGenerated(r.output)));
}


/*** Helpers: code that may crash or hang ***/

TEST(RunInChildTest, Returns) {
    ChildResult r = runInChild([] {});
    EXPECT_TRUE(r.exited);
    EXPECT_EQ(r.exitCode, 0);
}

TEST(RunInChildTest, Exits) {
    ChildResult r = runInChild([] { _exit(5); });
    EXPECT_TRUE(r.exited);
    EXPECT_EQ(r.exitCode, 5);
}

TEST(RunInChildTest, Aborts) {
    ChildResult r = runInChild([] { abort(); });
    EXPECT_TRUE(r.signaled);
    EXPECT_EQ(r.signal, SIGABRT);
    EXPECT_FALSE(r.timedOut);
}

TEST(RunInChildTest, Hangs) {
    ChildResult r = runInChild([] { for (;;) sleep(1); }, 1);
    EXPECT_TRUE(r.timedOut);
}


/*** Helpers: masking and golden files ***/

TEST(MaskTest, MasksWhatChanges) {
    std::string text = "Created Wed Dec 31 17:00:00 1969 in /tmp/dpptest-abc/out.chk\n"
                       "at 2025-11-24 17:04:13 from " DPPTEST_WORKPRINT_DIR "/a.dwp\n";
    EXPECT_EQ(maskGenerated(text, "/tmp/dpptest-abc"),
              "Created <DATE> in <SCRATCH>/out.chk\n"
              "at <DATE> from <WORKPRINTS>/a.dwp\n");
}

TEST(GoldenTest, ReportsTheFirstDifference) {
    const char *update = getenv("DPPTEST_UPDATE_GOLDEN");
    if (update != NULL && std::string(update) == "1")
        GTEST_SKIP() << "golden files are being updated";

    // Compare with a golden file that exists, so the report names the line.
    ::testing::AssertionResult r = matchesGolden("dumpdpp-usage.txt", "different\n");
    EXPECT_FALSE(r);
    EXPECT_NE(std::string(r.message()).find("line 1"), std::string::npos);
    EXPECT_FALSE(matchesGolden("no-such-golden.txt", "x"));
}


/*** Helpers: reading chunk files ***/

static std::vector<uint8_t> sampleFile(bool bigEndian) {
    ChunkBuilder b(bigEndian);
    b.form(bigEndian ? "PPBE" : "PPLE", "MLRT", [&] {
        b.chunk("hdr ", { 0, 0, 0, 1, 0, 0, 0, 2 });
        b.form("LIST", "MRFL", [&] {
            b.chunk("mref", { 1, 2, 3 });
        });
    });
    return b.bytes();
}

TEST(ChunkFileTest, ReadsBothByteOrders) {
    for (bool bigEndian : { true, false }) {
        ChunkFile file;
        ASSERT_TRUE(parseChunkBytes(sampleFile(bigEndian), file)) << file.error;
        EXPECT_EQ(file.bigEndian, bigEndian);
        ASSERT_EQ(file.chunks.size(), 1u);

        const Chunk &top = file.chunks[0];
        EXPECT_EQ(top.tag, bigEndian ? "PPBE" : "PPLE");
        EXPECT_EQ(top.form, "MLRT");
        ASSERT_EQ(top.children.size(), 2u);
        EXPECT_EQ(top.children[0].tag, "hdr ");
        EXPECT_EQ(top.children[0].size, 8u);
        EXPECT_EQ(top.children[1].tag, "LIST");
        EXPECT_EQ(top.children[1].form, "MRFL");
        ASSERT_EQ(top.children[1].children.size(), 1u);
        EXPECT_EQ(top.children[1].children[0].data, (std::vector<uint8_t>{ 1, 2, 3 }));
    }
}

TEST(ChunkFileTest, DescribesTheTree) {
    ChunkFile file;
    ASSERT_TRUE(parseChunkBytes(sampleFile(true), file));
    EXPECT_EQ(describeChunks(file),
              "big endian\n"
              "'PPBE' size=40 form='MLRT'\n"
              "  'hdr ' size=8 data=00 00 00 01 00 00 00 02\n"
              "  'LIST' size=12 form='MRFL'\n"
              "    'mref' size=3 data=01 02 03\n");
    EXPECT_NE(describeChunks(file, { "hdr " }).find("data=<omitted>"), std::string::npos);
}

TEST(ChunkFileTest, RejectsBadFiles) {
    std::vector<uint8_t> good = sampleFile(true);
    ChunkFile file;

    // Cut short at every length: never accepted as a whole, never a crash.
    for (size_t n = 0; n < good.size(); n++) {
        std::vector<uint8_t> cut(good.begin(), good.begin() + n);
        EXPECT_FALSE(parseChunkBytes(cut, file)) << "length " << n;
        EXPECT_FALSE(file.error.empty()) << "length " << n;
    }

    // No playprint tag.
    std::vector<uint8_t> junk = good;
    junk[0] = 'X';
    EXPECT_FALSE(parseChunkBytes(junk, file));

    // A size that is beyond the end of the file.
    std::vector<uint8_t> big = good;
    big[4] = 0x7f;
    EXPECT_FALSE(parseChunkBytes(big, file));
}

TEST_F(ScratchTest, ReadsAChunkFileFromAFile) {
    ASSERT_TRUE(writeFile(scratchFile("a.chk"), sampleFile(false)));
    ChunkFile file;
    ASSERT_TRUE(readChunkFile(scratchFile("a.chk"), file)) << file.error;
    EXPECT_FALSE(file.bigEndian);
    EXPECT_FALSE(readChunkFile(scratchFile("missing.chk"), file));
}

// The reader and the writers agree: a chunk file that genmedia wrote is read
// in both byte orders, and the two have the same structure.
TEST_F(ScratchTest, ReadsWhatGenmediaWrote) {
    ASSERT_FALSE(toolPath("genmedia").empty()) << "genmedia is not installed in $MLE_ROOT/bin";
    std::string workprint = workprintPath("dppImageTest/DppImageTest.dwp");
    std::string trees[2];
    for (int i = 0; i < 2; i++) {
        bool little = (i == 1);
        std::string dir = scratchFile(little ? "le" : "be");
        ASSERT_EQ(mkdir(dir.c_str(), 0755), 0);
        ToolResult r = runTool("genmedia", { little ? "-l" : "-b", "none", workprint, "inventory" }, dir);
        ASSERT_EQ(r.status, 0) << r.output;

        ChunkFile file;
        ASSERT_TRUE(readChunkFile(dir + "/logoImage.chk", file)) << file.error;
        EXPECT_EQ(file.bigEndian, ! little);
        ASSERT_EQ(file.chunks.size(), 1u);
        EXPECT_EQ(file.chunks[0].children.size(), 2u); // hdr and LIST
        // The data of a chunk is in the byte order of the file, so only the
        // structure is the same. The tag of the file differs too.
        trees[i] = describeChunks(file, { "hdr ", "info", "mref" });
        trees[i].erase(0, trees[i].find('\n'));
        for (std::string name : { "PPBE", "PPLE" }) {
            size_t at = trees[i].find(name);
            if (at != std::string::npos)
                trees[i].replace(at, 4, "PPxx");
        }
    }
    EXPECT_EQ(trees[0], trees[1]);
}
