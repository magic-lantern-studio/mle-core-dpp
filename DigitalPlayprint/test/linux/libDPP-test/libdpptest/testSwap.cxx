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

// Phase 2 tests of libplayprint: byte order functions, tags and the swap flags.

// Include DPP test header files.
#include "libdpptest.h"

// Include Magic Lantern header files.
#include <mle/mlMalloc.h>
#include <mle/playprnt.h>
#include <mle/ppoutput.h>
#include <mle/chkfile.h>

using namespace dpptest;

/*** Byte order functions ***/

TEST(SwapTest, Short) {
    EXPECT_EQ(mlDppSwapShort(0x1234), 0x3412);
    EXPECT_EQ(mlDppSwapShort(0x0000), 0x0000);
    EXPECT_EQ(mlDppSwapShort(0xffff), 0xffff);
    EXPECT_EQ(mlDppSwapShort(0x00ff), 0xff00);
}

TEST(SwapTest, Int) {
    EXPECT_EQ(mlDppSwapInt(0x01020304u), 0x04030201u);
    EXPECT_EQ(mlDppSwapInt(0u), 0u);
    EXPECT_EQ(mlDppSwapInt(0xffffffffu), 0xffffffffu);
    EXPECT_EQ(mlDppSwapInt(0x000000ffu), 0xff000000u);
    EXPECT_EQ(mlDppSwapInt(0x80000000u), 0x00000080u);
}

TEST(SwapTest, Long) {
    EXPECT_EQ(mlDppSwapLong(0x0102030405060708ull), 0x0807060504030201ull);
    EXPECT_EQ(mlDppSwapLong(0ull), 0ull);
    EXPECT_EQ(mlDppSwapLong(0xffffffffffffffffull), 0xffffffffffffffffull);
    EXPECT_EQ(mlDppSwapLong(0x00000000000000ffull), 0xff00000000000000ull);
}

TEST(SwapTest, SwappingTwiceGivesTheValue) {
    for (unsigned v : { 0u, 1u, 0x12345678u, 0x80000000u, 0xdeadbeefu }) {
        EXPECT_EQ(mlDppSwapInt(mlDppSwapInt(v)), v);
        EXPECT_EQ(mlDppSwapShort(mlDppSwapShort(static_cast<MlUShort>(v))), static_cast<MlUShort>(v));
        EXPECT_EQ(mlDppSwapLong(mlDppSwapLong(v * 0x100000001ull)), v * 0x100000001ull);
    }
}


/*** Tags ***/

TEST(TagTest, MakeTag) {
    // The first character is the high byte.
    EXPECT_EQ(static_cast<unsigned>(mlDppMakeTag('P','P','B','E')), 0x50504245u);
    EXPECT_EQ(MLE_PPBE_TAG, mlDppMakeTag('P','P','B','E'));
    EXPECT_EQ(MLE_PPLE_TAG, mlDppMakeTag('P','P','L','E'));
    EXPECT_EQ(MLE_LIST_TAG, mlDppMakeTag('L','I','S','T'));
    EXPECT_NE(MLE_PPBE_TAG, MLE_PPLE_TAG);
}

TEST(TagTest, StringToTag) {
    char abcd[] = "abcd", ab[] = "ab", empty[] = "", longer[] = "abcdef", upper[] = "ab";
    EXPECT_EQ(mlDppStringToTag(abcd, 0), mlDppMakeTag('a','b','c','d'));
    // A short string is padded with spaces, a long one is cut.
    EXPECT_EQ(mlDppStringToTag(ab, 0), mlDppMakeTag('a','b',' ',' '));
    EXPECT_EQ(mlDppStringToTag(empty, 0), mlDppMakeTag(' ',' ',' ',' '));
    EXPECT_EQ(mlDppStringToTag(longer, 0), mlDppMakeTag('a','b','c','d'));
    EXPECT_EQ(mlDppStringToTag(upper, MLE_DPP_TOUPPER), mlDppMakeTag('A','B',' ',' '));
}

TEST(TagTest, TagToString) {
    char *str = mlDppTagToString(mlDppMakeTag('h','d','r',' '));
    ASSERT_NE(str, nullptr);
    EXPECT_STREQ(str, "hdr ");
    mlFree(str);

    char abcd[] = "wxyz";
    str = mlDppTagToString(mlDppStringToTag(abcd, 0));
    EXPECT_STREQ(str, "wxyz");
    mlFree(str);
}


/*** The swap flags ***/

class SwapFlagsTest : public ScratchFixture {};

// The flags are counts that are global to the library: an object that swaps
// adds one, and takes it away when it is deleted.

TEST_F(SwapFlagsTest, GlobalFlagsStartOff) {
    EXPECT_FALSE(mlDppGetSwapRead());
    EXPECT_FALSE(mlDppGetSwapWrite());
}

TEST_F(SwapFlagsTest, GlobalFlagsAreIndependentAndNest) {
    mlDppSetSwapRead(TRUE);
    EXPECT_TRUE(mlDppGetSwapRead());
    EXPECT_FALSE(mlDppGetSwapWrite());
    mlDppSetSwapRead(TRUE);
    mlDppSetSwapRead(FALSE);
    EXPECT_TRUE(mlDppGetSwapRead());   // still one user
    mlDppSetSwapRead(FALSE);
    EXPECT_FALSE(mlDppGetSwapRead());

    mlDppSetSwapWrite(TRUE);
    EXPECT_TRUE(mlDppGetSwapWrite());
    EXPECT_FALSE(mlDppGetSwapRead());
    mlDppSetSwapWrite(FALSE);
    EXPECT_FALSE(mlDppGetSwapWrite());
}

TEST_F(SwapFlagsTest, PlayprintObjectSetsAndRestoresTheFlags) {
    {
        MleDppOutput out(scratchFile("a.dpp").c_str());
        EXPECT_FALSE(out.getSwapRead());
        EXPECT_FALSE(out.getSwapWrite());

        out.setSwapWrite(TRUE);
        EXPECT_TRUE(out.getSwapWrite());
        EXPECT_FALSE(out.getSwapRead());
        EXPECT_TRUE(mlDppGetSwapWrite());
        EXPECT_FALSE(mlDppGetSwapRead());

        out.setSwap(TRUE);
        EXPECT_TRUE(out.getSwapRead());
        EXPECT_TRUE(mlDppGetSwapRead());
    }
    // Deleting the object takes its part of the flags away.
    EXPECT_FALSE(mlDppGetSwapRead());
    EXPECT_FALSE(mlDppGetSwapWrite());
}

TEST_F(SwapFlagsTest, ChunkFileObjectSetsAndRestoresTheFlags) {
    {
        MleChunkFile file(scratchFile("a.chk").c_str(), MleChunkFile::WRITING);
        file.setSwap(TRUE);
        EXPECT_TRUE(file.getSwapRead());
        EXPECT_TRUE(file.getSwapWrite());
        EXPECT_TRUE(mlDppGetSwapRead());
        EXPECT_TRUE(mlDppGetSwapWrite());
    }
    EXPECT_FALSE(mlDppGetSwapRead());
    EXPECT_FALSE(mlDppGetSwapWrite());
}

// Asking an object not to swap, when it did not, must not change the flags
// of the other objects. The flags are global, so this runs in a child process.
TEST_F(SwapFlagsTest, SwapOffOnAnObjectThatDidNotSwapChangesNothing) {
    std::string a = scratchFile("a.dpp"), b = scratchFile("b.dpp");
    ChildResult r = runInChild([&] {
        MleDppOutput first(a.c_str());
        first.setSwap(FALSE);               // it did not swap
        MleDppOutput second(b.c_str());
        second.setSwap(TRUE);
        _exit(mlDppGetSwapRead() && mlDppGetSwapWrite() ? 0 : 1);
    });
    ASSERT_TRUE(r.exited);
    EXPECT_EQ(r.exitCode, 0) << "a later object that swaps is not seen as swapping";
}
