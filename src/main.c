#include "traverse.h"
#include "traverse_gtk.h"
#include "utils.h"

/* global non-static variables, must be accessed by all functions, so they are created on the heap */
cairo_surface_t* G_SURFACE;
pawn**  pChessboard;     /* like the real life, a cell on a chessboard can be empty or with a pawn on it */
pawn*** pPlayersPawns;   /* Array of address of each players */
pawn**  pPawnsPlayer1;   /* Array of address of each player1's pawns */
pawn**  pPawnsPlayer2;   /* Array of address of each player2's pawns */
pawn**  pPawnsPlayer3;   /* Array of address of each player3's pawns */
pawn**  pPawnsPlayer4;   /* Array of address of each player4's pawns */

/* function called when "draw" event is generated (so each time the mouse move on the drawing surface) */
/* GameLogic function draw inside G_SURFACE buffer and DrawCallback render the buffer to the screen */
static gboolean DrawCallback(GtkWidget* widget, cairo_t* cr, gpointer data) {
  cairo_set_source_surface(cr, G_SURFACE, 0, 0);
  cairo_paint(cr);
  return FALSE;
}

/* draw the game, every time the user move the mouse */
static gboolean GameLogic(GtkWidget* widget, GdkEventMotion* event, gpointer data) {
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

  return TRUE;
}

/* clean memory when quit the game */
static void CloseWindow() {
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
gboolean ConfigureSurface(GtkWidget*         widget,
                          GdkEventConfigure* event,
                          gpointer           data) {
  if (G_SURFACE) cairo_surface_destroy(G_SURFACE);

  G_SURFACE = gdk_window_create_similar_surface(
      gtk_widget_get_window(widget), CAIRO_CONTENT_COLOR,
      gtk_widget_get_allocated_width(widget),
      gtk_widget_get_allocated_height(widget));

  /* Initialize the surface to white */
  ClearSurface();

  /* We've handled the configure event, no need for further processing. */
  return TRUE;
}

/* the real main behind the main */
static void RunningApp(GtkApplication* app, gpointer user_data) {
  /***** START GTK Init *****/
  /* create GTK windows */
  GtkWidget* window;
  window = gtk_application_window_new(app);
  gtk_window_set_title(GTK_WINDOW(window), "Traverse");
  gtk_container_set_border_width(GTK_CONTAINER(window), 8);
  gtk_window_set_resizable((GtkWindow*)window, FALSE); /* cant resize it */

  /* create GTK frame */
  GtkWidget* frame;
  frame = gtk_frame_new(NULL);
  gtk_frame_set_shadow_type(GTK_FRAME(frame), GTK_SHADOW_IN);
  gtk_container_add(GTK_CONTAINER(window), frame);
  /* window events : destroy surface when exit */
  g_signal_connect(window, "destroy", G_CALLBACK(CloseWindow), NULL);

  /* create GTK drawing area */
  GtkWidget* drawingArea = gtk_drawing_area_new();
  gtk_widget_set_size_request(drawingArea, (CELL_SIZE * 10), (CELL_SIZE * 10));
  gtk_container_add(GTK_CONTAINER(frame), drawingArea);

  /* subscrite to mouse event, normally drawingArea doesn't listen this kind of
   * event, poor boy */
  gtk_widget_set_events(drawingArea, gtk_widget_get_events(drawingArea) | GDK_POINTER_MOTION_MASK);
  g_signal_connect(drawingArea, "configure-event", G_CALLBACK(ConfigureSurface), NULL);

  /* refresh DrawingArea */
  g_signal_connect(drawingArea, "draw", G_CALLBACK(DrawCallback), NULL);
  /* mouse event on drawingArea */
  g_signal_connect(drawingArea, "motion-notify-event", G_CALLBACK(GameLogic), NULL);
  /***** END GTK Init *****/

  /***** START GAME LOGIC Init ****/
  /* init all variables */
  u8 nbPlayers = 4;
  /* malloc all games variables */ 
  pChessboard   = malloc((CHESSBOARD_SIZE * CHESSBOARD_SIZE) * sizeof(pawn*));
  pPlayersPawns = malloc(4 * sizeof(pawn **));
  pPawnsPlayer1 = malloc(NB_PAWNS * sizeof(pawn*));
  pPawnsPlayer2 = malloc(NB_PAWNS * sizeof(pawn*));
  pPawnsPlayer3 = malloc(NB_PAWNS * sizeof(pawn*));
  pPawnsPlayer4 = malloc(NB_PAWNS * sizeof(pawn*));

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

  /* Loading all pawns png data in array */
  LoadPNGSurface();

  /* show all */
  gtk_widget_show_all(window);

  /* first time draw chessboard */
  GameLogic(drawingArea, NULL, NULL);
}


/* main function */
int main(int argc, char** argv) {
  GtkApplication* app;
  int             status;

  /* Running GTK App */
  app = gtk_application_new("org.gtk.games.traverse", G_APPLICATION_FLAGS_NONE);
  g_signal_connect(app, "activate", G_CALLBACK(RunningApp), NULL);
  status = g_application_run(G_APPLICATION(app), argc, argv);
  g_object_unref(app);

  return status;
}