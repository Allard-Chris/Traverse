#include "traverse_gtk.h"

/* each cell of the chessboard has a constant color */
const color CHESSBOARD_COLOR[CHESSBOARD_SIZE][CHESSBOARD_SIZE] = {
    {GREY_LIGHT,    RED_DARK,   RED_LIGHT,  RED_DARK,   RED_LIGHT,  RED_DARK,   RED_LIGHT,  RED_DARK,   RED_LIGHT,  GREY_DARK},
    {YELLOW_DARK,   GREY_LIGHT, GREY_DARK,  GREY_LIGHT, GREY_DARK,  GREY_LIGHT, GREY_DARK,  GREY_LIGHT, GREY_DARK,  GREEN_LIGHT},
    {YELLOW_LIGHT,  GREY_DARK,  GREY_LIGHT, GREY_DARK,  GREY_LIGHT, GREY_DARK,  GREY_LIGHT, GREY_DARK,  GREY_LIGHT, GREEN_DARK},
    {YELLOW_DARK,   GREY_LIGHT, GREY_DARK,  GREY_LIGHT, GREY_DARK,  GREY_LIGHT, GREY_DARK,  GREY_LIGHT, GREY_DARK,  GREEN_LIGHT},
    {YELLOW_LIGHT,  GREY_DARK,  GREY_LIGHT, GREY_DARK,  GREY_LIGHT, GREY_DARK,  GREY_LIGHT, GREY_DARK,  GREY_LIGHT, GREEN_DARK},
    {YELLOW_DARK,   GREY_LIGHT, GREY_DARK,  GREY_LIGHT, GREY_DARK,  GREY_LIGHT, GREY_DARK,  GREY_LIGHT, GREY_DARK,  GREEN_LIGHT},
    {YELLOW_LIGHT,  GREY_DARK,  GREY_LIGHT, GREY_DARK,  GREY_LIGHT, GREY_DARK,  GREY_LIGHT, GREY_DARK,  GREY_LIGHT, GREEN_DARK},
    {YELLOW_DARK,   GREY_LIGHT, GREY_DARK,  GREY_LIGHT, GREY_DARK,  GREY_LIGHT, GREY_DARK,  GREY_LIGHT, GREY_DARK,  GREEN_LIGHT},
    {YELLOW_LIGHT,  GREY_DARK,  GREY_LIGHT, GREY_DARK,  GREY_LIGHT, GREY_DARK,  GREY_LIGHT, GREY_DARK,  GREY_LIGHT, GREEN_DARK},
    {GREY_DARK,     BLUE_LIGHT, BLUE_DARK,  BLUE_LIGHT, BLUE_DARK,  BLUE_LIGHT, BLUE_DARK,  BLUE_LIGHT, BLUE_DARK,  GREY_LIGHT}};

const color PLAYERS_COLOR[4] = {PLAYER1_COLOR, PLAYER2_COLOR, PLAYER3_COLOR, PLAYER4_COLOR};

/* array of png data used to draw pawns */
/* they are 4 differents png for each player's pawn */
/* the first index is player's id */
/* the second index is pawn type for this player */
/* the second index is correlated with pawns type */
/* the pawn index 2, 3, 4 and 5 used the same pawn (triangle) */
cairo_surface_t* PAWN_IMAGES[MAX_NB_PLAYER][NB_PAWNS_TYPE];

/* read_t function to take every byte from array to be push into stream */
/* data come from PNG file (8bit per pixel RGBA) converted into binary array by ld at compile time */
cairo_status_t cairo_read_png_from_array(void* closure, u8* data, unsigned int length) {
  cairo_read_png_closure_t* c = (cairo_read_png_closure_t*)(closure);
  memcpy(data, &c->data[c->last], length);
  c->last += length;
  return CAIRO_STATUS_SUCCESS;
}

/* create cairo surface from png stream */
cairo_surface_t* cairo_surface_from_png_data(u8* data) {
  cairo_surface_t*         surface;
  cairo_read_png_closure_t closure;
  closure.last = 0;
  closure.data = data;
  surface = cairo_image_surface_create_from_png_stream(cairo_read_png_from_array, &closure);
  return surface;
}

/* get on wich cell is the mouse on */
void GetMouseToCell(gdouble mX, gdouble mY, u8* mouseCellX, u8* mouseCellY) {
  *mouseCellX = ((int)(mX)) / CELL_SIZE;
  *mouseCellY = ((int)(mY)) / CELL_SIZE;
}

/* clear cairo surface */
void ClearSurface() {
  cairo_t* cr = cairo_create(G_SURFACE);
  cairo_set_source_rgb(cr, 1, 1, 1);
  cairo_paint(cr);
  cairo_destroy(cr);
}

/* Load all PNG images for each pawn for each player */
void LoadPNGSurface() {
  PAWN_IMAGES[0][0] = cairo_surface_from_png_data((u8*)&_binary_src_assets_p1s_png_start);
  PAWN_IMAGES[0][1] = cairo_surface_from_png_data((u8*)&_binary_src_assets_p1t_png_start);
  PAWN_IMAGES[0][2] = cairo_surface_from_png_data((u8*)&_binary_src_assets_p1t_png_start);
  PAWN_IMAGES[0][3] = cairo_surface_from_png_data((u8*)&_binary_src_assets_p1t_png_start);
  PAWN_IMAGES[0][4] = cairo_surface_from_png_data((u8*)&_binary_src_assets_p1t_png_start);
  PAWN_IMAGES[0][5] = cairo_surface_from_png_data((u8*)&_binary_src_assets_p1d_png_start);
  PAWN_IMAGES[0][6] = cairo_surface_from_png_data((u8*)&_binary_src_assets_p1c_png_start);
  PAWN_IMAGES[1][0] = cairo_surface_from_png_data((u8*)&_binary_src_assets_p2s_png_start);
  PAWN_IMAGES[1][1] = cairo_surface_from_png_data((u8*)&_binary_src_assets_p2t_png_start);
  PAWN_IMAGES[1][2] = cairo_surface_from_png_data((u8*)&_binary_src_assets_p2t_png_start);
  PAWN_IMAGES[1][3] = cairo_surface_from_png_data((u8*)&_binary_src_assets_p2t_png_start);
  PAWN_IMAGES[1][4] = cairo_surface_from_png_data((u8*)&_binary_src_assets_p2t_png_start);
  PAWN_IMAGES[1][5] = cairo_surface_from_png_data((u8*)&_binary_src_assets_p2d_png_start);
  PAWN_IMAGES[1][6] = cairo_surface_from_png_data((u8*)&_binary_src_assets_p2c_png_start);
  PAWN_IMAGES[2][0] = cairo_surface_from_png_data((u8*)&_binary_src_assets_p3s_png_start);
  PAWN_IMAGES[2][1] = cairo_surface_from_png_data((u8*)&_binary_src_assets_p3t_png_start);
  PAWN_IMAGES[2][2] = cairo_surface_from_png_data((u8*)&_binary_src_assets_p3t_png_start);
  PAWN_IMAGES[2][3] = cairo_surface_from_png_data((u8*)&_binary_src_assets_p3t_png_start);
  PAWN_IMAGES[2][4] = cairo_surface_from_png_data((u8*)&_binary_src_assets_p3t_png_start);
  PAWN_IMAGES[2][5] = cairo_surface_from_png_data((u8*)&_binary_src_assets_p3d_png_start);
  PAWN_IMAGES[2][6] = cairo_surface_from_png_data((u8*)&_binary_src_assets_p3c_png_start);
  PAWN_IMAGES[3][0] = cairo_surface_from_png_data((u8*)&_binary_src_assets_p4s_png_start);
  PAWN_IMAGES[3][1] = cairo_surface_from_png_data((u8*)&_binary_src_assets_p4t_png_start);
  PAWN_IMAGES[3][2] = cairo_surface_from_png_data((u8*)&_binary_src_assets_p4t_png_start);
  PAWN_IMAGES[3][3] = cairo_surface_from_png_data((u8*)&_binary_src_assets_p4t_png_start);
  PAWN_IMAGES[3][4] = cairo_surface_from_png_data((u8*)&_binary_src_assets_p4t_png_start);
  PAWN_IMAGES[3][5] = cairo_surface_from_png_data((u8*)&_binary_src_assets_p4d_png_start);
  PAWN_IMAGES[3][6] = cairo_surface_from_png_data((u8*)&_binary_src_assets_p4c_png_start);
}

/* call at the end to deletre all cairo_surface_t */
void FreePNGSurface() {
  for (unsigned int i = 0; i < MAX_NB_PLAYER; i++){
    for (unsigned int j = 0; j < NB_PAWNS_TYPE; j++){
      cairo_surface_destroy(PAWN_IMAGES[i][j]);
    }
  }
}

/* draw all pawns */
void DrawPawns(GtkWidget* widget, pawn** pChessboard) {
  cairo_t*  cr = cairo_create(G_SURFACE);
  u8        playerId = 0;
  u8        pawnType = 0;

  for (u8 line = 0; line < CHESSBOARD_SIZE; line++) {
    for (u8 column = 0; column < CHESSBOARD_SIZE; column++) {
      if (pChessboard[(line * CHESSBOARD_SIZE) + column] != NULL) {
        playerId = pChessboard[(line * CHESSBOARD_SIZE) + column]->player;
        pawnType = pChessboard[(line * CHESSBOARD_SIZE) + column]->type;
        cairo_set_source_surface(cr, PAWN_IMAGES[playerId][pawnType], (column * 50) + 1 , (line *50) + 1);
        cairo_paint(cr);
      }
    }
  }
  cairo_destroy(cr);
  gtk_widget_queue_draw_area(widget, 0, 0, (CELL_SIZE * 10), (CELL_SIZE * 10));
}

/* draw circle around the current selected pawn */
void DrawCircle(GtkWidget* widget, u8 line, u8 column, color currentColor) {
  cairo_t* cr = cairo_create(G_SURFACE);
  cairo_set_line_width(cr, 2);
  cairo_set_source_rgb(cr, currentColor.r, currentColor.g, currentColor.b);
  cairo_arc(cr, ((column * CELL_SIZE) + HALF_CELL_SIZE), ((line * CELL_SIZE) + HALF_CELL_SIZE), HALF_CELL_SIZE - 1, 0, 2 * M_PI);
  cairo_stroke_preserve(cr);
  cairo_destroy(cr);
  gtk_widget_queue_draw_area(widget, 0, 0, (CELL_SIZE * 10), (CELL_SIZE * 10));
}

/* Draw chessboard background */
void DrawChessboardCells(GtkWidget* widget) {
  for (u8 line = 0; line < CHESSBOARD_SIZE; line++) { /* line loop */
    for (u8 column = 0; column < CHESSBOARD_SIZE; column++) { /* column loop */
      /* color cells in terms of */
      DrawCell(widget, line, column, CHESSBOARD_COLOR[line][column]);
    }
  }
}

/* draw single cell */
void DrawCell(GtkWidget* widget, u8 line, u8 column, color currentColor) {
  cairo_t* cr = cairo_create(G_SURFACE);
  cairo_set_antialias (cr, CAIRO_ANTIALIAS_NONE); /* no antialias, to draw on pixel perfect line  */

  /* paint to the surface, where we store our state */
  cairo_set_line_join(cr, CAIRO_LINE_JOIN_MITER);
  cairo_set_line_width(cr, CELL_BORDER_SIZE);

  /* color cells in terms of */
  cairo_set_source_rgb(cr, currentColor.r, currentColor.g, currentColor.b);

  /* draw cell background */
  cairo_rectangle(cr, (column * CELL_SIZE), (line * CELL_SIZE), CELL_SIZE, CELL_SIZE);
  cairo_fill(cr);

  /* draw border down */
  cairo_set_source_rgb(cr, CELL_BORDER_DOWN_COLOR, CELL_BORDER_DOWN_COLOR, CELL_BORDER_DOWN_COLOR);
  cairo_move_to(cr, (column * CELL_SIZE), ((line * CELL_SIZE) + (CELL_SIZE - 0.5)));
  cairo_line_to(cr, ((column * CELL_SIZE) + (CELL_SIZE + 0.5)), ((line * CELL_SIZE) + (CELL_SIZE - 0.5)));
  cairo_move_to(cr, ((column * CELL_SIZE) + (CELL_SIZE - 0.5)), ((line * CELL_SIZE) + (CELL_SIZE + 0.5)));
  cairo_line_to(cr, ((column * CELL_SIZE) + (CELL_SIZE - 0.5)), ((line * CELL_SIZE)));
  cairo_stroke(cr);
  /* draw border up */
  cairo_set_source_rgb(cr, CELL_BORDER_UP_COLOR, CELL_BORDER_UP_COLOR, CELL_BORDER_UP_COLOR);
  cairo_move_to(cr, (column * CELL_SIZE) + 0.5, (line * CELL_SIZE) + 0.5);
  cairo_line_to(cr, ((column * CELL_SIZE) + CELL_SIZE), (line * CELL_SIZE));
  cairo_move_to(cr, (column * CELL_SIZE) + 0.5, (line * CELL_SIZE) + 0.5);
  cairo_line_to(cr, (column * CELL_SIZE), ((line * CELL_SIZE) + CELL_SIZE));
  cairo_stroke(cr);

  cairo_destroy(cr);

  /* Now invalidate the affected region of the drawing area */
  gtk_widget_queue_draw_area(widget, 0, 0, (CELL_SIZE * 10), (CELL_SIZE * 10));
}