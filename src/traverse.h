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

// game constants
#define CHESSBOARD_SIZE    10
#define MAX_NB_PLAYER      4
#define DEFAULT_NB_PLAYER  2
#define DEFAULT_DIFFICULTY 0
#define OUT_OF_BOUND       254
#define MAX_DEPTH          10
#define NB_PAWNS_TYPE      7
#define NB_PAWNS           8
#define CPU                1
#define HUMAIN             0
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

// position of a pawn
typedef struct Position {
  int line;
  int column;
} Position;

// used when compute future moves to know where we can go
typedef struct PawnShift {
  struct Position shiftPosition;
  int             pawnType[8];
} PawnShift;

// linked list for all moves we can do
typedef struct Move {
  struct Position newPosition;
  struct Move*    next;    // pointer to the next move we can do
  struct Move*    from;    // path to this position
  u8              jumped;  // set at TRUE if we must to jump again
  u8              canStay; // if we can stay on this cell
} Move;

// define what is a pawn in this game
typedef struct Pawn {
  struct Position pos;
  u8              type;     // 0:square, 1:triangle_n, 2:triangle_s, 3:triangle_e, 4:triangle_w, 5:diamond, 6:circle
  u8              playerId; // 0, 1, 2 or 3
  u8              pawnId;   // unique id
} Pawn;

// use to store all score for CPU moves
typedef struct AiMove {
  float           score; // score for this AiMove;
  u8              pawnId;
  struct Position newPosition;
  struct AiMove*  next;
} AiMove;

// constant array
extern const Position  PLAYER1_GOAL[NB_PAWNS];      // NOLINT
extern const Position  PLAYER2_GOAL[NB_PAWNS];      // NOLINT
extern const Position  PLAYER3_GOAL[NB_PAWNS];      // NOLINT
extern const Position  PLAYER4_GOAL[NB_PAWNS];      // NOLINT
extern const Position* PLAYERS_GOAL[MAX_NB_PLAYER]; // NOLINT

// functions for pawns
Pawn* CreatePawn(u8 line, u8 column, u8 type, u8 playerId, u8 pawnId);
void  FreePlayerPawns(Pawn** playerPaws);
u8    IsPawnInsideGoal(u8 playerId, Pawn* pawn);

// functions for chessboard
void ClearChessboard(Pawn** pChessboard);
void UpdateChessboard(u8 nbPlayers, Pawn*** pPlayersPawns, Pawn** pChessboard);
void MovePawn(Pawn** pChessboard, Pawn* pawn, u8 newLine, u8 newColumn);
u8   GetTypeOfCell(u8 line, u8 column, u8 playerId, Pawn** pChessboard);
u8   IsCellPlayerGoal(u8 line, u8 column, u8 playerId);

// functions for calculate possible movements
Move* ComputeFutureMoves(Move* head, Move* from, u8 depth, u8 oldLine, u8 oldColumn, Pawn* pawn, u8 jumped,
                         Pawn** pChessboard);
Move* CreateMove(u8 line, u8 column, u8 jumped, u8 canStay, Move* from);
Move* TopPushMove(Move* head, Move* newMove);
Move* BottomPushMove(Move* head, Move* newMove);
u8    IsAlreadyExiste(Move* head, u8 line, u8 column);
void  FreeLinkedListMoves(Move** head);

// functions for game logic
u8*   ComputeSqrtDistanceTable();
s8    GameOver(u8 nbPlayer, Pawn*** pPlayersPawns);
float GetPlayerScore(u8 playerId, Pawn** pPlayerPawns);
float GetPlayerAverageDistance(u8 playerId, Pawn** pPlayerPawns, Pawn** pChessboard, u8* sqrtDistanceTable);
float GetGameScore(Pawn*** pPlayersPawns, Pawn** pChessboard, u8* sqrtDistanceTable);

// functions for AiMove
float   MinMax(Pawn** pChessboard, u8 depth, u8 player, Pawn*** pPlayersPawns, u8* sqrtDistanceTable);
AiMove* CreateAiMove(float score, u8 pawnId, u8 line, u8 column);
AiMove* TopPushAiMove(AiMove* head, AiMove* newAiMove);
AiMove* BottomPushAiMove(AiMove* head, AiMove* newAiMove);
void    FreeLinkedListAiMoves(AiMove** head);

#endif // _HEADER_TRAVERSE_H