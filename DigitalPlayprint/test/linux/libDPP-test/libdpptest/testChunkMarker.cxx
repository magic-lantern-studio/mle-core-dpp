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

// Phase 2 tests of the byte order marker of the chunk files: the Group, Scene
// and Set chunk files start with a marker that gives their byte order, the
// tools write it and read it, and gendpp (MleDppOutput) converts or refuses
// a chunk file that is not in the byte order of the playprint.

// Include system header files.
#include <string.h>

// Include DPP test header files.
#include "libdpptest.h"

// Include Magic Lantern header files.
#include <mle/agopcode.h>
#include <mle/chktable.h>
#include <mle/mlErrno.h>
#include <mle/playprnt.h>
#include <mle/ppoutput.h>

using namespace dpptest;

namespace {

uint32_t intIn(const std::vector<uint8_t> &data, size_t at, bool big) {
    uint32_t v = 0;
    for (int i = 0; i < 4; i++)
        v = (v << 8) | data[at + (big ? i : 3 - i)];
    return v;
}

} // anonymous namespace

class ChunkMarkerTest : public ScratchFixture {
  protected:
    // Set when the generators cannot run (the test is skipped).
    bool m_skip = false;

    // The tools' chunk files for a workprint, in a directory for each byte order.
    void runGenerators(bool big, std::string &dir) {
        dir = scratchFile(big ? "be" : "le");
        ASSERT_EQ(system(("mkdir " + dir).c_str()), 0);
        std::string workprint = workprintPath("dppImageTest/DppImageTest.dwp");
        ToolResult g = runTool("gengroup", { big ? "-b" : "-l", "none", workprint, "a.h", "g.h" }, dir);
        // A Debug build stops on an assertion that long is as big as float,
        // which is not so on a 64 bit machine (see the test plan).
        if (g.output.find("sizeof(long) == sizeof(float)") != std::string::npos) {
            m_skip = true;
            return;
        }
        ASSERT_EQ(g.status, 0) << g.output;
        ToolResult s = runTool("genscene", { big ? "-b" : "-l", "none", workprint, "s.h" }, dir);
        ASSERT_EQ(s.status, 0) << s.output;
    }

    // Write one chunk file to the playprint with a function of MleDppOutput.
    // The playprint is in the given byte order.
    template <class Write>
    bool writePlayprint(bool playBig, const std::string &path, Write write) {
        MleDppOutput out(path.c_str());
        out.setSwap(needsSwap(playBig));
        out.begin(playBig ? MLE_DPP_CREATEPPBE : MLE_DPP_CREATEPPLE);
        out.writeHeader();
        bool ok = write(out);
        out.end();
        return ok;
    }

    std::vector<uint8_t> chunkData(const std::string &playprint, const char *tag) {
        ChunkFile file;
        EXPECT_TRUE(readChunkFile(playprint, file)) << file.error;
        for (const Chunk &c : file.chunks[0].children)
            if (c.tag == tag)
                return c.data;
        ADD_FAILURE() << "no " << tag << " chunk";
        return {};
    }
};


/*** The tools ***/

TEST_F(ChunkMarkerTest, ToolsWriteTheMarkerAndTheSizesInTheByteOrderOfTheFile) {
    ASSERT_FALSE(toolPath("gengroup").empty());
    ASSERT_FALSE(toolPath("genscene").empty());
    for (bool big : { true, false }) {
        std::string dir;
        runGenerators(big, dir);
        if (m_skip)
            GTEST_SKIP() << "gengroup does not run in this build";
        const char *marker = big ? "MLBE" : "MLLE";
        for (const char *name : { "g0.chk", "MySet.chk", "Scene1.chk" }) {
            std::vector<uint8_t> bytes;
            ASSERT_TRUE(readFile(dir + "/" + name, bytes)) << name;
            ASSERT_GE(bytes.size(), 8u) << name;
            EXPECT_EQ(std::string(bytes.begin(), bytes.begin() + 4), marker) << name;
            uint32_t first = intIn(bytes, 4, big);
            if (std::string(name) == "MySet.chk")
                EXPECT_EQ(first, bytes.size()) << name << ": the size of the file";
            else {
                EXPECT_GE(first, 8u) << name << ": the offset of the names";
                EXPECT_LE(first, bytes.size()) << name;
            }
        }
    }
}

TEST_F(ChunkMarkerTest, DumpToolsReadTheByteOrderFromTheMarker) {
    ASSERT_FALSE(toolPath("dumpgroup").empty());
    ASSERT_FALSE(toolPath("dumpscene").empty());
    std::string be, le;
    runGenerators(true, be);
    runGenerators(false, le);
    if (m_skip)
        GTEST_SKIP() << "gengroup does not run in this build";
    // The same workprint gives the same dump from either byte order, except
    // for the bytes of the properties, which are in the byte order of the file.
    auto withoutData = [](const std::string &text) {
        std::string kept;
        size_t at = 0;
        while (at < text.size()) {
            size_t end = text.find('\n', at);
            end = end == std::string::npos ? text.size() : end + 1;
            if (text.compare(at, 14, "         Data ") != 0)
                kept += text.substr(at, end - at);
            at = end;
        }
        return kept;
    };
    ToolResult gb = runTool("dumpgroup", { "g0.chk" }, be), gl = runTool("dumpgroup", { "g0.chk" }, le);
    EXPECT_EQ(gb.status, 0) << gb.output;
    EXPECT_EQ(withoutData(gb.output), withoutData(gl.output));
    EXPECT_NE(gb.output, gl.output);
    EXPECT_NE(gb.output.find("Number of Actors: 1"), std::string::npos) << gb.output;
    ToolResult sb = runTool("dumpscene", { "Scene1.chk" }, be), sl = runTool("dumpscene", { "Scene1.chk" }, le);
    EXPECT_EQ(sb.status, 0) << sb.output;
    EXPECT_EQ(sb.output, sl.output);
    EXPECT_NE(sb.output.find("Number of Groups: 1"), std::string::npos) << sb.output;

    // With -b or -l the file has to be in that byte order.
    EXPECT_EQ(runTool("dumpgroup", { "-b", "g0.chk" }, be).status, 0);
    EXPECT_EQ(runTool("dumpgroup", { "-l", "g0.chk" }, le).status, 0);
    ToolResult wrong = runTool("dumpgroup", { "-l", "g0.chk" }, be);
    EXPECT_EQ(wrong.status, 1);
    EXPECT_NE(wrong.output.find("Big Endian, not Little Endian"), std::string::npos) << wrong.output;
    EXPECT_EQ(runTool("dumpscene", { "-b", "Scene1.chk" }, be).status, 0);
    wrong = runTool("dumpscene", { "-b", "Scene1.chk" }, le);
    EXPECT_EQ(wrong.status, 1);
    EXPECT_NE(wrong.output.find("Little Endian, not Big Endian"), std::string::npos) << wrong.output;
}

TEST_F(ChunkMarkerTest, DumpToolsRefuseAFileWithoutAMarker) {
    // A chunk file of the old format starts with the offset.
    std::vector<uint8_t> old = groupChunkBytes(false);
    old.erase(old.begin(), old.begin() + MLE_DPP_CHUNKFILE_MARKER_SIZE);
    ASSERT_TRUE(writeFile(scratchFile("old.chk"), old));
    ToolResult g = runTool("dumpgroup", { "old.chk" }, scratch());
    EXPECT_EQ(g.status, 1);
    EXPECT_NE(g.output.find("no byte order marker"), std::string::npos) << g.output;
    std::vector<uint8_t> oldScene = sceneChunkBytes();
    oldScene.erase(oldScene.begin(), oldScene.begin() + MLE_DPP_CHUNKFILE_MARKER_SIZE);
    ASSERT_TRUE(writeFile(scratchFile("oldscene.chk"), oldScene));
    EXPECT_EQ(runTool("dumpscene", { "oldscene.chk" }, scratch()).status, 1);
}


/*** gendpp: a chunk file in the other byte order ***/

TEST_F(ChunkMarkerTest, SceneInTheOtherByteOrderIsSwapped) {
    for (bool playBig : { true, false }) {
        std::string scene = scratchFile("scene.chk"), pp = scratchFile("pp.dpp");
        ASSERT_TRUE(writeFile(scene, sceneChunkBytes(! playBig)));
        MleDppChunkTable table;
        table.addEntry(CHUNK_SCENE, const_cast<char *>(scene.c_str()), const_cast<char *>("scene1"));
        EXPECT_TRUE(writePlayprint(playBig, pp, [&](MleDppOutput &out) { return out.writeScene(&table, 0); }));

        // The same ints as a chunk file in the byte order of the playprint.
        std::vector<uint8_t> expected = sceneChunkBytes(playBig);
        expected.erase(expected.begin(), expected.begin() + MLE_DPP_CHUNKFILE_HEADER_SIZE);
        EXPECT_EQ(chunkData(pp, "scn "), expected) << (playBig ? "BE" : "LE") << " playprint";
    }
}

TEST_F(ChunkMarkerTest, GroupInTheOtherByteOrderIsRefused) {
    for (bool playBig : { true, false }) {
        std::string group = scratchFile("group.chk"), pp = scratchFile("pp.dpp");
        ASSERT_TRUE(writeFile(group, groupChunkBytes(false, ! playBig)));
        MleDppChunkTable table;
        table.addEntry(CHUNK_GROUP, const_cast<char *>(group.c_str()), const_cast<char *>("group1"));
        mlSetErrno(0);
        EXPECT_FALSE(writePlayprint(playBig, pp, [&](MleDppOutput &out) { return out.writeGroup(&table, 0); }));
        EXPECT_EQ(mlGetErrno(), static_cast<int>(MLE_DPP_ERR_BYTEORDER));

        // In the byte order of the playprint it is written.
        ASSERT_TRUE(writeFile(group, groupChunkBytes(false, playBig)));
        EXPECT_TRUE(writePlayprint(playBig, pp, [&](MleDppOutput &out) { return out.writeGroup(&table, 0); }));
    }
}

TEST_F(ChunkMarkerTest, SetInTheOtherByteOrderIsRefused) {
    for (bool playBig : { true, false }) {
        std::string set = scratchFile("set.chk"), pp = scratchFile("pp.dpp");
        ASSERT_TRUE(writeFile(set, setChunkBytes(! playBig)));
        mlSetErrno(0);
        EXPECT_FALSE(writePlayprint(playBig, pp, [&](MleDppOutput &out) {
            return out.writeSet(const_cast<char *>(set.c_str())); }));
        EXPECT_EQ(mlGetErrno(), static_cast<int>(MLE_DPP_ERR_BYTEORDER));

        ASSERT_TRUE(writeFile(set, setChunkBytes(playBig)));
        EXPECT_TRUE(writePlayprint(playBig, pp, [&](MleDppOutput &out) {
            return out.writeSet(const_cast<char *>(set.c_str())); }));
        std::vector<uint8_t> expected = setChunkBytes(playBig);
        expected.erase(expected.begin(), expected.begin() + MLE_DPP_CHUNKFILE_HEADER_SIZE);
        EXPECT_EQ(chunkData(pp, "set "), expected);
    }
}

TEST_F(ChunkMarkerTest, DamagedSetFilesAreRefused) {
    std::vector<uint8_t> good = setChunkBytes(true), noMarker = good, wrongSize = good;
    noMarker[0] = 'X';
    setChunkFileInt(wrongSize, 99, true);
    struct { const char *name; std::vector<uint8_t> bytes; } cases[] = {
        { "empty", {} }, { "marker only", { 'M', 'L', 'B', 'E' } },
        { "no marker", noMarker }, { "size is not the size of the file", wrongSize },
    };
    for (auto &c : cases) {
        std::string set = scratchFile("set.chk"), pp = scratchFile("pp.dpp");
        ASSERT_TRUE(writeFile(set, c.bytes));
        EXPECT_FALSE(writePlayprint(true, pp, [&](MleDppOutput &out) {
            return out.writeSet(const_cast<char *>(set.c_str())); })) << c.name;
    }
}

TEST_F(ChunkMarkerTest, MediaInTheOtherByteOrderIsConverted) {
    for (bool playBig : { true, false }) {
        std::string same = scratchFile("same.chk"), other = scratchFile("other.chk");
        ASSERT_TRUE(writeMediaChunkFile(same, playBig));
        ASSERT_TRUE(writeMediaChunkFile(other, ! playBig));
        std::string ppSame = scratchFile("same.dpp"), ppOther = scratchFile("other.dpp");
        EXPECT_TRUE(writePlayprint(playBig, ppSame, [&](MleDppOutput &out) {
            return out.writeMedia(const_cast<char *>(same.c_str())); }));
        EXPECT_TRUE(writePlayprint(playBig, ppOther, [&](MleDppOutput &out) {
            return out.writeMedia(const_cast<char *>(other.c_str())); }));
        // The playprints are the same, byte for byte.
        std::vector<uint8_t> a, b;
        ASSERT_TRUE(readFile(ppSame, a));
        ASSERT_TRUE(readFile(ppOther, b));
        EXPECT_EQ(a, b) << (playBig ? "BE" : "LE") << " playprint";
        EXPECT_GT(a.size(), 40u);
    }
}

// The chunk file used to loop on counts that it had not read when it was not in
// the byte order of the playprint; a file that is cut is an error too.
TEST_F(ChunkMarkerTest, DamagedMediaFilesAreRefusedWithoutHanging) {
    std::string good = scratchFile("good.chk");
    ASSERT_TRUE(writeMediaChunkFile(good, true));
    std::vector<uint8_t> bytes;
    ASSERT_TRUE(readFile(good, bytes));
    // The last chunk has a pad byte, which a reader does not need.
    for (size_t n = 0; n + 1 < bytes.size(); n++) {
        std::string media = scratchFile("cut.chk"), pp = scratchFile("cut.dpp");
        ASSERT_TRUE(writeFile(media, std::vector<uint8_t>(bytes.begin(), bytes.begin() + n)));
        ChildResult r = runInChild([&] {
            _exit(writePlayprint(true, pp, [&](MleDppOutput &out) {
                return out.writeMedia(const_cast<char *>(media.c_str())); }) ? 0 : 1);
        }, 5);
        EXPECT_TRUE(r.exited) << n << " bytes" << (r.timedOut ? " (hang)" : "");
        EXPECT_EQ(r.exitCode, 1) << n << " bytes";
    }
    std::string missing = scratchFile("no-such-file");
    EXPECT_FALSE(writePlayprint(true, scratchFile("m.dpp"), [&](MleDppOutput &out) {
        return out.writeMedia(const_cast<char *>(missing.c_str())); }));
}
