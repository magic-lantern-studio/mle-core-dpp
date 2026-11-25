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

// Phase 2 tests of libplayprint: a damaged playprint must give an error, and
// not crash or hang. The playprint of the fixture is cut at every length, and
// each of its bytes is changed in turn, and a reader reads all of it, in a
// child process.

// Include system header files.
#include <map>

// Include DPP test header files.
#include "libdpptest.h"

// Include Magic Lantern header files.
#include <mle/mlMalloc.h>
#include <mle/playprnt.h>
#include <mle/ppinput.h>

using namespace dpptest;

namespace {

int accept(void *, void *) { return TRUE; }

// A callback for MediaRefs: the name is allocated by the reader.
int acceptMref(void *, void *data) {
    MleMediaRefChunk *m = static_cast<MleMediaRefChunk *>(data);
    if (m->m_data)
        mlFree(m->m_data);
    m->m_data = NULL;
    return TRUE;
}

// Read all of a playprint, the way a loader does. Returns 0 if it was read, 1
// if a reader gave an error.
int readAll(const std::string &path, bool big) {
    MleDppInput in(path.c_str());
    if (in.getFp() == NULL)
        return 1;
    in.setSwap(needsSwap(big));
    if (! in.begin(big ? MLE_DPP_FINDPPBE : MLE_DPP_FINDPPLE))
        return 1;
    int status = 0;
    if (! in.readHeader(accept, NULL))
        status = 1;
    if (! in.readTOC(accept, NULL))
        status = 1;
    if (! in.readGroup(accept, NULL))
        status = 1;
    if (! in.beginMref(accept, NULL))
        status = 1;
    else {
        if (! in.readMedia(acceptMref, NULL))
            status = 1;
        if (! in.endMref(accept, NULL))
            status = 1;
    }
    if (! in.readSet(accept, NULL))
        status = 1;
    if (! in.readScene(accept, NULL))
        status = 1;
    return status;
}

} // anonymous namespace

// "12: signal 11", "30: hang" -> a count of each kind and the first few cases.
std::string summarize(const std::vector<std::string> &failures) {
    std::map<std::string, int> kinds;
    for (const std::string &f : failures)
        kinds[f.substr(f.find(": ") + 2)]++;
    std::string text;
    for (auto &k : kinds)
        text += k.first + " x" + std::to_string(k.second) + "; ";
    text += "first:";
    for (size_t i = 0; i < failures.size() && i < 8; i++)
        text += " [" + failures[i] + "]";
    return text;
}

class BadInputTest : public ScratchFixture {
  protected:
    // Runs readAll on the bytes, in a child. Returns a description of how the
    // child ended if it did not end with 0 or 1, and an empty string if it did.
    std::string tryBytes(const std::vector<uint8_t> &bytes, bool big, bool mustFail = false) {
        std::string path = scratchFile("bad.dpp");
        if (! writeFile(path, bytes))
            return "cannot write";
        ChildResult r = runInChild([&] { _exit(readAll(path, big)); }, 5);
        if (r.timedOut)
            return "hang";
        if (r.signaled)
            return "signal " + std::to_string(r.signal);
        if (r.exited && r.exitCode > 1)
            return "exit " + std::to_string(r.exitCode);
        if (mustFail && r.exitCode != 1)
            return "not rejected";
        return "";
    }

    std::vector<uint8_t> goodBytes(bool big) {
        PlayprintFiles files;
        EXPECT_TRUE(buildPlayprint(scratch(), big, files));
        std::vector<uint8_t> bytes;
        EXPECT_TRUE(readFile(files.playprint, bytes));
        return bytes;
    }
};

TEST_F(BadInputTest, TheGoodPlayprintIsRead) {
    for (bool big : { true, false }) {
        EXPECT_EQ(tryBytes(goodBytes(big), big), "");
        std::string path = scratchFile("bad.dpp");
        ASSERT_TRUE(writeFile(path, goodBytes(big)));
        ChildResult r = runInChild([&] { _exit(readAll(path, big)); });
        EXPECT_TRUE(r.exited);
        EXPECT_EQ(r.exitCode, 0) << (big ? "big" : "little") << " endian";
    }
}

TEST_F(BadInputTest, TruncationAtEveryLength) {
    for (bool big : { true, false }) {
        std::vector<uint8_t> good = goodBytes(big);
        std::vector<std::string> failures;
        for (size_t n = 0; n < good.size(); n++) {
            std::vector<uint8_t> cut(good.begin(), good.begin() + n);
            std::string how = tryBytes(cut, big, true);   // a cut file is an error
            if (! how.empty())
                failures.push_back(std::to_string(n) + ": " + how);
        }
        EXPECT_TRUE(failures.empty())
            << (big ? "big" : "little") << " endian, " << failures.size() << " of "
            << good.size() << " lengths: " << summarize(failures);
    }
}

TEST_F(BadInputTest, EveryByteChanged) {
    for (bool big : { true, false }) {
        std::vector<uint8_t> good = goodBytes(big);
        std::vector<std::string> failures;
        for (size_t at = 0; at < good.size(); at++) {
            for (uint8_t value : { 0x00, 0xff, 0x80 }) {
                if (good[at] == value)
                    continue;
                std::vector<uint8_t> bad = good;
                bad[at] = value;
                std::string how = tryBytes(bad, big);
                if (! how.empty())
                    failures.push_back(std::to_string(at) + "=" + std::to_string(value) + ": " + how);
            }
        }
        EXPECT_TRUE(failures.empty())
            << (big ? "big" : "little") << " endian, " << failures.size()
            << " failures: " << summarize(failures);
    }
}
