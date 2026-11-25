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

#ifndef _LIBDPPTEST_H_
#define _LIBDPPTEST_H_

// Include system header files.
#include <stdint.h>
#include <functional>
#include <memory>
#include <string>
#include <vector>

// Include Google Test header files.
#include "gtest/gtest.h"

// Include Magic Lantern Digital Workprint header files.
#include <mle/DwpItem.h>

// Register the DPP test environment, which initializes the Digital Playprint
// library once for the whole test run. Call this from main() before running
// the tests. It also forces libdpptest.so to be loaded.
//
// mleDppInit() registers the workprint items and the playprint items. It must
// not be called more than once. libplayprint and libDPPGen have no
// initialization call of their own.
extern bool initDPPTest();

namespace dpptest {

/*** Paths ***/

// Return the path of a test workprint, from the "workprints" directory of the
// test source tree. The tests do not depend on the current directory.
std::string workprintPath(const char *name);

// Return the path of a golden file, from the "golden" directory.
std::string goldenPath(const char *name);

// Return the path of a tool installed in $MLE_ROOT/bin, or an empty string if
// it is not installed.
std::string toolPath(const char *name);


/*** Scratch directory ***/

// Base class for test fixtures that write files. Each test has a new, empty
// directory that is removed when the test finishes.
class ScratchFixture : public ::testing::Test
{
  protected:
    void SetUp() override;
    void TearDown() override;

    // The scratch directory of the test.
    const std::string &scratch() const { return m_dir; }

    // The path of a file in the scratch directory.
    std::string scratchFile(const std::string &name) const;

  private:
    std::string m_dir;
};

// Write a file. Returns false if it cannot be written.
bool writeFile(const std::string &path, const std::vector<uint8_t> &bytes);
bool writeFile(const std::string &path, const std::string &text);

// Read a whole file. Returns false if it cannot be read.
bool readFile(const std::string &path, std::vector<uint8_t> &bytes);
bool readFile(const std::string &path, std::string &text);


/*** Running tools and code that may fail ***/

// The outcome of a tool.
struct ToolResult
{
    int  status;     // The exit status, or -1 if the tool was killed.
    int  signal;     // The signal that killed the tool, or 0.
    bool timedOut;   // True if the tool ran too long and was killed.
    std::string output; // Standard output and standard error.
};

// Run a tool from $MLE_ROOT/bin (or the path given), with its arguments, in
// the given directory (the current directory if empty). The tool is killed
// after the time limit. It does not read standard input.
ToolResult runTool(const std::string &tool,
                   const std::vector<std::string> &args,
                   const std::string &workDir = "",
                   int timeoutSeconds = 20);

// The outcome of code that was run in a child process.
struct ChildResult
{
    bool exited;     // True if the code returned or called exit().
    int  exitCode;   // The exit status, if it exited.
    bool signaled;   // True if the process was killed by a signal.
    int  signal;     // The signal, if it was killed by one.
    bool timedOut;   // True if it was stopped by the time limit.
};

// Run code in a child process, for code that is expected to abort, crash or
// hang. The child is stopped after the time limit (an alarm), and the result
// says how it ended. The child ends with exit status 0 if the code returns.
ChildResult runInChild(const std::function<void()> &code,
                       int timeoutSeconds = 10);


/*** Comparing generated files ***/

// Mask what changes from one run to the next in the text of a generated file:
// dates, the scratch directory and the test source directory.
std::string maskGenerated(const std::string &text,
                          const std::string &scratchDir = "");

// Compare text with a golden file. A difference is reported with the first
// line that differs. If the environment variable DPPTEST_UPDATE_GOLDEN is 1,
// the golden file is written instead.
::testing::AssertionResult matchesGolden(const char *name,
                                         const std::string &actual);


/*** Chunk files ***/

// A chunk in a chunk file or in a playprint. A form (PPBE, PPLE or LIST) has
// a form tag and children; the other chunks have data.
struct Chunk
{
    std::string tag;               // Four characters, for example "hdr ".
    std::string form;              // The form tag of a form, or empty.
    uint32_t    size;              // The size in the file.
    size_t      offset;            // The offset of the chunk in the file.
    std::vector<uint8_t> data;     // The data of a chunk that is not a form.
    std::vector<Chunk>   children; // The chunks in a form.
};

// A chunk file that has been read into a tree.
struct ChunkFile
{
    bool bigEndian;                // The byte order of the file.
    std::vector<Chunk> chunks;     // The chunks at the top level.
    std::string error;             // What is wrong with the file, or empty.
};

// Read a chunk file or a playprint into a tree. Returns false, and sets the
// error, if the file is not well formed (a size beyond the end of the file,
// or no playprint tag).
bool readChunkFile(const std::string &path, ChunkFile &file);
bool parseChunkBytes(const std::vector<uint8_t> &bytes, ChunkFile &file);

// Return the tree as text, one chunk to a line, indented by depth. At most
// maxData bytes of the data of a chunk are shown. The data of the chunks with
// the tags in omitData is not shown, as it changes from one run to the next
// (for example the time in the "hdr " chunk).
std::string describeChunks(const ChunkFile &file,
                           const std::vector<std::string> &omitData = {},
                           size_t maxData = 16);

} // namespace dpptest

#endif /* _LIBDPPTEST_H_ */
