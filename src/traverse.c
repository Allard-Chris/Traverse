#include "traverse.h"

// constant array
const char* PLAYERS_STR[4] = {"Player 1", "Player 2", "Player 3", "Player 4"};
const int   PLAYER_GOALS[4] = {PLAYER1_GOAL, PLAYER2_GOAL, PLAYER3_GOAL, PLAYER4_GOAL};

// alloc new pawn
pawn* InitPawn(u8 type, u8 line, u8 column, u8 playerId, u8 pawnId) {
  pawn* newPawn;
  newPawn = (pawn*)malloc(sizeof(struct pawn));
  if (newPawn == NULL) {
    perror("Error: can't init new pawn. Exit.\n");
    exit(-1);
  }
  newPawn->type = type;
  newPawn->pos.column = column;
  newPawn->pos.line = line;
  newPawn->playerId = playerId;
  newPawn->pawnId = pawnId;
  return newPawn;
}

// free all pawns for a player
void FreePlayerPawns(pawn** pPlayerPawns) {
  for (u8 pawnId = 0; pawnId < NB_PAWNS; pawnId++) {
    if (pPlayerPawns[pawnId] != NULL) {
      free(pPlayerPawns[pawnId]);
      pPlayerPawns[pawnId] = NULL;
    }
  }
}

// All cells on the chessboard init with null address
void ClearChessboard(pawn** pChessboard) {
  for (u8 line = 0; line < CHESSBOARD_SIZE; line++) {
    for (u8 column = 0; column < CHESSBOARD_SIZE; column++) {
      pChessboard[INDEX_2D(column, line)] = NULL;
    }
  }
}

// Set the address of the pawn, on his current Cell
void UpdateChessboard(u8 nbPlayers, pawn*** pPlayersPawns, pawn** pChessboard) {
  u8 line;
  u8 column;
  for (u8 playerId = 0; playerId < nbPlayers; playerId++) {
    for (u8 pawnId = 0; pawnId < NB_PAWNS; pawnId++) {
      line = pPlayersPawns[playerId][pawnId]->pos.line;
      column = pPlayersPawns[playerId][pawnId]->pos.column;
      pChessboard[INDEX_2D(column, line)] = pPlayersPawns[playerId][pawnId];
    }
  }
}

// Create a new move, must be added inside linked list
move* CreateNewMove(u8 line, u8 column, u8 mustJump, jump* path) {
  // alloc in the memory the new move
  // DON'T FORGET TO FREE THE MEMORY, that's what the professor was yelling at me !
  move* newMove;
  newMove = (move*)malloc(sizeof(struct move));
  if (newMove == NULL) {
    perror("Error: can't init new move. Exit.\n");
    exit(-1);
  }
  newMove->newPos.line = line;
  newMove->newPos.column = column;
  newMove->mustJump = mustJump;
  newMove->path = path;
  newMove->pNextMove = NULL; // by default, they are no next move
  return newMove;
}

// push a new move at the end of the linked list
move* PushNewMove(move* allNewMoves, move* newMove) {
  if (newMove != NULL) {
    // we are at the end of the linkedlist
    if (allNewMoves == NULL) return newMove;
    allNewMoves->pNextMove = PushNewMove(allNewMoves->pNextMove, newMove);
  }
  return allNewMoves; // return the new linkedlist with the new move at the end
}

// free the memory when we finish to use it
void FreeLinkedListMoves(move** allNewMoves) {
  move* tmp; // to keep the next move to erase before remove it in the current move
  while (*allNewMoves != NULL) {
    tmp = *allNewMoves;
    FreeLinkedListJumps(&(tmp->path));
    *allNewMoves = (move*)(*allNewMoves)->pNextMove;
    free(tmp);
  }
  allNewMoves = NULL;
}

// Create a new move, must be added inside linked list
jump* CreateNewJump(u8 line, u8 column) {
  // alloc in the memory the new jumpsPath
  // DON'T FORGET TO FREE THE MEMORY
  jump* path;
  path = (jump*)malloc(sizeof(struct jump));
  if (path == NULL) {
    perror("Error: can't init new jump. Exit.\n");
    exit(-1);
  }
  path->pos.line = line;
  path->pos.column = column;
  path->pNextJump = NULL; // by default, they are no next move
  return path;
}

// push a new jump at the end of the linked list
jump* PushNewJump(jump* path, jump* newJump) {
  if (newJump != NULL) {
    // we are at the end of the linkedlist
    if (path == NULL) return newJump;
    path->pNextJump = PushNewJump(path->pNextJump, newJump);
  }
  return path; // return the new linkedlist with the new jump at the end
}

// free the memory when we finish to use it
void FreeLinkedListJumps(jump** path) {
  jump* tmp;
  while (*path != NULL) {
    tmp = *path;
    *path = (jump*)(*path)->pNextJump;
    free(tmp);
  }
  path = NULL; // just to be sure
}

// return what kind of Cell it is for the player
u8 GetTypeOfCell(u8 line, u8 column, u8 playerId, pawn** pChessboard) {
  u8 typeOfCell = 0;
  // first are we outside the chessboard ?
  if ((line < 0) || (line > (CHESSBOARD_SIZE - 1)) || (column < 0) || (column > (CHESSBOARD_SIZE - 1))) {
    typeOfCell = IS_OUT_OF_BOUND;
  } else {
    // the cell is empty ?
    if (pChessboard[(line * CHESSBOARD_SIZE) + column] == NULL) {
      typeOfCell = IS_EMPTY;
      // the cell is in the edge of the chessboard
      if ((line == 0) || (line == 9) || (column == 0) || (column == 9)) typeOfCell = IS_IN_EDGE;
      // it is our goal ?
      if ((playerId == PLAYER1) || (playerId == PLAYER2)) {
        if (line == PLAYER_GOALS[playerId]) typeOfCell = IS_IN_GOAL;
      } else {
        if (column == PLAYER_GOALS[playerId]) typeOfCell = IS_IN_GOAL;
      }
    } else
      typeOfCell = IS_FULL;
  }
  return typeOfCell;
}

// compute all the moves we can do
move* ComputeFutureMoves(u8 cLine, u8 cColumn, u8 oLine, u8 oColumn, u8 type, u8 playerId, u8 mustJump,
                         pawn** pChessboard) {
  move* allNewMoves = NULL;
  move* tmpNewMoves = NULL;
  jump* tmpJump = NULL;
  u8    cTypeOfCell = 0;
  // neighbour
  u8 nLine = 0;
  u8 nColumn = 0;
  u8 nTypeOfCell = 0;
  // jumpedNeighbour
  u8 jnLine = 0;
  u8 jnColumn = 0;
  u8 jTypeOfCell = 0;

  // the neighbour we can test, by our type
  struct neighbourCells neighbours[8] = {
      // L   C    S      T_N    T_S    T_E    T_W    D      C
      {{-1, -1}, {FALSE, TRUE, FALSE, FALSE, TRUE, TRUE, TRUE}}, // up left
      {{-1, 0}, {TRUE, FALSE, TRUE, FALSE, FALSE, FALSE, TRUE}}, // up
      {{-1, +1}, {FALSE, TRUE, FALSE, TRUE, FALSE, TRUE, TRUE}}, // up right
      {{0, +1}, {TRUE, FALSE, FALSE, FALSE, TRUE, FALSE, TRUE}}, // right
      {{+1, +1}, {FALSE, FALSE, TRUE, TRUE, FALSE, TRUE, TRUE}}, // down right
      {{+1, 0}, {TRUE, TRUE, FALSE, FALSE, FALSE, FALSE, TRUE}}, // down
      {{+1, -1}, {FALSE, FALSE, TRUE, FALSE, TRUE, TRUE, TRUE}}, // down left
      {{0, -1}, {TRUE, FALSE, FALSE, TRUE, FALSE, FALSE, TRUE}}  // left
  };

  // do we need to jump to move ?
  if (mustJump == FALSE) {
    for (u8 i = 0; i < 8; i++) {                    // for all neighbours around this cell
      if (neighbours[i].usedByType[type] == TRUE) { // if this neighbours can be tested by our type

        // compute neighbours positions and type of cell
        nLine = cLine + neighbours[i].pos.line;
        nColumn = cColumn + neighbours[i].pos.column;
        jnLine = cLine + (neighbours[i].pos.line * 2);
        jnColumn = cColumn + (neighbours[i].pos.column * 2);
        nTypeOfCell = GetTypeOfCell(nLine, nColumn, playerId, pChessboard);
        jTypeOfCell = GetTypeOfCell(jnLine, jnColumn, playerId, pChessboard);

        if ((nTypeOfCell == IS_IN_GOAL) || (nTypeOfCell == IS_EMPTY)) {
          // OK for this position
          allNewMoves = PushNewMove(allNewMoves, CreateNewMove(nLine, nColumn, FALSE, NULL));
        } else if ((nTypeOfCell == IS_FULL) && ((jTypeOfCell != IS_OUT_OF_BOUND) && (jTypeOfCell != IS_FULL))) {
          // Ok check chained jumps
          tmpNewMoves = ComputeFutureMoves(jnLine, jnColumn, cLine, cColumn, type, playerId, TRUE, pChessboard);
          if (tmpNewMoves != NULL) allNewMoves = PushNewMove(allNewMoves, tmpNewMoves);
        }
      }
    }
  } else {                                          // we must to jump to move
    for (u8 i = 0; i < 8; i++) {                    // for all neighbours around this cell
      if (neighbours[i].usedByType[type] == TRUE) { // if this neighbours can be tested by our type
        // compute neighbours positions and type of cell
        nLine = cLine + neighbours[i].pos.line;
        nColumn = cColumn + neighbours[i].pos.column;
        jnLine = cLine + (neighbours[i].pos.line * 2);
        jnColumn = cColumn + (neighbours[i].pos.column * 2);
        nTypeOfCell = GetTypeOfCell(nLine, nColumn, playerId, pChessboard);
        jTypeOfCell = GetTypeOfCell(jnLine, jnColumn, playerId, pChessboard);
        cTypeOfCell = GetTypeOfCell(cLine, cColumn, playerId, pChessboard);

        // only test if it's not our old position, to avoid infinite compute
        // only test jumpedNeighbours, if our neighbourCell if full
        if (((jnLine != oLine) || (jnColumn != oColumn)) && (nTypeOfCell == IS_FULL)) {
          if ((jTypeOfCell != IS_OUT_OF_BOUND) && ((jTypeOfCell == IS_EMPTY) || (jTypeOfCell == IS_IN_GOAL))) {
            // Ok check chained jumps
            tmpNewMoves = ComputeFutureMoves(jnLine, jnColumn, cLine, cColumn, type, playerId, TRUE, pChessboard);
            if (tmpNewMoves != NULL) {
              PushNewJump(tmpNewMoves->path, CreateNewJump(oLine, oColumn));
              allNewMoves = PushNewMove(allNewMoves, tmpNewMoves);
            }
          }
        }
      }
    }
    if (((cTypeOfCell == IS_IN_GOAL) || (cTypeOfCell == IS_EMPTY))) {
      // we can stay where we are
      tmpJump = CreateNewJump(oLine, oColumn);
      allNewMoves = PushNewMove(allNewMoves, CreateNewMove(cLine, cColumn, TRUE, tmpJump));
    }
  }
  return allNewMoves;
}

// Check if we can move any pawn we have
u8 GetNbMoves(u8 playerId, pawn** pPawnsplayerId, pawn** pChessboard) {
  u8    nbMoves = 0;
  u8    cLine = 0;
  u8    cColumn = 0;
  u8    currentType = 0;
  move* tmp = NULL;
  for (u8 currentPawnId = 0; currentPawnId < NB_PAWNS; currentPawnId++) {
    tmp = NULL;
    cLine = pPawnsplayerId[currentPawnId]->pos.line;
    cColumn = pPawnsplayerId[currentPawnId]->pos.column;
    currentType = pPawnsplayerId[currentPawnId]->type;
    tmp = ComputeFutureMoves(cLine, cColumn, OUT_OF_BOUND, OUT_OF_BOUND, currentType, playerId, FALSE, pChessboard);
    if (tmp != NULL) nbMoves++;
    FreeLinkedListMoves(&tmp);
  }
  return nbMoves;
}
