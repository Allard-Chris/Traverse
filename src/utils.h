#ifndef _HEADER_UTILS_H
#define _HEADER_UTILS_H

// imports libs
#include <errno.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

// type
typedef unsigned char u8;
typedef signed char   s8;

// constants
#define MAX_DIGITS     3
#define SAVES_FOLDER   "/.cache/traverse/"
#define INDEX_2D(x, y) ((y * CHESSBOARD_SIZE) + x)
#define INDEX_4D(x1, y1, x2, y2)                                                                                       \
  (x1 + (y1 * CHESSBOARD_SIZE) + (x2 * CHESSBOARD_SIZE * CHESSBOARD_SIZE) +                                            \
   (y2 * CHESSBOARD_SIZE * CHESSBOARD_SIZE * CHESSBOARD_SIZE))

extern const char* ITOA[10];

#endif // _UTILS_TRAVERSE_H