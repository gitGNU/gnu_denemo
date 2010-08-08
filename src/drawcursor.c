/* drawcursor.cpp
 * functions for drawing the cursor
 *
 * for Denemo, a gtk+ frontend to GNU Lilypond
 * (c) 1999-2005  Matthew Hiller, Adam Tee
 */
#include <math.h>
#include "drawingprims.h"
#include "gcs.h"
#include "utils.h"

#define CURSOR_MINUS 2
#define CURSOR_WIDTH 10
#define CURSOR_HEIGHT 5


/**
 * Draw the cursor on the canvas at the given position
 *
 */
void
draw_cursor (cairo_t *cr, DenemoScore * si,
	     gint xx, gint y, input_mode mode, gint dclef)
{
  if(!cr) return;
  gint height = calculateheight (si->cursor_y, dclef);

  static GdkGC *blackgc = NULL;
  static GdkGC *graygc;
  static GdkGC *greengc;
  static GdkGC *redgc;
  static GdkGC *bluegc;
  static GdkGC *purplegc;
  GdkGC *paintgc;
  //xx -=5;
  if (!blackgc)
    {
      blackgc = gcs_blackgc ();
      graygc = gcs_graygc ();
      greengc = gcs_greengc ();
      redgc = gcs_redgc ();
      bluegc = gcs_bluegc ();
      purplegc = gcs_purplegc ();
    }

  paintgc = (mode & INPUTREST) ? graygc :
    (mode & INPUTBLANK) ? bluegc : si->cursoroffend ? redgc : greengc;

  if(si->cursor_appending)
    paintgc = bluegc;

  cairo_save( cr );
  setcairocolor( cr, paintgc );
  if(si->cursor_appending)
    cairo_rectangle( cr, xx, height + y, 2*CURSOR_WIDTH, 2*CURSOR_HEIGHT );
  else
    cairo_rectangle( cr, xx, height + y - CURSOR_MINUS, CURSOR_WIDTH, CURSOR_HEIGHT );
  cairo_fill( cr );

  if(Denemo.prefs.cursor_highlight) {
    gdouble length = 20/si->zoom;
    if(!si->cursor_appending) {
      setcairocolor( cr, bluegc );
      cairo_move_to( cr, xx-2, y);
      cairo_rel_line_to( cr, 0, STAFF_HEIGHT);
      cairo_stroke( cr );
      setcairocolor( cr, paintgc );
    }
    cairo_set_line_width (cr, 6.0/si->zoom);
    cairo_set_source_rgba (cr, 0, 1, 0, 0.40);


    cairo_arc(cr, xx + CURSOR_WIDTH/2, height + y, length, 0, 2 * M_PI);

    cairo_stroke( cr );
   
  }
  cairo_restore( cr );

  /* Now draw ledgers if necessary and we're done */
  draw_ledgers (cr, height, height, xx, y, CURSOR_WIDTH);
}
