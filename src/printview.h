#ifndef __PRINT_VIEW_H__
#define __PRINT_VIEW_H__

#include <denemo/denemo.h>

gboolean get_offset (gdouble * x, gdouble * y);
gboolean get_positions (gdouble * neary, gdouble * fary, gboolean for_slur);
gboolean get_new_target (void);
gboolean get_reference_point (void);
gboolean get_control_point (gint which);
gboolean get_curve (gdouble * x1, gdouble * y1, gdouble * x2, gdouble * y2, gdouble * x3, gdouble * y3, gdouble * x4, gdouble * y4);

void present_print_view_window();
void typeset_part (void);
gboolean continuous_typesetting (void);
gboolean get_new_point (void);
int check_lily_version (gchar * version);
gboolean typeset_for_script (gchar * script);
gboolean print_typeset_pdf (void);
void typeset_current_layout (void);
void _show_print_view (GtkAction * action);
void install_printpreview (GtkWidget * vbox);
void refresh_print_view (gboolean interactive);
void printview_finished (G_GNUC_UNUSED GPid pid, G_GNUC_UNUSED gint status, gboolean print);
void print_from_print_view (gboolean all_movements);

#endif