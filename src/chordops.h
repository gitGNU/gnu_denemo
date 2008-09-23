/* chordops.h
 *
 * the header files for chordops.c, functions which manipulate chords
 * For denemo, a gtk+ frontend to Lilypond, the GNU music typesetter
 *
 * (c) 2000-2005 Matthew Hiller, Adam Tee
 *
 */

#include <glib.h>
#include <denemo/denemo.h> 
#ifndef CHORDOPSH
#define CHORDOPSH

void newclefify (DenemoObject * thechord, gint dclef);

void findreversealigns (DenemoObject * thechord);

DenemoObject *newchord (gint baseduration, gint numdots, int tied);

objnode *nearestnote (DenemoObject * thechord, gint mid_c_offset);

void modify_note (chord *thechord,  gint mid_c_offset, gint enshift, gint dclef);

note* addtone (DenemoObject * mudelaobj, gint mid_c_offset, gint enshift,
	      gint dclef);

gboolean removetone (DenemoObject * mudelaobj, gint mid_c_offset, gint dclef);

void shiftpitch (DenemoObject * mudelaobj, gint mid_c_offset,
		 gint is_sharpening);

void changenote (DenemoObject * thechord, gint mid_c_offset, gint new_offset, gint dclef);

void changeenshift (DenemoObject * thechord, gint mid_c_offset, gint accidental);

void changedur (DenemoObject * thechord, gint baseduration, gint numdots);

void changenumdots (DenemoObject * thechord, int number);

void freechord (DenemoObject * mudelaobj);

DenemoObject *clone_chord (DenemoObject * mudelaobj);
DenemoObject *hidechord (DenemoObject * thechord);


#endif /*CHORDOPSH */
