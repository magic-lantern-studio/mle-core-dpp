/** @defgroup MleDPPMaster Magic Lantern Digital Playprint Library API - Master */

/**
 * @file playprint.c
 * @ingroup MleDPPMaster
 *
 * Magic Lantern Digital Playprint Library API.
 *
 * @author Mark S. Millard
 * @date September 15, 2004
 */

// COPYRIGHT_BEGIN
//
// Copyright (c) 2015-2024 Wizzer Works
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

/* Include system header files. */
#ifdef WIN32
#include <memory.h>
#endif
#include <string.h>
#include <ctype.h>

/* Include Magic Lantern header files. */
#include "mle/mlErrno.h"
#include "mle/mlFileio.h"
#include "mle/mlMalloc.h"
#include "mle/mlMacros.h"
#include "mle/mlAssert.h"
#include "mle/playprnt.h"


#if !defined(MLE_RUNTIME)

#ifndef psx
/* Include system header files. */
#include <sys/stat.h>
#endif

static int _mlSwapRead = 0;
static int _mlSwapWrite = 0;


MlULong mlDppSwapLong(MlULong x)
{
    x = ((x << 8) & 0xFF00FF00FF00FF00ULL ) | ((x >> 8) & 0x00FF00FF00FF00FFULL );
    x = ((x << 16) & 0xFFFF0000FFFF0000ULL ) | ((x >> 16) & 0x0000FFFF0000FFFFULL );
    return (x << 32) | (x >> 32);
}

MlUInt mlDppSwapInt(MlUInt x)
{
    //x = ((x << 8) & 0xFF00FF00 ) | ((x >> 8) & 0xFF00FF );
    //return (x << 16) | (x >> 16);
    return ((x << 24) | ((x & 0xFF00) << 8) | ((x >> 8) & 0xFF00) | (x >> 24));
}


MlUShort mlDppSwapShort(MlUShort x)
{
   return (x << 8) | (x >> 8);
}


void mlDppSetSwapRead(MlBoolean flag)
{
    if (flag == TRUE)
        _mlSwapRead++;
    else
        _mlSwapRead--;
}


void mlDppSetSwapWrite(MlBoolean flag)
{
    if (flag == TRUE)
        _mlSwapWrite++;
    else
        _mlSwapWrite--;
}


MlBoolean mlDppGetSwapRead()
{
    if (_mlSwapRead > 0)
        return(TRUE);
    else
        return(FALSE);
}


MlBoolean mlDppGetSwapWrite()
{
    if (_mlSwapWrite > 0)
        return(TRUE);
    else
        return(FALSE);
}

#endif /* MLE_RUNTIME */


MlBoolean mlDppAscend(FILE *fp,MleDppChunkInfo *info,MlUInt flags)
{
    /* Declare local variables. */
    unsigned char pad = 0;
    MlInt chunkSize,seekTo,longWrite;
    MlBoolean retValue = TRUE;

    MLE_VALIDATE_PTR(fp);
    MLE_VALIDATE_PTR(info);

#if !defined(MLE_RUNTIME)
    if ((info->m_flags & MLE_DPP_DIRTY) == MLE_DPP_DIRTY)
	{
        /* Flush any buffered data. */
        mlFFlush(fp);

	    /* Correct the chunk size if necessary. */
        chunkSize = mlFTell(fp) - info->m_offset;
        if (chunkSize < 0)
		{
            mlSetErrno(MLE_DPP_ERR_CANNOTWRITE);
            return(FALSE);
        } else if (info->m_chunk.m_size != (MlUInt)chunkSize)
		{
            if ((info->m_chunk.m_tag == MLE_PPBE_TAG) ||
                (info->m_chunk.m_tag == MLE_PPLE_TAG) ||
                (info->m_chunk.m_tag == MLE_LIST_TAG))
                seekTo = chunkSize + sizeof(MlDppTag) + sizeof(MlUInt);
            else
                seekTo = chunkSize + sizeof(MlUInt);

            /* Seek to beginning of chunk. */
            if (mlFSeek(fp,-seekTo,SEEK_CUR) != 0)
			{
                mlSetErrno(MLE_DPP_ERR_CANNOTSEEK);
                return(FALSE);
			}

			/* Write the chunk size. */
			longWrite = chunkSize;
			if (mlDppGetSwapWrite())
				longWrite = mlDppSwapInt(longWrite);
			if (mlFWrite(&longWrite,sizeof(MlInt),1,fp) != 1)
			{
				mlSetErrno(MLE_DPP_ERR_CANNOTWRITE);
				return(FALSE);
			}

				/* Seek to end of chunk data. */
				if (mlFSeek(fp,seekTo - sizeof(MlUInt),SEEK_CUR) != 0)
				{
					mlSetErrno(MLE_DPP_ERR_CANNOTSEEK);
					return(FALSE);
			}

            /* Update info structure. */
            info->m_chunk.m_size = (MlUInt)chunkSize;
		}

		/* Write a pad byte if neccessary. */
		if (MLE_ODD(info->m_chunk.m_size))
		{
			if (mlFWrite(&pad,sizeof(MlByte),1,fp) != 1)
			{
				mlSetErrno(MLE_DPP_ERR_CANNOTWRITE);
				return(FALSE);
			}
        }

    } else {
#endif /* MLE_RUNTIME */

        seekTo = info->m_offset + info->m_chunk.m_size;
        if (MLE_ODD(info->m_chunk.m_size))
            seekTo += sizeof(MlByte);

        if (mlFSeek(fp,seekTo,SEEK_SET) != 0)
		{
            mlSetErrno(MLE_DPP_ERR_CANNOTSEEK);
            retValue = FALSE;
        }

#if !defined(MLE_RUNTIME)
    }
#endif /* MLE_RUNTIME */
    mlFSeek(fp, 0, SEEK_CUR);
    return(retValue);
}

static MlBoolean _readChunk(FILE *fp,MleDppChunkInfo *info)
{
    /* Declare local variables. */
    MleDppChunk chunk;
    MlDppTag form;
    MlUInt offset;
    MlInt numRead;
    MlBoolean retValue = TRUE;

    MLE_VALIDATE_PTR(fp);
    MLE_VALIDATE_PTR(info);

    /* Get current file position. */
    offset = mlFTell(fp);

    /* Read next chunk. */
    numRead = mlFRead(&chunk,sizeof(MleDppChunk),1,fp);
    if (numRead == 1)
	{
        offset += sizeof(MleDppChunk);

        if (mlDppGetSwapRead())
		{
            chunk.m_tag = mlDppSwapInt(chunk.m_tag);
            chunk.m_size = mlDppSwapInt(chunk.m_size);
        }
        info->m_chunk.m_tag = chunk.m_tag;
        info->m_chunk.m_size = chunk.m_size;
        info->m_flags = 0;

        if ((chunk.m_tag == MLE_PPBE_TAG) ||
            (chunk.m_tag == MLE_PPLE_TAG) ||
            (chunk.m_tag == MLE_LIST_TAG))
		{

            /* Read form type. */
            if (mlFRead(&form,sizeof(MlDppTag),1,fp) == 1)
			{
                offset += sizeof(MlDppTag);

                if (mlDppGetSwapRead())
                    form = mlDppSwapInt(form);
                info->m_form = form;
                info->m_offset = offset;
            } else
			{
                mlSetErrno(MLE_DPP_ERR_CHUNKNOTFOUND);
                retValue = FALSE;
			}
		} else
		{
            info->m_form = mlDppMakeTag(' ',' ',' ',' ');
            info->m_offset = offset;
        }
    } else
	{
       mlSetErrno(MLE_DPP_ERR_CHUNKNOTFOUND);
       retValue = FALSE;
    }

    return(retValue);
}


MlBoolean mlDppDescend(
    FILE *fp,MleDppChunkInfo *info,MleDppChunkInfo *pinfo,MlUInt flags)
{
    /* declare local variables. */
    MleDppChunkInfo tmpInfo;
    MlInt seekTo;
    MlUInt curSize = 0;
    MlBoolean found = FALSE;
    MlBoolean retValue = TRUE;

    MLE_VALIDATE_PTR(fp);
    MLE_VALIDATE_PTR(info);

    switch (flags)
	{
        case MLE_DPP_FINDCHUNK:
            MLE_VALIDATE_PTR(pinfo);

            if (pinfo != NULL)
			{
                /* XXX - should probably check for proper form type here. */

                /* Seek relative to parent chunk. */
                if (mlFSeek(fp,pinfo->m_offset,SEEK_SET) == 0)
				{
                    while ((! found) && (curSize < pinfo->m_chunk.m_size))
					{
                        if (_readChunk(fp,&tmpInfo) == FALSE)
						{
                            break;
                        } else if (tmpInfo.m_chunk.m_tag != info->m_chunk.m_tag)
						{
                            seekTo = tmpInfo.m_chunk.m_size;
                            if (MLE_ODD(tmpInfo.m_chunk.m_size))
                                 seekTo += sizeof(MlByte);

                            if (mlFSeek(fp,seekTo,SEEK_CUR) == 0)
                                curSize += tmpInfo.m_chunk.m_size;
                            else
								break;
                        } else
						{
                            info->m_chunk.m_size = tmpInfo.m_chunk.m_size;
                            info->m_form = tmpInfo.m_form;
                            info->m_offset = tmpInfo.m_offset;
                            info->m_flags = tmpInfo.m_flags;
                            found = TRUE;
						}
					} /* While chunk not found. */
				}
			}

            break;

        case MLE_DPP_FINDPPBE:
            MLE_ASSERT(pinfo == NULL);

            /* Seek to beginning of file. */
            if (mlFSeek(fp,0,SEEK_SET) == 0)
			{
                while (! found)
				{
                    if (_readChunk(fp,&tmpInfo) == FALSE)
					{
                        break;
                    } else if ((tmpInfo.m_chunk.m_tag != MLE_PPBE_TAG) ||
                               (tmpInfo.m_form != info->m_form)) {
/*
XXX -- Remove this after a grace period. msm (Tue Aug 22 17:02:21 PDT 1995)
                        seekTo = tmpInfo.chunk.size;
                        if (MLE_ODD(tmpInfo.chunk.size))
                            seekTo += sizeof(unsigned char);

                        if (mlFSeek(fp,seekTo,SEEK_CUR) != 0)
			    break;
*/
                        break;
                    } else
					{
                        info->m_chunk.m_tag = tmpInfo.m_chunk.m_tag;
                        info->m_chunk.m_size = tmpInfo.m_chunk.m_size;
                        info->m_offset = tmpInfo.m_offset;
                        info->m_flags = tmpInfo.m_flags;
                        found = TRUE;
					}
				} /* While chunk not found. */
			}
            break;

        case MLE_DPP_FINDPPLE:
            MLE_ASSERT(pinfo == NULL);

            /* Seek to beginning of file. */
            if (mlFSeek(fp,0,SEEK_SET) == 0)
			{
                while (! found)
				{
                    if (_readChunk(fp,&tmpInfo) == FALSE)
					{
                        break;
                    } else if ((tmpInfo.m_chunk.m_tag != MLE_PPLE_TAG) ||
                               (tmpInfo.m_form != info->m_form)) {
/*
XXX -- Remove this after a grace period. msm (Tue Aug 22 17:02:21 PDT 1995)
                        seekTo = tmpInfo.chunk.size;
                        if (MLE_ODD(tmpInfo.chunk.size))
                            seekTo += sizeof(unsigned char);

                        if (mlFSeek(fp,seekTo,SEEK_CUR) != 0)
			    break;
*/
                        break;
                    } else
					{
                        info->m_chunk.m_tag = tmpInfo.m_chunk.m_tag;
                        info->m_chunk.m_size = tmpInfo.m_chunk.m_size;
                        info->m_offset = tmpInfo.m_offset;
                        info->m_flags = tmpInfo.m_flags;
                        found = TRUE;
					}
				} /* While chunk not found. */
			}
            break;

        case MLE_DPP_FINDLIST:
            MLE_VALIDATE_PTR(pinfo);

            if (pinfo != NULL)
			{
                /* XXX - should probably check for proper form type here. */

                /* Seek relative to parent chunk. */
                if (mlFSeek(fp,pinfo->m_offset,SEEK_SET) == 0)
				{
                    while ((! found) && (curSize < pinfo->m_chunk.m_size))
					{
                        if (_readChunk(fp,&tmpInfo) == FALSE)
						{
                            break;
                        } else if ((tmpInfo.m_chunk.m_tag != MLE_LIST_TAG) ||
                                   (tmpInfo.m_form != info->m_form))
						{
                            seekTo = tmpInfo.m_chunk.m_size;
                            if (MLE_ODD(tmpInfo.m_chunk.m_size))
                                 seekTo += sizeof(MlByte);

                            if (mlFSeek(fp,seekTo,SEEK_CUR) == 0)
                                curSize += tmpInfo.m_chunk.m_size;
                            else
							break;
                        } else
						{
                            info->m_chunk.m_tag = tmpInfo.m_chunk.m_tag;
                            info->m_chunk.m_size = tmpInfo.m_chunk.m_size;
                            info->m_offset = tmpInfo.m_offset;
                            info->m_flags = tmpInfo.m_flags;
                            found = TRUE;
						}
					} /* While chunk not found. */
				}
			}

            break;

        default:
            /* Begin reading from current file position. */

            /* Read next chunk. */
            if (_readChunk(fp,info) == TRUE)
                found = TRUE;
    }

    if (! found)
	{
        mlSetErrno(MLE_DPP_ERR_CHUNKNOTFOUND);
        retValue = FALSE;
    }
    mlFSeek(fp, 0, SEEK_CUR);
    return(retValue);
}


#if !defined(MLE_RUNTIME)
/*
 * The "info" structure must be filled with information about the chunk
 * being created. The MleDppChunkInfo structure should be set up as follows:
 *
 *     chunk.tag    - This field specifies the chunk tag of the chunk. If
 *                    the "flags" parameter includes MLE_DPP_CREATEPPBE,
 *                    MLE_DPP_CREATEPPLE, or MLE_DPP_CREATELIST, this field will
 *                    be filled by mlDppCreateChunk().
 *
 *     chunk.size   - This field specifies the size of the data portion of
 *                    the chunk, including the form type or list type (if
 *                    any). If this value is no correct when mlDppAscend()
 *                    is called to mark the end of the chunk, then
 *                    mlDppAscend() will correct the chunk size.
 *
 *     form         - This field specifies the form type or list type if
 *                    chunk is a "PPBE", "PPLE" or "LIST" chunk. If the
 *                    chunk is not one of these, this field need not be
 *                    filled in.
 *
 *     offset       - This field need not be filled in. mlDppCreateChunk()
 *                    will fill this field with the file offset of the
 *                    data portion of the chunk.
 *
 *     flags        - This field need not be filled either. mlDppCreateChunk()
 *                    will set the MLE_DPP_DIRTY flag.
 */

MlBoolean mlDppCreateChunk(FILE *fp,MleDppChunkInfo *info,MlUInt flags)
{
    /* declare local variables */
    MleDppChunk chunk;
    MlUInt offset;
    MlInt longWrite;
    MlBoolean retValue = TRUE;

    MLE_VALIDATE_PTR(fp);
    MLE_VALIDATE_PTR(info);

    if ((flags & MLE_DPP_CREATEPPBE) == MLE_DPP_CREATEPPBE)
	{
        /* Create the chunk tag. */
        chunk.m_tag = MLE_PPBE_TAG;

        /* Set the chunk tag field in "info". */
        info->m_chunk.m_tag = chunk.m_tag;
    } else if ((flags & MLE_DPP_CREATEPPLE) == MLE_DPP_CREATEPPLE)
	{
        /* Create the chunk tag. */
        chunk.m_tag = MLE_PPLE_TAG;

        /* Set the chunk tag field in "info". */
        info->m_chunk.m_tag = chunk.m_tag;
    } else if ((flags & MLE_DPP_CREATELIST) == MLE_DPP_CREATELIST)
	{
        /* Create the chunk tag. */
        chunk.m_tag = MLE_LIST_TAG;

        /* set the chunk tag field in "info". */
        info->m_chunk.m_tag = chunk.m_tag;
    } else {
        /* The chunk tag should already be set by caller. */
        chunk.m_tag = info->m_chunk.m_tag;
    }
    chunk.m_size = info->m_chunk.m_size;
    offset = mlFTell(fp);

    /* Write chunk to file. */
    longWrite = chunk.m_tag;
    if (mlDppGetSwapWrite())
        longWrite = mlDppSwapInt(longWrite);
    if (mlFWrite(&longWrite,sizeof(MlInt),1,fp) != 1)
	{
        mlSetErrno(MLE_DPP_ERR_CANNOTWRITE);
        return(FALSE);
    }
    longWrite = chunk.m_size;
    if (mlDppGetSwapWrite())
        longWrite = mlDppSwapInt(longWrite);
    if (mlFWrite(&longWrite,sizeof(MlInt),1,fp) != 1)
	{
        mlSetErrno(MLE_DPP_ERR_CANNOTWRITE);
        return(FALSE);
    }

    /* Update offset. */
    offset += sizeof(MleDppChunk);

    /* write the form type, if necessary */
    if (((flags & MLE_DPP_CREATEPPBE) == MLE_DPP_CREATEPPBE) ||
        ((flags & MLE_DPP_CREATEPPLE) == MLE_DPP_CREATEPPLE) ||
        ((flags & MLE_DPP_CREATELIST) == MLE_DPP_CREATELIST))
	{
        longWrite = info->m_form;
        if (mlDppGetSwapWrite())
            longWrite = mlDppSwapInt(longWrite);
        if (mlFWrite(&longWrite,sizeof(MlInt),1,fp) != 1)
		{
            mlSetErrno(MLE_DPP_ERR_CANNOTWRITE);
            return(FALSE);
        }

        /* Update offset and flags. */
        offset += sizeof(MlDppTag);
    }

    /* Set offset field in "info" structure. */
    info->m_offset = offset;

    /* Mark chunk as being dirty. */
    info->m_flags |= MLE_DPP_DIRTY;

    return(retValue);
}


MlDppTag mlDppStringToTag(char *str,MlUInt flags)
{
    /* Declare local variables. */
    MlDppTag tag;
    char ch0 = ' ',ch1 = ' ',ch2 = ' ',ch3 = ' ';
    MlUInt len;

    MLE_VALIDATE_PTR(str);

    len = strlen(str);
    if (len >= 1) ch0 = *str++;
    if (len >= 2) ch1 = *str++;
    if (len >= 3) ch2 = *str++;
    if (len >= 4) ch3 = *str;

    if (flags == MLE_DPP_TOUPPER)
	{
        ch0 = toupper(ch0);
        ch1 = toupper(ch1);
        ch2 = toupper(ch2);
        ch3 = toupper(ch3);
    }

    tag = mlDppMakeTag(ch0,ch1,ch2,ch3);

    return(tag);
}


char *mlDppTagToString(MlDppTag tag)
{
    /* Declare local variables. */
    char *str;

    if ((str = (char *)mlMalloc(sizeof(MlDppTag) + 1)) != NULL)
	{
        str[0] = (char)((tag & 0xff000000) >> 24L);
        str[1] = (char)((tag & 0x00ff0000) >> 16L);
        str[2] = (char)((tag & 0x0000ff00) >> 8L);
        str[3] = (char)(tag & 0x000000ff);
        str[4] = '\0';
    }

    return(str);
}


#endif /* MLE_RUNTIME */


int mlDppCheckFormat(FILE *fp)
{
    /* Declare local variables. */
    MlDppTag chunkFileTag;
    long offset;

    /* Read first 4 bytes in file. */
    size_t nBytes;
    offset = mlFTell(fp);
    mlFSeek(fp,0L,SEEK_SET);
    nBytes = mlFRead(&chunkFileTag,sizeof(MlDppTag),1,fp);
    mlFSeek(fp,offset,SEEK_SET);

#if BYTE_ORDER == BIG_ENDIAN
    if (chunkFileTag == MLE_PPBE_TAG) {
        /* Chunk file is in Big Endian format. */
        return(1);
    } else if (chunkFileTag == mlDppMakeTag('E','L','P','P'))
	{
        /* Chunk file is in Little Endian format. */
        return(2);
    } else {
        /* Unknown format */
        fprintf(stderr,"Unknown Chunk File format: %d\n",
                chunkFileTag);
        return(-1);
    }
#elif BYTE_ORDER == LITTLE_ENDIAN
    if (chunkFileTag == MLE_PPLE_TAG)
	{
        /* Chunk file is in Little Endian format */
        return(2);
    } else if (chunkFileTag == mlDppMakeTag('E','B','P','P'))
	{
        /* Chunk file is in Bit Endian format */
        return(1);
    } else {
        /* Unknown format */
        fprintf(stderr,"Unknown Chunk File format: %d\n",
                chunkFileTag);
        return(-1);
    }
#else
    /* Unknown format */
    fprintf(stderr,"Unknown Chunk File format: %d\n",
            chunkFileTag);
    return(-1);
#endif
}


#ifdef UNIT_TEST

/* include system header files */
#include <stdlib.h>
#include <sys/types.h>
#include <time.h>

/* Include Magic Lantern header files. */

#define TEST_FILE "test.dpp"


main(int argc,char *argv[])
{
    /* Declare local variables */
    MleDppChunkInfo  ppbeInfo;
    MleDppChunkInfo  hdrInfo;
    MleDppChunkInfo  actrInfo;
    MleDppHeader     hdrData;
    MlInt            dateStamp;
    FILE            *fp;

    /* Initialize the test. */
    fprintf(stdout,"Testing Digital Playprint Creation ...\n");
    memset(&ppbeInfo,0,sizeof(MleDppChunkInfo));
    memset(&hdrInfo,0,sizeof(MleDppChunkInfo));
    memset(&actrInfo,0,sizeof(MleDppChunkInfo));

    /* Create a Playprint file.  */
    fp = mlFOpen(TEST_FILE,"w+");
    MLE_VALIDATE_PTR(fp);

    /* Initialize the Playprint file. */
    ppbeInfo.m_form = mlDppMakeTag('M','L','R','T');
    MLE_WARN(mlDppCreateChunk(fp,&ppbeInfo,MLE_DPP_CREATEPPBE));

    hdrInfo.m_chunk.m_tag = mlDppMakeTag('h','d','r',' ');
    hdrInfo.m_chunk.m_size = sizeof(MleDppHeader);
    MLE_WARN(mlDppCreateChunk(fp,&hdrInfo,0));

    hdrData.m_version = 1;
    dateStamp = time(NULL);
    hdrData.m_date = dateStamp;
    if (mlFWrite(&hdrData,sizeof(hdrData),1,fp) != 1)
	{
        mlFClose(fp);
        mlUnlink(TEST_FILE);
        exit(-1);
    }

    MLE_WARN(mlDppAscend(fp,&hdrInfo,0));

    /* Write Magic Lantner Runtime Data into Playprint file. */
    actrInfo.m_form = mlDppMakeTag('G','R','P','L');
    MLE_WARN(mlDppCreateChunk(fp,&actrInfo,MLE_DPP_CREATELIST));

    MLE_WARN(mlDppAscend(fp,&actrInfo,0));
    MLE_WARN(mlDppAscend(fp,&ppbeInfo,0));

    /* Close Playprint file. */
    mlFClose(fp);

    /* Retrieve the Playprint file. */
    fprintf(stdout,"Testing Digital Playprint Retrieval ...\n");
    memset(&ppbeInfo,0,sizeof(MleDppChunkInfo));
    memset(&hdrInfo,0,sizeof(MleDppChunkInfo));
    memset(&actrInfo,0,sizeof(MleDppChunkInfo));

    fp = mlFOpen(TEST_FILE,"r");
    MLE_VALIDATE_PTR(fp);

    ppbeInfo.m_form = mlDppMakeTag('M','L','R','T');
    MLE_WARN(mlDppDescend(fp,&ppbeInfo,NULL,MLE_DPP_FINDPPBE));
    fprintf(stdout,"FORM TYPE: %s\n",mlDppTagToString(ppbeInfo.m_form));
    fprintf(stdout,"FORM SIZE: %d\n",ppbeInfo.m_chunk.m_size);

    MLE_WARN(mlDppDescend(fp,&hdrInfo,NULL,0));
    if (hdrInfo.m_chunk.m_tag == mlDppMakeTag('h','d','r',' '))
	{
        mlFRead(&hdrData,hdrInfo.m_chunk.m_size,1,fp);
        fprintf(stdout,"\tCHUNK TAG: %s\n",mlDppTagToString(hdrInfo.m_chunk.m_tag));
        fprintf(stdout,"\tCHUNK SIZE: %d\n",hdrInfo.m_chunk.m_size);
        fprintf(stdout,"\tCHUNK DATA:\n");
        fprintf(stdout,"\t\tHeader Version: %d\n",hdrData.m_version);
        fprintf(stdout,"\t\tHeader Date: %s\n",ctime((time_t *)&hdrData.m_date));
    }
    MLE_WARN(mlDppAscend(fp,&hdrInfo,0));

    actrInfo.m_form = mlDppMakeTag('G','R','P','L');
    MLE_WARN(mlDppDescend(fp,&actrInfo,NULL,0));
    fprintf(stdout,"\tFORM TYPE: %s\n",mlDppTagToString(actrInfo.m_form));
    fprintf(stdout,"\tFORM SIZE: %d\n",actrInfo.m_chunk.m_size);
    MLE_WARN(mlDppAscend(fp,&actrInfo,0));

    MLE_WARN(mlDppAscend(fp,&ppbeInfo,0));

    /* Close Playprint file */
    mlFClose(fp);

    /* remove Playprint file */
    /* mlUnlink(TEST_FILE); */
}

#endif /* UNIT_TEST */
