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

// Include system header files.
#include <fcntl.h>
#include <poll.h>
#include <signal.h>
#include <stdlib.h>
#include <string.h>
#include <sys/resource.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <filesystem>
#include <fstream>
#include <regex>
#include <sstream>

// Include DPP test header files.
#include "libdpptest.h"

// Include Magic Lantern Digital Playprint header files.
#include <mle/Dpp.h>

// The directories of the test source tree. The build defines these as
// absolute paths.
#ifndef DPPTEST_WORKPRINT_DIR
#define DPPTEST_WORKPRINT_DIR "workprints"
#endif
#ifndef DPPTEST_GOLDEN_DIR
#define DPPTEST_GOLDEN_DIR "golden"
#endif

namespace fs = std::filesystem;

// Initializes the DPP library once, before any test runs. mleDppInit() must
// not be called more than once.
class DppEnvironment : public ::testing::Environment
{
  public:
    void SetUp() override {
        mleDppInit();
    }
};

bool initDPPTest() {
    static bool registered = false;
    if (! registered) {
        // Google Test takes ownership of the environment.
        ::testing::AddGlobalTestEnvironment(new DppEnvironment);
        registered = true;
    }
    return true;
}

namespace dpptest {

/*** Paths ***/

std::string workprintPath(const char *name) {
    return std::string(DPPTEST_WORKPRINT_DIR) + "/" + name;
}

std::string goldenPath(const char *name) {
    return std::string(DPPTEST_GOLDEN_DIR) + "/" + name;
}

static std::string mleRoot() {
    const char *root = getenv("MLE_ROOT");
    return root != NULL && *root != '\0' ? root : "/opt/MagicLantern";
}

std::string toolPath(const char *name) {
    std::string path = mleRoot() + "/bin/" + name;
    return access(path.c_str(), X_OK) == 0 ? path : std::string();
}


/*** Files ***/

bool writeFile(const std::string &path, const std::vector<uint8_t> &bytes) {
    std::ofstream out(path, std::ios::binary | std::ios::trunc);
    out.write(reinterpret_cast<const char *>(bytes.data()), bytes.size());
    return out.good();
}

bool writeFile(const std::string &path, const std::string &text) {
    return writeFile(path, std::vector<uint8_t>(text.begin(), text.end()));
}

bool readFile(const std::string &path, std::vector<uint8_t> &bytes) {
    std::ifstream in(path, std::ios::binary);
    if (! in)
        return false;
    bytes.assign(std::istreambuf_iterator<char>(in),
                 std::istreambuf_iterator<char>());
    return true;
}

bool readFile(const std::string &path, std::string &text) {
    std::vector<uint8_t> bytes;
    if (! readFile(path, bytes))
        return false;
    text.assign(bytes.begin(), bytes.end());
    return true;
}


/*** Scratch directory ***/

void ScratchFixture::SetUp() {
    std::string pattern = (fs::temp_directory_path() / "dpptest-XXXXXX").string();
    std::vector<char> buffer(pattern.begin(), pattern.end());
    buffer.push_back('\0');
    ASSERT_NE(mkdtemp(buffer.data()), nullptr) << "cannot create " << pattern;
    m_dir = buffer.data();
}

void ScratchFixture::TearDown() {
    // Only remove a directory that this fixture made.
    if (! m_dir.empty() && m_dir.find("dpptest-") != std::string::npos) {
        std::error_code ec;
        fs::remove_all(m_dir, ec);
    }
    m_dir.clear();
}

std::string ScratchFixture::scratchFile(const std::string &name) const {
    return m_dir + "/" + name;
}


/*** Running tools and code that may fail ***/

ToolResult runTool(const std::string &tool,
                   const std::vector<std::string> &args,
                   const std::string &workDir,
                   int timeoutSeconds) {
    ToolResult result = { -1, 0, false, "" };

    // A name without a directory is a tool in $MLE_ROOT/bin.
    std::string path = tool.find('/') == std::string::npos ? toolPath(tool.c_str()) : tool;
    if (path.empty()) {
        result.output = "tool not installed: " + tool;
        return result;
    }

    int fds[2];
    if (pipe(fds) != 0) {
        result.output = "cannot create a pipe";
        return result;
    }

    std::vector<char *> argv;
    argv.push_back(const_cast<char *>(path.c_str()));
    for (const std::string &arg : args)
        argv.push_back(const_cast<char *>(arg.c_str()));
    argv.push_back(NULL);

    pid_t pid = fork();
    if (pid < 0) {
        close(fds[0]);
        close(fds[1]);
        result.output = "cannot fork";
        return result;
    }
    if (pid == 0) {
        // The tool: no input, output and errors to the pipe, MLE_ROOT set for
        // the code templates and the libraries found in MLE_ROOT/lib.
        close(fds[0]);
        int devNull = open("/dev/null", O_RDONLY);
        if (devNull >= 0)
            dup2(devNull, 0);
        dup2(fds[1], 1);
        dup2(fds[1], 2);
        close(fds[1]);
        std::string root = mleRoot();
        setenv("MLE_ROOT", root.c_str(), 1);
        const char *oldPath = getenv("LD_LIBRARY_PATH");
        std::string libPath = root + "/lib" + (oldPath != NULL ? std::string(":") + oldPath : "");
        setenv("LD_LIBRARY_PATH", libPath.c_str(), 1);
        if (! workDir.empty() && chdir(workDir.c_str()) != 0)
            _exit(126);
        execv(path.c_str(), argv.data());
        _exit(127);
    }

    // Read the output until the tool closes it or the time is up.
    close(fds[1]);
    int waitMs = timeoutSeconds * 1000;
    char buffer[4096];
    for (;;) {
        struct pollfd pfd = { fds[0], POLLIN, 0 };
        int ready = poll(&pfd, 1, waitMs);
        if (ready == 0) {
            result.timedOut = true;
            kill(pid, SIGKILL);
            break;
        }
        if (ready < 0)
            break;
        ssize_t n = read(fds[0], buffer, sizeof(buffer));
        if (n <= 0)
            break;
        result.output.append(buffer, n);
    }
    close(fds[0]);

    int status = 0;
    waitpid(pid, &status, 0);
    if (WIFEXITED(status))
        result.status = WEXITSTATUS(status);
    else if (WIFSIGNALED(status))
        result.signal = WTERMSIG(status);
    return result;
}

ChildResult runInChild(const std::function<void()> &code, int timeoutSeconds) {
    ChildResult result = { false, 0, false, 0, false };

    fflush(NULL);
    pid_t pid = fork();
    if (pid < 0)
        return result;
    if (pid == 0) {
        // The child stops with SIGALRM if the code does not finish. A crash
        // does not leave a core file.
        struct rlimit noCore = { 0, 0 };
        setrlimit(RLIMIT_CORE, &noCore);
        alarm(timeoutSeconds);
        try {
            code();
        } catch (...) {
            // An exception must not leave the child to carry on as the test.
            _exit(kChildException);
        }
        _exit(0);
    }

    int status = 0;
    waitpid(pid, &status, 0);
    if (WIFEXITED(status)) {
        result.exited = true;
        result.exitCode = WEXITSTATUS(status);
    } else if (WIFSIGNALED(status)) {
        result.signaled = true;
        result.signal = WTERMSIG(status);
        result.timedOut = (result.signal == SIGALRM);
    }
    return result;
}


/*** Comparing generated files ***/

std::string maskGenerated(const std::string &text, const std::string &scratchDir) {
    std::string masked = text;

    // The scratch directory, and the directories of the test sources.
    auto replaceAll = [&](const std::string &what, const std::string &with) {
        if (what.empty())
            return;
        for (size_t pos = masked.find(what); pos != std::string::npos;
             pos = masked.find(what, pos + with.size()))
            masked.replace(pos, what.size(), with);
    };
    replaceAll(scratchDir, "<SCRATCH>");
    replaceAll(DPPTEST_WORKPRINT_DIR, "<WORKPRINTS>");
    replaceAll(mleRoot(), "<MLE_ROOT>");

    // A date as printed by ctime(), "Wed Dec 31 17:00:00 1969", and as an ISO
    // date and time, "2025-11-24 17:04:13".
    static const std::regex ctimeDate(
        "(Mon|Tue|Wed|Thu|Fri|Sat|Sun) (Jan|Feb|Mar|Apr|May|Jun|Jul|Aug|Sep|Oct|Nov|Dec) "
        "[ 0-9][0-9] [0-9]{2}:[0-9]{2}:[0-9]{2} [0-9]{4}");
    static const std::regex isoDate(
        "[0-9]{4}-[0-9]{2}-[0-9]{2}[ T][0-9]{2}:[0-9]{2}:[0-9]{2}");
    masked = std::regex_replace(masked, ctimeDate, "<DATE>");
    masked = std::regex_replace(masked, isoDate, "<DATE>");
    return masked;
}

::testing::AssertionResult matchesGolden(const char *name, const std::string &actual) {
    std::string path = goldenPath(name);

    const char *update = getenv("DPPTEST_UPDATE_GOLDEN");
    if (update != NULL && std::string(update) == "1") {
        if (! writeFile(path, actual))
            return ::testing::AssertionFailure() << "cannot write golden file " << path;
        return ::testing::AssertionSuccess();
    }

    std::string expected;
    if (! readFile(path, expected))
        return ::testing::AssertionFailure() << "cannot read golden file " << path
            << " (DPPTEST_UPDATE_GOLDEN=1 writes it)";
    if (expected == actual)
        return ::testing::AssertionSuccess();

    // Report the first line that differs.
    std::istringstream e(expected), a(actual);
    std::string eLine, aLine;
    int line = 0;
    for (;;) {
        bool eMore = static_cast<bool>(std::getline(e, eLine));
        bool aMore = static_cast<bool>(std::getline(a, aLine));
        line++;
        if (! eMore && ! aMore)
            break;
        if (eMore != aMore || eLine != aLine)
            return ::testing::AssertionFailure() << path << " differs at line " << line
                << "\n  golden: " << (eMore ? eLine : "<end of file>")
                << "\n  actual: " << (aMore ? aLine : "<end of file>");
    }
    return ::testing::AssertionFailure() << path << " differs";
}


/*** Chunk files ***/

namespace {

uint32_t getInt(const std::vector<uint8_t> &bytes, size_t pos, bool bigEndian) {
    uint32_t v = 0;
    for (int i = 0; i < 4; i++) {
        uint8_t b = bytes[pos + (bigEndian ? i : 3 - i)];
        v = (v << 8) | b;
    }
    return v;
}

// A tag is an integer with the first character in the high byte.
std::string tagName(uint32_t tag) {
    std::string name(4, ' ');
    for (int i = 0; i < 4; i++)
        name[i] = static_cast<char>((tag >> (24 - 8 * i)) & 0xff);
    return name;
}

bool isForm(const std::string &tag) {
    return tag == "PPBE" || tag == "PPLE" || tag == "LIST";
}

// Parse the chunks in [pos, end). Returns false and sets the error if a chunk
// does not fit.
bool parseChunks(const std::vector<uint8_t> &bytes, size_t pos, size_t end,
                 bool bigEndian, std::vector<Chunk> &chunks, std::string &error) {
    while (pos < end) {
        if (end - pos < 8) {
            error = "a chunk header at offset " + std::to_string(pos) + " is cut short";
            return false;
        }
        Chunk chunk;
        chunk.offset = pos;
        chunk.tag = tagName(getInt(bytes, pos, bigEndian));
        chunk.size = getInt(bytes, pos + 4, bigEndian);
        pos += 8;

        if (isForm(chunk.tag)) {
            if (end - pos < 4) {
                error = "the form of the chunk at offset " + std::to_string(chunk.offset) + " is cut short";
                return false;
            }
            chunk.form = tagName(getInt(bytes, pos, bigEndian));
            pos += 4;
            if (chunk.size > end - pos) {
                error = "the size of chunk '" + chunk.tag + "' at offset " +
                        std::to_string(chunk.offset) + " is beyond its parent";
                return false;
            }
            if (! parseChunks(bytes, pos, pos + chunk.size, bigEndian, chunk.children, error))
                return false;
            pos += chunk.size;
        } else {
            if (chunk.size > end - pos) {
                error = "the size of chunk '" + chunk.tag + "' at offset " +
                        std::to_string(chunk.offset) + " is beyond its parent";
                return false;
            }
            chunk.data.assign(bytes.begin() + pos, bytes.begin() + pos + chunk.size);
            pos += chunk.size;
            // A chunk of odd size is followed by a pad byte.
            if ((chunk.size & 1) != 0 && pos < end)
                pos++;
        }
        chunks.push_back(chunk);
    }
    return true;
}

void describeChunk(const Chunk &chunk, int depth,
                   const std::vector<std::string> &omitData, size_t maxData,
                   std::ostringstream &out) {
    out << std::string(depth * 2, ' ') << "'" << chunk.tag << "' size=" << chunk.size;
    if (! chunk.form.empty())
        out << " form='" << chunk.form << "'";
    bool omit = false;
    for (const std::string &tag : omitData)
        omit = omit || tag == chunk.tag;
    if (! chunk.data.empty()) {
        out << " data=";
        if (omit) {
            out << "<omitted>";
        } else {
            static const char hex[] = "0123456789abcdef";
            for (size_t i = 0; i < chunk.data.size() && i < maxData; i++) {
                if (i > 0)
                    out << ' ';
                out << hex[chunk.data[i] >> 4] << hex[chunk.data[i] & 15];
            }
            if (chunk.data.size() > maxData)
                out << " ...";
        }
    }
    out << "\n";
    for (const Chunk &child : chunk.children)
        describeChunk(child, depth + 1, omitData, maxData, out);
}

} // anonymous namespace

bool parseChunkBytes(const std::vector<uint8_t> &bytes, ChunkFile &file) {
    file.bigEndian = true;
    file.chunks.clear();
    file.error.clear();

    // The first four bytes are the playprint tag. A little endian file has the
    // bytes of the tag in the opposite order.
    if (bytes.size() < 8) {
        file.error = "the file is too short to be a chunk file";
        return false;
    }
    std::string first(reinterpret_cast<const char *>(bytes.data()), 4);
    if (first == "PPBE")
        file.bigEndian = true;
    else if (first == "ELPP")
        file.bigEndian = false;
    else {
        file.error = "the file does not start with a playprint tag";
        return false;
    }
    return parseChunks(bytes, 0, bytes.size(), file.bigEndian, file.chunks, file.error);
}

bool readChunkFile(const std::string &path, ChunkFile &file) {
    std::vector<uint8_t> bytes;
    if (! readFile(path, bytes)) {
        file.error = "cannot read " + path;
        return false;
    }
    return parseChunkBytes(bytes, file);
}

std::string describeChunks(const ChunkFile &file,
                           const std::vector<std::string> &omitData,
                           size_t maxData) {
    std::ostringstream out;
    out << (file.bigEndian ? "big endian" : "little endian") << "\n";
    for (const Chunk &chunk : file.chunks)
        describeChunk(chunk, 0, omitData, maxData, out);
    return out.str();
}

} // namespace dpptest
