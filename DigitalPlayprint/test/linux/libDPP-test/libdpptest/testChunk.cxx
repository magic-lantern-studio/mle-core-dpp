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

// Phase 2 tests of libplayprint: the chunk functions that read and write the
// IFF-like structure of a chunk file: create, ascend and descend, the format
// of the file and how much of the file is left.

// Include system header files.
#include <stdio.h>

// Include DPP test header files.
#include "libdpptest.h"

// Include Magic Lantern header files.
#include <mle/playprnt.h>

using namespace dpptest;

namespace {

MlDppTag tagOf(const char *t) { return mlDppMakeTag(t[0], t[1], t[2], t[3]); }

// A chunk that is not a form, ready to be created.
MleDppChunkInfo plainChunk(const char *tag, MlUInt size) {
    MleDppChunkInfo info;
    memset(&info, 0, sizeof(info));
    info.m_chunk.m_tag = tagOf(tag);
    info.m_chunk.m_size = size;
    return info;
}

// Write bytes to the file.
void put(FILE *fp, std::initializer_list<uint8_t> bytes) {
    std::vector<uint8_t> v(bytes);
    ASSERT_EQ(fwrite(v.data(), 1, v.size(), fp), v.size());
}

} // anonymous namespace

class ChunkTest : public ScratchFixture {};


/*** Writing ***/

// A form with two chunks: one of odd size (3), which has a pad byte, and one
// of even size (4). The size of the form is not known when it is created, and
// ascend writes it.
TEST_F(ChunkTest, CreateAndAscendWriteTheSizes) {
    for (bool big : { true, false }) {
        std::string path = scratchFile(big ? "be.bin" : "le.bin");
        FILE *fp = fopen(path.c_str(), "wb+");
        ASSERT_NE(fp, nullptr);
        {
            SwapGuard swap(false, needsSwap(big));

            MleDppChunkInfo form = plainChunk("    ", 0);
            form.m_form = tagOf("MLRT");
            ASSERT_TRUE(mlDppCreateChunk(fp, &form, big ? MLE_DPP_CREATEPPBE : MLE_DPP_CREATEPPLE));
            EXPECT_EQ(form.m_chunk.m_tag, big ? MLE_PPBE_TAG : MLE_PPLE_TAG);
            EXPECT_EQ(form.m_offset, 12u);  // after the tag, the size and the form
            EXPECT_TRUE(form.m_flags & MLE_DPP_DIRTY);

            MleDppChunkInfo odd = plainChunk("odd ", 3);
            ASSERT_TRUE(mlDppCreateChunk(fp, &odd, 0));
            EXPECT_EQ(odd.m_offset, 20u);   // after the tag and the size
            put(fp, { 1, 2, 3 });
            ASSERT_TRUE(mlDppAscend(fp, &odd, 0));  // writes the pad byte

            MleDppChunkInfo even = plainChunk("even", 4);
            ASSERT_TRUE(mlDppCreateChunk(fp, &even, 0));
            EXPECT_EQ(even.m_offset, 32u);  // 20 + 3 + 1 pad + 8
            put(fp, { 4, 5, 6, 7 });
            ASSERT_TRUE(mlDppAscend(fp, &even, 0));

            ASSERT_TRUE(mlDppAscend(fp, &form, 0));
            EXPECT_EQ(form.m_chunk.m_size, 24u);    // 8 + 3 + 1 + 8 + 4
        }
        fclose(fp);

        ChunkFile file;
        ASSERT_TRUE(readChunkFile(path, file)) << file.error;
        EXPECT_EQ(file.bigEndian, big);
        ASSERT_EQ(file.chunks.size(), 1u);
        EXPECT_EQ(file.chunks[0].tag, big ? "PPBE" : "PPLE");
        EXPECT_EQ(file.chunks[0].form, "MLRT");
        EXPECT_EQ(file.chunks[0].size, 24u);
        ASSERT_EQ(file.chunks[0].children.size(), 2u);
        EXPECT_EQ(file.chunks[0].children[0].tag, "odd ");
        EXPECT_EQ(file.chunks[0].children[0].data, (std::vector<uint8_t>{ 1, 2, 3 }));
        EXPECT_EQ(file.chunks[0].children[1].tag, "even");
        EXPECT_EQ(file.chunks[0].children[1].data, (std::vector<uint8_t>{ 4, 5, 6, 7 }));
    }
}

// The size that was given when a chunk was created is not trusted: ascend
// writes the size that was written.
TEST_F(ChunkTest, AscendCorrectsAWrongSize) {
    std::string path = scratchFile("a.bin");
    FILE *fp = fopen(path.c_str(), "wb+");
    ASSERT_NE(fp, nullptr);
    SwapGuard swap(false, needsSwap(true));

    MleDppChunkInfo form = plainChunk("    ", 0);
    form.m_form = tagOf("MLRT");
    ASSERT_TRUE(mlDppCreateChunk(fp, &form, MLE_DPP_CREATEPPBE));
    MleDppChunkInfo chunk = plainChunk("big ", 100);   // but only 2 bytes are written
    ASSERT_TRUE(mlDppCreateChunk(fp, &chunk, 0));
    put(fp, { 9, 8 });
    ASSERT_TRUE(mlDppAscend(fp, &chunk, 0));
    EXPECT_EQ(chunk.m_chunk.m_size, 2u);
    ASSERT_TRUE(mlDppAscend(fp, &form, 0));
    fclose(fp);

    ChunkFile file;
    ASSERT_TRUE(readChunkFile(path, file)) << file.error;
    ASSERT_EQ(file.chunks[0].children.size(), 1u);
    EXPECT_EQ(file.chunks[0].children[0].size, 2u);
}

TEST_F(ChunkTest, ZeroSizeAndNestedChunks) {
    std::string path = scratchFile("a.bin");
    FILE *fp = fopen(path.c_str(), "wb+");
    ASSERT_NE(fp, nullptr);
    {
        SwapGuard swap(false, needsSwap(true));
        MleDppChunkInfo form = plainChunk("    ", 0);
        form.m_form = tagOf("MLRT");
        ASSERT_TRUE(mlDppCreateChunk(fp, &form, MLE_DPP_CREATEPPBE));

        MleDppChunkInfo list = plainChunk("    ", 0);
        list.m_form = tagOf("MRFL");
        ASSERT_TRUE(mlDppCreateChunk(fp, &list, MLE_DPP_CREATELIST));

        MleDppChunkInfo empty = plainChunk("zero", 0);
        ASSERT_TRUE(mlDppCreateChunk(fp, &empty, 0));
        ASSERT_TRUE(mlDppAscend(fp, &empty, 0));    // no pad byte for size 0

        MleDppChunkInfo one = plainChunk("one ", 1);
        ASSERT_TRUE(mlDppCreateChunk(fp, &one, 0));
        put(fp, { 0xaa });
        ASSERT_TRUE(mlDppAscend(fp, &one, 0));      // a pad byte for size 1

        ASSERT_TRUE(mlDppAscend(fp, &list, 0));
        ASSERT_TRUE(mlDppAscend(fp, &form, 0));
    }
    fclose(fp);

    ChunkFile file;
    ASSERT_TRUE(readChunkFile(path, file)) << file.error;
    ASSERT_EQ(file.chunks.size(), 1u);
    const Chunk &list = file.chunks[0].children.at(0);
    EXPECT_EQ(list.tag, "LIST");
    EXPECT_EQ(list.form, "MRFL");
    ASSERT_EQ(list.children.size(), 2u);
    EXPECT_EQ(list.children[0].tag, "zero");
    EXPECT_EQ(list.children[0].size, 0u);
    EXPECT_EQ(list.children[1].tag, "one ");
    EXPECT_EQ(list.children[1].data, (std::vector<uint8_t>{ 0xaa }));
    // 4 (form) + 8 (zero) + 8 + 1 + 1 pad (one) = 18 for the list: the sizes
    // add up, so a reader finds the chunk that follows.
    EXPECT_EQ(list.size, 18u);
    EXPECT_EQ(file.chunks[0].size, 8u + 4u + 18u);
}


/*** Reading ***/

// A playprint in memory: a header, a list with two chunks, and a last chunk.
static std::vector<uint8_t> sampleBytes(bool big) {
    ChunkBuilder b(big);
    b.form(big ? "PPBE" : "PPLE", "MLRT", [&] {
        b.chunk("hdr ", { 0, 0, 0, 1, 0, 0, 0, 2 });
        b.form("LIST", "MRFL", [&] {
            b.chunk("info", { 1, 2, 3 });
            b.chunk("mref", { 4, 5 });
        });
        b.chunk("last", { 9 });
    });
    return b.bytes();
}

class ChunkReadTest : public ScratchFixture {
  protected:
    FILE *open(bool big) {
        std::string path = scratchFile("a.bin");
        EXPECT_TRUE(writeFile(path, sampleBytes(big)));
        return fopen(path.c_str(), "rb");
    }
};

TEST_F(ChunkReadTest, DescendReadsTheNextChunk) {
    for (bool big : { true, false }) {
        FILE *fp = open(big);
        ASSERT_NE(fp, nullptr);
        SwapGuard swap(needsSwap(big), false);

        MleDppChunkInfo form;
        memset(&form, 0, sizeof(form));
        ASSERT_TRUE(mlDppDescend(fp, &form, NULL, 0));
        EXPECT_EQ(form.m_chunk.m_tag, big ? MLE_PPBE_TAG : MLE_PPLE_TAG);
        EXPECT_EQ(form.m_form, tagOf("MLRT"));
        EXPECT_EQ(form.m_offset, 12u);

        // The next chunk starts inside the form, after the form tag.
        MleDppChunkInfo hdr;
        memset(&hdr, 0, sizeof(hdr));
        ASSERT_TRUE(mlDppDescend(fp, &hdr, NULL, 0));
        EXPECT_EQ(hdr.m_chunk.m_tag, tagOf("hdr "));
        EXPECT_EQ(hdr.m_chunk.m_size, 8u);
        EXPECT_EQ(hdr.m_offset, 20u);
        ASSERT_TRUE(mlDppAscend(fp, &hdr, 0));

        // After the ascend the file is after the chunk, at the next one.
        MleDppChunkInfo list;
        memset(&list, 0, sizeof(list));
        ASSERT_TRUE(mlDppDescend(fp, &list, NULL, 0));
        EXPECT_EQ(list.m_chunk.m_tag, MLE_LIST_TAG);
        EXPECT_EQ(list.m_form, tagOf("MRFL"));
        fclose(fp);
    }
}

TEST_F(ChunkReadTest, AscendSkipsThePadByte) {
    FILE *fp = open(false);
    ASSERT_NE(fp, nullptr);
    SwapGuard swap(needsSwap(false), false);

    MleDppChunkInfo form, list, info, mref;
    memset(&form, 0, sizeof(form));
    memset(&list, 0, sizeof(list));
    memset(&info, 0, sizeof(info));
    memset(&mref, 0, sizeof(mref));
    ASSERT_TRUE(mlDppDescend(fp, &form, NULL, 0));
    MleDppChunkInfo hdr;
    memset(&hdr, 0, sizeof(hdr));
    ASSERT_TRUE(mlDppDescend(fp, &hdr, NULL, 0));
    ASSERT_TRUE(mlDppAscend(fp, &hdr, 0));
    ASSERT_TRUE(mlDppDescend(fp, &list, NULL, 0));
    ASSERT_TRUE(mlDppDescend(fp, &info, NULL, 0));   // size 3: odd
    EXPECT_EQ(info.m_chunk.m_tag, tagOf("info"));
    ASSERT_TRUE(mlDppAscend(fp, &info, 0));
    ASSERT_TRUE(mlDppDescend(fp, &mref, NULL, 0));   // found after the pad byte
    EXPECT_EQ(mref.m_chunk.m_tag, tagOf("mref"));
    EXPECT_EQ(mref.m_chunk.m_size, 2u);
    fclose(fp);
}

TEST_F(ChunkReadTest, FindsTheForm) {
    FILE *fp = open(true);
    ASSERT_NE(fp, nullptr);
    SwapGuard swap(needsSwap(true), false);

    MleDppChunkInfo form;
    memset(&form, 0, sizeof(form));
    form.m_form = tagOf("MLRT");
    ASSERT_TRUE(mlDppDescend(fp, &form, NULL, MLE_DPP_FINDPPBE));
    EXPECT_EQ(form.m_chunk.m_tag, MLE_PPBE_TAG);
    EXPECT_EQ(form.m_offset, 12u);

    // Not a form of this kind, or of this type.
    MleDppChunkInfo wrongOrder;
    memset(&wrongOrder, 0, sizeof(wrongOrder));
    wrongOrder.m_form = tagOf("MLRT");
    EXPECT_FALSE(mlDppDescend(fp, &wrongOrder, NULL, MLE_DPP_FINDPPLE));
    MleDppChunkInfo wrongForm;
    memset(&wrongForm, 0, sizeof(wrongForm));
    wrongForm.m_form = tagOf("XXXX");
    EXPECT_FALSE(mlDppDescend(fp, &wrongForm, NULL, MLE_DPP_FINDPPBE));
    fclose(fp);
}

TEST_F(ChunkReadTest, FindsAChunkAndAListInAForm) {
    FILE *fp = open(false);
    ASSERT_NE(fp, nullptr);
    SwapGuard swap(needsSwap(false), false);

    MleDppChunkInfo form;
    memset(&form, 0, sizeof(form));
    form.m_form = tagOf("MLRT");
    ASSERT_TRUE(mlDppDescend(fp, &form, NULL, MLE_DPP_FINDPPLE));

    // A chunk after others (the list and its chunks are skipped).
    MleDppChunkInfo last = plainChunk("last", 0);
    ASSERT_TRUE(mlDppDescend(fp, &last, &form, MLE_DPP_FINDCHUNK));
    EXPECT_EQ(last.m_chunk.m_size, 1u);

    MleDppChunkInfo list;
    memset(&list, 0, sizeof(list));
    list.m_form = tagOf("MRFL");
    ASSERT_TRUE(mlDppDescend(fp, &list, &form, MLE_DPP_FINDLIST));
    EXPECT_EQ(list.m_chunk.m_tag, MLE_LIST_TAG);

    // A chunk that is not there.
    MleDppChunkInfo missing = plainChunk("none", 0);
    EXPECT_FALSE(mlDppDescend(fp, &missing, &form, MLE_DPP_FINDCHUNK));
    fclose(fp);
}

TEST_F(ChunkReadTest, EmptyAndShortFiles) {
    for (size_t n : { 0, 1, 7 }) {
        std::string path = scratchFile("short.bin");
        ASSERT_TRUE(writeFile(path, std::vector<uint8_t>(n, 'P')));
        FILE *fp = fopen(path.c_str(), "rb");
        ASSERT_NE(fp, nullptr);
        MleDppChunkInfo info;
        memset(&info, 0, sizeof(info));
        EXPECT_FALSE(mlDppDescend(fp, &info, NULL, 0)) << n << " bytes";
        fclose(fp);
    }
}


/*** The format of a file ***/

TEST_F(ChunkReadTest, CheckFormat) {
    FILE *be = open(true);
    ASSERT_NE(be, nullptr);
    EXPECT_EQ(mlDppCheckFormat(be), 1);
    fclose(be);

    FILE *le = open(false);
    ASSERT_NE(le, nullptr);
    EXPECT_EQ(mlDppCheckFormat(le), 2);

    // The position in the file is not changed.
    fseek(le, 5, SEEK_SET);
    EXPECT_EQ(mlDppCheckFormat(le), 2);
    EXPECT_EQ(ftell(le), 5);
    fclose(le);
}

TEST_F(ChunkReadTest, CheckFormatOfBadFiles) {
    struct { const char *name; std::vector<uint8_t> bytes; } cases[] = {
        { "empty", {} },
        { "short", { 'P', 'P' } },
        { "wrong tag", { 'R', 'I', 'F', 'F', 0, 0, 0, 0 } },
        { "text", { 'h', 'e', 'l', 'l', 'o', '\n' } },
    };
    for (auto &c : cases) {
        std::string path = scratchFile("bad.bin");
        ASSERT_TRUE(writeFile(path, c.bytes));
        FILE *fp = fopen(path.c_str(), "rb");
        ASSERT_NE(fp, nullptr);
        EXPECT_EQ(mlDppCheckFormat(fp), -1) << c.name;
        fclose(fp);
    }
}


/*** How much of the file is left ***/

TEST_F(ChunkReadTest, BytesRemain) {
    std::string path = scratchFile("a.bin");
    ASSERT_TRUE(writeFile(path, std::vector<uint8_t>(100, 0)));
    FILE *fp = fopen(path.c_str(), "rb");
    ASSERT_NE(fp, nullptr);

    EXPECT_TRUE(mlDppBytesRemain(fp, 0));
    EXPECT_TRUE(mlDppBytesRemain(fp, 100));
    EXPECT_FALSE(mlDppBytesRemain(fp, 101));
    EXPECT_FALSE(mlDppBytesRemain(fp, 0xffffffffu));
    EXPECT_EQ(ftell(fp), 0);        // the position is not changed

    fseek(fp, 60, SEEK_SET);
    EXPECT_TRUE(mlDppBytesRemain(fp, 40));
    EXPECT_FALSE(mlDppBytesRemain(fp, 41));
    EXPECT_EQ(ftell(fp), 60);

    fseek(fp, 0, SEEK_END);
    EXPECT_TRUE(mlDppBytesRemain(fp, 0));
    EXPECT_FALSE(mlDppBytesRemain(fp, 1));
    fclose(fp);
}

// A chunk can say that it is larger than the file. Reading its header works;
// a reader has to check the size before it uses it.
TEST_F(ChunkReadTest, ChunkLargerThanTheFile) {
    ChunkBuilder b(true);
    b.tag("PPBE");
    b.putInt(0x7fffff00);       // the form says it is huge
    b.tag("MLRT");
    b.tag("huge");
    b.putInt(0x10000000);       // so does the chunk
    for (int i = 0; i < 4; i++)
        b.putInt(i);            // but only 16 bytes follow
    std::string path = scratchFile("huge.bin");
    ASSERT_TRUE(writeFile(path, b.bytes()));

    FILE *fp = fopen(path.c_str(), "rb");
    ASSERT_NE(fp, nullptr);
    SwapGuard swap(needsSwap(true), false);
    MleDppChunkInfo form, chunk;
    memset(&form, 0, sizeof(form));
    memset(&chunk, 0, sizeof(chunk));
    ASSERT_TRUE(mlDppDescend(fp, &form, NULL, 0));
    ASSERT_TRUE(mlDppDescend(fp, &chunk, NULL, 0));
    EXPECT_EQ(chunk.m_chunk.m_size, 0x10000000u);
    EXPECT_FALSE(mlDppBytesRemain(fp, chunk.m_chunk.m_size));
    EXPECT_TRUE(mlDppBytesRemain(fp, 16));
    fclose(fp);

    // The tree reader refuses it too.
    ChunkFile file;
    EXPECT_FALSE(readChunkFile(path, file));
}
