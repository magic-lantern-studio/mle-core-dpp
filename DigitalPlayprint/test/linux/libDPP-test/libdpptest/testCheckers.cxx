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

// Phase 2 tests of libplayprint: the Actor Group and Scene chunk checkers,
// which read a chunk file as written by the mastering tools. They check their
// input with MLE_ASSERT, so only good input is tested for its result; damaged
// input is tested to not hang (see testBadInput.cxx for the readers that
// return an error).

// Include DPP test header files.
#include "libdpptest.h"

// Include Magic Lantern header files.
#include <mle/agchk.h>
#include <mle/agopcode.h>
#include <mle/mlMalloc.h>
#include <mle/scenechk.h>

using namespace dpptest;

namespace {

void putName(std::vector<uint8_t> &bytes, const char *name) {
    bytes.insert(bytes.end(), name, name + strlen(name) + 1);
}

// A Group chunk file with the given media names.
std::vector<uint8_t> groupWithNames(std::initializer_list<const char *> names) {
    std::vector<uint8_t> bytes = groupChunkBytes(false);   // no names
    for (const char *n : names)
        putName(bytes, n);
    return bytes;
}

} // anonymous namespace

class CheckerTest : public ScratchFixture {};


/*** Actor Group chunks ***/

TEST_F(CheckerTest, GroupCountsAndWalksTheNames) {
    std::string path = scratchFile("g.chk");
    ASSERT_TRUE(writeFile(path, groupWithNames({ "one", "second", "3" })));

    MleDppActorGroupChunk chunk(path.c_str(), hostIsLittle());
    EXPECT_EQ(chunk.countNames(), 3);
    const char *name = chunk.getNames();
    ASSERT_NE(name, nullptr);
    EXPECT_STREQ(name, "one");
    name = chunk.nextName(name);
    ASSERT_NE(name, nullptr);
    EXPECT_STREQ(name, "second");
    name = chunk.nextName(name);
    ASSERT_NE(name, nullptr);
    EXPECT_STREQ(name, "3");
    EXPECT_EQ(chunk.nextName(name), nullptr);       // the end
    EXPECT_EQ(chunk.nextName(NULL), nullptr);
}

TEST_F(CheckerTest, GroupWithoutNames) {
    std::string path = scratchFile("g.chk");
    ASSERT_TRUE(writeFile(path, groupChunkBytes(false)));
    MleDppActorGroupChunk chunk(path.c_str(), hostIsLittle());
    EXPECT_EQ(chunk.countNames(), 0);
    EXPECT_EQ(chunk.getNames(), nullptr);
}

TEST_F(CheckerTest, GroupResolvesNamesToIndices) {
    std::string path = scratchFile("g.chk");
    ASSERT_TRUE(writeFile(path, groupChunkBytes(true)));

    // The name of the MediaRef, index 0 in the chunk file, is index 5 in the
    // table of the playprint.
    MleDppActorGroupChunk chunk(path.c_str(), hostIsLittle());
    ASSERT_EQ(chunk.countNames(), 1);
    EXPECT_STREQ(chunk.getNames(), "image");
    const MlInt indices[] = { 5 };
    int length = 0;
    const unsigned char *resolved = chunk.resolveNames(&length, indices);
    ASSERT_NE(resolved, nullptr);

    std::vector<uint8_t> got(resolved, resolved + length);
    std::vector<uint8_t> expected = { 7, 0, 0, 0, 1,
        createActorOpcode, setPropertyLengthOpcode, 4, setPropertyOffsetOpcode,
        copyPropertyOpcode, 0x11, 0x22, 0x33, 0x44,
        copyMediaRefOpcode, 5, endActorGroupOpcode };
    EXPECT_EQ(got, expected);
    mlFree(const_cast<unsigned char *>(resolved));
}

TEST_F(CheckerTest, GroupWithManyIndicesUsesMoreThanOneByte) {
    // An index of 128 or more needs two bytes: seven bits at a time, with the
    // high bits first.
    std::string path = scratchFile("g.chk");
    ASSERT_TRUE(writeFile(path, groupChunkBytes(true)));
    MleDppActorGroupChunk chunk(path.c_str(), hostIsLittle());
    const MlInt indices[] = { 300 };
    int length = 0;
    const unsigned char *resolved = chunk.resolveNames(&length, indices);
    ASSERT_NE(resolved, nullptr);
    ASSERT_EQ(length, 18);
    EXPECT_EQ(resolved[14], copyMediaRefOpcode);
    EXPECT_EQ(resolved[15], 300 >> 7);
    EXPECT_EQ(resolved[16], 300 & 0x7f);
    EXPECT_EQ(resolved[17], endActorGroupOpcode);
    mlFree(const_cast<unsigned char *>(resolved));
}


/*** Scene chunks ***/

// A Scene chunk file with group names: the integers, then the names.
static std::vector<uint8_t> sceneWithNames(std::initializer_list<const char *> names) {
    std::vector<uint8_t> bytes = sceneChunkBytes();
    for (const char *n : names)
        putName(bytes, n);
    return bytes;
}

TEST_F(CheckerTest, SceneCountsAndWalksTheNames) {
    std::string path = scratchFile("s.chk");
    ASSERT_TRUE(writeFile(path, sceneWithNames({ "groupA", "groupB" })));

    MleDppSceneChunk chunk(path.c_str(), hostIsLittle());
    EXPECT_EQ(chunk.countNames(), 2);
    const char *name = chunk.getNames();
    ASSERT_NE(name, nullptr);
    EXPECT_STREQ(name, "groupA");
    name = chunk.nextName(name);
    ASSERT_NE(name, nullptr);
    EXPECT_STREQ(name, "groupB");
    EXPECT_EQ(chunk.nextName(name), nullptr);
}

TEST_F(CheckerTest, SceneWithoutNames) {
    std::string path = scratchFile("s.chk");
    ASSERT_TRUE(writeFile(path, sceneChunkBytes()));
    MleDppSceneChunk chunk(path.c_str(), hostIsLittle());
    EXPECT_EQ(chunk.countNames(), 0);
    EXPECT_EQ(chunk.getNames(), nullptr);
}

TEST_F(CheckerTest, SceneLoadsItsIntegers) {
    std::string path = scratchFile("s.chk");
    ASSERT_TRUE(writeFile(path, sceneWithNames({ "groupA" })));
    MleDppSceneChunk chunk(path.c_str(), hostIsLittle());
    MlInt length = 0;
    const MlByte *loaded = chunk.loadChunk(&length);
    ASSERT_NE(loaded, nullptr);
    // The file without its offset and its names.
    std::vector<uint8_t> expected = sceneChunkBytes();
    expected.erase(expected.begin(), expected.begin() + 4);
    ASSERT_GT(length, 0);
    EXPECT_LE(static_cast<size_t>(length), expected.size() * 2);
    delete[] const_cast<MlByte *>(loaded);
}


/*** Damaged files ***/

// The checkers stop with an assertion in a debug build, and read what is not
// there in a release build. The test is that neither hangs; the way that
// they fail is recorded in the test plan.
TEST_F(CheckerTest, DamagedGroupFilesDoNotHang) {
    std::vector<uint8_t> good = groupChunkBytes(true);
    struct { const char *name; std::vector<uint8_t> bytes; } cases[] = {
        { "empty", {} },
        { "three bytes", { 1, 2, 3 } },
        { "offset beyond the file", { 0x7f, 0, 0, 0, 1, 2, 3, 4 } },
        { "negative offset", { 0xff, 0xff, 0xff, 0xff, 1, 2, 3, 4 } },
        { "truncated", std::vector<uint8_t>(good.begin(), good.begin() + good.size() / 2) },
    };
    for (auto &c : cases) {
        std::string path = scratchFile("bad.chk");
        ASSERT_TRUE(writeFile(path, c.bytes));
        ChildResult r = runInChild([&] {
            MleDppActorGroupChunk chunk(path.c_str(), hostIsLittle());
            chunk.countNames();
            int length;
            const MlInt indices[] = { 0 };
            chunk.resolveNames(&length, indices);
        }, 5);
        EXPECT_FALSE(r.timedOut) << c.name;
    }
}

TEST_F(CheckerTest, DamagedSceneFilesDoNotHang) {
    struct { const char *name; std::vector<uint8_t> bytes; } cases[] = {
        { "empty", {} },
        { "three bytes", { 1, 2, 3 } },
        { "offset beyond the file", { 0x7f, 0, 0, 0, 1, 2, 3, 4 } },
        { "negative offset", { 0xff, 0xff, 0xff, 0xff, 1, 2, 3, 4 } },
    };
    for (auto &c : cases) {
        std::string path = scratchFile("bad.chk");
        ASSERT_TRUE(writeFile(path, c.bytes));
        ChildResult r = runInChild([&] {
            MleDppSceneChunk chunk(path.c_str(), hostIsLittle());
            chunk.countNames();
            MlInt length;
            chunk.loadChunk(&length);
        }, 5);
        EXPECT_FALSE(r.timedOut) << c.name;
    }
}
