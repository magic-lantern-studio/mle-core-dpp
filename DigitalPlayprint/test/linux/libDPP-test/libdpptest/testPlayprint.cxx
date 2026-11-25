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

// Phase 2 tests of libplayprint: MleDppOutput writes a playprint and
// MleDppInput reads it, in big and little endian.

// Include system header files.
#include <string.h>

// Include DPP test header files.
#include "libdpptest.h"

// Include Magic Lantern header files.
#include <mle/agopcode.h>
#include <mle/mlMalloc.h>
#include <mle/playprnt.h>
#include <mle/ppinput.h>
#include <mle/ppoutput.h>

using namespace dpptest;

namespace {

uint32_t intIn(const std::vector<uint8_t> &data, size_t at, bool big) {
    uint32_t v = 0;
    for (int i = 0; i < 4; i++)
        v = (v << 8) | data[at + (big ? i : 3 - i)];
    return v;
}

// What the Group chunk of the fixture playprint holds: the stream of the Group
// chunk file without the offset of the names, and with the index of the media
// name written as its index, 1, in the table of chunk files.
std::vector<uint8_t> resolvedGroup() {
    return { 7, 0, 0, 0,                        // the id of the group
             1,                                 // one Actor
             createActorOpcode,
             setPropertyLengthOpcode, 4,
             setPropertyOffsetOpcode,
             copyPropertyOpcode, 0x11, 0x22, 0x33, 0x44,
             copyMediaRefOpcode, 1,             // was 0
             endActorGroupOpcode };
}

std::vector<uint8_t> setData() {
    std::vector<uint8_t> chunk = setChunkBytes();
    return std::vector<uint8_t>(chunk.begin() + 4, chunk.end());
}

std::vector<uint8_t> sceneData() {
    std::vector<uint8_t> chunk = sceneChunkBytes();
    return std::vector<uint8_t>(chunk.begin() + 4, chunk.end());
}

// What the callbacks of a MleDppInput collect.
struct Collected {
    int headerCalls = 0, tocCalls = 0, groupCalls = 0, mrefBeginCalls = 0,
        mrefCalls = 0, mrefEndCalls = 0, setCalls = 0, sceneCalls = 0;
    MleDppHeader header = { 0, 0 };
    std::vector<std::pair<int, unsigned>> toc;           // label, offset
    std::vector<uint8_t> group, set, scene;
    MleMediaRefInfoChunk mrefInfo = { 0, 0, 0 };
    MlUInt mrefFlags = 99, mrefSize = 0;
    MlDppTag mrefType = 0;
    std::string mrefName;
    size_t groupSize = 0, setSize = 0, sceneSize = 0;      // set by the test
};

int onHeader(void *client, void *data) {
    Collected *c = static_cast<Collected *>(client);
    c->headerCalls++;
    c->header = *static_cast<MleDppHeader *>(data);
    return TRUE;
}

int onToc(void *client, void *data) {
    Collected *c = static_cast<Collected *>(client);
    c->tocCalls++;
    for (MleDppTOC *e = static_cast<MleDppTOC *>(data); e != NULL; e = e->m_next)
        c->toc.push_back({ e->m_entry.m_label, e->m_entry.m_offset });
    return TRUE;
}

int onGroup(void *client, void *data) {
    Collected *c = static_cast<Collected *>(client);
    c->groupCalls++;
    const uint8_t *p = static_cast<uint8_t *>(data);
    c->group.assign(p, p + c->groupSize);
    return TRUE;
}

int onSet(void *client, void *data) {
    Collected *c = static_cast<Collected *>(client);
    c->setCalls++;
    const uint8_t *p = static_cast<uint8_t *>(data);
    c->set.assign(p, p + c->setSize);
    return TRUE;
}

int onScene(void *client, void *data) {
    Collected *c = static_cast<Collected *>(client);
    c->sceneCalls++;
    const uint8_t *p = static_cast<uint8_t *>(data);
    c->scene.assign(p, p + c->sceneSize);
    return TRUE;
}

int onMrefBegin(void *client, void *data) {
    Collected *c = static_cast<Collected *>(client);
    c->mrefBeginCalls++;
    c->mrefInfo = *static_cast<MleMediaRefInfoChunk *>(data);
    return TRUE;
}

int onMref(void *client, void *data) {
    Collected *c = static_cast<Collected *>(client);
    c->mrefCalls++;
    MleMediaRefChunk *m = static_cast<MleMediaRefChunk *>(data);
    c->mrefFlags = m->m_flags;
    c->mrefType = m->m_type;
    c->mrefSize = m->m_size;
    c->mrefName = m->m_data;
    return TRUE;
}

int onMrefEnd(void *client, void *) {
    static_cast<Collected *>(client)->mrefEndCalls++;
    return TRUE;
}

} // anonymous namespace

class PlayprintTest : public ScratchFixture {
  protected:
    // Build the playprint of the fixture in the given byte order.
    void build(bool big) {
        ASSERT_TRUE(buildPlayprint(scratch(), big, m_files));
    }
    PlayprintFiles m_files;
};


/*** Writing ***/

TEST_F(PlayprintTest, WritesTheChunksInOrder) {
    for (bool big : { true, false }) {
        build(big);
        ChunkFile file;
        ASSERT_TRUE(readChunkFile(m_files.playprint, file)) << file.error;
        EXPECT_EQ(file.bigEndian, big);
        ASSERT_EQ(file.chunks.size(), 1u);

        const Chunk &form = file.chunks[0];
        EXPECT_EQ(form.tag, big ? "PPBE" : "PPLE");
        EXPECT_EQ(form.form, "MLRT");
        ASSERT_EQ(form.children.size(), 6u);
        const char *expected[] = { "hdr ", "toc ", "grp ", "LIST", "set ", "scn " };
        for (size_t i = 0; i < 6; i++)
            EXPECT_EQ(form.children[i].tag, expected[i]) << "chunk " << i;

        // The form is as large as what it holds.
        size_t held = 0;
        for (const Chunk &c : form.children)
            held += 8 + (c.form.empty() ? c.size : c.size + 4) + (c.size & 1 && c.form.empty() ? 1 : 0);
        EXPECT_EQ(form.size, held);
    }
}

TEST_F(PlayprintTest, WritesTheHeader) {
    for (bool big : { true, false }) {
        build(big);
        ChunkFile file;
        ASSERT_TRUE(readChunkFile(m_files.playprint, file)) << file.error;
        const Chunk &hdr = file.chunks[0].children[0];
        ASSERT_EQ(hdr.data.size(), 8u);
        EXPECT_EQ(intIn(hdr.data, 0, big), static_cast<uint32_t>(MleDpp::PLAYPRINT_VERSION));
        EXPECT_GT(intIn(hdr.data, 4, big), 0u);   // the date
    }
}

TEST_F(PlayprintTest, TableOfContentsHasTheOffsetsOfTheChunks) {
    for (bool big : { true, false }) {
        build(big);
        ChunkFile file;
        ASSERT_TRUE(readChunkFile(m_files.playprint, file)) << file.error;
        const std::vector<Chunk> &kids = file.chunks[0].children;
        const Chunk &toc = kids[1];

        // The count, then a label and an offset for the Group, the MediaRef
        // list, the Set and the Scene.
        ASSERT_EQ(toc.data.size(), 4u + 4u * 8u);
        EXPECT_EQ(intIn(toc.data, 0, big), 4u);
        const size_t chunks[4] = { 2, 3, 4, 5 };
        for (int i = 0; i < 4; i++) {
            EXPECT_EQ(intIn(toc.data, 4 + 8 * i + 4, big), kids[chunks[i]].offset)
                << "entry " << i;
        }
    }
}

TEST_F(PlayprintTest, WritesTheGroupWithTheResolvedMediaIndex) {
    for (bool big : { true, false }) {
        build(big);
        ChunkFile file;
        ASSERT_TRUE(readChunkFile(m_files.playprint, file)) << file.error;
        EXPECT_EQ(file.chunks[0].children[2].data, resolvedGroup());
    }
}

TEST_F(PlayprintTest, WritesTheSetAndTheScene) {
    for (bool big : { true, false }) {
        build(big);
        ChunkFile file;
        ASSERT_TRUE(readChunkFile(m_files.playprint, file)) << file.error;
        EXPECT_EQ(file.chunks[0].children[4].data, setData());
        EXPECT_EQ(file.chunks[0].children[5].data, sceneData());
    }
}

TEST_F(PlayprintTest, WritesTheMediaRefList) {
    for (bool big : { true, false }) {
        build(big);
        ChunkFile file;
        ASSERT_TRUE(readChunkFile(m_files.playprint, file)) << file.error;
        const Chunk &list = file.chunks[0].children[3];
        EXPECT_EQ(list.form, "MRFL");
        ASSERT_EQ(list.children.size(), 2u);
        EXPECT_EQ(list.children[0].tag, "info");
        EXPECT_EQ(list.children[1].tag, "mref");

        // info: flags, type, number of MediaRefs; mref: flags, type, size, name.
        const std::vector<uint8_t> &info = list.children[0].data;
        ASSERT_EQ(info.size(), 12u);
        EXPECT_EQ(intIn(info, 8, big), 1u);
        const std::vector<uint8_t> &mref = list.children[1].data;
        EXPECT_EQ(intIn(mref, 0, big), 0u);      // external
        EXPECT_EQ(intIn(mref, 4, big), static_cast<uint32_t>(mlDppMakeTag('l','o','g','o')));
        EXPECT_EQ(intIn(mref, 8, big), std::string("media/image.png").size());
        EXPECT_EQ(std::string(mref.begin() + 12, mref.end()), "media/image.png");
    }
}

TEST_F(PlayprintTest, HeaderOnlyPlayprint) {
    for (bool big : { true, false }) {
        std::string path = scratchFile("header.dpp");
        {
            MleDppOutput out(path.c_str());
            out.setSwap(needsSwap(big));
            out.begin(big ? MLE_DPP_CREATEPPBE : MLE_DPP_CREATEPPLE);
            EXPECT_TRUE(out.writeHeader());
            out.end();
        }
        ChunkFile file;
        ASSERT_TRUE(readChunkFile(path, file)) << file.error;
        ASSERT_EQ(file.chunks[0].children.size(), 1u);
        EXPECT_EQ(file.chunks[0].children[0].tag, "hdr ");
    }
}


/*** Reading ***/

TEST_F(PlayprintTest, ReadsWhatWasWritten) {
    for (bool big : { true, false }) {
        build(big);
        Collected c;
        c.groupSize = resolvedGroup().size();
        c.setSize = setData().size();
        c.sceneSize = sceneData().size();

        MleDppInput in(m_files.playprint.c_str());
        in.setSwap(needsSwap(big));
        ASSERT_NE(in.getFp(), nullptr);
        EXPECT_EQ(mlDppCheckFormat(in.getFp()), big ? 1 : 2);
        ASSERT_TRUE(in.begin(big ? MLE_DPP_FINDPPBE : MLE_DPP_FINDPPLE));

        EXPECT_TRUE(in.readHeader(onHeader, &c));
        EXPECT_EQ(c.headerCalls, 1);
        EXPECT_EQ(c.header.m_version, static_cast<MlUInt>(MleDpp::PLAYPRINT_VERSION));
        EXPECT_GT(c.header.m_date, 0);

        EXPECT_TRUE(in.readTOC(onToc, &c));
        EXPECT_EQ(c.tocCalls, 1);
        EXPECT_EQ(c.toc.size(), 4u);

        EXPECT_TRUE(in.readGroup(onGroup, &c));
        EXPECT_EQ(c.groupCalls, 1);
        EXPECT_EQ(c.group, resolvedGroup());

        EXPECT_TRUE(in.beginMref(onMrefBegin, &c));
        EXPECT_EQ(c.mrefInfo.m_numMrefs, 1u);
        EXPECT_TRUE(in.readMedia(onMref, &c));
        EXPECT_EQ(c.mrefCalls, 1);
        EXPECT_EQ(c.mrefFlags, 0u);
        EXPECT_EQ(c.mrefType, mlDppMakeTag('l','o','g','o'));
        EXPECT_EQ(c.mrefName, "media/image.png");
        EXPECT_EQ(c.mrefSize, c.mrefName.size() + 1);   // with the terminator
        EXPECT_TRUE(in.endMref(onMrefEnd, &c));
        EXPECT_EQ(c.mrefEndCalls, 1);

        EXPECT_TRUE(in.readSet(onSet, &c));
        EXPECT_EQ(c.set, setData());

        EXPECT_TRUE(in.readScene(onScene, &c));
        EXPECT_EQ(c.scene, sceneData());

        EXPECT_TRUE(in.end());
    }
}

TEST_F(PlayprintTest, TableOfContentsFindsEachChunk) {
    for (bool big : { true, false }) {
        build(big);
        ChunkFile tree;
        ASSERT_TRUE(readChunkFile(m_files.playprint, tree)) << tree.error;
        const std::vector<Chunk> &kids = tree.chunks[0].children;

        Collected c;
        c.setSize = setData().size();
        c.sceneSize = sceneData().size();
        MleDppInput in(m_files.playprint.c_str());
        in.setSwap(needsSwap(big));
        ASSERT_TRUE(in.begin(big ? MLE_DPP_FINDPPBE : MLE_DPP_FINDPPLE));
        ASSERT_TRUE(in.readHeader(NULL, NULL));
        ASSERT_TRUE(in.readTOC(onToc, &c));
        ASSERT_EQ(c.toc.size(), 4u);

        // The offsets are those of the chunks in the file, and an entry
        // can be used to go to a chunk, in any order: the Scene, then the Set.
        EXPECT_EQ(in.getTOCOffset(0), static_cast<MlInt>(kids[2].offset));
        EXPECT_EQ(in.getTOCOffset(1), static_cast<MlInt>(kids[3].offset));
        EXPECT_EQ(in.getTOCOffset(2), static_cast<MlInt>(kids[4].offset));
        EXPECT_EQ(in.getTOCOffset(3), static_cast<MlInt>(kids[5].offset));

        in.goTo(in.getTOCOffset(3));
        EXPECT_TRUE(in.readScene(onScene, &c));
        EXPECT_EQ(c.scene, sceneData());
        in.goTo(in.getTOCOffset(2));
        EXPECT_TRUE(in.readSet(onSet, &c));
        EXPECT_EQ(c.set, setData());
    }
}

TEST_F(PlayprintTest, ReadsWithoutCallbacks) {
    build(false);
    MleDppInput in(m_files.playprint.c_str());
    in.setSwap(needsSwap(false));
    ASSERT_TRUE(in.begin(MLE_DPP_FINDPPLE));
    EXPECT_TRUE(in.readHeader(NULL, NULL));
    EXPECT_TRUE(in.readTOC(NULL, NULL));
    EXPECT_TRUE(in.readGroup(NULL, NULL));
}

// The playprint is in one byte order, and reading it in the other finds
// nothing: the form of the file is not there.
TEST_F(PlayprintTest, OtherByteOrderIsNotFound) {
    build(true);
    MleDppInput in(m_files.playprint.c_str());
    EXPECT_EQ(mlDppCheckFormat(in.getFp()), 1);
    ChildResult r = runInChild([&] {
        MleDppChunkInfo info;
        memset(&info, 0, sizeof(info));
        info.m_form = mlDppMakeTag('M','L','R','T');
        _exit(mlDppDescend(in.getFp(), &info, NULL, MLE_DPP_FINDPPLE) ? 1 : 0);
    });
    ASSERT_TRUE(r.exited);
    EXPECT_EQ(r.exitCode, 0);
}
