#include "traverse.h"

// constant array
// clang-format off
const char *PLAYERS_STR[4] = {"Player 1", "Player 2", "Player 3", "Player 4"};
const position PLAYER1_GOAL[NB_PAWNS] = {{0, 1}, {0, 2}, {0, 3}, {0, 4}, {0, 5}, {0, 6}, {0, 7}, {0, 8}};
const position PLAYER2_GOAL[NB_PAWNS] = {{9, 1}, {9, 2}, {9, 3}, {9, 4}, {9, 5}, {9, 6}, {9, 7}, {9, 8}};
const position PLAYER3_GOAL[NB_PAWNS] = {{1, 9}, {2, 9}, {3, 9}, {4, 9}, {5, 9}, {6, 9}, {7, 9}, {8, 9}};
const position PLAYER4_GOAL[NB_PAWNS] = {{1, 0}, {2, 0}, {3, 0}, {4, 0}, {5, 0}, {6, 0}, {7, 0}, {8, 0}};
const position *PLAYERS_GOAL[MAX_NB_PLAYER] = {PLAYER1_GOAL, PLAYER2_GOAL, PLAYER3_GOAL, PLAYER4_GOAL};
// clang-format on

// create New Ai Move
// New Ai Move = a new movement with a score for it
aiMove* CreateNewAiMove(float score, u8 pawnId, u8 line, u8 column) {
  aiMove* newAiMove;
  newAiMove = (aiMove*)malloc(sizeof(struct aiMove));
  if (newAiMove == NULL) {
    perror("Error: can't init aiMove. Exit.\n");
    exit(-1);
  }
  newAiMove->score = score;
  newAiMove->column = column;
  newAiMove->line = line;
  newAiMove->pawnId = pawnId;
  newAiMove->pNextAiMove = NULL; // by default, they are no next aiMove
  return newAiMove;
}

// push a new aiMove at the end of the linked list
aiMove* PushNewAiMove(aiMove* allAiMove, aiMove* newAiMove) {
  if (newAiMove != NULL) {
    // we are at the end of the linkedlist
    if (allAiMove == NULL) return newAiMove;
    allAiMove->pNextAiMove = PushNewAiMove(allAiMove->pNextAiMove, newAiMove);
  }
  return allAiMove;
}

// free the memory when we finish to use it
void FreeLinkedAiMove(aiMove** allAiMove) {
  aiMove* tmp; // to keep the next move to erase before remove it in the current move
  while (*allAiMove != NULL) {
    tmp = *allAiMove;
    *allAiMove = (aiMove*)(*allAiMove)->pNextAiMove;
    free(tmp);
  }
  allAiMove = NULL;
}

// compute score with current pawns inside our goal
// result between 0 and 1
float GetPlayerScore(u8 playerId, pawn** pPawnsPlayer) {
  float score = 0;
  for (u8 pawnId = 0; pawnId < NB_PAWNS; pawnId++) {
    for (u8 goalCell = 0; goalCell < NB_PAWNS; goalCell++) {
      if ((PLAYERS_GOAL[playerId][goalCell].line == pPawnsPlayer[pawnId]->pos.line) &&
          (PLAYERS_GOAL[playerId][goalCell].column == pPawnsPlayer[pawnId]->pos.column)) {
        score++;
      }
    }
  }
  return (score / (float)NB_PAWNS);
}

// return TRUE: cell position is one of our goal
u8 IsCellPlayerGoal(u8 playerId, u8 line, u8 column) {
  for (u8 goalCell = 0; goalCell < NB_PAWNS; goalCell++) {
    if ((PLAYERS_GOAL[playerId][goalCell].line == line) && (PLAYERS_GOAL[playerId][goalCell].column == column)) {
      return TRUE;
    }
  }
  return FALSE;
}

// return TRUE: pawn is inside our goal
u8 IsPawnInsideGoal(u8 playerId, pawn* pawnPlayer) {
  for (u8 goalCell = 0; goalCell < NB_PAWNS; goalCell++) {
    if ((PLAYERS_GOAL[playerId][goalCell].line == pawnPlayer->pos.line) &&
        (PLAYERS_GOAL[playerId][goalCell].column == pawnPlayer->pos.column)) {
      return TRUE;
    }
  }
  return FALSE;
}

// calculate the shortest distance between each pawn and their nearest goal
// get an average distance for the player and his goal
float GetPlayerAverageDistance(u8 playerId, pawn** pPawnsPlayer, pawn** pChessboard, u8* sqrtDistanceTable) {
  u8  tmpAverageDistance[NB_PAWNS];
  int averageDistance = 0;
  u8  cLine = 0;
  u8  cColumn = 0;
  u8  dLine = 0;
  u8  dColumn = 0;
  u8  min = OUT_OF_BOUND;
  for (u8 pawnId = 0; pawnId < NB_PAWNS; pawnId++) {
    cLine = pPawnsPlayer[pawnId]->pos.line;
    cColumn = pPawnsPlayer[pawnId]->pos.column;
    // if the pawn is not already in a goal
    if (IsPawnInsideGoal(playerId, pPawnsPlayer[pawnId]) == FALSE) {
      // get distance between the current pawn and each cell goal
      for (u8 cellGoal = 0; cellGoal < NB_PAWNS; cellGoal++) {
        dLine = PLAYERS_GOAL[playerId][cellGoal].line;
        dColumn = PLAYERS_GOAL[playerId][cellGoal].column;
        if ((pChessboard[INDEX_2D(dColumn, dLine)] == NULL) ||
            (pChessboard[INDEX_2D(dColumn, dLine)]->playerId != playerId)) {
          tmpAverageDistance[cellGoal] = sqrtDistanceTable[INDEX_4D(cColumn, cLine, dColumn, dLine)];
        } else {
          tmpAverageDistance[cellGoal] = OUT_OF_BOUND;
        }
      }
      // at the end, get the smallest distance
      min = OUT_OF_BOUND;
      for (u8 distance = 0; distance < NB_PAWNS; distance++) {
        if (tmpAverageDistance[distance] < min) min = tmpAverageDistance[distance];
      }
      averageDistance += min;
    }
  }
  return ((averageDistance / 81.0) / 8.0); // 162 = the max distance we can get between two pawns
}

// compute score for the global game
// take the average distance of each player with their goal
// take the current goal of their pawn inside goal
// result between -1 and 1
// the nearest we are from -1, the more CPU are currently winning
// the nearest we are from 1, the more the player are currently winning
// only for 1v1: CPU vs HUM, bored to do with 3 and 4 players
float GetChessboardScore(pawn*** pPlayersPawns, pawn** pChessboard, u8* sqrtDistanceTable) {
  float playersPawnsScore[2];
  float playersDistanceScore[2];
  float globalScore = 0.0;

  // get pawn and distance score for each players
  for (u8 cPlayer = 0; cPlayer < 2; cPlayer++) {
    playersPawnsScore[cPlayer] = GetPlayerScore(cPlayer, pPlayersPawns[cPlayer]);
    playersDistanceScore[cPlayer] =
        1.0 - GetPlayerAverageDistance(cPlayer, pPlayersPawns[cPlayer], pChessboard, sqrtDistanceTable);
  }

  // global score = HUM score - CPU score.
  // the pawn score count for 2/3 of the global score for a player
  globalScore =
      // score player
      (((2 * playersPawnsScore[0]) + playersDistanceScore[0]) / 3) -
      (((2 * playersPawnsScore[1]) + playersDistanceScore[1]) / 3);

  return globalScore;
}

// calculate the distances of each cell between them
// done one time at the beginning of the game
u8* ComputeSqrtDistanceTable() {
  u8* tmpSqrtDistanceTable =
      (u8*)malloc((CHESSBOARD_SIZE * CHESSBOARD_SIZE * CHESSBOARD_SIZE * CHESSBOARD_SIZE) * sizeof(u8));
  s8 dX = 0;
  s8 dY = 0;
  if (tmpSqrtDistanceTable == NULL) {
    perror("Error: can't init new pawn. Exit.\n");
    exit(-1);
  }
  for (u8 y1 = 0; y1 < CHESSBOARD_SIZE; y1++) {
    for (u8 x1 = 0; x1 < CHESSBOARD_SIZE; x1++) {
      for (u8 y2 = 0; y2 < CHESSBOARD_SIZE; y2++) {
        for (u8 x2 = 0; x2 < CHESSBOARD_SIZE; x2++) {
          dX = x2 - x1;
          dY = y2 - y1;
          // only the square is use, not the root square
          tmpSqrtDistanceTable[INDEX_4D(x1, y1, x2, y2)] = (dX * dX) + (dY * dY);
        }
      }
    }
  }
  return tmpSqrtDistanceTable;
}

// minimax algorithme for AI
// simple but okay for this game
float MinMax(pawn** pChessboard, u8 depth, u8 maximizingPlayer, pawn*** pPlayersPawns, u8 nbPlayers,
             u8* sqrtDistanceTable) {
  float   maxEval;
  float   minEval;
  u8      currentLine;
  u8      currentColumn;
  u8      currentType;
  aiMove* allAiMove = NULL;
  aiMove* tmpAiMove = NULL;
  move*   allNewMoves = NULL;
  move*   pTmpMove = NULL;

  if ((depth == 0) || (GameOver(nbPlayers, pPlayersPawns))) {
    return GetChessboardScore(pPlayersPawns, pChessboard, sqrtDistanceTable);
  }

  // we want the highest score for the HUM player
  // so closest to 1
  if (maximizingPlayer == TRUE) {
    maxEval = -1.0;
    // Loop on each pawn, we want to test all possible new moves
    for (u8 pawnId = 0; pawnId < NB_PAWNS; pawnId++) {
      // compute future move for this pawn
      currentLine = pPlayersPawns[HUMAIN][pawnId]->pos.line;
      currentColumn = pPlayersPawns[HUMAIN][pawnId]->pos.column;
      currentType = pPlayersPawns[HUMAIN][pawnId]->type;
      allNewMoves = ComputeFutureMoves(currentLine, currentColumn, OUT_OF_BOUND, OUT_OF_BOUND, currentType, HUMAIN,
                                       FALSE, pChessboard);

      // for all futures new moves
      pTmpMove = allNewMoves;
      while (pTmpMove != NULL) {
        // move, temporary, this pawn to get the score about this move
        MovePawnOnChessboard(pChessboard, pPlayersPawns, HUMAIN, pawnId, pTmpMove->newPos.column,
                             pTmpMove->newPos.line);
        // store the score of this new movement
        allAiMove = PushNewAiMove(allAiMove, CreateNewAiMove(MinMax(pChessboard, depth - 1, FALSE, pPlayersPawns,
                                                                    nbPlayers, sqrtDistanceTable),
                                                             pawnId, currentLine, currentColumn));
        // moveback this pawn to his previous place
        MovePawnOnChessboard(pChessboard, pPlayersPawns, HUMAIN, pawnId, currentColumn, currentLine);
        pTmpMove = pTmpMove->pNextMove;
      }
      FreeLinkedListMoves(&allNewMoves);
    }
    // keep the best one
    tmpAiMove = allAiMove;
    while (tmpAiMove != NULL) {
      if (tmpAiMove->score > maxEval) maxEval = tmpAiMove->score;
      tmpAiMove = tmpAiMove->pNextAiMove;
    }
    FreeLinkedAiMove(&allAiMove);
    return maxEval;
  }

  // we want the highest score for the CPU player
  // so closest to -1
  else {
    minEval = 1.0;
    // Loop on each pawn, we want to test all possible new moves
    for (u8 pawnId = 0; pawnId < NB_PAWNS; pawnId++) {
      // compute future move for this pawn
      currentLine = pPlayersPawns[CPU][pawnId]->pos.line;
      currentColumn = pPlayersPawns[CPU][pawnId]->pos.column;
      currentType = pPlayersPawns[CPU][pawnId]->type;
      allNewMoves = ComputeFutureMoves(currentLine, currentColumn, OUT_OF_BOUND, OUT_OF_BOUND, currentType, CPU, FALSE,
                                       pChessboard);

      // for all futures new moves
      pTmpMove = allNewMoves;
      while (pTmpMove != NULL) {
        // move, temporary, this pawn to get the score about this move
        MovePawnOnChessboard(pChessboard, pPlayersPawns, CPU, pawnId, pTmpMove->newPos.column, pTmpMove->newPos.line);
        // store the score of this new movement
        allAiMove = PushNewAiMove(allAiMove, CreateNewAiMove(MinMax(pChessboard, depth - 1, TRUE, pPlayersPawns,
                                                                    nbPlayers, sqrtDistanceTable),
                                                             pawnId, currentLine, currentColumn));
        // moveback this pawn to his previous place
        MovePawnOnChessboard(pChessboard, pPlayersPawns, HUMAIN, pawnId, currentColumn, currentLine);
        pTmpMove = pTmpMove->pNextMove;
      }
      FreeLinkedListMoves(&allNewMoves);
    }
    // keep the best one
    tmpAiMove = allAiMove;
    while (tmpAiMove != NULL) {
      if (tmpAiMove->score < minEval) minEval = tmpAiMove->score;
      tmpAiMove = tmpAiMove->pNextAiMove;
    }
    FreeLinkedAiMove(&allAiMove);
    return minEval;
  }
}

// check if one player win the game
// -1 no one win
// else return the player id of the winner
s8 GameOver(u8 nbPlayer, pawn*** pPlayersPawns) {
  u8 winner;
  for (u8 playerId = 0; playerId < nbPlayer; playerId++) {
    winner = TRUE;
    for (u8 pawnId = 0; pawnId < NB_PAWNS; pawnId++) {
      if (IsPawnInsideGoal(playerId, pPlayersPawns[playerId][pawnId]) == FALSE) {
        winner = -1;
        break;
      }
    }
    // we have a winner
    if (winner != -1) {
      winner = playerId;
      break;
    }
  }
  return winner;
}

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

// move pawn to a new position
void MovePawnOnChessboard(pawn** pChessboard, pawn*** pPlayersPawns, u8 player, u8 pawnId, u8 newColumn, u8 newLine) {
  u8 currentLine = pPlayersPawns[player][pawnId]->pos.line;
  u8 currentColumn = pPlayersPawns[player][pawnId]->pos.column;
  // move to the new position
  pPlayersPawns[player][pawnId]->pos.column = newColumn;
  pPlayersPawns[player][pawnId]->pos.line = newLine;
  pChessboard[INDEX_2D(newColumn, newLine)] = pPlayersPawns[player][pawnId];
  // clear the old position
  pChessboard[INDEX_2D(currentColumn, currentLine)] = NULL;
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
    if (pChessboard[INDEX_2D(column, line)] == NULL) {
      typeOfCell = IS_EMPTY;
      // the cell is in the edge of the chessboard
      if ((line == 0) || (line == 9) || (column == 0) || (column == 9)) typeOfCell = IS_IN_EDGE;
      // it is our goal ?
      if (IsCellPlayerGoal(playerId, line, column) == TRUE) typeOfCell = IS_IN_GOAL;
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
