#ifndef _HEADER_MAIN_H
#define _HEADER_MAIN_H

/* imports libs */
#include "traverse.h"
#include "traverse_gtk.h"
#include "utils.h"

#define GAME_STATUS_STR "GameStatus"

/* globals widgets */
/* must be accessed by all functions, so they are created on the heap */
GtkWidget* window;
GtkWidget *drawingArea;
cairo_surface_t *G_SURFACE;
GtkWidget* statusBar;
GtkWidget* dialogAbout;
GtkWidget* dialogRules;

/* globals pointers for game logic */
pawn **pChessboard;    /* like the real life, a cell on a chessboard can be empty or with a pawn on it */
pawn ***pPlayersPawns; /* Array of address of each players */
pawn **pPawnsPlayer1;  /* Array of address of each player1's pawns */
pawn **pPawnsPlayer2;  /* Array of address of each player2's pawns */
pawn **pPawnsPlayer3;  /* Array of address of each player3's pawns */
pawn **pPawnsPlayer4;  /* Array of address of each player4's pawns */
pawn *selectPawn;      /* address of the current selected pawn */
move *allNewMoves;
move *tmp;

/* global variables for game logic */
u8 nbPlayers;
u8 difficulty;
u8 currentPlayer;
u8 currentRound;
u8 currentType;
u8 currentLine;
u8 currentColumn;

/* functions */
void RunningApp(GtkApplication *app, gpointer user_data);
gboolean EventClickOnBoard(GtkWidget *widget, GdkEventButton *event);
gboolean GameLogic(GtkWidget *widget, GdkEventMotion *event, gpointer data);
gboolean ConfigureSurface(GtkWidget *widget, GdkEventConfigure *event, gpointer data);
gboolean DrawCallback(GtkWidget *widget, cairo_t *cr, gpointer data);
void EventNbPlayersChanged(GtkWidget* radioMenuItem, gpointer data);
void EventDifficultyChanged(GtkWidget* radioMenuItem, gpointer data);
void UpdateStatusBar(GtkWidget* statusBar, u8 currentPlayer, u8 currentRound);
void ResetGameLogicVariables();
void AllocGameVariables();
void UpdateStatusBar(GtkWidget* statusBar, u8 currentPlayer, u8 currentRound);
void AboutMessage();
void RulesMessage();
void QuitGame();

#endif /* _HEADER_MAIN_H */