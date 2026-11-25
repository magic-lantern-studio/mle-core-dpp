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

// Fixtures for the tests of libplayprint: chunk files and a playprint, built
// as the mastering tools build them.

// Include system header files.
#include <string.h>

// Include DPP test header files.
#include "libdpptest.h"

// Include Magic Lantern header files.
#include <mle/agopcode.h>
#include <mle/chktable.h>
#include <mle/mrefchk.h>
#include <mle/playprnt.h>
#include <mle/ppoutput.h>

namespace dpptest {

/*** Byte order ***/

bool hostIsLittle() {
    unsigned short one = 1;
    return *reinterpret_cast<unsigned char *>(&one) == 1;
}

SwapGuard::SwapGuard(bool read, bool write) : m_read(read), m_write(write) {
    if (m_read)
        mlDppSetSwapRead(TRUE);
    if (m_write)
        mlDppSetSwapWrite(TRUE);
}

SwapGuard::~SwapGuard() {
    if (m_read)
        mlDppSetSwapRead(FALSE);
    if (m_write)
        mlDppSetSwapWrite(FALSE);
}


/*** ChunkBuilder ***/

void ChunkBuilder::putInt(uint32_t v) {
    for (int i = 0; i < 4; i++)
        m_bytes.push_back((m_big ? v >> (24 - 8 * i) : v >> (8 * i)) & 0xff);
}

void ChunkBuilder::chunk(const char *t, const std::vector<uint8_t> &data) {
    tag(t);
    putInt(data.size());
    m_bytes.insert(m_bytes.end(), data.begin(), data.end());
    if (data.size() & 1)
        m_bytes.push_back(0);
}

void ChunkBuilder::form(const char *t, const char *f, const std::function<void()> &children) {
    tag(t);
    size_t sizeAt = m_bytes.size();
    putInt(0);
    tag(f);
    size_t start = m_bytes.size();
    children();
    uint32_t size = m_bytes.size() - start;
    std::vector<uint8_t> saved(m_bytes.begin() + sizeAt + 8, m_bytes.end());
    m_bytes.resize(sizeAt);
    putInt(size);
    tag(f);
    m_bytes.insert(m_bytes.end(), saved.begin(), saved.end());
}


/*** Chunk files ***/

namespace {

void putHostInt(std::vector<uint8_t> &bytes, int32_t v) {
    uint8_t raw[4];
    memcpy(raw, &v, 4);
    bytes.insert(bytes.end(), raw, raw + 4);
}

void setHostInt(std::vector<uint8_t> &bytes, size_t at, int32_t v) {
    memcpy(&bytes[at], &v, 4);
}

} // anonymous namespace

std::vector<uint8_t> groupChunkBytes(bool withMediaRef) {
    // The file is the offset of the media names, the stream of opcodes, and
    // the media names. The stream is the id of the group, the number of
    // Actors, and for each Actor its opcodes.
    std::vector<uint8_t> bytes;
    putHostInt(bytes, 0);                // the offset of the names, set below
    putHostInt(bytes, 7);                // the id of the group
    bytes.push_back(1);                  // one Actor
    // An index of 0 is not written: it is a number of no digits.
    bytes.push_back(createActorOpcode);  // Actor 0
    bytes.push_back(setPropertyLengthOpcode);
    bytes.push_back(4);                  // a property of four bytes
    bytes.push_back(setPropertyOffsetOpcode);   // at offset 0
    bytes.push_back(copyPropertyOpcode);
    for (uint8_t b : { 0x11, 0x22, 0x33, 0x44 })
        bytes.push_back(b);
    if (withMediaRef)
        bytes.push_back(copyMediaRefOpcode);    // the first media name, 0
    bytes.push_back(endActorGroupOpcode);
    setHostInt(bytes, 0, bytes.size());
    if (withMediaRef) {
        const char name[] = "image";
        bytes.insert(bytes.end(), name, name + sizeof(name));
    }
    return bytes;
}

std::vector<uint8_t> setChunkBytes() {
    std::vector<uint8_t> bytes;
    putHostInt(bytes, 0);                // the size of the file, set below
    for (uint8_t b : { 1, 2, 3, 4, 5, 6, 7, 8 })
        bytes.push_back(b);
    setHostInt(bytes, 0, bytes.size());
    return bytes;
}

std::vector<uint8_t> sceneChunkBytes() {
    // The offset of the group names, and the integers of the Scene: the offset
    // of the media references, the id of the scene, and two group indices.
    std::vector<uint8_t> bytes;
    putHostInt(bytes, 0);
    putHostInt(bytes, 0);
    putHostInt(bytes, 1);
    putHostInt(bytes, 0);
    putHostInt(bytes, 1);
    setHostInt(bytes, 0, bytes.size());
    return bytes;
}

bool writeMediaChunkFile(const std::string &path, bool bigEndian, const std::string &mediaFile) {
    MleChunkFileHeader header = { 0, MleMrefChunkFile::PLAYPRINT_VERSION, 1000000, 0 };
    MleMediaRefInfoChunk info = { 0, 0, 1 };
    std::string name = mediaFile;
    MleMediaRefChunk data;
    data.m_flags = MleMrefChunkFile::EXTERNAL;
    data.m_type = mlDppMakeTag('l','o','g','o');
    data.m_size = name.size();
    data.m_data = const_cast<char *>(name.c_str());

    MleMrefChunkFile *out = new MleMrefChunkFile(path.c_str(), MleChunkFile::WRITING);
    out->setSwap(needsSwap(bigEndian));
    out->begin(bigEndian ? MLE_DPP_CREATEPPBE : MLE_DPP_CREATEPPLE);
    out->writeHeader(&header);
    out->beginList();
    out->writeInfo(&info);
    out->writeData(&data);
    out->endList();
    out->end();
    delete out;
    return true;
}

bool buildPlayprint(const std::string &dir, bool bigEndian, PlayprintFiles &files) {
    files.playprint = dir + "/pp.dpp";
    files.group = dir + "/group1.chk";
    files.set = dir + "/set1.chk";
    files.scene = dir + "/scene1.chk";
    files.media = dir + "/image.chk";
    if (! writeFile(files.group, groupChunkBytes(true)) ||
        ! writeFile(files.set, setChunkBytes()) ||
        ! writeFile(files.scene, sceneChunkBytes()) ||
        ! writeMediaChunkFile(files.media, bigEndian))
        return false;

    // The table of chunk files.
    MleDppChunkTable table;
    table.addEntry(CHUNK_GROUP, const_cast<char *>(files.group.c_str()), const_cast<char *>("group1"));
    table.addEntry(CHUNK_MEDIA, const_cast<char *>(files.media.c_str()), const_cast<char *>("image"));
    table.addEntry(CHUNK_SET, const_cast<char *>(files.set.c_str()), const_cast<char *>("set1"));
    table.addEntry(CHUNK_SCENE, const_cast<char *>(files.scene.c_str()), const_cast<char *>("scene1"));

    MleDppOutput *dpp = new MleDppOutput(files.playprint.c_str());
    dpp->setSwap(needsSwap(bigEndian));
    dpp->begin(bigEndian ? MLE_DPP_CREATEPPBE : MLE_DPP_CREATEPPLE);
    dpp->writeHeader();
    dpp->beginTOC(table.getUsed());
    for (int i = 0; i < table.getUsed(); i++) {
        switch (table.getType(i)) {
            case CHUNK_GROUP:
                dpp->writeGroup(&table, i);
                break;
            case CHUNK_MEDIA:
                dpp->writeMedia(table.getFilename(i));
                break;
            case CHUNK_SET:
                dpp->writeSet(table.getFilename(i));
                break;
            case CHUNK_SCENE:
                dpp->writeScene(&table, i);
                break;
        }
    }
    dpp->endTOC();
    dpp->end();
    delete dpp;
    return true;
}

} // namespace dpptest
