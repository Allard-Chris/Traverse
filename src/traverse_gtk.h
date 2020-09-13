#ifndef _HEADER_TRAVERSE_GTK_H
#define _HEADER_TRAVERSE_GTK_H

// To avoid Wall on extern libraries
#pragma GCC diagnostic push // save diagnostic state
#pragma GCC diagnostic ignored "-Wall"
#include "gtk/gtk.h"
#include "math.h"
#pragma GCC diagnostic pop // turn the warnings back on

#include "traverse.h"
#include "utils.h"

#ifndef M_PI
#define M_PI 3.14159
#endif

// GTK GAME CONSTANTS
#define CELL_SIZE              50
#define HALF_CELL_SIZE         25
#define CELL_BORDER_SIZE       1
#define CELL_BORDER_UP_COLOR   1
#define CELL_BORDER_DOWN_COLOR 0.4
#define PATH_LINE_SIZE         10

// Cells colors
// clang-format off
#define GREY_DARK           { 0.8, 0.8, 0.8 }
#define GREY_LIGHT          { 0.9, 0.9, 0.9 }
#define RED_DARK            { 0.9, 0.8, 0.8 }
#define RED_LIGHT           { 1.0, 0.9, 0.9 }
#define BLUE_DARK           { 0.8, 0.8, 0.9 }
#define BLUE_LIGHT          { 0.9, 0.9, 1.0 }
#define GREEN_DARK          { 0.8, 0.9, 0.8 }
#define GREEN_LIGHT         { 0.9, 1.0, 0.9 }
#define YELLOW_DARK         { 0.9, 0.9, 0.8 }
#define YELLOW_LIGHT        { 1.0, 1.0, 0.9 }
#define PLAYER1_COLOR       { 1.0, 0.2, 0.2 }
#define PLAYER2_COLOR       { 0.4, 0.4, 1.0 }
#define PLAYER3_COLOR       { 0.4, 0.8, 0.1 }
#define PLAYER4_COLOR       { 1.0, 0.8, 0.2 }
#define PLAYER1_PATH_COLOR  { 0.8, 0.0, 0.0 }
#define PLAYER2_PATH_COLOR  { 0.2, 0.2, 0.8 }
#define PLAYER3_PATH_COLOR  { 0.2, 0.7, 0.0 }
#define PLAYER4_PATH_COLOR  { 1.0, 0.6, 0.0 }
// clang-format on

// external array png image for pawn
// i want to compile png images inside final binarie and link this data here
// i dont like to open image where im not when sure they are not in correct location...
extern const char _binary_src_assets_p1t_png_start;
extern const char _binary_src_assets_p1d_png_start;
extern const char _binary_src_assets_p1s_png_start;
extern const char _binary_src_assets_p1c_png_start;
extern const char _binary_src_assets_p2t_png_start;
extern const char _binary_src_assets_p2d_png_start;
extern const char _binary_src_assets_p2s_png_start;
extern const char _binary_src_assets_p2c_png_start;
extern const char _binary_src_assets_p3t_png_start;
extern const char _binary_src_assets_p3d_png_start;
extern const char _binary_src_assets_p3s_png_start;
extern const char _binary_src_assets_p3c_png_start;
extern const char _binary_src_assets_p4t_png_start;
extern const char _binary_src_assets_p4d_png_start;
extern const char _binary_src_assets_p4s_png_start;
extern const char _binary_src_assets_p4c_png_start;

// structure for read png data
typedef struct {
  unsigned int   last;
  unsigned char* data;
} cairo_read_png_closure_t;

// structure for RGB color
typedef struct {
  double r;
  double g;
  double b;
} color;

// constant array
extern const color      CHESSBOARD_COLOR[CHESSBOARD_SIZE][CHESSBOARD_SIZE]; // each cell has a constant color
extern const color      PLAYERS_COLOR[4];                                   // constant color for each player
extern const color      PLAYERS_PATH_COLOR[4];                              // constant color for each player
extern cairo_surface_t* PAWN_IMAGES[4][7]; // array where there is surface of each pawn which we must to draw
extern cairo_surface_t* G_SURFACE;         // global surface where we render the game

// functions
cairo_status_t   cairo_read_png_from_array(void* closure, u8* data, unsigned int length);
cairo_surface_t* cairo_surface_from_png_data(u8* data);
void             LoadPNGSurface();
void             FreePNGSurface();
void             GetMouseToCellPosition(gdouble mX, gdouble mY, u8* cX, u8* cY);
void             ClearSurface();
void             DrawChessboard(GtkWidget* widget);
void             DrawPawns(GtkWidget* widget, Pawn** pChessboard);
void             DrawCell(GtkWidget* widget, u8 line, u8 column, color currentColor);
void             DrawCircle(GtkWidget* widget, u8 line, u8 column, color currentColor);
void             DrawCellPath(GtkWidget* widget, u8 cx1, u8 cy1, u8 cx2, u8 cy2, color currentColor);
#endif // _HEADER_TRAVERSE_GTK_H