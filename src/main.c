#include "main.h"

/* main function */
int main(int argc, char **argv) {
  GtkApplication *app;
  int status;

  /* Running GTK App */
  app = gtk_application_new("org.gtk.games.traverse", G_APPLICATION_FLAGS_NONE);
  g_signal_connect(app, "activate", G_CALLBACK(RunningApp), NULL);
  status = g_application_run(G_APPLICATION(app), argc, argv);
  g_object_unref(app);

  return status;
}

/* the real main behind the main */
void RunningApp(GtkApplication *app, gpointer user_data) {
  /***** START GTK Init *****/
  /* create GTK windows */
  GtkWidget* window;
  window = gtk_application_window_new(app);
  gtk_window_set_title(GTK_WINDOW(window), "Traverse");
  gtk_container_set_border_width(GTK_CONTAINER(window), 8);
  gtk_window_set_resizable((GtkWindow *)window, FALSE); /* cant resize it */
  g_signal_connect(window, "delete-event", G_CALLBACK(QuitGame), NULL);

  /* create menu bar */
  GtkWidget* vbox;
  GtkWidget* menuBar;
  vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
  gtk_container_add(GTK_CONTAINER(window), vbox);
  menuBar = gtk_menu_bar_new();

  /* gameMenu */
  GtkWidget* gameMenu    = gtk_menu_new();
  GtkWidget* gameBarItem = gtk_menu_item_new_with_label("Game");
  GtkWidget* newItem     = gtk_menu_item_new_with_label("New");
  GtkWidget* quitItem    = gtk_menu_item_new_with_label("Quit");
  gtk_menu_shell_append(GTK_MENU_SHELL(gameMenu), newItem);
  gtk_menu_shell_append(GTK_MENU_SHELL(gameMenu), quitItem);
  gtk_menu_item_set_submenu(GTK_MENU_ITEM(gameBarItem), gameMenu);
  gtk_menu_shell_append(GTK_MENU_SHELL(menuBar), gameBarItem);

  /* playerMenu */
  GtkWidget* playerMenu = gtk_menu_new();
  GtkWidget* playerBarItem = gtk_menu_item_new_with_label("Player");
  GSList*    playerRadioGroup = NULL;
  GtkWidget* player2RadioItem = gtk_radio_menu_item_new_with_label(playerRadioGroup, "Player2");
  playerRadioGroup = gtk_radio_menu_item_get_group(GTK_RADIO_MENU_ITEM (player2RadioItem));
  GtkWidget* player3RadioItem = gtk_radio_menu_item_new_with_label(playerRadioGroup, "Player3");
  GtkWidget* player4RadioItem = gtk_radio_menu_item_new_with_label(playerRadioGroup, "Player4");
  gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(player2RadioItem), TRUE);
  gtk_menu_shell_append(GTK_MENU_SHELL(playerMenu), player2RadioItem);
  gtk_menu_shell_append(GTK_MENU_SHELL(playerMenu), player3RadioItem);
  gtk_menu_shell_append(GTK_MENU_SHELL(playerMenu), player4RadioItem);
  gtk_menu_item_set_submenu(GTK_MENU_ITEM(playerBarItem), playerMenu);
  gtk_menu_shell_append(GTK_MENU_SHELL(menuBar), playerBarItem);
  g_signal_connect(player2RadioItem, "toggled", G_CALLBACK(EventNbPlayersChanged), (gpointer)2);
  g_signal_connect(player3RadioItem, "toggled", G_CALLBACK(EventNbPlayersChanged), (gpointer)3);
  g_signal_connect(player4RadioItem, "toggled", G_CALLBACK(EventNbPlayersChanged), (gpointer)4);

  /* difficulty menu */
  GtkWidget* difficultyMenu = gtk_menu_new();
  GtkWidget* difficultyBarItem = gtk_menu_item_new_with_label("Difficulty");
  GSList*    difficultyRadioGroup = NULL;
  GtkWidget* humanRadioItem = gtk_radio_menu_item_new_with_label(difficultyRadioGroup, "VS Human");
  difficultyRadioGroup = gtk_radio_menu_item_get_group(GTK_RADIO_MENU_ITEM(humanRadioItem));
  GtkWidget* normalRadioItem = gtk_radio_menu_item_new_with_label(difficultyRadioGroup, "AI Normal");
  GtkWidget* hardRadioItem = gtk_radio_menu_item_new_with_label(difficultyRadioGroup, "AI Hard");
  gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(humanRadioItem), TRUE);
  gtk_menu_shell_append(GTK_MENU_SHELL(difficultyMenu), humanRadioItem);
  gtk_menu_shell_append(GTK_MENU_SHELL(difficultyMenu), normalRadioItem);
  gtk_menu_shell_append(GTK_MENU_SHELL(difficultyMenu), hardRadioItem);
  gtk_menu_item_set_submenu(GTK_MENU_ITEM(difficultyBarItem), difficultyMenu);
  gtk_menu_shell_append(GTK_MENU_SHELL(menuBar), difficultyBarItem);
  g_signal_connect(humanRadioItem,  "toggled", G_CALLBACK(EventDifficultyChanged), (gpointer)0);
  g_signal_connect(normalRadioItem, "toggled", G_CALLBACK(EventDifficultyChanged), (gpointer)1);
  g_signal_connect(hardRadioItem,   "toggled", G_CALLBACK(EventDifficultyChanged), (gpointer)2);

  /* about menu */
  GtkWidget* aboutMenu = gtk_menu_new();
  GtkWidget* aboutBarItem = gtk_menu_item_new_with_label("About");
  GtkWidget* rulesItem    = gtk_menu_item_new_with_label("Rules");
  GtkWidget* aboutItem    = gtk_menu_item_new_with_label("About");
  gtk_menu_shell_append(GTK_MENU_SHELL(aboutMenu), rulesItem);
  gtk_menu_shell_append(GTK_MENU_SHELL(aboutMenu), aboutItem);
  gtk_menu_item_set_submenu(GTK_MENU_ITEM(aboutBarItem), aboutMenu);
  gtk_menu_shell_append(GTK_MENU_SHELL(menuBar), aboutBarItem);

  /* add menu bar to vbox */
  gtk_box_pack_start(GTK_BOX(vbox), menuBar, FALSE, FALSE, 2);

  /* create GTK drawing area */
  drawingArea = gtk_drawing_area_new();
  gtk_widget_set_size_request(drawingArea, (CELL_SIZE * 10), (CELL_SIZE * 10));
  gtk_container_add(GTK_CONTAINER(vbox), drawingArea);

  /* subscrite to mouse event, normally drawingArea doesn't listen this kind of event, poor boy */
  gtk_widget_set_events(drawingArea, gtk_widget_get_events(drawingArea) | GDK_POINTER_MOTION_MASK);
  gtk_widget_set_events(drawingArea, gtk_widget_get_events(drawingArea) | GDK_BUTTON_PRESS_MASK);
  g_signal_connect(drawingArea, "configure-event", G_CALLBACK(ConfigureSurface), NULL);
  g_signal_connect(drawingArea, "button_press_event", G_CALLBACK(EventClickOnBoard), NULL);
  g_signal_connect(drawingArea, "draw", G_CALLBACK(DrawCallback), NULL);
  g_signal_connect(drawingArea, "motion-notify-event", G_CALLBACK(GameLogic), NULL);

  /***** STATUSBAR WIDGET SECTION ******/
  statusBar = gtk_statusbar_new();
  guint statusBarContext = gtk_statusbar_get_context_id(GTK_STATUSBAR(statusBar), GAME_STATUS_STR);
  gtk_container_add(GTK_CONTAINER(vbox), statusBar);
  gtk_statusbar_push(GTK_STATUSBAR(statusBar), statusBarContext, "Init");

  /***** START GAME LOGIC Init ****/
  AllocGameVariables();               /* alloc memories */
  ResetGameLogicVariables();          /* init gameLogic variables */
  LoadPNGSurface();                   /* Loading all pawns png data in array */
  gtk_widget_show_all(window);        /* show all */
  GameLogic(drawingArea, NULL, NULL); /* first time draw chessboard */
  gtk_widget_queue_draw(drawingArea);
}

/* do all memories alloc for game logic variables */
void AllocGameVariables() {
  pChessboard = malloc((CHESSBOARD_SIZE * CHESSBOARD_SIZE) * sizeof(pawn *));
  pPlayersPawns = malloc(4 * sizeof(pawn **));
  pPawnsPlayer1 = malloc(NB_PAWNS * sizeof(pawn *));
  pPawnsPlayer2 = malloc(NB_PAWNS * sizeof(pawn *));
  pPawnsPlayer3 = malloc(NB_PAWNS * sizeof(pawn *));
  pPawnsPlayer4 = malloc(NB_PAWNS * sizeof(pawn *));
  pPlayersPawns[0] = pPawnsPlayer1;
  pPlayersPawns[1] = pPawnsPlayer2;
  pPlayersPawns[2] = pPawnsPlayer3;
  pPlayersPawns[3] = pPawnsPlayer4;

  /* default number of players */
  nbPlayers = 2;
  /* default difficulty: VS Human */
  difficulty = 0;

  /* init all pawns pointers to NULL */
  for (u8 playerId = 0; playerId < 4; playerId++){
    for (u8 pawnId = 0; pawnId < NB_PAWNS; pawnId++){
      pPlayersPawns[playerId][pawnId] = (pawn*)NULL;
    }
  }
}

/* when start a new game, init all game variables */
void ResetGameLogicVariables() {
  /* if not free before */
  FreePlayerPawns(pPawnsPlayer1);
  FreePlayerPawns(pPawnsPlayer2);
  FreePlayerPawns(pPawnsPlayer3);
  FreePlayerPawns(pPawnsPlayer4);

  /* reinit variables */
  allNewMoves = NULL;
  tmp = NULL;
  currentPlayer = 0;
  currentType = 0;
  currentLine = 0;
  currentRound = 1;
  currentColumn = 0;
  selectPawn = NULL;

  /* init player1 pawns */
  pPawnsPlayer1[0] = InitPawn(0, PLAYER1_BASE, 1, PLAYER1, 0);
  pPawnsPlayer1[1] = InitPawn(1, PLAYER1_BASE, 2, PLAYER1, 1);
  pPawnsPlayer1[2] = InitPawn(5, PLAYER1_BASE, 3, PLAYER1, 2);
  pPawnsPlayer1[3] = InitPawn(6, PLAYER1_BASE, 4, PLAYER1, 3);
  pPawnsPlayer1[4] = InitPawn(6, PLAYER1_BASE, 5, PLAYER1, 4);
  pPawnsPlayer1[5] = InitPawn(5, PLAYER1_BASE, 6, PLAYER1, 5);
  pPawnsPlayer1[6] = InitPawn(1, PLAYER1_BASE, 7, PLAYER1, 6);
  pPawnsPlayer1[7] = InitPawn(0, PLAYER1_BASE, 8, PLAYER1, 7);
  pPlayersPawns[0] = pPawnsPlayer1;

  /* init player2 pawns */
  pPawnsPlayer2[0] = InitPawn(0, PLAYER2_BASE, 1, PLAYER2, 0);
  pPawnsPlayer2[1] = InitPawn(2, PLAYER2_BASE, 2, PLAYER2, 1);
  pPawnsPlayer2[2] = InitPawn(5, PLAYER2_BASE, 3, PLAYER2, 2);
  pPawnsPlayer2[3] = InitPawn(6, PLAYER2_BASE, 4, PLAYER2, 3);
  pPawnsPlayer2[4] = InitPawn(6, PLAYER2_BASE, 5, PLAYER2, 4);
  pPawnsPlayer2[5] = InitPawn(5, PLAYER2_BASE, 6, PLAYER2, 5);
  pPawnsPlayer2[6] = InitPawn(2, PLAYER2_BASE, 7, PLAYER2, 6);
  pPawnsPlayer2[7] = InitPawn(0, PLAYER2_BASE, 8, PLAYER2, 7);
  pPlayersPawns[1] = pPawnsPlayer2;

  /* init player3 pawns */
  pPawnsPlayer3[0] = InitPawn(0, 1, PLAYER3_BASE, PLAYER3, 0);
  pPawnsPlayer3[1] = InitPawn(3, 2, PLAYER3_BASE, PLAYER3, 1);
  pPawnsPlayer3[2] = InitPawn(5, 3, PLAYER3_BASE, PLAYER3, 2);
  pPawnsPlayer3[3] = InitPawn(6, 4, PLAYER3_BASE, PLAYER3, 3);
  pPawnsPlayer3[4] = InitPawn(6, 5, PLAYER3_BASE, PLAYER3, 4);
  pPawnsPlayer3[5] = InitPawn(5, 6, PLAYER3_BASE, PLAYER3, 5);
  pPawnsPlayer3[6] = InitPawn(3, 7, PLAYER3_BASE, PLAYER3, 6);
  pPawnsPlayer3[7] = InitPawn(0, 8, PLAYER3_BASE, PLAYER3, 7);
  pPlayersPawns[2] = pPawnsPlayer3;

  /* init player4 pawns */
  pPawnsPlayer4[0] = InitPawn(0, 1, PLAYER4_BASE, PLAYER4, 0);
  pPawnsPlayer4[1] = InitPawn(4, 2, PLAYER4_BASE, PLAYER4, 1);
  pPawnsPlayer4[2] = InitPawn(5, 3, PLAYER4_BASE, PLAYER4, 2);
  pPawnsPlayer4[3] = InitPawn(6, 4, PLAYER4_BASE, PLAYER4, 3);
  pPawnsPlayer4[4] = InitPawn(6, 5, PLAYER4_BASE, PLAYER4, 4);
  pPawnsPlayer4[5] = InitPawn(5, 6, PLAYER4_BASE, PLAYER4, 5);
  pPawnsPlayer4[6] = InitPawn(4, 7, PLAYER4_BASE, PLAYER4, 6);
  pPawnsPlayer4[7] = InitPawn(0, 8, PLAYER4_BASE, PLAYER4, 7);
  pPlayersPawns[3] = pPawnsPlayer4;

  /* clear chessboard cell pointers and set all players's paws on it */
  ClearChessboard(pChessboard);
  UpdateChessboard(nbPlayers, pPlayersPawns, pChessboard);
  GameLogic(drawingArea, NULL, NULL);
}

/* draw the game, every time the user move the mouse */
/* not great, not terrible... */
gboolean GameLogic(GtkWidget *widget, GdkEventMotion *event, gpointer data) {
  u8 mouseCellX = 0;
  u8 mouseCellY = 0;

  /* get the current cell where is the mouse */
  if (event != NULL) GetMouseToCell(event->x, event->y, &mouseCellX, &mouseCellY);

  /* clear screen */
  ClearSurface();

  /* draw chessboard cells */
  DrawChessboardCells(widget);

  /* draw all pawns for each player */
  DrawPawns(widget, pChessboard);

  /* update status bar */
  UpdateStatusBar(statusBar, currentPlayer, currentRound);

  /* if the current players clic on one of his pawn */
  if (selectPawn != NULL){
     /* Draw circle on this pawn */
    DrawCircle(widget, selectPawn->pos.line, selectPawn->pos.column, PLAYERS_COLOR[selectPawn->player]);
     /* Compute futurs moves for this pawn */
    allNewMoves = ComputeFutureMoves(currentLine, currentColumn, OUT_OF_BOUND, OUT_OF_BOUND, currentType, currentPlayer, FALSE, pChessboard);
    tmp = allNewMoves;
    /* draw possible new moves */
    while (tmp != NULL) {
      DrawCell(widget, tmp->newPos.line, tmp->newPos.column, PLAYERS_COLOR[selectPawn->player]);
      tmp = tmp->pNextMove;
    }
    FreeLinkedListMoves(allNewMoves);
  }
  return TRUE;
}

/* function called when "draw" event is generated (so each time the mouse move on the drawing surface) */
/* GameLogic function draw inside G_SURFACE buffer and DrawCallback render the buffer to the screen */
gboolean DrawCallback(GtkWidget *widget, cairo_t *cr, gpointer data) {
  cairo_set_source_surface(cr, G_SURFACE, 0, 0);
  cairo_paint(cr);
  return FALSE;
}

/* clean memory when quit the game */
void QuitGame() {
  /* remove PNG pawns surfaces */
  FreePNGSurface();

  /* free all pointers data, in a good order to avoid pointers deferences */
  FreePlayerPawns(pPlayersPawns[0]);
  FreePlayerPawns(pPlayersPawns[1]);
  FreePlayerPawns(pPlayersPawns[2]);
  FreePlayerPawns(pPlayersPawns[3]);
  free(pPawnsPlayer1);
  free(pPawnsPlayer2);
  free(pPawnsPlayer3);
  free(pPawnsPlayer4);
  free(pPlayersPawns);
  free(pChessboard);

  /* remove global surface on which we draw the game */
  cairo_surface_destroy(G_SURFACE);
}

/* Create a new surface of the appropriate size to store our scribbles */
gboolean ConfigureSurface(GtkWidget *widget, GdkEventConfigure *event, gpointer data) {
  if (G_SURFACE)
    cairo_surface_destroy(G_SURFACE);

  G_SURFACE = gdk_window_create_similar_surface(
      gtk_widget_get_window(widget), CAIRO_CONTENT_COLOR,
      gtk_widget_get_allocated_width(widget),
      gtk_widget_get_allocated_height(widget));

  /* Initialize the surface to white */
  ClearSurface();

  /* We've handled the configure event, no need for further processing. */
  return TRUE;
}

gboolean EventClickOnBoard(GtkWidget *widget, GdkEventButton *event) {
  u8 mouseCellX = 0;
  u8 mouseCellY = 0;
  GetMouseToCell(event->x, event->y, &mouseCellX, &mouseCellY);

  /* left clic */
  if (event->button == 1) {
    /* if the first boolean check is false, we dont check the second one, so no segmentation fault */
    if ((pChessboard[(mouseCellY * CHESSBOARD_SIZE) + mouseCellX] == NULL) ||
          (pChessboard[(mouseCellY * CHESSBOARD_SIZE) + mouseCellX]->player != currentPlayer)) {
      selectPawn = NULL;
    } else {
      selectPawn = pChessboard[(mouseCellY * CHESSBOARD_SIZE) + mouseCellX];
      currentType = pChessboard[(mouseCellY * CHESSBOARD_SIZE) + mouseCellX]->type;
      currentLine = pChessboard[(mouseCellY * CHESSBOARD_SIZE) + mouseCellX]->pos.line;
      currentColumn = pChessboard[(mouseCellY * CHESSBOARD_SIZE) + mouseCellX]->pos.column;
    }
  }

  /* call GameLogic to update chessboard screen */
  GameLogic(widget, NULL, NULL);

  return TRUE;
};

/* update StatusBar String with current player and round number */
void UpdateStatusBar(GtkWidget* statusBar, u8 currentPlayer, u8 currentRound) {
  guint tmpGuint = gtk_statusbar_get_context_id(GTK_STATUSBAR(statusBar), GAME_STATUS_STR);
  gchar statusBarString[45];
  snprintf(statusBarString, sizeof statusBarString,"Player's turn: %s, current round: %d", PLAYERS_STR[currentPlayer], currentRound);
  gtk_statusbar_pop(GTK_STATUSBAR(statusBar), tmpGuint);
  gtk_statusbar_push(GTK_STATUSBAR(statusBar), tmpGuint, statusBarString);
}

/* change difficulty and restart the game */
void EventDifficultyChanged(GtkWidget* radioMenuItem, gpointer data) {
  /* the function is called two times with toggled event, one when the old widget is disable */
  /* one when the new widget is enable. We only want to work with the second one */
  if (gtk_check_menu_item_get_active(GTK_CHECK_MENU_ITEM(radioMenuItem))) {
    switch (GPOINTER_TO_INT(data)) {
    case 0: difficulty = 0; break;
    case 1: difficulty = 1; break;
    case 2: difficulty = 2; break;
    }
  }
  ResetGameLogicVariables();
}

/* change number of players and restart the game */
void EventNbPlayersChanged(GtkWidget* radioMenuItem, gpointer data) {
  /* the function is called two times with toggled event, one when the old widget is disable */
  /* one when the new widget is enable. We only want to work with the second one */
  if (gtk_check_menu_item_get_active(GTK_CHECK_MENU_ITEM(radioMenuItem))) {
    switch (GPOINTER_TO_INT(data)) {
    case 2: nbPlayers = 2; break;
    case 3: nbPlayers = 3; break;
    case 4: nbPlayers = 4; break;
    }
  }
  ResetGameLogicVariables();
}
