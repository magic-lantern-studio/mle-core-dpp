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

// Include system header files.
#include <algorithm>

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
    expected.erase(expected.begin(), expected.begin() + MLE_DPP_CHUNKFILE_HEADER_SIZE);
    ASSERT_GT(length, 0);
    EXPECT_LE(static_cast<size_t>(length), expected.size() * 2);
    delete[] const_cast<MlByte *>(loaded);
}


/*** Damaged files ***/

// The checkers read files that come from outside, so a damaged file is an
// error that they report: the constructor says the file is not valid, and
// resolveNames and loadChunk return NULL. They do not stop, in a debug build
// either, and do not read what is not there.

namespace {

// A chunk file with an offset that is not set from its length, in the byte
// order of this machine: the marker, the offset, and the bytes.
std::vector<uint8_t> withOffset(int32_t offset, std::initializer_list<uint8_t> rest) {
    std::vector<uint8_t> bytes = chunkFileStart(! hostIsLittle(), offset);
    bytes.insert(bytes.end(), rest.begin(), rest.end());
    return bytes;
}

// A Group chunk file with the given stream (after the id of the group) and
// media names, built with a correct offset, in the byte order of this machine.
std::vector<uint8_t> rawGroup(const std::vector<uint8_t> &stream, std::initializer_list<const char *> names) {
    std::vector<uint8_t> bytes = chunkFileStart(! hostIsLittle(), 0);
    for (uint8_t b : { 7, 0, 0, 0 })            // the id of the group
        bytes.push_back(b);
    if (! hostIsLittle())
        std::reverse(bytes.end() - 4, bytes.end());
    bytes.insert(bytes.end(), stream.begin(), stream.end());
    setChunkFileInt(bytes, bytes.size(), ! hostIsLittle());
    for (const char *n : names)
        putName(bytes, n);
    return bytes;
}

} // anonymous namespace

TEST_F(CheckerTest, DamagedGroupFilesAreNotValid) {
    std::vector<uint8_t> good = groupChunkBytes(true);
    std::vector<uint8_t> noNul = good;
    noNul.pop_back();                   // the last name has no NUL
    std::vector<uint8_t> noMarker = good;
    noMarker[0] = 'X';                  // not a marker
    struct { const char *name; std::vector<uint8_t> bytes; } cases[] = {
        { "empty", {} },
        { "three bytes", { 1, 2, 3 } },
        { "no marker", noMarker },
        { "offset beyond the file", withOffset(0x7f, { 1, 2, 3, 4 }) },
        { "negative offset", withOffset(-1, { 1, 2, 3, 4 }) },
        { "offset inside the header", withOffset(6, { 1, 2, 3, 4 }) },
        { "truncated", std::vector<uint8_t>(good.begin(), good.begin() + good.size() / 2) },
        { "name without a NUL", noNul },
    };
    for (auto &c : cases) {
        std::string path = scratchFile("bad.chk");
        ASSERT_TRUE(writeFile(path, c.bytes));
        ChildResult r = runInChild([&] {
            MleDppActorGroupChunk chunk(path.c_str(), hostIsLittle());
            bool ok = ! chunk.isValid() && chunk.countNames() == 0 && chunk.getNames() == NULL;
            int length = -1;
            const MlInt indices[] = { 0 };
            ok = ok && chunk.resolveNames(&length, indices) == NULL && length == 0;
            _exit(ok ? 0 : 1);
        }, 5);
        EXPECT_TRUE(r.exited) << c.name;
        EXPECT_EQ(r.exitCode, 0) << c.name;
    }
}

TEST_F(CheckerTest, MissingGroupFileIsNotValid) {
    MleDppActorGroupChunk chunk(scratchFile("no-such-file").c_str(), hostIsLittle());
    EXPECT_FALSE(chunk.isValid());
    int length = -1;
    EXPECT_EQ(chunk.resolveNames(&length, NULL), nullptr);
    EXPECT_EQ(length, 0);
}

TEST_F(CheckerTest, DamagedGroupStreamsGiveNoResult) {
    struct { const char *name; std::vector<uint8_t> stream; } cases[] = {
        { "no stream", {} },
        { "unknown opcode", { 1, 0xf0 } },                 // 0xf0 is not an opcode
        { "ends in the middle of an int", { 1, createActorOpcode, copyDynamicPropertyOpcode, 0, 0 } },
        { "no end opcode", { 1, createActorOpcode } },
        { "property is longer than the stream", { 1, createActorOpcode, setPropertyLengthOpcode, 50,
                                                  copyPropertyOpcode, 1, 2 } },
        { "media index not in the names", { 1, createActorOpcode, copyMediaRefOpcode, 9,
                                            endActorGroupOpcode } },
        { "media index with no names", { 1, createActorOpcode, copyMediaRefOpcode, 0,
                                         endActorGroupOpcode } },
        { "negative dynamic length", { 1, createActorOpcode, copyDynamicPropertyOpcode,
                                       0xff, 0xff, 0xff, 0xff, endActorGroupOpcode } },
        { "dynamic length beyond the stream", { 1, createActorOpcode, copyDynamicPropertyOpcode,
                                                0xff, 0xff, 0xff, 0x7f, endActorGroupOpcode } },
        { "array larger than the stream", { 1, createActorOpcode, setPropertyLengthOpcode, 100,
                                            copyArrayPropertyOpcode, 0xff, 0xff, 0xff, 0x7f,
                                            endActorGroupOpcode } },
        { "index without an end", { 1, createActorOpcode, setPropertyOffsetOpcode, 5 } },
    };
    for (auto &c : cases) {
        std::string path = scratchFile("bad.chk");
        // All but the case of the media index with no names have one name.
        bool named = std::string(c.name) != "media index with no names";
        ASSERT_TRUE(writeFile(path, named ? rawGroup(c.stream, { "one" }) : rawGroup(c.stream, {})));
        ChildResult r = runInChild([&] {
            MleDppActorGroupChunk chunk(path.c_str(), hostIsLittle());
            if (! chunk.isValid())
                _exit(2);
            int length = -1;
            const MlInt indices[] = { 0 };
            const unsigned char *resolved = chunk.resolveNames(&length, indices);
            _exit(resolved == NULL && length == 0 ? 0 : 1);
        }, 5);
        EXPECT_TRUE(r.exited) << c.name;
        EXPECT_EQ(r.exitCode, 0) << c.name;
    }
}

TEST_F(CheckerTest, GroupCanBeResolvedAgainAfterAFailure) {
    std::string path = scratchFile("g.chk");
    ASSERT_TRUE(writeFile(path, groupChunkBytes(true)));
    MleDppActorGroupChunk chunk(path.c_str(), hostIsLittle());
    int length = 0;
    // No indices for a stream that has a media reference.
    EXPECT_EQ(chunk.resolveNames(&length, NULL), nullptr);
    const MlInt indices[] = { 2 };
    const unsigned char *resolved = chunk.resolveNames(&length, indices);
    ASSERT_NE(resolved, nullptr);
    EXPECT_EQ(length, 17);
    mlFree(const_cast<unsigned char *>(resolved));
}

TEST_F(CheckerTest, DamagedSceneFilesAreNotValid) {
    std::vector<uint8_t> noNul = sceneWithNames({ "groupA" });
    noNul.pop_back();
    std::vector<uint8_t> noMarker = sceneChunkBytes();
    noMarker[1] = 'X';                  // not a marker
    struct { const char *name; std::vector<uint8_t> bytes; } cases[] = {
        { "empty", {} },
        { "three bytes", { 1, 2, 3 } },
        { "no marker", noMarker },
        { "offset beyond the file", withOffset(0x7f, { 1, 2, 3, 4 }) },
        { "negative offset", withOffset(-1, { 1, 2, 3, 4 }) },
        { "offset inside the header", withOffset(6, { 1, 2, 3, 4 }) },
        { "name without a NUL", noNul },
    };
    for (auto &c : cases) {
        std::string path = scratchFile("bad.chk");
        ASSERT_TRUE(writeFile(path, c.bytes));
        ChildResult r = runInChild([&] {
            MleDppSceneChunk chunk(path.c_str(), hostIsLittle());
            bool ok = ! chunk.isValid() && chunk.countNames() == 0 && chunk.getNames() == NULL;
            MlInt length = -1;
            ok = ok && chunk.loadChunk(&length) == NULL && length == 0;
            _exit(ok ? 0 : 1);
        }, 5);
        EXPECT_TRUE(r.exited) << c.name;
        EXPECT_EQ(r.exitCode, 0) << c.name;
    }
}

TEST_F(CheckerTest, MissingSceneFileIsNotValid) {
    MleDppSceneChunk chunk(scratchFile("no-such-file").c_str(), hostIsLittle());
    EXPECT_FALSE(chunk.isValid());
    MlInt length = -1;
    EXPECT_EQ(chunk.loadChunk(&length), nullptr);
    EXPECT_EQ(length, 0);
}

TEST_F(CheckerTest, SceneWithPartOfAnIntOrNoIntsGivesNoResult) {
    std::vector<std::vector<uint8_t>> cases;
    cases.push_back(withOffset(11, { 1, 2, 3 }));   // 3 bytes of ints
    cases.push_back(withOffset(8, {}));             // no ints
    for (auto &bytes : cases) {
        std::string path = scratchFile("bad.chk");
        ASSERT_TRUE(writeFile(path, bytes));
        MleDppSceneChunk chunk(path.c_str(), hostIsLittle());
        ASSERT_TRUE(chunk.isValid());
        MlInt length = -1;
        EXPECT_EQ(chunk.loadChunk(&length), nullptr) << bytes.size() << " bytes";
        EXPECT_EQ(length, 0);
    }
}


/*** Byte order ***/

TEST_F(CheckerTest, FileByteOrderComesFromTheMarker) {
    for (bool big : { true, false }) {
        std::string path = scratchFile("g.chk");
        ASSERT_TRUE(writeFile(path, groupChunkBytes(true, big)));
        // Whatever the byte order that is asked for, the file says its own.
        for (bool resultLittle : { true, false }) {
            MleDppActorGroupChunk chunk(path.c_str(), resultLittle);
            ASSERT_TRUE(chunk.isValid());
            EXPECT_EQ(chunk.isFileLittle() ? true : false, ! big);
            EXPECT_EQ(chunk.isByteOrderMatch() ? true : false, resultLittle == ! big);
            EXPECT_EQ(chunk.countNames(), 1);
            EXPECT_STREQ(chunk.getNames(), "image");
        }
    }
}

TEST_F(CheckerTest, GroupInOtherByteOrderIsRefused) {
    for (bool fileBig : { true, false }) {
        std::string path = scratchFile("g.chk");
        ASSERT_TRUE(writeFile(path, groupChunkBytes(true, fileBig)));
        const MlInt indices[] = { 2 };
        int length = -1;

        // The same byte order: the stream is copied as it is.
        MleDppActorGroupChunk same(path.c_str(), ! fileBig);
        const unsigned char *resolved = same.resolveNames(&length, indices);
        ASSERT_NE(resolved, nullptr);
        ASSERT_EQ(length, 17);
        EXPECT_EQ(resolved[0], fileBig ? 0 : 7);        // the id of the group
        EXPECT_EQ(resolved[3], fileBig ? 7 : 0);
        mlFree(const_cast<unsigned char *>(resolved));

        // The other: the bytes of a property cannot be swapped.
        MleDppActorGroupChunk other(path.c_str(), fileBig);
        EXPECT_TRUE(other.isValid());
        length = -1;
        EXPECT_EQ(other.resolveNames(&length, indices), nullptr);
        EXPECT_EQ(length, 0);
    }
}

TEST_F(CheckerTest, SceneIsSwappedToTheByteOrderOfTheResult) {
    for (bool fileBig : { true, false }) {
        std::string path = scratchFile("s.chk");
        ASSERT_TRUE(writeFile(path, sceneChunkBytes(fileBig)));
        for (bool resultBig : { true, false }) {
            MleDppSceneChunk chunk(path.c_str(), ! resultBig);
            ASSERT_TRUE(chunk.isValid());
            EXPECT_EQ(chunk.isFileLittle() ? true : false, ! fileBig);
            MlInt length = 0;
            const MlByte *loaded = chunk.loadChunk(&length);
            ASSERT_NE(loaded, nullptr);
            // The ints are the same in either byte order of the file, and
            // are in the byte order of the result.
            std::vector<uint8_t> expected = sceneChunkBytes(resultBig);
            expected.erase(expected.begin(), expected.begin() + MLE_DPP_CHUNKFILE_HEADER_SIZE);
            EXPECT_EQ(std::vector<uint8_t>(loaded, loaded + length), expected)
                << "file " << (fileBig ? "BE" : "LE") << ", result " << (resultBig ? "BE" : "LE");
            delete[] const_cast<MlByte *>(loaded);
        }
    }
}
