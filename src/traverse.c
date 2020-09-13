#include "traverse.h"

// constant array
// clang-format off
const char *PLAYERS_STRING[4] = {"Player 1", "Player 2", "Player 3", "Player 4"}; // NOLINT
const Position PLAYER1_GOAL[NB_PAWNS] = {{0, 1}, {0, 2}, {0, 3}, {0, 4}, {0, 5}, {0, 6}, {0, 7}, {0, 8}}; // NOLINT
const Position PLAYER2_GOAL[NB_PAWNS] = {{9, 1}, {9, 2}, {9, 3}, {9, 4}, {9, 5}, {9, 6}, {9, 7}, {9, 8}}; // NOLINT
const Position PLAYER3_GOAL[NB_PAWNS] = {{1, 9}, {2, 9}, {3, 9}, {4, 9}, {5, 9}, {6, 9}, {7, 9}, {8, 9}}; // NOLINT
const Position PLAYER4_GOAL[NB_PAWNS] = {{1, 0}, {2, 0}, {3, 0}, {4, 0}, {5, 0}, {6, 0}, {7, 0}, {8, 0}}; // NOLINT
const Position *PLAYERS_GOAL[MAX_NB_PLAYER] = {PLAYER1_GOAL, PLAYER2_GOAL, PLAYER3_GOAL, PLAYER4_GOAL}; // NOLINT
// clang-format on

// create a new move, must be added inside linked list
Move* CreateMove(u8 line, u8 column, u8 jumped, u8 canStay, Move* from) {
  Move* newMove = (Move*)malloc(sizeof(struct Move));
  if (newMove == NULL) {
    perror("Error: can't create move. Exit.\n");
    exit(-1);
  }
  newMove->newPosition.line = line;
  newMove->newPosition.column = column;
  newMove->jumped = jumped;
  newMove->canStay = canStay;
  newMove->from = from;
  newMove->next = NULL;
  return newMove;
}

// push a move at the beginning of the linked list
Move* TopPushMove(Move* head, Move* newMove) {
  newMove->next = head;
  head = newMove;
  return head;
}

// push a move at the end of the linked list
Move* BottomPushMove(Move* head, Move* newMove) {
  Move* cursor = head;
  if (cursor != NULL) {
    while (cursor->next != NULL) {
      cursor = cursor->next;
    }
    cursor->next = newMove;
  } else {
    head = newMove;
  }
  return head;
}

u8 IsAlreadyExiste(Move* head, u8 line, u8 column) {
  Move* cursor;
  u8    result = FALSE;
  cursor = head;
  while (cursor != NULL) {
    if ((cursor->newPosition.line == line) && (cursor->newPosition.column == column)) {
      result = TRUE;
    }
    cursor = cursor->next;
  }
  return result;
}

// free the memory when we finish to use it
void FreeLinkedListMoves(Move** head) {
  Move* cursor = *head;
  Move* next;
  while (cursor != NULL) {
    next = cursor->next;
    free(cursor);
    cursor = next;
  }
  *head = NULL;
}

// create a new AiMmove, must be added inside linked list
AiMove* CreateAiMove(float score, u8 pawnId, u8 line, u8 column) {
  AiMove* newAiMove = (AiMove*)malloc(sizeof(struct AiMove));
  if (newAiMove == NULL) {
    perror("Error: can't create AiMove. Exit.\n");
    exit(-1);
  }
  newAiMove->score = score;
  newAiMove->pawnId = pawnId;
  newAiMove->newPosition.line = line;
  newAiMove->newPosition.column = column;
  newAiMove->next = NULL;
  return newAiMove;
}

// push a AiMove at the beginning of the linked list
AiMove* TopPushAiMove(AiMove* head, AiMove* newAiMove) {
  newAiMove->next = head;
  head = newAiMove;
  return head;
}

// push a AiMove at the end of the linked list
AiMove* BottomPushAiMove(AiMove* head, AiMove* newAiMove) {
  AiMove* cursor = head;
  if (cursor != NULL) {
    while (cursor->next != NULL) {
      cursor = cursor->next;
    }
    cursor->next = newAiMove;
  } else {
    head = newAiMove;
  }
  return head;
}

// free the memory when we finish to use it
void FreeLinkedListAiMoves(AiMove** head) {
  AiMove* cursor = *head;
  AiMove* next;
  while (cursor != NULL) {
    next = cursor->next;
    free(cursor);
    cursor = next;
  }
  *head = NULL;
}

// create new pawn
Pawn* CreatePawn(u8 line, u8 column, u8 type, u8 playerId, u8 pawnId) {
  Pawn* newPawn = (Pawn*)malloc(sizeof(struct Pawn));
  if (newPawn == NULL) {
    perror("Error: can't create pawn. Exit.\n");
    exit(-1);
  }
  newPawn->pos.column = column;
  newPawn->pos.line = line;
  newPawn->type = type;
  newPawn->playerId = playerId;
  newPawn->pawnId = pawnId;
  return newPawn;
}

// free all pawns of a player
void FreePlayerPawns(Pawn** playerPaws) {
  for (u8 pawnIndex = 0; pawnIndex < NB_PAWNS; pawnIndex++) {
    if (playerPaws[pawnIndex] != NULL) {
      free(playerPaws[pawnIndex]);
      playerPaws[pawnIndex] = NULL;
    }
  }
}

// init all chessboard's cells with NULL
void ClearChessboard(Pawn** pChessboard) {
  for (u8 line = 0; line < CHESSBOARD_SIZE; line++) {
    for (u8 column = 0; column < CHESSBOARD_SIZE; column++) {
      pChessboard[INDEX_2D(column, line)] = NULL;
    }
  }
}

// cell empty = pointer to null
// cell not empty = address of the pawn on it
void UpdateChessboard(u8 nbPlayers, Pawn*** pPlayersPawns, Pawn** pChessboard) {
  u8 line;
  u8 column;
  for (u8 playerId = 0; playerId < nbPlayers; playerId++) {
    for (u8 pawnIndex = 0; pawnIndex < NB_PAWNS; pawnIndex++) {
      line = pPlayersPawns[playerId][pawnIndex]->pos.line;
      column = pPlayersPawns[playerId][pawnIndex]->pos.column;
      pChessboard[INDEX_2D(column, line)] = pPlayersPawns[playerId][pawnIndex];
    }
  }
}

// move pawn to a new position
void MovePawn(Pawn** pChessboard, Pawn* pawn, u8 newLine, u8 newColumn) {
  // clear the old position
  pChessboard[INDEX_2D(pawn->pos.column, pawn->pos.line)] = NULL;
  // move to the new position
  pChessboard[INDEX_2D(newColumn, newLine)] = pawn;
  pawn->pos.line = newLine;
  pawn->pos.column = newColumn;
}

// return what kind of cell it is, for this playerId
u8 GetTypeOfCell(u8 line, u8 column, u8 playerId, Pawn** pChessboard) {
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
      if (IsCellPlayerGoal(line, column, playerId) == TRUE) typeOfCell = IS_IN_GOAL;
    } else
      typeOfCell = IS_FULL;
  }
  return typeOfCell;
}

// cell position is one of our goal = return TRUE
u8 IsCellPlayerGoal(u8 line, u8 column, u8 playerId) {
  for (u8 cellIndex = 0; cellIndex < NB_PAWNS; cellIndex++) {
    if ((PLAYERS_GOAL[playerId][cellIndex].line == line) && (PLAYERS_GOAL[playerId][cellIndex].column == column)) {
      return TRUE;
    }
  }
  return FALSE;
}

// compute score with current pawns inside our goal
// result between 0.0 and 1.0
float GetPlayerScore(u8 playerId, Pawn** pPlayerPawns) {
  float score = 0.0;
  for (u8 pawnIndex = 0; pawnIndex < NB_PAWNS; pawnIndex++) {
    for (u8 cellIndex = 0; cellIndex < NB_PAWNS; cellIndex++) {
      if ((PLAYERS_GOAL[playerId][cellIndex].line == pPlayerPawns[pawnIndex]->pos.line) &&
          (PLAYERS_GOAL[playerId][cellIndex].column == pPlayerPawns[pawnIndex]->pos.column)) {
        score += 1.0;
      }
    }
  }
  return (score / (float)NB_PAWNS);
}

// if this pawn is inside our goal: return true
u8 IsPawnInsideGoal(u8 playerId, Pawn* pawn) {
  for (u8 cellIndex = 0; cellIndex < NB_PAWNS; cellIndex++) {
    if ((PLAYERS_GOAL[playerId][cellIndex].line == pawn->pos.line) &&
        (PLAYERS_GOAL[playerId][cellIndex].column == pawn->pos.column)) {
      return TRUE;
    }
  }
  return FALSE;
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

// calculate the shortest distance between each pawn and their nearest goal
// get an average distance for the player and his goal
float GetPlayerAverageDistance(u8 playerId, Pawn** pPlayerPawns, Pawn** pChessboard, u8* sqrtDistanceTable) {
  u8  tmpAverageDistance[NB_PAWNS];
  int averageDistance = 0;
  u8  currentLine = 0;
  u8  currentColumn = 0;
  u8  distantLine = 0;
  u8  distantColumn = 0;
  u8  min = OUT_OF_BOUND;

  for (u8 pawnIndex = 0; pawnIndex < NB_PAWNS; pawnIndex++) {
    currentLine = pPlayerPawns[pawnIndex]->pos.line;
    currentColumn = pPlayerPawns[pawnIndex]->pos.column;
    // if the pawn is not already in a goal
    if (IsPawnInsideGoal(playerId, pPlayerPawns[pawnIndex]) == FALSE) {
      // get distance between the current pawn and each cell goal
      for (u8 cellIndex = 0; cellIndex < NB_PAWNS; cellIndex++) {
        distantLine = PLAYERS_GOAL[playerId][cellIndex].line;
        distantColumn = PLAYERS_GOAL[playerId][cellIndex].column;
        if ((pChessboard[INDEX_2D(distantColumn, distantLine)] == NULL) ||
            (pChessboard[INDEX_2D(distantColumn, distantLine)]->playerId != playerId)) {
          tmpAverageDistance[cellIndex] =
              sqrtDistanceTable[INDEX_4D(currentColumn, currentLine, distantColumn, distantLine)];
        } else {
          tmpAverageDistance[cellIndex] = OUT_OF_BOUND;
        }
      }
      // at the end, get the smallest distance
      min = OUT_OF_BOUND;
      for (u8 index = 0; index < NB_PAWNS; index++) {
        if (tmpAverageDistance[index] < min) min = tmpAverageDistance[index];
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
float GetGameScore(Pawn*** pPlayersPawns, Pawn** pChessboard, u8* sqrtDistanceTable) {
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

// check if one player win the game
// -1 no one win
// else return the player id of the winner
s8 GameOver(u8 nbPlayer, Pawn*** pPlayersPawns) {
  s8 winner = TRUE;
  for (u8 playerId = 0; playerId < nbPlayer; playerId++) {
    winner = TRUE;
    for (u8 pawnIndex = 0; pawnIndex < NB_PAWNS; pawnIndex++) {
      if (IsPawnInsideGoal(playerId, pPlayersPawns[playerId][pawnIndex]) == FALSE) {
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

// calculate possible movements we can do for this pawn
Move* ComputeFutureMoves(Move* head, Move* from, u8 depth, u8 oldLine, u8 oldColumn, Pawn* pawn, u8 jumped,
                         Pawn** pChessboard) {

  // avoid too deep compute move and stack overflow
  if (depth == 0) {
    return head;
  }

  // current pawn properties
  Move* pMove = NULL;
  u8    pawnLine = pawn->pos.line;
  u8    pawnColumn = pawn->pos.column;
  u8    pawnType = pawn->type;
  u8    pawnPlayerId = pawn->playerId;
  u8    currentCell = 0;
  // neighbouring cell properties
  u8 neighbouringLine = 0;
  u8 neighbouringColumn = 0;
  u8 neighbouringCell = 0;
  // jumped neighbouring cell properties
  u8 jumpedNeighbouringLine = 0;
  u8 jumpedNeighbouringColumn = 0;
  u8 jumpedNeighbouringCell = 0;

  // clang-format off
  // the neighbor we can test, by our type
  struct PawnShift shift[8] = {
      // L   C    S      T_N    T_S    T_E    T_W    D      C
      {{-1, -1}, {FALSE, TRUE,  FALSE, FALSE, TRUE,  TRUE,  TRUE}}, // up left
      {{-1,  0}, {TRUE,  FALSE, TRUE,  FALSE, FALSE, FALSE, TRUE}}, // up
      {{-1, +1}, {FALSE, TRUE,  FALSE, TRUE,  FALSE, TRUE,  TRUE}}, // up right
      {{ 0, +1}, {TRUE,  FALSE, FALSE, FALSE, TRUE,  FALSE, TRUE}}, // right
      {{+1, +1}, {FALSE, FALSE, TRUE,  TRUE,  FALSE, TRUE,  TRUE}}, // down right
      {{+1,  0}, {TRUE,  TRUE,  FALSE, FALSE, FALSE, FALSE, TRUE}}, // down
      {{+1, -1}, {FALSE, FALSE, TRUE,  FALSE, TRUE,  TRUE,  TRUE}}, // down left
      {{ 0, -1}, {TRUE,  FALSE, FALSE, TRUE,  FALSE, FALSE, TRUE}}  // left
  };
  // clang-format on

  // add the pawn position as a new move
  // remove the pawn on the cell to get the good type, can be improve
  pChessboard[INDEX_2D(pawnColumn, pawnLine)] = NULL;
  currentCell = GetTypeOfCell(pawnLine, pawnColumn, pawnPlayerId, pChessboard);
  if ((oldLine == OUT_OF_BOUND) || (oldColumn == OUT_OF_BOUND)) {
    if (IsAlreadyExiste(head, pawnLine, pawnColumn) == FALSE) {
      pMove = CreateMove(pawnLine, pawnColumn, jumped, FALSE, from);
      head = BottomPushMove(head, pMove);
    }
  } else if (currentCell == IS_IN_EDGE) {
    if (IsAlreadyExiste(head, pawnLine, pawnColumn) == FALSE) {
      pMove = CreateMove(pawnLine, pawnColumn, jumped, FALSE, from);
      head = BottomPushMove(head, pMove);
    }
  } else {
    if (IsAlreadyExiste(head, pawnLine, pawnColumn) == FALSE) {
      pMove = CreateMove(pawnLine, pawnColumn, jumped, TRUE, from);
      head = BottomPushMove(head, pMove);
    }
  }
  pChessboard[INDEX_2D(pawnColumn, pawnLine)] = pawn; // put back de pawn

  for (u8 index = 0; index < 8; index++) {         // for all neighbours around this pawn
    if (shift[index].pawnType[pawnType] == TRUE) { // if this neighbours can be tested by our type of pawn
      // compute neighbours positions and type of cell
      neighbouringLine = pawnLine + shift[index].shiftPosition.line;
      neighbouringColumn = pawnColumn + shift[index].shiftPosition.column;
      jumpedNeighbouringLine = pawnLine + (shift[index].shiftPosition.line * 2);
      jumpedNeighbouringColumn = pawnColumn + (shift[index].shiftPosition.column * 2);
      neighbouringCell = GetTypeOfCell(neighbouringLine, neighbouringColumn, pawnPlayerId, pChessboard);
      jumpedNeighbouringCell =
          GetTypeOfCell(jumpedNeighbouringLine, jumpedNeighbouringColumn, pawnPlayerId, pChessboard);

      // clang-format off
      if (jumped == FALSE) {
        if ((neighbouringCell == IS_IN_GOAL) || (neighbouringCell == IS_EMPTY)) {
          head = BottomPushMove(head, CreateMove(neighbouringLine, neighbouringColumn, FALSE, TRUE, pMove)); // OK for this position
        } else if ((neighbouringCell == IS_FULL) && ((jumpedNeighbouringCell != IS_OUT_OF_BOUND) && (jumpedNeighbouringCell != IS_FULL))) {
          // Ok check chained jumps
          MovePawn(pChessboard, pawn, jumpedNeighbouringLine, jumpedNeighbouringColumn);
          head = ComputeFutureMoves(head, pMove, depth - 1, pawnLine, pawnColumn, pawn, TRUE, pChessboard);
          MovePawn(pChessboard, pawn, pawnLine, pawnColumn);
        }
      } else { // we jumped
        if (((jumpedNeighbouringLine != oldLine) || (jumpedNeighbouringColumn != oldColumn)) && (neighbouringCell == IS_FULL)) { // only test if it's not our old position, to avoid infinite compute
          if ((jumpedNeighbouringCell != IS_OUT_OF_BOUND) && ((jumpedNeighbouringCell == IS_EMPTY) || (jumpedNeighbouringCell == IS_IN_GOAL))) { // only test jumpedNeighbours, if our neighbourCell if full
            // Ok check chained jumps
            MovePawn(pChessboard, pawn, jumpedNeighbouringLine, jumpedNeighbouringColumn);
            head = ComputeFutureMoves(head, pMove, depth - 1, pawnLine, pawnColumn, pawn, TRUE, pChessboard);
            MovePawn(pChessboard, pawn, pawnLine, pawnColumn);
          }
        }
      }
      // clang-format on
    }
  }
  return head;
}

// minimax algorithme for AI
// simple but okay for this game
float MinMax(Pawn** pChessboard, u8 depth, u8 player, Pawn*** pPlayersPawns, u8* sqrtDistanceTable) {
  float   maxEval;
  float   minEval;
  float   score = 0.0;
  u8      iPlayer; // If player = HUMAIN, iPlayer = CPU
  u8      currentLine;
  u8      currentColumn;
  AiMove* aiMoves = NULL;
  AiMove* cursorAiMoves = NULL;
  Move*   newMoves = NULL;
  Move*   cursorNewMoves = NULL;

  if (player == HUMAIN) {
    iPlayer = CPU;
  } else {
    iPlayer = HUMAIN;
  }

  if ((depth == 0) || (GameOver(2, pPlayersPawns))) {
    return GetGameScore(pPlayersPawns, pChessboard, sqrtDistanceTable);
  }

  maxEval = -1.0;
  minEval = 1.0;
  // Loop on each pawn, we want to test all possible new moves
  for (u8 pawnIndex = 0; pawnIndex < NB_PAWNS; pawnIndex++) {

    // compute future move for this pawn
    currentLine = pPlayersPawns[player][pawnIndex]->pos.line;
    currentColumn = pPlayersPawns[player][pawnIndex]->pos.column;
    newMoves = ComputeFutureMoves(newMoves, NULL, MAX_DEPTH, OUT_OF_BOUND, OUT_OF_BOUND,
                                  pPlayersPawns[player][pawnIndex], FALSE, pChessboard);

    // for all futures new moves
    cursorNewMoves = newMoves;
    while (cursorNewMoves != NULL) {
      if (cursorNewMoves->canStay == TRUE) {
        // move, temporary, this pawn to get the score about this move
        MovePawn(pChessboard, pPlayersPawns[player][pawnIndex], cursorNewMoves->newPosition.line,
                 cursorNewMoves->newPosition.column);
        score = MinMax(pChessboard, depth - 1, iPlayer, pPlayersPawns, sqrtDistanceTable);
        aiMoves = TopPushAiMove(aiMoves, CreateAiMove(score, pawnIndex, cursorNewMoves->newPosition.line,
                                                      cursorNewMoves->newPosition.column));
        MovePawn(pChessboard, pPlayersPawns[player][pawnIndex], currentLine, currentColumn);
        cursorNewMoves = cursorNewMoves->next;
      }
    }
  }
  cursorNewMoves = NULL;
  FreeLinkedListMoves(&newMoves);

  // now we must to choose one of the best score for the player(CPU or HUMAIN)
  // here get how many time we get the best score
  cursorAiMoves = aiMoves;
  while (cursorAiMoves != NULL) {
    // processing for humains
    if (player == HUMAIN) {
      if (cursorAiMoves->score > maxEval) {
        maxEval = cursorAiMoves->score;
      }
    } else { // processing for CPU
      if (cursorAiMoves->score < minEval) {
        minEval = cursorAiMoves->score;
      }
    }
    cursorAiMoves = cursorAiMoves->next;
  }
  FreeLinkedListAiMoves(&cursorAiMoves);

  if (player == HUMAIN)
    return maxEval;
  else
    return minEval;
}
