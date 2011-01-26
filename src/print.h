#ifndef PRINT_H
#define PRINT_H

#include <denemo/denemo.h> 

void
printall_cb(GtkAction *action, gpointer param);
void
printpreview_cb(GtkAction *action, gpointer param);
void
printselection_cb(GtkAction *action, gpointer param);
void
printexcerptpreview_cb(GtkAction *action, gpointer param);
void
printpart_cb(GtkAction *action, gpointer param);
gchar *
get_printfile_pathbasename(void);
void viewer(DenemoGUI *gui);
void export_mudela_action (GtkAction *action, gpointer param);
void export_pdf_action (GtkAction *action, gpointer param);
void export_png_action (GtkAction *action, gpointer param);
void export_midi_action (GtkAction *action, gpointer param);
void export_csound_action (GtkAction *action, gpointer param);
void export_pdf (gchar *filename, DenemoGUI * gui);
void export_png (gchar *filename, gboolean show_preview, DenemoGUI * gui);
void install_printpreview(DenemoGUI *gui, GtkWidget *vbox);
void
refresh_print_view(gboolean interactive);
gchar *
get_lily_version_string (void);
void
print_and_view (gchar **arguments);
#endif /*PRINT_H*/	
