/*
 ============================================================================
 Name        : exampleProgram.c
 Author      : Mark Millard
 Version     :
 Copyright   : (C) Wizzer Works 2015
 Description : Uses shared library to print greeting
               To run the resulting executable the LD_LIBRARY_PATH must be
               set to ${project_loc}/libDPP/.libs
               Alternatively, libtool creates a wrapper shell script in the
               build directory of this program which can be used to run it.
               Here the script will be called exampleProgram.
 ============================================================================
 */

#include "libDPP.h"

unsigned short g_mlErrno;

int main(void) {
  return 0;
}
