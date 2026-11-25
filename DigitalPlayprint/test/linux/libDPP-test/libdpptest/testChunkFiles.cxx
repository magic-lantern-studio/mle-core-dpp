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

// Phase 2 tests of libplayprint: the chunk files (MleChunkFile and
// MleMrefChunkFile), processMediaRef, and the table of chunk files.

// Include system header files.
#include <string.h>
#include <algorithm>

// Include DPP test header files.
#include "libdpptest.h"

// Include Magic Lantern header files.
#include <mle/chktable.h>
#include <mle/mlMalloc.h>
#include <mle/mrefchk.h>
#include <mle/playprnt.h>

using namespace dpptest;

namespace {

uint32_t intIn(const std::vector<uint8_t> &data, size_t at, bool big) {
    uint32_t v = 0;
    for (int i = 0; i < 4; i++)
        v = (v << 8) | data[at + (big ? i : 3 - i)];
    return v;
}

// A MediaRef chunk file with the given MediaRef chunks. The byte order is
// that of the file.
void writeMrefFile(const std::string &path, bool big,
                   std::vector<MleMediaRefChunk> mrefs, MlUInt type = 0) {
    MleChunkFileHeader header = { 3, MleMrefChunkFile::PLAYPRINT_VERSION, 12345, 0 };
    MleMediaRefInfoChunk info = { 0, type, static_cast<MlUInt>(mrefs.size()) };

    MleMrefChunkFile out(path.c_str(), MleChunkFile::WRITING);
    out.setSwap(needsSwap(big));
    out.begin(big ? MLE_DPP_CREATEPPBE : MLE_DPP_CREATEPPLE);
    out.writeHeader(&header);
    out.beginList();
    out.writeInfo(&info);
    for (MleMediaRefChunk &m : mrefs)
        out.writeData(&m);          // swaps the fields of m, when it swaps
    out.endList();
    out.end();
}

MleMediaRefChunk external(const char *name, const char *label = "logo") {
    MleMediaRefChunk m;
    m.m_flags = MleMrefChunkFile::EXTERNAL;
    m.m_type = mlDppStringToTag(const_cast<char *>(label), 0);
    m.m_size = strlen(name);
    m.m_data = const_cast<char *>(name);
    return m;
}

} // anonymous namespace

class ChunkFileClassTest : public ScratchFixture {};


/*** MleChunkFile ***/

TEST_F(ChunkFileClassTest, HeaderIsWrittenInTheByteOrderOfTheFile) {
    for (bool big : { true, false }) {
        std::string path = scratchFile("a.chk");
        {
            MleChunkFileHeader header = { 1, 2, 3, 4 };
            MleChunkFile out(path.c_str(), MleChunkFile::WRITING);
            out.setSwap(needsSwap(big));
            out.begin(big ? MLE_DPP_CREATEPPBE : MLE_DPP_CREATEPPLE);
            EXPECT_TRUE(out.writeHeader(&header));
            out.end();
            // The header that was passed is not changed.
            EXPECT_EQ(header.m_id, 1u);
            EXPECT_EQ(header.m_crc, 4u);
        }
        ChunkFile file;
        ASSERT_TRUE(readChunkFile(path, file)) << file.error;
        EXPECT_EQ(file.bigEndian, big);
        EXPECT_EQ(file.chunks[0].form, "MLCF");
        ASSERT_EQ(file.chunks[0].children.size(), 1u);
        const Chunk &hdr = file.chunks[0].children[0];
        EXPECT_EQ(hdr.tag, "hdr ");
        ASSERT_EQ(hdr.data.size(), 16u);
        for (int i = 0; i < 4; i++)
            EXPECT_EQ(intIn(hdr.data, 4 * i, big), static_cast<uint32_t>(i + 1));
    }
}

TEST_F(ChunkFileClassTest, HeaderRoundTrip) {
    for (bool big : { true, false }) {
        std::string path = scratchFile("a.chk");
        {
            MleChunkFileHeader header = { 11, 22, 1700000000, 44 };
            MleChunkFile out(path.c_str(), MleChunkFile::WRITING);
            out.setSwap(needsSwap(big));
            out.begin(big ? MLE_DPP_CREATEPPBE : MLE_DPP_CREATEPPLE);
            out.writeHeader(&header);
            out.end();
        }
        MleChunkFileHeader back = { 0, 0, 0, 0 };
        MleChunkFile in(path.c_str(), MleChunkFile::READING);
        in.setSwap(needsSwap(big));
        EXPECT_TRUE(in.begin(big ? MLE_DPP_FINDPPBE : MLE_DPP_FINDPPLE));
        EXPECT_TRUE(in.readHeader(&back));
        EXPECT_EQ(back.m_id, 11u);
        EXPECT_EQ(back.m_version, 22u);
        EXPECT_EQ(back.m_time, 1700000000u);
        EXPECT_EQ(back.m_crc, 44u);
        EXPECT_TRUE(in.end());
    }
}

TEST_F(ChunkFileClassTest, ReadingFromAWritingObjectDoesNothing) {
    // The write functions do nothing on a reader, and the read functions on
    // a writer, so one code path can serve both.
    MleChunkFileHeader header = { 1, 2, 3, 4 };
    MleChunkFile in(scratchFile("a.chk").c_str(), MleChunkFile::WRITING);
    EXPECT_TRUE(in.readHeader(&header));
    EXPECT_EQ(header.m_id, 1u);
}


/*** MleMrefChunkFile ***/

TEST_F(ChunkFileClassTest, MediaRefFileIsWrittenInTheByteOrderOfTheFile) {
    for (bool big : { true, false }) {
        std::string path = scratchFile("m.chk");
        writeMrefFile(path, big, { external("media/a.png") }, 5);

        ChunkFile file;
        ASSERT_TRUE(readChunkFile(path, file)) << file.error;
        EXPECT_EQ(file.bigEndian, big);
        const Chunk &form = file.chunks[0];
        ASSERT_EQ(form.children.size(), 2u);         // hdr and the list
        const Chunk &list = form.children[1];
        EXPECT_EQ(list.tag, "LIST");
        EXPECT_EQ(list.form, "MRFL");
        ASSERT_EQ(list.children.size(), 2u);
        const std::vector<uint8_t> &info = list.children[0].data;
        EXPECT_EQ(intIn(info, 0, big), 0u);          // flags
        EXPECT_EQ(intIn(info, 4, big), 5u);          // type
        EXPECT_EQ(intIn(info, 8, big), 1u);          // one MediaRef
        const std::vector<uint8_t> &mref = list.children[1].data;
        EXPECT_EQ(intIn(mref, 0, big), 0u);          // external
        EXPECT_EQ(intIn(mref, 8, big), 11u);         // length of the name
        EXPECT_EQ(std::string(mref.begin() + 12, mref.end()), "media/a.png");
    }
}

TEST_F(ChunkFileClassTest, MediaRefsRoundTrip) {
    for (bool big : { true, false }) {
        std::string path = scratchFile("m.chk");
        writeMrefFile(path, big, { external("media/one.png", "img1"), external("two", "snd2"),
                                   external("", "none") }, 7);

        MleMrefChunkFile in(path.c_str(), MleChunkFile::READING);
        in.setSwap(needsSwap(big));
        MleChunkFileHeader header;
        MleMediaRefInfoChunk info;
        ASSERT_TRUE(in.begin(big ? MLE_DPP_FINDPPBE : MLE_DPP_FINDPPLE));
        ASSERT_TRUE(in.readHeader(&header));
        EXPECT_EQ(header.m_id, 3u);
        EXPECT_EQ(header.m_version, static_cast<MlUInt>(MleMrefChunkFile::PLAYPRINT_VERSION));
        EXPECT_EQ(header.m_time, 12345u);
        ASSERT_TRUE(in.beginList());
        ASSERT_TRUE(in.readInfo(&info));
        EXPECT_EQ(info.m_type, 7u);
        ASSERT_EQ(info.m_numMrefs, 3u);

        const char *names[] = { "media/one.png", "two", "" };
        const char *labels[] = { "img1", "snd2", "none" };
        for (int i = 0; i < 3; i++) {
            MleMediaRefChunk m;
            ASSERT_TRUE(in.readData(&m)) << i;
            EXPECT_EQ(m.m_flags, static_cast<MlUInt>(MleMrefChunkFile::EXTERNAL));
            EXPECT_EQ(m.m_type, mlDppStringToTag(const_cast<char *>(labels[i]), 0));
            EXPECT_EQ(m.m_size, strlen(names[i]));
            EXPECT_STREQ(m.m_data, names[i]);   // read terminates the name
            mlFree(m.m_data);
        }
        EXPECT_TRUE(in.endList());
        EXPECT_TRUE(in.end());
    }
}

// The writers swap what they write, not the structure of the caller: the same
// structure can be written to files of both byte orders, and used afterwards.
// (The writers of MleDppOutput are private to writeMedia, which uses its own.)
TEST_F(ChunkFileClassTest, MediaRefWritersDoNotChangeTheStructures) {
    for (bool big : { true, false }) {
        SCOPED_TRACE(big ? "big endian" : "little endian");
        char name[] = "media/image.png";
        MleChunkFileHeader header = { 3, MleMrefChunkFile::PLAYPRINT_VERSION, 12345, 0 };
        MleMediaRefInfoChunk info = { 0x01020304, 0x0a0b0c0d, 1 };
        MleMediaRefChunk ref;
        ref.m_flags = MleMrefChunkFile::EXTERNAL;
        ref.m_type = mlDppMakeTag('l','o','g','o');
        ref.m_size = sizeof(name) - 1;
        ref.m_data = name;
        const MleMediaRefInfoChunk infoBefore = info;
        const MleMediaRefChunk refBefore = ref;

        {
            MleMrefChunkFile out(scratchFile("a.chk").c_str(), MleChunkFile::WRITING);
            out.setSwap(needsSwap(big));
            out.begin(big ? MLE_DPP_CREATEPPBE : MLE_DPP_CREATEPPLE);
            out.writeHeader(&header);
            out.beginList();
            EXPECT_TRUE(out.writeInfo(&info));
            EXPECT_TRUE(out.writeData(&ref));
            out.endList();
            out.end();
        }
        EXPECT_EQ(0, memcmp(&info, &infoBefore, sizeof(info)));
        EXPECT_EQ(0, memcmp(&ref, &refBefore, sizeof(ref)));

        // The file holds the values in its own byte order.
        std::vector<uint8_t> a;
        ASSERT_TRUE(readFile(scratchFile("a.chk"), a));
        const uint8_t want[4] = { 1, 2, 3, 4 };
        uint8_t expected[4];
        for (int i = 0; i < 4; i++)
            expected[i] = big ? want[i] : want[3 - i];
        EXPECT_NE(std::search(a.begin(), a.end(), expected, expected + 4), a.end());
    }
}

TEST_F(ChunkFileClassTest, InternalMediaRefKeepsItsBytes) {
    for (bool big : { true, false }) {
        std::string path = scratchFile("m.chk");
        char bytes[] = { 1, 2, 3, 0, 5 };
        MleMediaRefChunk m;
        m.m_flags = MleMrefChunkFile::INTERNAL;
        m.m_type = mlDppStringToTag(const_cast<char *>("data"), 0);
        m.m_size = sizeof(bytes);
        m.m_data = bytes;
        writeMrefFile(path, big, { m });

        MleMrefChunkFile in(path.c_str(), MleChunkFile::READING);
        in.setSwap(needsSwap(big));
        MleChunkFileHeader header;
        MleMediaRefInfoChunk info;
        MleMediaRefChunk back;
        ASSERT_TRUE(in.begin(big ? MLE_DPP_FINDPPBE : MLE_DPP_FINDPPLE));
        ASSERT_TRUE(in.readHeader(&header));
        ASSERT_TRUE(in.beginList());
        ASSERT_TRUE(in.readInfo(&info));
        ASSERT_TRUE(in.readData(&back));
        EXPECT_EQ(back.m_flags, static_cast<MlUInt>(MleMrefChunkFile::INTERNAL));
        ASSERT_EQ(back.m_size, sizeof(bytes));
        EXPECT_EQ(memcmp(back.m_data, bytes, sizeof(bytes)), 0);
        mlFree(back.m_data);
    }
}


/*** processMediaRef ***/

class ProcessMediaRefTest : public ScratchFixture {
  protected:
    void SetUp() override {
        ScratchFixture::SetUp();
        // Any chunk file object can process a MediaRef.
        m_file = new MleMrefChunkFile(scratchFile("p.chk").c_str(), MleChunkFile::WRITING);
    }
    void TearDown() override {
        delete m_file;
        ScratchFixture::TearDown();
    }
    MleMrefChunkFile *m_file = nullptr;
};

TEST_F(ProcessMediaRefTest, ExternalKeepsTheName) {
    MleMediaRefChunk in = external("media/a.png");
    MleMediaRefChunk *out = m_file->processMediaRef(&in);
    ASSERT_NE(out, nullptr);
    EXPECT_EQ(out->m_flags, static_cast<MlUInt>(MleMrefChunkFile::EXTERNAL));
    EXPECT_EQ(out->m_type, in.m_type);
    EXPECT_EQ(out->m_size, 11u);
    EXPECT_STREQ(out->m_data, "media/a.png");
}

TEST_F(ProcessMediaRefTest, InternalReadsTheFile) {
    std::vector<uint8_t> bytes = { 10, 20, 30, 40, 50, 60 };
    std::string media = scratchFile("media.bin");
    ASSERT_TRUE(writeFile(media, bytes));

    MleMediaRefChunk in;
    in.m_flags = MleMrefChunkFile::INTERNAL;
    in.m_type = mlDppStringToTag(const_cast<char *>("data"), 0);
    in.m_size = media.size();
    in.m_data = const_cast<char *>(media.c_str());
    MleMediaRefChunk *out = m_file->processMediaRef(&in);
    ASSERT_NE(out, nullptr);
    EXPECT_EQ(out->m_flags, static_cast<MlUInt>(MleMrefChunkFile::INTERNAL));
    EXPECT_EQ(out->m_size, bytes.size());           // the size of the file
    ASSERT_NE(out->m_data, nullptr);
    EXPECT_EQ(memcmp(out->m_data, bytes.data(), bytes.size()), 0);
    // The reference that was passed in is not changed.
    EXPECT_EQ(in.m_data, media.c_str());
}

TEST_F(ProcessMediaRefTest, MissingFileAndUnknownTypeAreRefused) {
    std::string missing = scratchFile("no-such-file");
    MleMediaRefChunk in;
    in.m_flags = MleMrefChunkFile::INTERNAL;
    in.m_type = 0;
    in.m_size = missing.size();
    in.m_data = const_cast<char *>(missing.c_str());
    EXPECT_EQ(m_file->processMediaRef(&in), nullptr);

    in.m_flags = 7;     // neither external nor internal
    EXPECT_EQ(m_file->processMediaRef(&in), nullptr);
}


/*** The table of chunk files ***/

TEST(ChunkTableTest, AddsAndFindsEntries) {
    MleDppChunkTable table;
    EXPECT_EQ(table.getUsed(), 0);

    table.addEntry(CHUNK_GROUP, const_cast<char *>("g.chk"), const_cast<char *>("group"), 10);
    table.addEntry(CHUNK_MEDIA, const_cast<char *>("m.chk"), const_cast<char *>("media"));
    table.addEntry(CHUNK_SET, const_cast<char *>("s.chk"), const_cast<char *>("set"), 30);
    table.addEntry(CHUNK_SCENE, const_cast<char *>("c.chk"), const_cast<char *>("scene"), 40);
    ASSERT_EQ(table.getUsed(), 4);

    EXPECT_EQ(table.getType(0), CHUNK_GROUP);
    EXPECT_EQ(table.getType(1), CHUNK_MEDIA);
    EXPECT_EQ(table.getType(2), CHUNK_SET);
    EXPECT_EQ(table.getType(3), CHUNK_SCENE);
    EXPECT_STREQ(table.getFilename(0), "g.chk");
    EXPECT_STREQ(table.getName(1), "media");
    EXPECT_EQ(table.getOffset(0), 10u);
    EXPECT_EQ(table.getOffset(1), 0u);      // the default

    table.setOffset(1, 99);
    EXPECT_EQ(table.getOffset(1), 99u);
}

TEST(ChunkTableTest, EntriesAreCopies) {
    MleDppChunkTable table;
    char filename[] = "file.chk", name[] = "name";
    table.addEntry(CHUNK_GROUP, filename, name);
    strcpy(filename, "other");
    strcpy(name, "xxxx");
    EXPECT_STREQ(table.getFilename(0), "file.chk");
    EXPECT_STREQ(table.getName(0), "name");
}

TEST(ChunkTableTest, IndexBeyondTheEntriesGivesNothing) {
    MleDppChunkTable table;
    table.addEntry(CHUNK_GROUP, const_cast<char *>("g.chk"), const_cast<char *>("group"));
    EXPECT_EQ(table.getFilename(1), nullptr);
    EXPECT_EQ(table.getName(1), nullptr);
    EXPECT_EQ(table.getOffset(1), 0u);
    EXPECT_EQ(table.getFilename(1000), nullptr);
    table.setOffset(5, 1);                  // ignored
    EXPECT_EQ(table.getUsed(), 1);
}

TEST(ChunkTableTest, GrowsBeyondItsFirstAllocation) {
    MleDppChunkTable table;
    for (int i = 0; i < 300; i++) {
        std::string file = "f" + std::to_string(i), name = "n" + std::to_string(i);
        table.addEntry(CHUNK_SET, const_cast<char *>(file.c_str()), const_cast<char *>(name.c_str()), i);
    }
    ASSERT_EQ(table.getUsed(), 300);
    for (int i : { 0, 127, 128, 255, 256, 299 }) {
        EXPECT_EQ(std::string(table.getFilename(i)), "f" + std::to_string(i));
        EXPECT_EQ(std::string(table.getName(i)), "n" + std::to_string(i));
        EXPECT_EQ(table.getOffset(i), static_cast<MlUInt>(i));
    }
}
