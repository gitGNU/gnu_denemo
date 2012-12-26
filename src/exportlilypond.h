/* exportlilypond.h
 * Header file for lilypond generation

 * for Denemo, a gtk+ frontend to GNU Lilypond
 * (c) 2000-2005 Matthew Hiller, Adam Tee, 2011 Richard Shann
 */
#ifndef EXPORTMUDELAH
#define EXPORTMUDELAH

#include <gtk/gtk.h>
#include <stdio.h>
#define TAB "        "

void exportlilypond (gchar * thefilename,  DenemoGUI *gui, gboolean all_movements);

void export_lilypond_parts(char *filename, DenemoGUI *gui);
void export_lilypond_part(char *filename, DenemoGUI *gui, gboolean all_movements);

/* generate the LilyPond for the current part, all movements, into the LilyPond textview window */
void generate_lilypond_part(void);
void make_scoreblock_editable(void);
/* generate lilypond text for the object passed in - the string should
   be g_freed by the caller when finished with*/
gchar *generate_lily (objnode *obj);
void refresh_lily_cb (GtkAction *action, DenemoGUI *gui);
void force_lily_refresh(DenemoGUI *gui);
void toggle_lily_visible_cb (GtkAction *action, gpointer param);

void custom_lily_cb (GtkAction *action, gpointer param);

void delete_lily_cb (GtkAction *action, gpointer param);
void set_lily_error(gint line, gint column, DenemoGUI *gui);
void highlight_lily_error(DenemoGUI *gui);
gboolean goto_lilypond_position(gint line, gint column);


void set_initiate_scoreblock(DenemoScore *si, GString *scoreblock);
gchar *get_lilypond_for_clef(clef *theclef);
gchar *get_lilypond_for_keysig(struct keysig *key);
gchar *get_lilypond_for_timesig(timesig *time);
gchar *get_lilypond_paper(void);
const gchar *get_prevailing_clef_as_lilypond(void);
const gchar *get_prevailing_keysig_as_lilypond(void);
const gchar *get_prevailing_timesig_as_lilypond(void);
#endif
