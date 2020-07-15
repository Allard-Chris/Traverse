#include "traverse.h"

/* constant array */
const char* TERMINAL_COLORS[4]  = {ANSI_COLOR_RED, ANSI_COLOR_BLUE, ANSI_COLOR_GREEN, ANSI_COLOR_YELLOW};
const char* PAWN_TYPE_SYM[7]    = {SYM_SQUARE, SYM_TRIANGLE_N, SYM_TRIANGLE_S, SYM_TRIANGLE_E, SYM_TRIANGLE_W, SYM_DIAMOND, SYM_CIRCLE};
const char* PAWN_TYPE_STR[7]    = {"SQUARE", "TRIANGLE_N", "TRIANGLE_S", "TRIANGLE_E", "TRIANGLE_W", "DIAMOND", "CIRCLE"};
const char* COLUMN_STR[10]      = {"A", "B", "C", "D", "E", "F", "G", "H", "I", "J"};
const char* PLAYERS_STR[4]      = {"Player 1", "Player 2", "Player 3", "Player 4"};
const int   PLAYER_GOALS[4]     = {PLAYER1_GOAL, PLAYER2_GOAL, PLAYER3_GOAL, PLAYER4_GOAL};

/* alloc new pawn */
pawn* InitPawn(u8 type, u8 line, u8 column, u8 player, u8 pawnId){
  pawn* newPawn;
  newPawn = (pawn *)malloc(sizeof(struct pawn));
  newPawn->type = type;
  newPawn->pos.column = column;
  newPawn->pos.line = line;
  newPawn->player = player;
  newPawn->pawnId = pawnId;
  return newPawn;
}

/* free all pawns for a player*/
void FreePlayerPawns(pawn** pPawnsPlayer){
  for (u8 pawnId = 0; pawnId < NB_PAWNS; pawnId++){
    if (pPawnsPlayer[pawnId] != NULL) {
      free(pPawnsPlayer[pawnId]);
      pPawnsPlayer[pawnId] = NULL;
    }
  }
}

/* All cells on the chessboard init with null address */
void ClearChessboard(pawn** pChessboard) {
  for (u8 line = 0; line < CHESSBOARD_SIZE; line++) {
    for (u8 column = 0; column < CHESSBOARD_SIZE; column++) {
      pChessboard[(line * CHESSBOARD_SIZE) + column] = NULL;
    }
  }
}

/* Set the address of the pawn, on his current Cell */
void UpdateChessboard(u8 nbPlayers, pawn*** pPlayersPawns, pawn** pChessboard) {
  u8 line;
  u8 column;
  for (u8 currentPlayer = 0; currentPlayer < nbPlayers; currentPlayer++) {
    for (u8 currentPawn = 0; currentPawn < NB_PAWNS; currentPawn++) {
      line = pPlayersPawns[currentPlayer][currentPawn]->pos.line;
      column = pPlayersPawns[currentPlayer][currentPawn]->pos.column;
      pChessboard[(line * CHESSBOARD_SIZE) + column] = pPlayersPawns[currentPlayer][currentPawn];
    }
  }
}

/* draw chessboard in the terminal */
void  DrawChessboard(pawn** pChessboard, u8 currentPlayer, u8 round) {
  system(CLEAR);
  printf("------------------------------------------------------------------------\n");
  printf("Round: %d\n", round);
  printf("It's time for: %sPlayer %d%s\n", TERMINAL_COLORS[currentPlayer], currentPlayer, ANSI_COLOR_RESET);
  printf("\n");
  printf("     A      B      C      D      E      F      G      H      I      J\n");
  printf("  |-----||-----||-----||-----||-----||-----||-----||-----||-----||-----|\n");
  for (u8 line = 0; line < CHESSBOARD_SIZE; line++) {
    printf("%d ", line);
    for (u8 column = 0; column < CHESSBOARD_SIZE; column++) {
      if (pChessboard[(line * CHESSBOARD_SIZE) + column] != NULL){
          printf("|%s %s_%d %s|", TERMINAL_COLORS[pChessboard[(line * CHESSBOARD_SIZE) + column]->player], PAWN_TYPE_SYM[pChessboard[(line * CHESSBOARD_SIZE) + column]->type], pChessboard[(line * CHESSBOARD_SIZE) + column]->pawnId, ANSI_COLOR_RESET);
      } else {
        printf("|     |");
      }
    }
    printf("\n");
    printf("  |-----||-----||-----||-----||-----||-----||-----||-----||-----||-----|\n");
  }
  printf("\n");
  printf("------------------------------------------------------------------------\n");
}

/* Create a new move, must be added inside linked list */
move* CreateNewMove(u8 line, u8 column, u8 mustJump, jump* path) {
  /* alloc in the memory the new move */
  /* DON'T FORGET TO FREE THE MEMORY, that's what the professor was yelling at me ! */
  move* newMove;
  newMove = (move*)malloc(sizeof(struct move));
  newMove->newPos.line = line;
  newMove->newPos.column = column;
  newMove->mustJump = mustJump;
  newMove->path = path;
  newMove->pNextMove = NULL; /* by default, they are no next move */
  return newMove;
}

/* push a new move at the end of the linked list */
move* PushNewMove(move* allNewMoves, move* newMove) {
  if (newMove != NULL) {
    /* we are at the end of the linkedlist */
    if (allNewMoves == NULL) return newMove;
    allNewMoves->pNextMove = PushNewMove(allNewMoves->pNextMove, newMove);
  }
  return allNewMoves; /* return the new linkedlist with the new move at the end */
}

/* free the memory when we finish to use it */
void FreeLinkedListMoves(move* allNewMoves) {
  move* tmp; /* to keep the next move to erase before remove it in the current move */
  while (allNewMoves != NULL) {
    tmp = allNewMoves->pNextMove;
    FreeLinkedListJumps(allNewMoves->path);
    free(allNewMoves);
    allNewMoves = tmp;
  }
}

/* Create a new move, must be added inside linked list */
jump* CreateNewJump(u8 line, u8 column) {
  /* alloc in the memory the new jumpsPath */
  /* DON'T FORGET TO FREE THE MEMORY */
  jump* path;
  path = (jump*)malloc(sizeof(struct jump));
  path->pos.line = line;
  path->pos.column = column;
  path->pNextJump = NULL; /* by default, they are no next move */
  return path;
}

/* push a new jump at the end of the linked list */
jump* PushNewJump(jump* path, jump* newJump) {
  if (newJump != NULL) {
    /* we are at the end of the linkedlist */
    if (path == NULL) return newJump;
    path->pNextJump = PushNewJump(path->pNextJump, newJump);
  }
  return path; /* return the new linkedlist with the new jump at the end */
}

/* free the memory when we finish to use it */
void FreeLinkedListJumps(jump* path) {
  jump* tmp;

  while (path != NULL) {
    tmp = path->pNextJump;
    free(path);
    path = tmp;
  }
}

/* return the number of pawn, for this player, in this line */
u8 GetNbPawnInLine(u8 playerId, u8 line, pawn** pChessboard) {
  u8 nbPawn = 0;
  for (u8 column = 0; column < CHESSBOARD_SIZE; column++) {
    if (pChessboard[(line * CHESSBOARD_SIZE) + column] != NULL) { /* there is a pawn on this cell */
      if (pChessboard[(line * CHESSBOARD_SIZE) + column]->player == playerId)
        nbPawn++; /* it's the player's pawn */
    }
  }
  return nbPawn;
}

/* return the number of pawn, for this player, in this line */
u8 GetNbPawnInColumn(u8 playerId, u8 column, pawn** pChessboard) {
  u8 nbPawn = 0;
  for (u8 line = 0; line < CHESSBOARD_SIZE; line++) {
    if (pChessboard[(line * CHESSBOARD_SIZE) + column] != NULL) { /* there is a pawn on this cell */
      if (pChessboard[(line * CHESSBOARD_SIZE) + column]->player == playerId)
        nbPawn++; /* it's the player's pawn */
    }
  }
  return nbPawn;
}

/* return the type of cell we want to move on */
u8 GetTypeOfCell(u8 line, u8 column, u8 currentPlayer, pawn** pChessboard) {
  u8 typeOfCell = 0;
  /* first are we outside the chessboard ? */
  if ((line < 0) || (line > (CHESSBOARD_SIZE - 1)) || (column < 0) || (column > (CHESSBOARD_SIZE - 1))) {
    typeOfCell = IS_OUT_OF_BOUND;
  } else {
    /* the cell is empty ?*/
    if (pChessboard[(line * CHESSBOARD_SIZE) + column] == NULL) {
      typeOfCell = IS_EMPTY;
      /* the cell is in the edge of the chessboard */
      if ((line == 0) || (line == 9) || (column == 0) || (column == 9)) typeOfCell = IS_IN_EDGE;
      /* it is our goal ? */
      if ((currentPlayer == PLAYER1) || (currentPlayer == PLAYER2)) {
        if (line == PLAYER_GOALS[currentPlayer]) typeOfCell = IS_IN_GOAL;
      } else {
        if (column == PLAYER_GOALS[currentPlayer]) typeOfCell = IS_IN_GOAL;
      }
    } else typeOfCell = IS_FULL;
  }
  return typeOfCell;
}

/* compute all the moves we can do */
move* ComputeFutureMoves(u8 currentLine, u8 currentColumn, u8 oldLine, u8 oldColumn, u8 type, u8 currentPlayer, u8 mustJump, pawn** pChessboard) {
  move* allNewMoves = NULL;
  move* tmpNewMoves = NULL;
  jump* tmpJump = NULL;
  u8    neighbourLine = 0;
  u8    neighbourColumn = 0;
  u8    jumpedNeighbourLine = 0;
  u8    jumpedNeighbourColumn = 0;
  u8    neighbourTypeOfCell = 0;
  u8    jumpedNeighbourTypeOfCell = 0;
  u8    currentTypeOfCell = 0;

  /* the neighbour we can test, by our type */
  struct neighbourCells neighbours[8] = {
      /* L   C    S      T_N    T_S    T_E    T_W    D      C  */
      {{-1, -1}, {FALSE, TRUE, FALSE, FALSE, TRUE, TRUE, TRUE}},  /* up left */
      {{-1, 0},  {TRUE, FALSE, TRUE, FALSE, FALSE, FALSE, TRUE}}, /* up */
      {{-1, +1}, {FALSE, TRUE, FALSE, TRUE, FALSE, TRUE, TRUE}},  /* up right */
      {{0,  +1}, {TRUE, FALSE, FALSE, FALSE, TRUE, FALSE, TRUE}}, /* right */
      {{+1, +1}, {FALSE, FALSE, TRUE, TRUE, FALSE, TRUE, TRUE}},  /* down right */
      {{+1, 0},  {TRUE, TRUE, FALSE, FALSE, FALSE, FALSE, TRUE}}, /* down */
      {{+1, -1}, {FALSE, FALSE, TRUE, FALSE, TRUE, TRUE, TRUE}},  /* down left */
      {{0, -1},  {TRUE, FALSE, FALSE, TRUE, FALSE, FALSE, TRUE}}  /* left */
  };

  /* do we need to jump to move ? */
  if (mustJump == FALSE) {
    for (u8 i = 0; i < 8; i++) { /* for all neighbours around this cell */
      if (neighbours[i].usedByType[type] ==
          TRUE) { /* if this neighbours can be tested by our type */

        /* compute neighbours positions and type of cell */
        neighbourLine         = currentLine + neighbours[i].pos.line;
        neighbourColumn       = currentColumn + neighbours[i].pos.column;
        jumpedNeighbourLine   = currentLine + (neighbours[i].pos.line * 2);
        jumpedNeighbourColumn = currentColumn + (neighbours[i].pos.column * 2);
        neighbourTypeOfCell   = GetTypeOfCell(neighbourLine, neighbourColumn, currentPlayer, pChessboard);
        jumpedNeighbourTypeOfCell = GetTypeOfCell(jumpedNeighbourLine, jumpedNeighbourColumn, currentPlayer, pChessboard);

        if ((neighbourTypeOfCell == IS_IN_GOAL) || (neighbourTypeOfCell == IS_EMPTY)) {
          /* OK for this position */
          allNewMoves = PushNewMove(allNewMoves, CreateNewMove(neighbourLine, neighbourColumn, FALSE, NULL));
        } else if ((neighbourTypeOfCell == IS_FULL) && (((jumpedNeighbourTypeOfCell) != IS_OUT_OF_BOUND) || ((jumpedNeighbourTypeOfCell) != IS_FULL))) {
          /* Ok check chained jumps */
          tmpNewMoves = ComputeFutureMoves(jumpedNeighbourLine, jumpedNeighbourColumn, currentLine, currentColumn, type, currentPlayer, TRUE, pChessboard);
          if (tmpNewMoves != NULL) allNewMoves = PushNewMove(allNewMoves, tmpNewMoves);
        }
      }
    }
  } else { /* we must to jump to move */
    for (u8 i = 0; i < 8; i++) { /* for all neighbours around this cell */
      if (neighbours[i].usedByType[type] == TRUE) { /* if this neighbours can be tested by our type */

        /* compute neighbours positions and type of cell */
        neighbourLine             = currentLine + neighbours[i].pos.line;
        neighbourColumn           = currentColumn + neighbours[i].pos.column;
        jumpedNeighbourLine       = currentLine + (neighbours[i].pos.line * 2);
        jumpedNeighbourColumn     = currentColumn + (neighbours[i].pos.column * 2);
        neighbourTypeOfCell       = GetTypeOfCell(neighbourLine, neighbourColumn, currentPlayer, pChessboard);
        jumpedNeighbourTypeOfCell = GetTypeOfCell(jumpedNeighbourLine, jumpedNeighbourColumn, currentPlayer, pChessboard);
        currentTypeOfCell         = GetTypeOfCell(currentLine, currentColumn, currentPlayer, pChessboard);

        /* only test if it's not our old position, to avoid infinite compute */
        /* only test jumpedNeighbours, if our neighbourCell if full */
        if (((jumpedNeighbourLine != oldLine) || (jumpedNeighbourColumn != oldColumn)) && (neighbourTypeOfCell == IS_FULL)) {
          if ((jumpedNeighbourTypeOfCell != IS_OUT_OF_BOUND) && (jumpedNeighbourTypeOfCell == IS_EMPTY)) {
            /* Ok check chained jumps */
            tmpNewMoves = ComputeFutureMoves(jumpedNeighbourLine, jumpedNeighbourColumn, currentLine, currentColumn, type, currentPlayer, TRUE, pChessboard);
            if (tmpNewMoves != NULL) {
              PushNewJump(tmpNewMoves->path, CreateNewJump(oldLine, oldColumn));
              allNewMoves = PushNewMove(allNewMoves, tmpNewMoves);
            }
          }
        }
      }
    }
    if (((currentTypeOfCell == IS_IN_GOAL) || (currentTypeOfCell == IS_EMPTY))) {
      /* we can stay where we are */
      tmpJump = CreateNewJump(oldLine, oldColumn);
      allNewMoves = PushNewMove(allNewMoves, CreateNewMove(currentLine, currentColumn, TRUE, tmpJump));
    }
  }
  return allNewMoves;
}

/* return the sum of all values inside array of int */
u8 GetSumArray(u8* array, u8 lenght) {
  u8 sum = 0;
  for (u8 i = 0; i < lenght; i++) {
    sum += array[i];
  }
  return sum;
}

/* return the index of the winner */
u8 GetWinnerIndex(u8* array, u8 lenght) {
  u8 index = 0;
  for (u8 i = 0; i < lenght; i++) {
    if (array[i] == 1) index = i;
  }
  return index;
}

/* Check if we can move any pawn we have */
u8 GetNbMoves(u8 currentPlayer, pawn** pPawnsCurrentPlayer, pawn** pChessboard) {
  u8    nbMoves = 0;
  u8    currentLine = 0;
  u8    currentColumn = 0;
  u8    currentType = 0;
  move* tmp = NULL;
  for (u8 currentPawnId = 0; currentPawnId < NB_PAWNS; currentPawnId++) {
    tmp = NULL;
    currentLine   = pPawnsCurrentPlayer[currentPawnId]->pos.line;
    currentColumn = pPawnsCurrentPlayer[currentPawnId]->pos.column;
    currentType   = pPawnsCurrentPlayer[currentPawnId]->type;
    tmp = ComputeFutureMoves(currentLine, currentColumn, OUT_OF_BOUND, OUT_OF_BOUND, currentType, currentPlayer, FALSE, pChessboard);
    if (tmp != NULL) nbMoves++;
    FreeLinkedListMoves(tmp);
  }
  return nbMoves;
}
