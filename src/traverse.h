#ifndef _HEADER_TRAVERSE_H
#define _HEADER_TRAVERSE_H

// imports libs
#include "utils.h"
#include <stdio.h>
#include <stdlib.h>

// players id
#define PLAYER1 0
#define PLAYER2 1
#define PLAYER3 2
#define PLAYER4 3

// players base
#define PLAYER1_BASE 9
#define PLAYER2_BASE 0
#define PLAYER3_BASE 0
#define PLAYER4_BASE 9

// players goal
#define PLAYER1_GOAL PLAYER2_BASE
#define PLAYER2_GOAL PLAYER1_BASE
#define PLAYER3_GOAL PLAYER4_BASE
#define PLAYER4_GOAL PLAYER3_BASE

// game constants
#define CHESSBOARD_SIZE    10
#define MAX_NB_PLAYER      4
#define DEFAULT_NB_PLAYER  2
#define DEFAULT_DIFFICULTY 0
#define OUT_OF_BOUND       254
#define NB_PAWNS_TYPE      7
#define NB_PAWNS           8
#ifndef FALSE
#define FALSE 0
#endif
#ifndef TRUE
#define TRUE 1
#endif

// pawn type constants
#define SQUARE     0
#define TRIANGLE_N 1
#define TRIANGLE_S 2
#define TRIANGLE_E 3
#define TRIANGLE_W 4
#define DIAMOND    5
#define CIRCLE     6

// type of cell
#define IS_OUT_OF_BOUND 0
#define IS_IN_EDGE      1
#define IS_IN_GOAL      2
#define IS_EMPTY        3
#define IS_FULL         4

// constant array
extern const int   PLAYER_GOALS[4];
extern const char* PLAYERS_STR[4];
// structures
// position of a pawn
typedef struct position {
  int line;
  int column;
} position;

// used when compute future moves
typedef struct neighbourCells {
  struct position pos;
  int usedByType[8]; // example usedByType[SQUARE] = 1 if this neighbour need to be test for this type of pawn
} neighbourCells;

// linked list for all moves we can do
typedef struct move {
  struct position newPos;
  u8              mustJump;  // set at TRUE if we must to jump again
  struct jump*    path;      // the path we travel to get into this position (used when chained jumps)
  struct move*    pNextMove; // pointer to the next move we can do
} move;

// linked list for all move we must to do when doing chained jumps
typedef struct jump {
  struct position pos;
  struct jump*    pNextJump; // pointer to the next jump
} jump;

// define what is a pawn in this game
typedef struct pawn {
  u8              type; // 0:square, 1:triangle_n, 2:triangle_s, 3:triangle_e, 4:triangle_w, 5:diamond, 6:circle
  struct position pos;
  u8              playerId; // 0, 1, 2 or 3
  u8              pawnId;   // unique id
} pawn;

// functions
// all for computes moves
jump* CreateNewNump(u8 line, u8 column);
jump* PushNewJump(jump* path, jump* newJump);
move* CreateNewMove(u8 line, u8 column, u8 mustJump, jump* path);
move* PushNewMove(move* allNewMoves, move* newMove);
move* ComputeFutureMoves(u8 cLine, u8 cColumn, u8 oLine, u8 oColumn, u8 type, u8 playerId, u8 mustJump,
                         pawn** pChessboard);
void  FreeLinkedListMoves(move** allNewMoves);
void  FreeLinkedListJumps(jump** path);
// game logic
pawn* InitPawn(u8 type, u8 line, u8 column, u8 playerId, u8 pawnId);
void  FreePlayerPawns(pawn** pPlayerPawns);
void  ClearChessboard(pawn** pChessboard);
void  UpdateChessboard(u8 nbPlayers, pawn*** pPlayersPawns, pawn** pChessboard);
u8    GetNbMoves(u8 playerId, pawn** pPlayerPawns, pawn** pChessboard);
u8    GetTypeOfCell(u8 line, u8 column, u8 playerId, pawn** pChessboard);

#endif // _HEADER_TRAVERSE_H