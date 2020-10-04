#ifndef _HEADER_MAIN_H
#define _HEADER_MAIN_H

// imports libs
#include "logo.h"
#include "math.h"
#include "traverse.h"
#include "traverse_gtk.h"
#include "utils.h"

#define PACKAGE_NAME    "Traverse Board Game"
#define PACKAGE_VERSION "1.0"
#define GAME_STATUS_STR "GameStatus"
#define WAIT_PAWN       0
#define PAWN_SELECTED   1
#define NEW_MOVE_DONE   2

// globals widgets
// must be accessed by all functions, so they are created on the heap
GtkWidget*       window;
GtkWidget*       drawingArea;
cairo_surface_t* G_SURFACE;
GtkWidget*       statusBar;
GtkWidget*       dialogAbout;
GtkWidget*       dialogRules;
GtkWidget*       dialogWinner;
// widget for menu
GtkWidget* humanRadioItem;
GtkWidget* player2RadioItem;
// widget for string
GtkWidget* lPlayerStr;
GtkWidget* lPlayer;
GtkWidget* lRoundStr;

// globals pointers for game logic
Pawn**  pChessboard;      // like the real life, a cell on a chessboard can be empty or with a pawn on it
Pawn*** pPlayersPawns;    // Array of address of each players
Pawn**  pPawnsPlayer1;    // Array of address of each player1's pawns
Pawn**  pPawnsPlayer2;    // Array of address of each player2's pawns
Pawn**  pPawnsPlayer3;    // Array of address of each player3's pawns
Pawn**  pPawnsPlayer4;    // Array of address of each player4's pawns
Pawn*   selectPlayerPawn; // address of the current selected pawn
Move*   newMoves;
u8*     sqrtDistanceTable;

// global variables for game logic
u8 nbPlayers;
u8 difficulty;
u8 currentPlayer;
u8 currentRound;
u8 currentType;
u8 currentLine;
u8 currentColumn;
u8 gameState;

// functions
void     RunningApp(GtkApplication* app, gpointer user_data);
gboolean EventClickOnBoard(GtkWidget* widget, GdkEventButton* event);
gboolean GameLogic(GtkWidget* widget, GdkEventMotion* event, gpointer data);
gboolean ConfigureSurface(GtkWidget* widget, GdkEventConfigure* event, gpointer data);
gboolean DrawCallback(GtkWidget* widget, cairo_t* cr, gpointer data);
void     EventNbPlayersChanged(GtkWidget* radioMenuItem, gpointer data);
void     EventDifficultyChanged(GtkWidget* radioMenuItem, gpointer data);
void     UpdateStatusBar(GtkWidget* statusBar, u8 currentPlayer, u8 currentRound);
void     ResetGameLogicVariables();
void     AllocGameVariables();
void     WaitPawnProcess();
void     NewPawnSelectedProcess(Pawn* selectedPawn);
void     AboutMessage();
void     RulesMessage();
void     WinnerAlert(u8 winnerId);
void     QuitGame();

#endif // _HEADER_MAIN_H