/* commandfuncs.c
 * functions invoked by user keypresses in score area
 *
 * for Denemo, a gtk+ frontend to GNU Lilypond
 * (c) 1999-2005 Matthew Hiller, Adam Tee
 */

#include <string.h>
#include <stdint.h>
#include "command/commandfuncs.h"
#include "display/calculatepositions.h"
#include "command/chord.h"
#include "command/contexts.h"
#include "display/draw.h"
#include "command/measure.h"
#include "audio/midi.h"
#include "command/object.h"
#include "ui/moveviewport.h"
#include "command/select.h"
#include "command/staff.h"
#include "core/utils.h"
#include "command/tuplet.h"
#include "command/grace.h"
#include "audio/instrumentname.h"
#include "export/file.h"
#include "export/exportlilypond.h"
#include "core/exportxml.h"
#include "core/prefops.h"
#include "command/keyresponses.h"
#include "core/view.h"
#include "audio/pitchentry.h"
#include "audio/audiointerface.h"
#include "display/displayanimation.h"

/**
 * Macro to get the current DenemoObject
 */
#define declarecurmudelaobj \
DenemoObject *curmudelaobj = \
       (DenemoObject *) ( (si->currentobject && si->currentobject->data) ? \
       ((((DenemoObject *)si->currentobject->data)->type == TUPCLOSE) ? \
       (si->currentobject->prev?si->currentobject->prev->data:si->currentobject->data) : si->currentobject->data) : NULL)


/**
 * Move the current rhythm on to the next one
 * FIXME re-arrange the list of rhythms each time as well so that it
 * is easy to alternate rhythms.
 */
void
nextrhythm (GtkAction* action, DenemoScriptParam* param)
{
  if (!Denemo.project->rhythms)
    return;
  if (Denemo.project->currhythm == NULL)
    Denemo.project->currhythm = g_list_last (Denemo.project->rhythms);
  unhighlight_rhythm ((RhythmPattern *) Denemo.project->currhythm->data);
  if (Denemo.project->currhythm->next)
    Denemo.project->currhythm = Denemo.project->currhythm->next;
  else
    Denemo.project->currhythm = Denemo.project->rhythms;
#define g  (Denemo.project->rstep)

  g = ((RhythmPattern *) Denemo.project->currhythm->data)->rsteps;


#define CURRP ((RhythmPattern *)Denemo.project->currhythm->data)

  Denemo.project->cstep = (CURRP->clipboard)->data;
  if (((RhythmElement *) g->data)->icon)
    {
      GtkWidget *label = LABEL (CURRP->button);
      //g_debug("markup is %s\n", ((RhythmElement*)g->data)->icon);
      gtk_label_set_markup (GTK_LABEL (label), ((RhythmElement *) g->data)->icon);
    }
#if 0
  gtk_widget_set_state (GTK_WIDGET (((RhythmPattern *) gui->currhythm->data)->button), GTK_STATE_PRELIGHT);
#else
  highlight_rhythm ((RhythmPattern *) Denemo.project->currhythm->data);
#endif
  //g_debug("selected active\n");
#undef CURRP
#undef g
  displayhelper (Denemo.project);
  score_status(Denemo.project, TRUE);
}




/**
 * Helper function for calculating the 
 * beam and stem direction
 * I think it calculates the beam and stem directions for the si->currentmeasure
 * on the assumption that si->curmeasureclef has been set to the appropriate value
 * before the function is called. Likewise for 
 * si->cursortime1/2 and si->curmeasure_stem_directive.
 * the calculated values are stored in the objects in the measure - the fields set are:
 * isstart/end_beamgroup, is_stemup, stemy, is_reversealigned. reversealign, minpixelsalloted, space_before
 * si->curmeasureclef is also set: it is set to the value prevailing at the end of the measure.
 */

void
beamandstemdirhelper (DenemoMovement * si)
{
  DenemoObject *theclef = NULL;
  if (si->currentmeasure->prev)
    {
      objnode *curobj = measure_last_obj_node (si->currentmeasure->prev);
      if (curobj)
        theclef = get_clef_before_object (curobj);
    }
  if (theclef)
    si->curmeasureclef = ((clef *) theclef->object)->type;
  else
    si->curmeasureclef = ((DenemoStaff *) si->currentstaff->data)->clef.type;

  calculatebeamsandstemdirs ((objnode *) si->currentmeasure->data, &(si->curmeasureclef), &(si->cursortime1), &(si->cursortime2), &(si->curmeasure_stem_directive));
}


/**
 * Set si->current* variables from currentmeasurenum
 * the current object is set to the first in the measure
 * the selection is not updated
 *
 */
void
setcurrents (DenemoMovement * si)
{
  if (((DenemoStaff *) si->currentstaff->data)->nummeasures >= si->currentmeasurenum)
    {
      si->currentmeasure = g_list_nth (staff_first_measure_node (si->currentstaff), si->currentmeasurenum - 1);
    }
  else
    {
      g_debug ("Setting measure to %d which is last in Staff\n", ((DenemoStaff *) si->currentstaff->data)->nummeasures);
      si->currentmeasure = g_list_nth (staff_first_measure_node (si->currentstaff), ((DenemoStaff *) si->currentstaff->data)->nummeasures - 1);
      si->currentmeasurenum = ((DenemoStaff *) si->currentstaff->data)->nummeasures;

    }

  si->cursor_x = 0;
  si->currentobject = (objnode *) si->currentmeasure->data;
  if (si->currentobject)
    si->cursor_appending = FALSE;
  else
    si->cursor_appending = TRUE;
  si->cursoroffend = FALSE;
}

/**
 * Push the score to the left off the 
 * displayed portion
 */
void
nudgerightward (DenemoProject * gui)
{
  if (set_rightmeasurenum (gui->movement) || (gui->movement->currentmeasurenum > gui->movement->rightmeasurenum))
    {
      if (gui->movement->currentmeasurenum > gui->movement->rightmeasurenum)
        {
          while (gui->movement->currentmeasurenum > gui->movement->rightmeasurenum)
            {
              gui->movement->leftmeasurenum++;
              set_rightmeasurenum (gui->movement);
            }

        }
      find_leftmost_allcontexts (gui->movement);
      update_hscrollbar (gui);
    }
}

/**
 * Push the score upwards off the displayed
 * portion
 */
void
nudge_downward (DenemoProject * gui)
{
  set_bottom_staff (gui);

  while (gui->movement->currentstaffnum > gui->movement->bottom_staff)
    {
      gui->movement->top_staff++;
      set_bottom_staff (gui);
    }
  update_vscrollbar (gui);
}

/**
 * Set the width available for drawing the measures of the movements from the width of the working scorearea widget
 */
void
set_width_to_work_with (DenemoProject * gui)
{
  if(Denemo.non_interactive)
    return;
  GList *g;

  for (g = gui->movements; g; g = g->next)
    {
      DenemoMovement *si = ((DenemoMovement *) g->data);

#if 0
      si->widthtoworkwith = (double) (Denemo.scorearea->allocation.width * ((int) (1 / si->system_height)) / si->zoom - (RIGHT_MARGIN + KEY_MARGIN + si->maxkeywidth + SPACE_FOR_TIME));
#else
      //the total line length for drawing DenemoObjects onto the screen
      // this length will be divided amongst the systems (line).
      // This length is in "pixels", the Denemo unit of display, which corresponds to a screen pixel when zoom ==1.0
      si->widthtoworkwith = (gint) ((get_widget_width (Denemo.scorearea) / si->zoom - (RIGHT_MARGIN + KEY_MARGIN + si->maxkeywidth + SPACE_FOR_TIME)) * ((int) (1 / si->system_height)));
#endif


      //g_debug("Width %d from num systems%d\n", si->widthtoworkwith, ((int)(1/si->system_height )));
    }
}

/**
 * Change the basic width of each measure
 */
void
adjustmeasurewidth (DenemoMovement * si, gint amount)
{
  si->measurewidth += amount;
  if (si->measurewidth < 10)
    si->measurewidth = 10;
  if (si->widthtoworkwith < si->measurewidth + SPACE_FOR_BARLINE)
    si->measurewidth = si->widthtoworkwith - SPACE_FOR_BARLINE;
  find_xes_in_all_measures (si);
  displayhelper (Denemo.project);
  score_status(Denemo.project, TRUE);
  /*nudgerightward (si); */
}

/**
 * Determines whether the closer jump will be up or down 
 */
gint
jumpcursor (gint cursor_y, gint fromnote, gint tonote)
{
  int distance;

  distance = (tonote - fromnote + 7) % 7;
  if (distance <= 3)            /* an upward jump is good */
    return cursor_y + distance;
  else                          /* jump down */
    return cursor_y - 7 + distance;
}

/**
 * Reset the cursor stats: sets currentobject to the si->cursor_x'th object in currentmeasure, if no such object sets cursor_appending. NOTE does not set cursor appending false if there is an object... 
 * @param si pointer to the scoreinfo structure
 * @return none
 */
static void
reset_cursor_stats (DenemoMovement * si)
{
  si->currentobject = g_list_nth ((objnode *) si->currentmeasure->data, si->cursor_x);
  if (!si->currentobject)
    {
      si->currentobject = g_list_last ((objnode *) si->currentmeasure->data);
      si->cursor_appending = TRUE;
    }
}

/**
 *  General function for inserting a DenemoObject
 *  into the score
 * the object is inserted at position gui->movement->cursor_x in the list of objects (counting from 0)
 * gui->movement->cursor_x is incremented
 * gui->movement->currentobject is set to the object at the new cursor_x position, unless this is too large, or we have cursor_appending in which case it is set to the last object.
 */
void
object_insert (DenemoProject * gui, DenemoObject * mudela_obj_new)
{
  DenemoMovement *si = gui->movement;

  /* update undo information */
  DenemoUndoData *undo;
  if (!si->undo_guard)
    {
      undo = (DenemoUndoData *) g_malloc (sizeof (DenemoUndoData));
      // should not be needed, we are inserting the object undo->object = dnm_clone_object (mudela_obj_new);
      //do position after inserting, so we can go back to it to delete
    }

  si->currentmeasure->data = g_list_insert ((objnode *) si->currentmeasure->data, mudela_obj_new, si->cursor_x);

  if (mudela_obj_new->type == CLEF)
    {
      reset_cursor_stats (si);
      staff_fix_note_heights ((DenemoStaff *) si->currentstaff->data);
      staff_beams_and_stems_dirs ((DenemoStaff *) si->currentstaff->data);
      find_xes_in_all_measures (si);
    }


  if (!si->undo_guard)
    {
      get_position (si, &undo->position);
      undo->position.appending = 0;
      undo->action = ACTION_INSERT;
      update_undo_info (si, undo);
    }

  si->cursor_x++;
  if (si->cursor_appending)
    si->currentobject = g_list_last ((objnode *) si->currentmeasure->data);
  else
    si->currentobject = g_list_nth ((objnode *) si->currentmeasure->data, si->cursor_x);

  if (si->currentobject == NULL)
    {
      g_warning ("problematic parameters on insert %d out of %d objects", si->cursor_x + 1, g_list_length ((objnode *) si->currentmeasure->data));
      si->cursor_x--;
      si->currentobject = g_list_nth ((objnode *) si->currentmeasure->data, si->cursor_x);
    }

  //g_debug("object insert appending %d cursor_x %d length %d\n", si->cursor_appending, si->cursor_x, g_list_length(si->currentmeasure->data));

  score_status (gui, TRUE);
  si->markstaffnum = 0;
  displayhelper (Denemo.project);
  score_status(Denemo.project, TRUE);
}

/**
 *  Change the y position of each staff
 */
void
adjuststaffheight (DenemoMovement * si, gint amount)
{
  si->staffspace += amount;
  if (si->staffspace < 2 * STAFF_HEIGHT)
    si->staffspace = 2 * STAFF_HEIGHT;
  displayhelper (Denemo.project);
  score_status(Denemo.project, TRUE);
  /*nudge_downward (si); */
}

/**
 * Move an entire measure to the left
 *
 */
static void
gomeasureleft (DenemoScriptParam * param, gboolean extend_selection)
{
  DenemoProject *gui = Denemo.project;
  DenemoMovement *si = gui->movement;
  DenemoScriptParam dummy;
  if (param == NULL)
    param = &dummy;
  param->status = FALSE;
  if (extend_selection && !si->markstaffnum)
    set_mark (NULL, NULL);
  if (gui->movement->currentmeasure->prev)
    {
      gui->movement->currentmeasurenum--;
      if (!gui->movement->playingnow) //during playback cursor moves should not affect viewport
        isoffleftside (gui);
      param->status = TRUE;
      write_status (gui);
    }
  setcurrents (gui->movement);
  if (extend_selection)
    calcmarkboundaries (gui->movement);
  if(!Denemo.non_interactive)
    gtk_widget_queue_draw(Denemo.scorearea);
}

/**
 * Move an entire measure to the left
 *
 */
void
measureleft (GtkAction* action, DenemoScriptParam * param)
{
  gomeasureleft (param, TRUE);
}

/**
 * Move an entire measure to the right
 *
 */
static void
gomeasureright (DenemoScriptParam * param, gboolean extend_selection)
{
  DenemoProject *gui = Denemo.project;
  DenemoMovement *si = gui->movement;
  DenemoScriptParam dummy;
  if (param == NULL)
    param = &dummy;
  param->status = FALSE;
  if (extend_selection && !si->markstaffnum)
    set_mark (NULL, NULL);
  if (gui->movement->currentmeasure->next)
    {
      gui->movement->currentmeasurenum++;
      if (!gui->movement->playingnow) //during playback cursor moves should not affect viewport
        isoffrightside (gui);
      setcurrents (gui->movement); 
      param->status = TRUE;
      if (extend_selection)
        calcmarkboundaries (gui->movement);
            write_status (gui);
    }
  if(!Denemo.non_interactive)
    gtk_widget_queue_draw(Denemo.scorearea);
}

void
measureright (GtkAction* action, DenemoScriptParam * param)
{
  gomeasureright (param, TRUE);
}

void
movetomeasureright (GtkAction* action, DenemoScriptParam * param)
{
  gomeasureright (param, FALSE);
}

void
movetomeasureleft (GtkAction* action, DenemoScriptParam * param)
{
  gomeasureleft (param, FALSE);
}


/**
 * swap the current movement for the previous one in the list of movements
 * return TRUE if movements are swapped
 */
gboolean
swapmovements (G_GNUC_UNUSED GtkAction * action, G_GNUC_UNUSED DenemoScriptParam * param)
{
  DenemoProject *gui = Denemo.project;
  (void) signal_structural_change (gui);
  GList *this = g_list_find (gui->movements, gui->movement);
  if (this->prev)
    {
      GList *prev = this->prev;
      GList *prevv = prev->prev;
      GList *next = this->next;
      if (next)
        next->prev = prev;
      if (prevv)
        prevv->next = this;
      else
        gui->movements = this;
      this->next = prev;
      this->prev = prevv;
      prev->next = next;
      prev->prev = this;
      set_movement_selector (gui);
      gchar *str = g_strdup_printf (_("This movement is now number %d in the score"), 1 + g_list_index (gui->movements, gui->movement));
      infodialog (str);
      g_free (str);
      return TRUE;
    }
  else
    warningdialog (_("There is no previous movement to swap with"));
  return FALSE;
}

/**
 * 
 *
 */
gboolean
swapstaffs (G_GNUC_UNUSED GtkAction * action, G_GNUC_UNUSED DenemoScriptParam * param)
{
  DenemoProject *gui = Denemo.project;
  (void) signal_structural_change (gui);
  if (gui->movement->currentstaff && gui->movement->currentstaff->prev)
    {
      DenemoStaff *temp;
      //if this is a staff with no voices extra voices on it then swap
      if (((DenemoStaff *) gui->movement->currentstaff->data)->voicecontrol == DENEMO_PRIMARY && ((gui->movement->currentstaff->next == NULL) || !(((DenemoStaff *) gui->movement->currentstaff->next->data)->voicecontrol & DENEMO_SECONDARY)))
        {
          temp = gui->movement->currentstaff->data;
          if (temp->context == DENEMO_NONE || confirm (_("A context is set on this staff"), _("You will need to alter the staff → properties → context of this and the previous staff; Proceed?")))
            {
              take_snapshot ();
              gui->movement->currentstaff->data = gui->movement->currentstaff->prev->data;
              gui->movement->currentstaff->prev->data = temp;
              gui->movement->currentstaffnum--;
              gui->movement->currentstaff = gui->movement->currentstaff->prev;
              staff_set_current_primary (gui->movement);
              setcurrents (gui->movement);
              move_viewport_up (gui);
              score_status (gui, TRUE);
              displayhelper (gui);
              return TRUE;
            }
        }
      else
        warningdialog (_("Split off voices from this staff first"));
    }
  else
    warningdialog (_("There is no previous staff to swap with"));
  return FALSE;
}

/**
 * 
 *
 */
gboolean
splitstaffs (G_GNUC_UNUSED GtkAction * action, G_GNUC_UNUSED DenemoScriptParam * param)
{
  DenemoProject *gui = Denemo.project;

  if (gui->movement->currentstaff && gui->movement->currentstaff->next)
    {
      take_snapshot ();
      DenemoStaff *thestaff = (DenemoStaff *) gui->movement->currentstaff->data;
      DenemoStaff *nextstaff = (DenemoStaff *) gui->movement->currentstaff->next->data;
      if ((thestaff->voicecontrol & DENEMO_PRIMARY) && (nextstaff->voicecontrol == DENEMO_SECONDARY))
        nextstaff->voicecontrol = DENEMO_SECONDARY | DENEMO_PRIMARY;
      else
        warningdialog (_("There is no voice below this one on this staff"));
      staff_set_current_primary (gui->movement);
      setcurrents (gui->movement);
      move_viewport_up (gui);
      score_status (gui, TRUE);
      displayhelper (gui);
      return TRUE;
    }
  else
    warningdialog (_("There is no voice below this one to split from"));
  return FALSE;
}


/**
 * 
 *
 */
gboolean
joinstaffs (G_GNUC_UNUSED GtkAction * action, G_GNUC_UNUSED DenemoScriptParam * param)
{
  DenemoProject *gui = Denemo.project;

  if (gui->movement->currentstaff && gui->movement->currentstaff->prev)
    {
      take_snapshot ();
      DenemoStaff *thestaff = (DenemoStaff *) gui->movement->currentstaff->data;
      thestaff->voicecontrol = DENEMO_SECONDARY;
      staff_set_current_primary (gui->movement);
      setcurrents (gui->movement);
      move_viewport_up (gui);
      score_status (gui, TRUE);
      displayhelper (gui);
      return TRUE;
    }
  else
    warningdialog (_("There is no staff above to move this staff into"));
  return FALSE;
}


/**
 * Move si->currentstaff up an one voice, return TRUE if successful
 * param is NULL for interactive calls, otherwise status is returned in param->status;
 * alter selection if extend_selection
 */
static gboolean
govoiceup (DenemoScriptParam * param, gboolean extend_selection)
{
  DenemoProject *gui = Denemo.project;
  DenemoMovement *si = gui->movement;
  DenemoScriptParam dummy;
  if (param == NULL)
    param = &dummy;
  param->status = FALSE;
  if (!gui->movement->currentstaff)
    return param->status = FALSE;//should never happen
  if (extend_selection && !si->markstaffnum)
    set_mark (NULL, NULL);
  if (gui->movement->currentstaff && (((DenemoStaff *) (gui->movement->currentstaff->data))->voicecontrol & DENEMO_SECONDARY))
    {
      hide_lyrics ();
      gui->movement->currentstaffnum--;
      gui->movement->currentstaff = gui->movement->currentstaff->prev;
      staff_set_current_primary (gui->movement);
      setcurrents (gui->movement);
      show_lyrics ();
      move_viewport_down (gui);
      set_cursor_transition ();
      param->status = TRUE;
    }
  else if (param == &dummy)     //is interactive
    warningmessage (_("This is the first voice"));
  write_status(gui);
  if(!Denemo.non_interactive)
    gtk_widget_queue_draw(Denemo.scorearea);
  return param->status;
}

/**
 * Move si->currentstaff up an one staff, not skipping voices unless displayed on same staff, return TRUE if successful
 * alter selection if extend_selection
 */
static gboolean
gostaffup (DenemoScriptParam * param, gboolean extend_selection)
{
  DenemoProject *gui = Denemo.project;
  DenemoMovement *si = gui->movement;
  DenemoScriptParam dummy;
  if (param == NULL)
    param = &dummy;
  param->status = FALSE;
  if (!gui->movement->currentstaff)
    return param->status = FALSE;//should never happen
  if (extend_selection && !si->markstaffnum)
    set_mark (NULL, NULL);
  while ((((DenemoStaff *) (gui->movement->currentstaff->data))->voicecontrol == DENEMO_SECONDARY) &&
    govoiceup (param, extend_selection))
      ;/* do nothing */
  if (gui->movement->currentstaff->prev)
    {
      hide_lyrics ();
      gui->movement->currentstaffnum--;
      gui->movement->currentstaff = gui->movement->currentstaff->prev;
      staff_set_current_primary (gui->movement);
      setcurrents (gui->movement);
      if (extend_selection)
        calcmarkboundaries (gui->movement);
      show_lyrics ();
      find_leftmost_allcontexts (si);
      update_drawing_cache ();;
      move_viewport_up (gui);
      set_cursor_transition ();
      param->status = TRUE;
    }
  else if (param == &dummy)     //is interactive
    warningmessage (_("This is the first staff"));
  write_status(gui);
  if(!Denemo.non_interactive)
    gtk_widget_queue_draw(Denemo.scorearea);
  return param->status;
}


/**
 * Move si->currentstaff down one voice, skipping voices, return TRUE if successful
 * alter selection if extend_selection
 */
static gboolean
govoicedown (DenemoScriptParam * param, gboolean extend_selection)
{
  DenemoProject *gui = Denemo.project;
  DenemoMovement *si = gui->movement;
  DenemoScriptParam dummy;
  if (param == NULL)
    param = &dummy;
  param->status = FALSE;
  if (!gui->movement->currentstaff)
    return param->status = FALSE;
  if (extend_selection && !si->markstaffnum)
    set_mark (NULL, NULL);
  if (gui->movement->currentstaff->next && ((DenemoStaff *) (gui->movement->currentstaff->next->data))->voicecontrol & DENEMO_SECONDARY)
    {
      hide_lyrics ();
      gui->movement->currentstaffnum++;
      gui->movement->currentstaff = gui->movement->currentstaff->next;
      staff_set_current_primary (gui->movement);
      setcurrents (gui->movement);
      if (extend_selection)
        calcmarkboundaries (gui->movement);
      show_lyrics ();
      move_viewport_down (gui);
      set_cursor_transition ();
      param->status = TRUE;
    }
  else if (param == &dummy)     //is interactive
    warningmessage (_("This is the last voice"));
  write_status(gui);
  if(!Denemo.non_interactive)
    gtk_widget_queue_draw(Denemo.scorearea);
  return param->status;
}

gboolean
movetovoicedown (GtkAction* action, DenemoScriptParam * param)
{

  return govoicedown (param, FALSE);
}

gboolean
voicedown (GtkAction* action, DenemoScriptParam * param)
{

  return govoicedown (param, TRUE);
}

gboolean
movetovoiceup (GtkAction* action, DenemoScriptParam * param)
{

  return govoiceup (param, FALSE);
}

gboolean
voiceup (GtkAction* action, DenemoScriptParam * param)
{

  return govoiceup (param, TRUE);
}



/**
 * Move si->currentstaff down one staff/voice,  not skipping voices unless displayed on same staff, return TRUE if successful
 * alter selection if extend_selection
 */
static gboolean
gostaffdown (DenemoScriptParam * param, gboolean extend_selection)
{
  DenemoProject *gui = Denemo.project;
  DenemoMovement *si = gui->movement;
  DenemoScriptParam dummy;
  if (param == NULL)
    param = &dummy;
  param->status = FALSE;

  if (!gui->movement->currentstaff)
    return param->status = FALSE;
  if (extend_selection && !si->markstaffnum)
    set_mark (NULL, NULL);
  while (gui->movement->currentstaff->next && (((DenemoStaff *) (gui->movement->currentstaff->next->data))->voicecontrol == DENEMO_SECONDARY)
    && govoicedown (param, extend_selection))
      ;     /* do nothing */
  if (gui->movement->currentstaff->next)
    {
      hide_lyrics ();
      gui->movement->currentstaffnum++;
      gui->movement->currentstaff = gui->movement->currentstaff->next;
      staff_set_current_primary (gui->movement);
      setcurrents (gui->movement);
      if (extend_selection)
        calcmarkboundaries (gui->movement);
      show_lyrics ();
      find_leftmost_allcontexts (si);

      update_drawing_cache ();
      move_viewport_down (gui);
      set_cursor_transition ();
    
      param->status = TRUE;
    }
  else if (param == &dummy)     //is interactive
    warningmessage (_("This is the last staff"));
  write_status(gui);
  if(!Denemo.non_interactive)
    gtk_widget_queue_draw(Denemo.scorearea);
  return param->status;
}

gboolean
movetostaffdown (GtkAction* action, DenemoScriptParam * param)
{

  return gostaffdown (param, FALSE);
}

gboolean
staffdown (GtkAction* action, DenemoScriptParam * param)
{

  return gostaffdown (param, TRUE);
}

gboolean
movetostaffup (GtkAction* action, DenemoScriptParam * param)
{

  return gostaffup (param, FALSE);
}

gboolean
staffup (GtkAction* action, DenemoScriptParam * param)
{

  return gostaffup (param, TRUE);
}



/**
 * move the cursor one position to the left
 *
 */
gboolean
move_left (DenemoScriptParam * param, gboolean extend_selection)
{
  DenemoProject *gui = Denemo.project;
  DenemoMovement *si = gui->movement;
  DenemoScriptParam dummy;
  if (param == NULL)
    param = &dummy;
  param->status = FALSE;
  if (extend_selection && !si->markstaffnum)
    set_mark (NULL, NULL);
  g_debug ("cursorleft: cursorpos %d\n", si->cursor_x);
  if (!si->cursor_x)
    {
      /* also the only situation where si->currentobject == NULL */
      if (si->currentmeasure->prev)
        {
          g_debug ("Currentmeasure prev == TRUE");
          /* Go to end of preceding measure */
          si->cursor_appending = TRUE;
          si->currentmeasure = si->currentmeasure->prev;
          si->currentmeasurenum--;
          if (!si->playingnow)  //during playback cursor moves should not affect viewport
            isoffleftside (gui);
          si->currentobject = g_list_last ((objnode *) si->currentmeasure->data);
          /* The preceding statement will set currentobject to
           * NULL if appropriate */
          si->cursor_x = g_list_length ((objnode *) si->currentmeasure->data);
          /* Despite appearances, there is not an off-by-one error in the
           * preceding command */
          param->status = TRUE;
        }
    }
  else if (si->cursor_appending)
    {
      /* Can back off from appending */
      si->cursor_appending = FALSE;
      si->cursor_x--;
      param->status = TRUE;
    }
  else
    {
      /* Can go back in the measure */
      if (si->currentobject && si->currentobject->prev)
        {
          si->currentobject = si->currentobject->prev;
          si->cursor_x--;
          param->status = TRUE;
        }
    }
  if (extend_selection)
    calcmarkboundaries (si);
  write_status (gui);
  if(!Denemo.non_interactive)
    gtk_widget_queue_draw(Denemo.scorearea);
  return param->status;
}

/**
 * move the cursor one position to the right
 * selection (if any) is extended if extend_selection is TRUE
 * sets param->status TRUE if cursor could still move right, and returns that value.
 */
gboolean
move_right (DenemoScriptParam * param, gboolean extend_selection)
{
  DenemoProject *gui = Denemo.project;
  DenemoMovement *si = gui->movement;
  DenemoScriptParam dummy;
  if (param == NULL)
    param = &dummy;
  param->status = FALSE;
  if (extend_selection && !si->markstaffnum)
    set_mark (NULL, NULL);
  if (si->cursor_appending && si->currentmeasure->next)
    {
      /* Go to the next measure */
      si->currentmeasure = si->currentmeasure->next;
      si->currentmeasurenum++;
      if (!si->playingnow)      //during playback cursor moves should not affect viewport
        isoffrightside (gui);
      si->currentobject = (objnode *) si->currentmeasure->data;
      si->cursor_x = 0;
      if (si->currentobject)
        si->cursor_appending = FALSE;
    }
  else if (si->currentobject)
    {
      /* See if we should go to appending position. If not, go to the
       * next note (if possible) */
      if (!si->cursor_appending && !si->currentobject->next)
        {
          /* Go to appending position */
          si->cursor_appending = TRUE;
          si->cursor_x++;
        }
      else if (si->currentobject->next)
        {
          si->currentobject = si->currentobject->next;
          si->cursor_x++;
        }
      else
        return param->status = FALSE;
    }
  if (extend_selection)
    calcmarkboundaries (si);
  write_status (gui);
  if(!Denemo.non_interactive)
    gtk_widget_queue_draw(Denemo.scorearea);
  return (param->status = (si->currentobject || (!si->cursor_appending) || si->currentmeasure->next));
}

/**
 * move the cursor one position to the right extending the selection if any
 * sets param->status TRUE if cursor could still move right, and returns that value.
 */
gboolean
cursorright (GtkAction* action, DenemoScriptParam * param)
{
  return move_right (param, TRUE);
}

/**
 * move the cursor one position to the left extending the selection if any
 * sets param->status TRUE if cursor could still move left, and returns that value.
 */
gboolean
cursorleft (GtkAction* action, DenemoScriptParam * param)
{
  return move_left (param, TRUE);
}

// cursor move without altering selection
gboolean
movecursorright (GtkAction* action, DenemoScriptParam * param)
{
  return move_right (param, FALSE);
}

gboolean
movecursorleft (GtkAction* action, DenemoScriptParam * param)
{
  return move_left (param, FALSE);
}



// moves the cursor in the direction indicated, observing within_measure and if stopping stopping at empty measures

static gboolean
to_object_direction (gboolean within_measure, gboolean right, gboolean stopping)
{
  if (!Denemo.project || !(Denemo.project->movement))
    return FALSE;
  GList *start_obj = Denemo.project->movement->currentobject;
  GList *start_measure = Denemo.project->movement->currentmeasure;
  gboolean was_appending = Denemo.project->movement->cursor_appending;
  if (start_obj && Denemo.project->movement->cursor_appending)
    movecursorleft (NULL, NULL);
  if (start_obj == NULL)
    {
      if (within_measure)
        return FALSE;
      // start object is NULL, not restricted to current measure
      if (right)
        {
          if (start_measure->next)
            {
              movetomeasureright (NULL, NULL);
              if (Denemo.project->movement->currentobject)
                return TRUE;
              else if (stopping)
                return FALSE;
              else
                return to_object_direction (within_measure, right, stopping);
            }
          else
            return FALSE;
        }
      // going left, start object is NULL, not restricted to current measure, going previous
      if (start_measure->prev)
        {
          movecursorleft (NULL, NULL);
          if (Denemo.project->movement->currentobject == NULL){
            if (stopping)
              return FALSE;
            else
              return to_object_direction (within_measure, right, stopping);
          }
          movecursorleft (NULL, NULL);
          return TRUE;
        }
      return FALSE;
    }
  //start object is not NULL
  if (within_measure)
    {
      if (right)
        {
          if (start_obj->next)
            {
              movecursorright (NULL, NULL);
              return TRUE;
            }
          if (was_appending)
            movecursorright (NULL, NULL);
          return FALSE;
        }
      //left
      if (start_obj->prev == NULL)
        return FALSE;
    }
  //not restricted to this measure
  if (right)
    {
      if (start_obj->next)
        {
          movecursorright (NULL, NULL);
          return TRUE;
        }
      if (start_measure->next)
        {
          movetomeasureright (NULL, NULL);
          if (Denemo.project->movement->currentobject == NULL){
            if (stopping)
              return FALSE;
            else
              return to_object_direction (within_measure, right, stopping);
          }
          return TRUE;
        }
      if (was_appending)
        movecursorright (NULL, NULL);
      return FALSE;
    }
  //left
  if (start_obj->prev)
    {
      movecursorleft (NULL, NULL);
      return TRUE;
    }
  if (start_measure->prev)
    {
      movecursorleft (NULL, NULL);
      if (Denemo.project->movement->currentobject == NULL){
        if (stopping)
          return FALSE;
        else
          return to_object_direction (within_measure, right, stopping);
      }
      movecursorleft (NULL, NULL);
      return TRUE;
    }
  return FALSE;
}

static gboolean
to_standalone_directive_direction (gboolean right)
{
  gboolean ret = to_object_direction (FALSE, right, FALSE);
  if (!ret)
    return ret;
    write_status(Denemo.project);
  if (Denemo.project->movement->currentobject && Denemo.project->movement->currentobject->data && ((DenemoObject *) Denemo.project->movement->currentobject->data)->type == LILYDIRECTIVE)
    return TRUE;
  else
    return to_standalone_directive_direction (right);
}

/* moves currentobject to object in the selection in the direction indicated by right.
   Steps over barlines (i.e. cursor_appending).
 returns TRUE if currentobject is different after than before the call
*/
static gboolean
to_selected_object_direction (gboolean right)
{
  if (!Denemo.project || !(Denemo.project->movement))
    return FALSE;
  gboolean success = to_object_direction (FALSE, right, FALSE);
  if (!success)
    success = to_object_direction (FALSE, right, FALSE);
  write_status(Denemo.project);
  if ((success) && in_selection (Denemo.project->movement))
    return TRUE;
  if (success)
    to_object_direction (FALSE, !right, FALSE);
  return FALSE;
}

static gboolean
to_chord_direction (gboolean right, gboolean stopping)
{
  gboolean ret = to_object_direction (FALSE, right, stopping);
  if (!ret)
    return ret;
  write_status(Denemo.project);
  if (Denemo.project->movement->currentobject && Denemo.project->movement->currentobject->data && ((DenemoObject *) Denemo.project->movement->currentobject->data)->type == CHORD)
    return TRUE;
  else
    return to_chord_direction (right, stopping);
}

static gboolean
to_chord_direction_in_measure (gboolean right)
{
  gboolean ret = to_object_direction (TRUE, right, TRUE);
  if (!ret)
    return ret;
  write_status(Denemo.project);
  if (Denemo.project->movement->currentobject && Denemo.project->movement->currentobject->data && ((DenemoObject *) Denemo.project->movement->currentobject->data)->type == CHORD)
    return TRUE;
  else
    return to_chord_direction_in_measure (right);
}

static gboolean
to_standalone_direction_in_measure (gboolean right)
{
  gboolean ret = to_object_direction (TRUE, right, TRUE);
  if (!ret)
    return ret;
  write_status(Denemo.project);
  if (Denemo.project->movement->currentobject && Denemo.project->movement->currentobject->data && ((DenemoObject *) Denemo.project->movement->currentobject->data)->type == LILYDIRECTIVE)
    return TRUE;
  else
    return to_standalone_direction_in_measure (right);
}

 // there is a significant problem with the concept of next note in a chord of several notes. We have no way of iterating over the notes of a chord
  // since the notes may be altered during the iteration and Denemo does not define a "currentnote"
  //but we can define cursor_to_top_note() and cursor_next_note_down() the latter moving to the next note below the cursor position (if any).
  
gboolean cursor_to_nth_note_height(gint n) {
    DenemoProject *gui = Denemo.project;
  DenemoObject *curObj;
  chord *thechord;
  note *thenote;
  if (!Denemo.project ||
   !(Denemo.project->movement) ||
    !(Denemo.project->movement->currentobject) ||
     !(curObj = Denemo.project->movement->currentobject->data) ||
      (curObj->type != CHORD) ||
       !(thechord = (chord *) curObj->object) ||
        !(thechord->notes) ||
         !(thenote = (note *) thechord->notes->data))
    return FALSE;
    if(n >= g_list_length (thechord->notes))
        return FALSE;
    thenote = g_list_nth (thechord->notes, n)->data;
    gint mid_c_offset = thenote->mid_c_offset;
    //g_print("Mid c offset required %d\n", mid_c_offset);
    //g_print ("Currently gui->movement->cursor_y = %d\n", gui->movement->cursor_y);
    if (gui->movement->cursor_y < mid_c_offset)
        while (gui->movement->cursor_y < mid_c_offset)
            cursorup (NULL, NULL);
    else
         while (gui->movement->cursor_y > mid_c_offset)
            cursordown (NULL, NULL);
    return (gui->movement->cursor_y == mid_c_offset);
}

gboolean cursor_to_next_note_height(void) {
  GList *thenotes;
  DenemoProject *gui = Denemo.project;
  DenemoObject *curObj;
  chord *thechord;
  note *thenote;
  gint mid_c_offset; 
  if (!Denemo.project ||
   !(Denemo.project->movement) ||
    !(Denemo.project->movement->currentobject) ||
     !(curObj = Denemo.project->movement->currentobject->data) ||
      (curObj->type != CHORD) ||
       !(thechord = (chord *) curObj->object) ||
        !(thenotes = thechord->notes) ||
         !(thenote = (note *) thenotes->data))
    return FALSE;
   
    cursorup (NULL, NULL); //seek *above* the current cursor position only
    for(;thenotes;thenotes=thenotes->next)
        {  
            thenote = (note *) thenotes->data;
            mid_c_offset = thenote->mid_c_offset;// g_print ("Currently gui->movement->cursor_y = %d considering %d\n", gui->movement->cursor_y, mid_c_offset);
            while (gui->movement->cursor_y < mid_c_offset)  
                cursorup (NULL, NULL);
            if (gui->movement->cursor_y == mid_c_offset)
                break;     
        }
    return (gui->movement->cursor_y == mid_c_offset);
}  
  
  
//This next note is next chord that is not a rest in the given direction.
static gboolean
to_note_direction (gboolean right, gboolean stopping)
{
  gboolean ret = to_chord_direction (right, stopping);
  if (!ret)
    return ret;
  write_status(Denemo.project);
  if (Denemo.project->movement->currentobject && Denemo.project->movement->currentobject->data && ((DenemoObject *) Denemo.project->movement->currentobject->data)->type == CHORD && ((((chord *) (((DenemoObject *) Denemo.project->movement->currentobject->data)->object))->notes)) && (!Denemo.project->movement->cursor_appending))
    return TRUE;
  else
    return to_note_direction (right, stopping);
}

/******** advances the cursor to the next note,  stopping
 at empty measures. The cursor is left after last note if no more notes */
gboolean
next_editable_note (void)
{
  gboolean ret = to_note_direction (TRUE, TRUE);
  if ((!ret) && Denemo.project->movement->currentobject == NULL)
    {
      to_note_direction (FALSE, TRUE);
    }
  if (!ret)
    movecursorright (NULL, NULL);
  else
    write_status(Denemo.project);
  return ret;
}

/******** advances the cursor to the next note,  stopping
 at empty measures and at appending position if any. The cursor is left after last note if no more notes */
gboolean
next_insert_or_editable_note (void)
{
 gboolean ret;
  if (Denemo.project->movement->currentobject && (Denemo.project->movement->currentobject->next == NULL))
    {
    ret = FALSE;
    }
  else 
    {
  
    ret = to_note_direction (TRUE, TRUE);
    if ((!ret) && Denemo.project->movement->currentobject == NULL)
        {
          to_note_direction (FALSE, TRUE);
        }
    }
  if (!ret)
    {
        movecursorright (NULL, NULL);
        if (Denemo.project->movement->currentobject == NULL)
            movecursorleft (NULL, NULL);
    }
  else
    write_status(Denemo.project);
  return ret;
}
gboolean
cursor_to_next_object (gboolean within_measure, gboolean stopping)
{

  return to_object_direction (within_measure, TRUE, stopping);
}

gboolean
cursor_to_prev_object (gboolean within_measure, gboolean stopping)
{

  return to_object_direction (within_measure, FALSE, stopping);
}


gboolean
cursor_to_next_selected_object (void)
{
  return to_selected_object_direction (TRUE);
}

gboolean
cursor_to_prev_selected_object (void)
{
  return to_selected_object_direction (FALSE);
}

gboolean
cursor_to_next_standalone_directive (void)
{
  return to_standalone_directive_direction (TRUE);
}

gboolean
cursor_to_prev_standalone_directive (void)
{
  return to_standalone_directive_direction (FALSE);
}

gboolean
cursor_to_next_standalone_in_measure (void)
{
  return to_standalone_direction_in_measure (TRUE);
}

gboolean
cursor_to_prev_standalone_in_measure (void)
{
  return to_standalone_direction_in_measure (FALSE);
}

gboolean
cursor_to_next_chord (void)
{
  return to_chord_direction (TRUE, FALSE);
}

gboolean
cursor_to_prev_chord (void)
{
  return to_chord_direction (FALSE, FALSE);
}

gboolean
cursor_to_next_chord_in_measure (void)
{
  return to_chord_direction_in_measure (TRUE);
}

gboolean
cursor_to_prev_chord_in_measure (void)
{
  return to_chord_direction_in_measure (FALSE);
}
//Badly named - it is a next chord that is not a rest
gboolean
cursor_to_next_note (void)
{
  return to_note_direction (TRUE, FALSE);
}
//Badly named - it is a prev chord that is not a rest
gboolean
cursor_to_prev_note (void)
{
  return to_note_direction (FALSE, FALSE);
}

/**
 * Move the cursor up one diatonic step 
 */
void
cursorup (GtkAction* action, DenemoScriptParam * param)
{
  DenemoProject *gui = Denemo.project;
  DenemoScriptParam dummy;
  if (param == NULL)
    param = &dummy;
  param->status = FALSE;
  gui->movement->cursor_y++;
  gui->movement->staffletter_y = (gui->movement->staffletter_y + 1) % 7;
  param->status = TRUE;         //FIXME introduce some range boundaries, settable by user for instrument ranges.
  //g_debug ("Cursor Y Position %d\n", gui->movement->cursor_y);
  if(!Denemo.non_interactive)
    gtk_widget_queue_draw(Denemo.scorearea);
}

/**
 * Move the cursor down one diatonic step 
 */
void
cursordown (GtkAction* action, DenemoScriptParam * param)
{
  DenemoProject *gui = Denemo.project;
  DenemoScriptParam dummy;
  if (param == NULL)
    param = &dummy;
  param->status = FALSE;

  gui->movement->cursor_y--;
  gui->movement->staffletter_y = (gui->movement->staffletter_y + 6) % 7;
  param->status = TRUE;         //FIXME introduce some range boundaries, settable by user for instrument ranges.
  //g_debug ("Cursor Y Position %d\n", gui->movement->cursor_y);
  if(!Denemo.non_interactive)
    gtk_widget_queue_draw(Denemo.scorearea);
}

static gboolean
prev_object_is_rhythm (DenemoProject * gui)
{
  if (gui->movement->currentobject == NULL)
    return FALSE;
  return ((DenemoObject *) (gui->movement->currentobject->data))->isinvisible;
}


/* insert a note into the score at the current cursor position following the current rhythm step */

void
insert_note_following_pattern (DenemoProject * gui)
{
#define g  (gui->rstep)
  if ((gui->mode & (INPUTEDIT | INPUTINSERT)) && g)
    {
      GList *h;
      gint mode = gui->mode;
      gui->mode = mode & ~INPUTRHYTHM;

      // if(gui->currhythm && gui->currhythm->data && ((RhythmPattern*)gui->currhythm->data)->clipboard) 
      if (gui->currhythm && gui->cstep)
        {
          //      g_debug("Have a clip\n");
          GList *objs;
          gboolean note_inserted = FALSE;
          for (objs = gui->cstep; objs; objs = objs->next)
            {
              if (((DenemoObject *) objs->data)->type == CHORD && (note_inserted))
                break;
              DenemoObject *clipobj = dnm_clone_object (objs->data);
              if ((((DenemoObject *) objs->data)->type == CHORD) && ((chord *) clipobj->object)->notes)
                {
                  chord *thechord = (chord *) clipobj->object;
                  note *thenote = (note *) (thechord->notes->data);
                  thenote->mid_c_offset = gui->movement->cursor_y;
                  thechord->lowesty = thechord->highesty = thenote->y = calculateheight (thenote->mid_c_offset, gui->movement->cursorclef);
                  thechord->lowestpitch = thechord->highestpitch = thechord->sum_mid_c_offset = thenote->mid_c_offset;
                  clipobj->isinvisible = FALSE;
                  note_inserted = TRUE;
                }

              g = g->next;
              insertion_point_for_type (gui->movement, ((DenemoObject *) objs->data)->type);

              insert_object (clipobj);
            }

          gui->cstep = (objs ? objs : (((RhythmPattern *) gui->currhythm->data)->clipboard)->data);
        }
      else
        {

          insertion_point (gui->movement);
          gui->movement->cursoroffend = FALSE;
          h = ((RhythmElement *) g->data)->functions;
#if GTK_MAJOR_VERSION==3
          ((GSourceFunc) h->data) (gui);
#else
          ((GtkFunction) h->data) (gui);
#endif
          displayhelper (gui);
        }

#define CURRP ((RhythmPattern *)gui->currhythm->data)
      if (((RhythmElement *) g->data)->icon)
        {                       /* singletons do not have icon */
          GtkWidget *label = LABEL (CURRP->button);
          //g_debug("Markup is %s\n", ((RhythmElement*)g->data)->icon);
          gtk_label_set_markup (GTK_LABEL (label), ((RhythmElement *) g->data)->icon);
        }
      gui->mode = mode;
      score_status (gui, TRUE);
    }
#undef CURRP
#undef g
}

/* get duration of next element in current rhythm pattern  */

gint
get_prevailing_duration (void)
{
  DenemoProject *gui = Denemo.project;
  gint duration = 0;
  if ((gui->mode & (INPUTEDIT | INPUTINSERT)) && gui->rstep)
    {
      if (gui->currhythm && gui->cstep)
        {
          GList *objs;
          for (objs = gui->cstep; objs; objs = objs->next)
            {
              if ((((DenemoObject *) objs->data)->type == CHORD))
                {
                  duration = ((chord *) ((DenemoObject *) objs->data)->object)->baseduration;
                  break;
                }
            }
        }
      else
        {
          for (duration = 0; duration < 7; duration++)
            if ((Denemo.project->prevailing_rhythm == Denemo.singleton_rhythms['0' + duration]) || (Denemo.project->prevailing_rhythm == Denemo.singleton_rhythms['r' + duration]))
              break;
        }
    }
  return duration;
}


/**
 * shiftcursor: FIXME change the name of this function!
 * Mode sensitive note actions:
 * In Classic mode: Move the cursor to a given note value nearest the current cursor
 * In Edit mode: Change the current note (or insert if none), if INPUTRHYTHM as well, move cursor to next note.
 * In Insert mode: Insert a note at the cursor
 */
void
shiftcursor (DenemoProject * gui, gint note_value)
{
  gint oldstaffletter_y = gui->movement->staffletter_y;
  gint oldcursor_y = gui->movement->cursor_y;
  gui->movement->staffletter_y = note_value;
  gui->movement->cursor_y = jumpcursor (gui->movement->cursor_y, oldstaffletter_y, gui->movement->staffletter_y);
  int mid_c_offset = gui->movement->cursor_y;

  /* in edit mode edit the current note name */
  if ((gui->mode & INPUTEDIT) && ((!gui->movement->cursor_appending) || prev_object_is_rhythm (gui)))
    {
      DenemoObject *theobj = (DenemoObject *) (gui->movement->currentobject->data);
      chord *thechord;
      if (theobj->type == CHORD && (thechord = (chord *) theobj->object)->notes)
        {
          store_for_undo_change (gui->movement, theobj);
          //turn off further storage of UNDO info while this takes place
          gui->movement->undo_guard++;
          theobj->isinvisible = FALSE;
          if (g_list_length (thechord->notes) > 1)
            {                   /* multi-note chord - remove and add a note */
              gui->movement->cursor_y = oldcursor_y;
              delete_chordnote (gui);
              gui->movement->cursor_y = mid_c_offset;
              insert_chordnote (gui);
            }
          else
            {                   /* single-note chord - change the note */
              gint dclef = find_prevailing_clef (gui->movement);
              if(!thechord->is_tied)
                modify_note (thechord, mid_c_offset, gui->movement->curmeasureaccs[note_value], dclef);
              else //if tied modify the tied note(s) too, FIXME but this breaks the UNDO mechanism, see store_for_undo_change (gui->movement, theobj) above 
                {
                  objnode *nextobj = gui->movement->currentobject;
                  measurenode *current = gui->movement->currentmeasure;
                  while (1)
                    {
                  
                      if(nextobj)
                        {
                          DenemoObject *thenextobj= (DenemoObject *) nextobj->data;
                              
                          if (thenextobj->type == CHORD)  
                            {
                                DenemoMovement *si = Denemo.project->movement;
                                chord *next = thenextobj->object;
                                modify_note (next, mid_c_offset, gui->movement->curmeasureaccs[note_value], dclef); 
                                calculatebeamsandstemdirs (current->data, &(si->curmeasureclef), &(si->cursortime1), &(si->cursortime2), &(si->curmeasure_stem_directive));
                                if(next->is_tied)
                                    {
                                        if(nextobj->next==NULL)
                                            {
                                              current = current->next;
                                              if(current && current->data)
                                                {
                                                   nextobj = current->data;
                                                   continue;
                                                }
                                            }
                                         nextobj = nextobj->next;
                                         if(nextobj)
                                            continue;
                                    }    
                            }
                        }
                    break;
                    }
                }
            }
          gui->movement->undo_guard--;
          score_status (gui, TRUE);
        }
    }
  else
    /* in INSERT (or EDIT and appending) we insert a note using the next step of the rhythm pattern */
    insert_note_following_pattern (gui);
  if(!Denemo.non_interactive)
    gtk_widget_queue_draw(Denemo.scorearea);
}


void
insert_rhythm_pattern (GtkAction* action, DenemoScriptParam* param)
{
  if (Denemo.project->currhythm == NULL)
    return;
  insert_clipboard (((RhythmPattern *) Denemo.project->currhythm->data)->clipboard);

}

void
insertion_point_for_type (DenemoMovement * si, DenemoObjType type)
{
  switch (type)
    {
    case TUPCLOSE:
      return;
    default:
      break;
    }
  insertion_point (si);
}


/**
 * insertion_point()
 * chooses/creates a good insertion point.
 * if the cursor is at the end of a full measure:
 *      creates a new measure and makes it the current one.
 * if the cursor is at the end of a full measure before an empty measure:
 *      it makes that empty measure current. 
 * 
 */
void
insertion_point (DenemoMovement * si)
{
  //gtk_widget_draw(Denemo.Denemo.scorearea, NULL);//FIXME efficiency????

  //update_drawing_cache ();;

  gboolean next_measure;

  /* First, check to see if the insertion'll cause the cursor to
   * jump to the next measure. (Denemo will implicitly create it
   * if it doesn't exist already.) */

 // next_measure = si->cursoroffend && si->cursor_appending && ( (!si->currentmeasure->next) || (!si->currentmeasure->next->data) ||
//                       ((((DenemoObject *)si->currentmeasure->next->data)->type == TIMESIG)   &&  ((si->currentmeasure->next->next==NULL) || (si->currentmeasure->next->next->data==NULL))));
  //g_debug ("next_measure %d\n", next_measure);
  next_measure = FALSE;
  if(si->cursoroffend && si->cursor_appending) {
     if ( (!si->currentmeasure->next) || (!si->currentmeasure->next->data))
      next_measure = TRUE;
     else 
        {
            objnode *objnode = si->currentmeasure->next->data;
            DenemoObject *obj = objnode->data;
            while (obj && (obj->type != CHORD)) 
                { 
                    objnode=objnode->next;obj = objnode?objnode->data:NULL;
                }
                
            if ((obj==NULL) || (obj->type != CHORD))
                next_measure = TRUE;         
        }
  }
  
  if (next_measure)
    {
      if (!si->currentmeasure->next)
        {
          gboolean all = TRUE;  //add to all measures
          //g_debug ("Appending a new measure\n");

          /* Add a measure and make it currentmeasure */
          if (!(all && si->currentstaff && g_list_length (((DenemoStaff *) si->currentstaff->data)->measures) == g_list_length (si->measurewidths)))
            all = FALSE;        // add only to current staff if it is shorter than some other staff
          si->currentmeasure = dnm_addmeasures (si, si->currentmeasurenum, 1, all);
        }
      else
        si->currentmeasure = si->currentmeasure->next;
        
        
        
        
      if (Denemo.project->mode & (INPUTRHYTHM))
        signal_measure_end ();
      /* Now the stuff that needs to be done for each case */
      si->currentmeasurenum++;
      si->currentobject = (objnode *) si->currentmeasure->data;
      si->cursor_x = 0;
      while(si->currentobject && (((DenemoObject *)si->currentobject->data)->type != CHORD))
        {
            si->currentobject = si->currentobject->next;
            si->cursor_x++;
        }
      memcpy (si->cursoraccs, si->nextmeasureaccs, SEVENGINTS);
      memcpy (si->curmeasureaccs, si->nextmeasureaccs, SEVENGINTS);
      si->curmeasureclef = si->cursorclef;
    }
}

/**
 * Insert a chord into the score
 * @param si pointer to the scoreinfo structure
 * @param duration the duration of the chord to insert
 * @param mode the current input mode
 * @param rest specifies a note is a rest
 */
void
dnm_insertchord (DenemoProject * gui, gint duration, input_mode mode, gboolean rest)
{
  DenemoMovement *si = gui->movement;
  DenemoObject *mudela_obj_new;
  gboolean inserting_midi = si->recording && (si->recording->type==DENEMO_RECORDING_MIDI) && si->marked_onset;
  if ((mode & INPUTEDIT) && !si->cursor_appending && !(mode & INPUTRHYTHM))
    {
      highlight_duration (gui, duration);
      changeduration (si, duration);
      return;
    }
  insertion_point (si);

//At this point, if it is the user's preference, check if there is room for this duration in the current measure.
//if not put in a shorter note and tie it, then call recursively to put in the remaining duration.
//only do this if we are in the appending position
//The difficulty here is that we do not have the prevailing time signature cached (?), so we do not know when a measure is full. draw.c is used to compute this.
//well, they are cached as cursortime1 and 2 in the DenemoMovement structure.
// is the curObj->starttickofnextnote > tickspermeasure where  tickspermeasure = WHOLE_NUMTICKS * time1 / time2

    if(Denemo.prefs.spillover && si->cursor_appending)
    { 
     DenemoObject *curObj;
    
            if(duration>= 0 && si->currentobject && (curObj=si->currentobject->data))
            {
                if(curObj->type==CHORD)
                    {//g_print("dur %d base %d\n", curObj->durinticks, curObj->basic_durinticks);
                        gint ticks  = (curObj->durinticks/ curObj->basic_durinticks) * WHOLE_NUMTICKS / (1 << duration); /* takes into account prevailing tuple */                  
                        gint tickspermeasure =  WHOLE_NUMTICKS * si->cursortime1 / si->cursortime2;
                        if ((curObj->starttickofnextnote < tickspermeasure) && ((ticks + curObj->starttickofnextnote) > tickspermeasure))
                        {
                            dnm_insertchord (gui, duration+1, mode, rest); 
                            toggle_tie (NULL, NULL);
                            dnm_insertchord (gui, duration+1, mode, rest);
                            return;
                        }
                    }
        }   
    }




  /* Now actually create the chord as an object (before insertion) */
  mudela_obj_new = newchord (duration, 0, 0);
  if ((mode & INPUTNORMAL) && (rest != TRUE))
    { 
        if(inserting_midi && si->recording && si->marked_onset && si->marked_onset->data)
        { 
            DenemoRecordedNote *midinote = (DenemoRecordedNote*)si->marked_onset->data;
            addtone (mudela_obj_new,  midinote->mid_c_offset + 7 * midinote->octave,  midinote->enshift, si->cursorclef);
            si->marked_onset = si->marked_onset->next;
        } else
        addtone (mudela_obj_new, si->cursor_y, si->cursoraccs[si->staffletter_y], si->cursorclef);

    }
  if ((mode & INPUTBLANK) || (gui->mode & INPUTBLANK) || (!rest && (Denemo.project->input_source == INPUTMIDI) && (gui->mode & (INPUTRHYTHM))))
    mudela_obj_new->isinvisible = TRUE;

  /* Insert the new note into the score.  Note that while we may have
     added a measure above, object_insert will invoke nudgerightward,
     which will in turn invoke update_hscrollbar, so we
     don't need to invoke that here.  */
  gboolean was_appending = si->cursor_appending;
  object_insert (gui, mudela_obj_new);
  if(inserting_midi)
      mudela_obj_new->isinvisible = FALSE;
      
  if (Denemo.project->input_source == INPUTMIDI && (gui->mode & (INPUTRHYTHM)))
    {
      if (Denemo.prefs.immediateplayback)
        {
            if(inserting_midi && !rest)
                {
                DenemoStaff *curstaffstruct = (DenemoStaff *) si->currentstaff->data;
                //play_notes (DEFAULT_BACKEND, curstaffstruct->midi_port, curstaffstruct->midi_channel, (chord *) mudela_obj_new->object);//compute duration and use that???
                 note *n = ((chord *) mudela_obj_new->object)->notes->data;

                    /* Because mid_c_offset is a measure of notes and we need a measure of
                    * half-steps, this array will help */
                  const gint key_offset[] = { -10, -8, -7, -5, -3, -1, 0, 2, 4, 5, 7, 9, 11 };

                  gint offset = n->mid_c_offset;

                  /* 60 is middle-C in MIDI keys */
                  gchar key = 60 + 12 * (offset / 7) + key_offset[offset % 7 + 6];
                  key += n->enshift;
#define MIDI_RESOLUTION (384)
                 gint duration_ms = 1000 * mudela_obj_new->durinticks * 60.0 / (si->tempo * MIDI_RESOLUTION);
                 g_debug("duration %d duration ms = %d\n", duration, duration_ms);
                 play_note (DEFAULT_BACKEND , curstaffstruct->midi_port , curstaffstruct->midi_channel, key, duration_ms, 127);
                
                
                }
            else
                rhythm_feedback (DEFAULT_BACKEND, duration, rest, FALSE);
        }
      if (!was_appending)
        movecursorleft (NULL, NULL);
    }
  else
    {
      if (Denemo.project->last_source == INPUTKEYBOARD)
        {
          DenemoStaff *curstaffstruct = (DenemoStaff *) si->currentstaff->data;
          if (Denemo.prefs.immediateplayback)
            {
              play_notes (DEFAULT_BACKEND, curstaffstruct->midi_port, curstaffstruct->midi_channel, (chord *) mudela_obj_new->object);
            }
        }
    }
}

gboolean
insert_marked_midi_note (void)
{
    DenemoMovement *si = Denemo.project->movement;
    gboolean inserting_midi = si->recording && (si->recording->type==DENEMO_RECORDING_MIDI) && si->marked_onset;

    if(inserting_midi && si->marked_onset && si->marked_onset->data)
        { 
            gint dots;
            DenemoRecordedNote *midinote = (DenemoRecordedNote*)si->marked_onset->data;//FIXME look at marked_onset->prev to see if we should add to a chord
            dnm_insertchord (Denemo.project, midinote->duration, INPUTNORMAL, FALSE);
            changenumdots (si->currentobject->data, midinote->dots);            
            return TRUE;
        }
return FALSE;
}
/**
 * Insert tuplet into the score
 * @param si pointer to the scoreinfo structure
 * @param type the type of tuplet to insert
 */
void
dnm_inserttuplet (DenemoProject * gui, tuplet_type type)
{
  DenemoMovement *si = gui->movement;
  DenemoObject *mudela_obj_new;

  insertion_point (si);
  switch (type)
    {
    case DUPLET:
      mudela_obj_new = tuplet_open_new (3, 2);
      break;
    case TRIPLET:
      mudela_obj_new = tuplet_open_new (2, 3);
      break;
    case QUADTUPLET:
      mudela_obj_new = tuplet_open_new (3, 4);
      break;
    case QUINTUPLET:
      mudela_obj_new = tuplet_open_new (4, 5);
      break;
    case SEXTUPLET:
      mudela_obj_new = tuplet_open_new (4, 6);
      break;
    case SEPTUPLET:
      mudela_obj_new = tuplet_open_new (4, 7);
      break;
    default:
      mudela_obj_new = tuplet_open_new (2, 3);
      break;
    }
  //g_debug ("Cursor pos %d (Before tup open)\n", si->cursor_x);
  object_insert (gui, mudela_obj_new);
  //g_debug ("Cursor pos %d (After tup open, before tup close)\n",         si->cursor_x);
  /* Add the closing bracket */
  object_insert (gui, tuplet_close_new ());
  //g_debug ("Cursor pos %d (After tup close)\n", si->cursor_x);
  si->cursor_x--;
  //g_debug ("Cursor pos %d( After move back)\n", si->cursor_x);

  si->currentobject = si->currentobject->prev;
  si->cursor_appending = FALSE;
  displayhelper (Denemo.project);
  score_status(Denemo.project, TRUE);
}


/**
 * Change the duration of the current note/rest
 * @param si pointer to the scoreinfo structure
 * @param duration the duration to change the current CHORD
 * object to
 */
void
changeduration (DenemoMovement * si, gint duration)
{
  declarecurmudelaobj;

  if (curmudelaobj && curmudelaobj->type == CHORD)
    {
      store_for_undo_change (si, curmudelaobj);
      changedur (curmudelaobj, duration, 0);
    }
}

/**
 *  notechange 
 * If REMOVE delete the note closest to si->cursor_y in a ~si->currentobject
 * else add a note at si->cursor_y to the ~si->currentobject
 * FIXME ~si->currentobject in this comment means the thing gotten by the macro declaremudelaobj. This macro is a horrible hack induced by trying to be clever with tuplets - enforcing pairing of begin/end. notechange 
 * @param si pointer to the scoreinfo structure
 * @param remove whether to remove note or not
 */
static gboolean
notechange (DenemoMovement * si, gboolean remove)
{
  declarecurmudelaobj;
  gboolean ret = FALSE;
  gboolean inserting_midi = si->recording && (si->recording->type==DENEMO_RECORDING_MIDI) && si->marked_onset;

  if (curmudelaobj && curmudelaobj->type == CHORD)
    {
      store_for_undo_change (si, curmudelaobj);
      if (remove == TRUE)
        ret = removetone (curmudelaobj, si->cursor_y /*mid_c_offset */ , si->cursorclef /*dclef */ );
      else {
          
        if(inserting_midi)
            {
            DenemoRecordedNote *midinote = (DenemoRecordedNote*)si->marked_onset->data;
            ret = addtone (curmudelaobj,  midinote->mid_c_offset + 7 * midinote->octave,  midinote->enshift, si->cursorclef);
            si->marked_onset = si->marked_onset->next;
            }
        else 
            ret = (gboolean) (intptr_t) addtone (curmudelaobj, si->cursor_y /* mid_c_offset */ ,
                                             si->cursoraccs[si->staffletter_y] /* enshift */ , si->cursorclef /*dclef */ );
        }

      if (Denemo.project->last_source == INPUTKEYBOARD)
        {
          DenemoStaff *curstaffstruct = (DenemoStaff *) si->currentstaff->data;

          if (Denemo.prefs.immediateplayback)
            {
              play_notes (DEFAULT_BACKEND, curstaffstruct->midi_port, curstaffstruct->midi_channel, (chord *) curmudelaobj->object);
            }
        }
      else
        {
          // Denemo.project->last_source = INPUTKEYBOARD;
        }
      displayhelper (Denemo.project);
      score_status(Denemo.project, TRUE);
    }
  return ret;
}

/**
 * Delete chord note closest to y cursor
 */
gboolean
delete_chordnote (DenemoProject * gui)
{
  notechange (gui->movement, TRUE);
  return TRUE;
}

/**
 * Insert chord note at y cursor position 
 */
gboolean
insert_chordnote (DenemoProject * gui)
{
  DenemoObject *curObj;
  if (gui->movement->currentobject && (curObj = Denemo.project->movement->currentobject->data) && (curObj->type == CHORD))
    notechange (gui->movement, FALSE);
  else
    insert_note_following_pattern (gui);
  return TRUE;
}
/**
 * Insert chord note at y cursor position or delete one if already present
 * return TRUE if inserted else FALSE
 */
gboolean
insert_or_delete_chordnote (gint enshift)
{
  DenemoProject *gui = Denemo.project;
  DenemoObject *curObj;
  gboolean remove = FALSE;
  if (gui->movement->currentobject && (curObj = Denemo.project->movement->currentobject->data) && (curObj->type == CHORD))
     {
      GList *node;
      for (node = ((chord *) curObj->object)->notes; node; node=node->next)
        {
            note *thenote = (note*)node->data;
            if((thenote->mid_c_offset == gui->movement->cursor_y) && (thenote->enshift == enshift))
               remove = TRUE;
        }
     notechange (gui->movement, remove);
    }
  else
    insert_note_following_pattern (gui);
    
  return !remove;
}
/**
 * Helper function that contains calls to all the display 
 * update functions
 *
 * @param gui pointer to the DenemoProject structure
 */
void
displayhelper (DenemoProject * gui)
{
  if(Denemo.non_interactive)
    return;

  DenemoMovement *si = gui->movement;
  beamandstemdirhelper (si);
  showwhichaccidentals ((objnode *) si->currentmeasure->data, si->curmeasurekey, si->curmeasureaccs);
  find_xes_in_measure (si, si->currentmeasurenum, si->cursortime1, si->cursortime2);
  nudgerightward (gui);
  set_bottom_staff (gui);
  write_status (gui);

#if 0
  if ((gui->mode & (INPUTRHYTHM)) && si->currentobject && (((DenemoObject *) (si->currentobject->data))->type == CHORD) && ((DenemoObject *) (si->currentobject->data))->starttickofnextnote >= WHOLE_NUMTICKS * si->cursortime1 / si->cursortime2)
    gdk_beep ();                //Signal new measures in Edit mode to catch out of step entry
#endif
  /*gtk_widget_draw (Denemo.scorearea, NULL); */
  draw_score_area();
  draw_score (NULL);
}



/**
 * Increment the enharmonic shift of the tone closest to the cursor.
 * @param si pointer to the DenemoMovement structure
 * @param direction, +ve for sharp -ve for flat
 */

void
incrementenshift (DenemoProject * gui, gint direction)
{
  DenemoMovement *si = gui->movement;
  declarecurmudelaobj;

  if (curmudelaobj && curmudelaobj->type == CHORD)
    {
      store_for_undo_change (si, curmudelaobj);

      shiftpitch (curmudelaobj, si->cursor_y, direction > 0);
      showwhichaccidentals ((objnode *) si->currentmeasure->data, si->curmeasurekey, si->curmeasureaccs);
      find_xes_in_measure (si, si->currentmeasurenum, si->cursortime1, si->cursortime2);
      
      
      //if tied ...
            chord *next = curmudelaobj->object;
            if (next->is_tied) 
                {
                 objnode *nextobj = gui->movement->currentobject->next;
                  measurenode *current = gui->movement->currentmeasure;
                  if(nextobj==NULL)
                        {
                          current = current->next;
                          if(current && current->data)
                            {
                               nextobj = current->data;
                           }
                       }
                  while (nextobj)
                    {
                      DenemoObject *thenextobj= (DenemoObject *) nextobj->data;
                          
                      if (thenextobj->type == CHORD)  
                        {
                            chord *next = thenextobj->object;
                            shiftpitch (thenextobj, si->cursor_y, direction > 0);
                            showwhichaccidentals ((objnode *) current->data, si->curmeasurekey, si->curmeasureaccs);
                            if(next->is_tied)
                             {
                                if(nextobj->next==NULL)
                                {
                                  current = current->next;
                                  if(current && current->data)
                                    {
                                       nextobj = current->data;
                                       continue;
                                    }
                                }
                                nextobj = nextobj->next;
                                continue;
                            }
                            else
                            break;
                        }
                    
                        break;
                    }
            
        }
      if (Denemo.project->last_source == INPUTKEYBOARD)
        {
          DenemoStaff *curstaffstruct = (DenemoStaff *) si->currentstaff->data;

          if (Denemo.prefs.immediateplayback)
            {
              play_notes (DEFAULT_BACKEND, curstaffstruct->midi_port, curstaffstruct->midi_channel, (chord *) curmudelaobj->object);
            }
        }
      else
        {
          //      Denemo.project->last_source = INPUTKEYBOARD;
        }

      displayhelper (Denemo.project);
      score_status(Denemo.project, TRUE);
    }
}

/**
 * Set the enharmonic shift of the tone closest to the cursor.
 * @param si pointer to the DenemoMovement structure
 * @param enshift -2 .. +2 for double flat to double sharp FIXME make this a system wide enum
 */

void
setenshift (DenemoMovement * si, gint enshift)
{
  declarecurmudelaobj;
  if (curmudelaobj && curmudelaobj->type == CHORD)
    {
      store_for_undo_change (si, curmudelaobj);

      changeenshift (curmudelaobj, si->cursor_y, enshift);


      if (Denemo.project->input_source == INPUTKEYBOARD)
        {
          DenemoStaff *curstaffstruct = (DenemoStaff *) si->currentstaff->data;
          if (Denemo.prefs.immediateplayback)
            {
              play_notes (DEFAULT_BACKEND, curstaffstruct->midi_port, curstaffstruct->midi_channel, (chord *) curmudelaobj->object);
            }
        }

    }
}

/**
 * Change the stemdirection of the current chord object
 * by a given amount
 * @param si  pointer to the scoreinfo structure
 * @param amount the stem direction change to make
 */
void
change_stem_directive (DenemoMovement * si, enum stemdirections amount)
{
  declarecurmudelaobj;

  if (curmudelaobj && curmudelaobj->type == STEMDIRECTIVE)
    {
      store_for_undo_change (si, curmudelaobj);

      switch (amount)
        {
        case DENEMO_STEMDOWN:
          ((stemdirective *) curmudelaobj->object)->type = DENEMO_STEMDOWN;
          break;
        case DENEMO_STEMUP:
          ((stemdirective *) curmudelaobj->object)->type = DENEMO_STEMUP;
          break;
        default:
          ((stemdirective *) curmudelaobj->object)->type = DENEMO_STEMBOTH;
          break;
        }
    displayhelper (Denemo.project);
    score_status(Denemo.project, TRUE);
    }
}


/**
 * Change the number of dots on the current chord
 *
 * @param si pointer to the scoreinfo structure
 * @param amount the number of dots to add/remove
 */
void
changedots (DenemoMovement * si, gint amount)
{
  declarecurmudelaobj;

  if (curmudelaobj && curmudelaobj->type == CHORD)
    {
      store_for_undo_change (si, curmudelaobj);

      if (Denemo.project->mode & (INPUTRHYTHM))
        {
          if (Denemo.prefs.immediateplayback)
            {
              chord *thechord = (chord *) curmudelaobj->object;
              gboolean rest = (thechord->notes == NULL);
              rhythm_feedback (DEFAULT_BACKEND, thechord->baseduration, rest, TRUE);
            }
        }
      changenumdots (curmudelaobj, amount);
    }
}

/**
 *  Insert measure into the score at the current position
 *
 * @param si pointer to the scoreinfo structure
 * @param number of measures to insert
 */
void
dnm_insertmeasures (DenemoMovement * si, gint number)
{
  si->currentmeasure = dnm_addmeasures (si, si->currentmeasurenum - 1, number, 1);
  si->cursor_x = 0;
  si->cursor_appending = TRUE;
  si->currentobject = NULL;
  set_rightmeasurenum (si);
  displayhelper (Denemo.project);
  score_status(Denemo.project, TRUE);
  //si->markstaffnum = 0;
  //calcmarkboundaries (si);
  /* update_hscrollbar (si); */
}

/**
 *  Insert measure into the staff after the current position
 
 */
void
insertmeasureafter (GtkAction* action, G_GNUC_UNUSED DenemoScriptParam* param)
{
  DenemoMovement *si = Denemo.project->movement;
  take_snapshot ();
  si->currentmeasure = addmeasures (si, si->currentmeasurenum++, 1, 0);
  si->cursor_x = 0;
  si->cursor_appending = TRUE;
  si->currentobject = NULL;
  set_rightmeasurenum (si);
  //si->markstaffnum = 0;
  //calcmarkboundaries (si);
  /* update_hscrollbar (si); */
}

/**
 *  Insert measure into the staffs after the current position
 
 */
void
addmeasureafter (GtkAction* action, G_GNUC_UNUSED DenemoScriptParam* param)
{
  DenemoMovement *si = Denemo.project->movement;
  take_snapshot ();
  si->currentmeasure = addmeasures (si, si->currentmeasurenum++, 1, 1);
  si->cursor_x = 0;
  si->cursor_appending = TRUE;
  si->currentobject = NULL;
  set_rightmeasurenum (si);
  //si->markstaffnum = 0;
  draw_score_area();
  score_status(Denemo.project, TRUE);
  // calcmarkboundaries (si);
  /* update_hscrollbar (si); */
}

/**
 *  Insert measure into the staff before the current position
 
 */
void
insertmeasurebefore (GtkAction* action, G_GNUC_UNUSED DenemoScriptParam* param)
{
  DenemoMovement *si = Denemo.project->movement;
  si->currentmeasure = addmeasures (si, si->currentmeasurenum - 1, 1, 0);
  si->cursor_x = 0;
  si->cursor_appending = TRUE;
  si->currentobject = NULL;
  set_rightmeasurenum (si);
  si->markstaffnum = 0;

  draw_score_area();// displayhelper (Denemo.project);
  score_status(Denemo.project, TRUE);
  /* update_hscrollbar (si); */
}

/**
 * Add measure to the end of the score
 * 
 * @param si pointer to the scoreinfo structure
 * @param number the number of measures to append
 */
void
appendmeasures (DenemoMovement * si, gint number)
{
  dnm_addmeasures (si, g_list_length (staff_first_measure_node (si->currentstaff)), number, FALSE);
  /* Reset these two variables because si->currentmeasure and
   * si->currentobject may now be pointing to dead data */
  si->currentmeasure = g_list_nth (staff_first_measure_node (si->currentstaff), si->currentmeasurenum - 1);
  si->currentobject = g_list_nth ((objnode *) si->currentmeasure->data, si->cursor_x - (si->cursor_appending == TRUE));
  set_rightmeasurenum (si);
  displayhelper (Denemo.project);
  score_status(Denemo.project, TRUE);
  /*update_hscrollbar (si); */
}

void
appendmeasurestoentirescore (DenemoMovement * si, gint number)
{
  dnm_addmeasures (si, g_list_length (staff_first_measure_node (si->currentstaff)), number, TRUE);
  /* Reset these two variables because si->currentmeasure and
   * si->currentobject may now be pointing to dead data */
  si->currentmeasure = g_list_nth (staff_first_measure_node (si->currentstaff), si->currentmeasurenum - 1);
  si->currentobject = g_list_nth ((objnode *) si->currentmeasure->data, si->cursor_x - (si->cursor_appending == TRUE));
  set_rightmeasurenum (si);
  /* update_hscrollbar (si); */

}





/**
 * Delete staff wrapper to delete the preceding staff 
 * 
 * @param action pointer to the GtkAction event
 * @param gui pointer to the DenemoProject structure
 */
void
delete_staff_before (GtkAction * action, DenemoScriptParam * param)
{
  DenemoProject *gui = Denemo.project;
  if (staffup (action, param))
    {
      staff_delete (gui, TRUE);
    }
}

/**
 * Delete staff wrapper to delete the next staff 
 * 
 * @param action pointer to the GtkAction event
 * @param gui pointer to the DenemoProject structure
 */
void
delete_staff_after (GtkAction * action, DenemoScriptParam * param)
{
  DenemoProject *gui = Denemo.project;
  if (staffdown (action, param))
    {
      staff_delete (gui, TRUE);
    }
}

/**
 * Delete staff wrapper to delete current staff
 * 
 * @param action pointer to the GtkAction event
 * @param gui pointer to the DenemoProject structure
 */
void
delete_staff_current (G_GNUC_UNUSED GtkAction * action, G_GNUC_UNUSED DenemoScriptParam * param)
{
  DenemoProject *gui = Denemo.project;
  staff_delete (gui, TRUE);
}



/**
 * Delete mesasure from score 
 * @param gui pointer to the DenemoProject structure
 * @return none
 *
 * This is only a wrapper for the real dnm_deletemeasure
 * function.
 */
void
deletemeasure (GtkAction* action, DenemoScriptParam* param)
{
  dnm_deletemeasure (Denemo.project->movement);
  isoffleftside (Denemo.project);
  displayhelper (Denemo.project);
  score_status(Denemo.project, TRUE);
}

/**
 * Delete mesasure from all staffs of score 
 * @param gui pointer to the DenemoProject structure
 * @return none
 *

 */
void
deletemeasureallstaffs (GtkAction* action, DenemoScriptParam* param)
{
  DenemoMovement *si = Denemo.project->movement;
  //take_snapshot(); this does not prevent the multiple undo steps needed
  si->currentmeasure = removemeasures (si, si->currentmeasurenum - 1, 1, TRUE);
  setcurrents (si);
  if (si->markstaffnum)
    calcmarkboundaries (si);
  score_status (Denemo.project, TRUE);
  si->markstaffnum = 0;
  isoffleftside (Denemo.project);
  displayhelper (Denemo.project);
  score_status(Denemo.project, TRUE);
}



/**
 * Delete measure from the score
 *
 * TODO remove measure from current staff 
 * rather than the entire score
 * @param gui pointer to the DenemoProject structure
 * @return none
 */
void
dnm_deletemeasure (DenemoMovement * si)
{
  si->currentmeasure = removemeasures (si, si->currentmeasurenum - 1, 1, FALSE);

  /* In case that was the last measure we just deleted, which'd cause
   * the current measure to be the left of what's displayed */

  setcurrents (si);
  if (si->markstaffnum)
    calcmarkboundaries (si);
  si->markstaffnum = 0;
  //g_debug("Removed current measure now %p number %d\n", si->currentmeasure, si->currentmeasurenum);

}

/**
 * Remove current object from the score
 *@param cur_measure pointer to the current measure
 * @param cur_objnode pointer to the current object
 * @return none
 */
static void
remove_object (measurenode * cur_measure, objnode * cur_objnode)
{
  if (cur_measure->data)
    {
      cur_measure->data = g_list_remove_link ((objnode *) cur_measure->data, cur_objnode);
      freeobject ((DenemoObject *) cur_objnode->data);
      g_list_free_1 (cur_objnode);
    }
}


/**
 * Helper to remove the current object an reset cursor stats
 * @param si pointer to the scoreinfo structure
 * @return none
 */
static void
delete_object_helper (DenemoMovement * si)
{
  remove_object (si->currentmeasure, si->currentobject);
  reset_cursor_stats (si);
}


/**
 * Helper to remove a object from the score
 * @return none.
 *
 */
void
deleteobject (GtkAction* action, DenemoScriptParam* param)
{
  dnm_deleteobject (Denemo.project->movement);
}

/**
 * Function to delete object from the score
 * @param gui - pointer to the DenemoProject structure
 * @return none
 */
void
dnm_deleteobject (DenemoMovement * si)
{
  declarecurmudelaobj;
  //staffnode *curstaff;
  //measurenode *curmeasure;
  //g_debug ("dnm_deleteobject undo/redo mode %d\n", si->undo_redo_mode);
  if (curmudelaobj == NULL)
    return;
  /* when tone_store is active, act on that, not the staff itself */
#ifdef _HAVE_PORTAUDIO_
  if (((DenemoStaff *) si->currentstaff->data)->tone_store)
    {
      if (si->currentobject && ((DenemoObject *) (si->currentobject->data))->type == CHORD)
        {
          if (delete_tone (si, ((DenemoObject *) (si->currentobject->data))->object))
            return;
        }
    }
#endif
  if (curmudelaobj->type == LILYDIRECTIVE && ((lilydirective *) curmudelaobj->object)->locked)
    {
      DenemoDirective *directive = (lilydirective *) curmudelaobj->object;
      DenemoScriptParam param;
      param.string = g_string_new ("delete");
      GtkAction *action = lookup_action_from_name (directive->tag->str);
      if (action && (Denemo.keyboard_state != GDK_MOD2_MASK /*NumLock */ ))
        {
          activate_script (action, &param);
          g_string_free (param.string, TRUE);
          return;
        }
    }
  DenemoUndoData *undo;
  if (!si->undo_guard)
    {
      undo = (DenemoUndoData *) g_malloc (sizeof (DenemoUndoData));
      undo->object = dnm_clone_object (curmudelaobj);
      //get position after delete
    }
  if (!si->cursor_appending)
    {
      switch (curmudelaobj->type)
        {
        case CHORD:
          delete_object_helper (si);

          break;
        case TUPOPEN:
        case TUPCLOSE:
          /* TODO - add code that will automatically delete a tupbracket's
           * corresponding bracket */

          delete_object_helper (si);

          break;
        case CLEF:
/* here we have to re-validate leftmost clef e.g. find_leftmost_allcontexts (gui->movement);
 which seems to be done... */
          delete_object_helper (si);
          staff_fix_note_heights ((DenemoStaff *) si->currentstaff->data);
          staff_beams_and_stems_dirs ((DenemoStaff *) si->currentstaff->data);
          find_xes_in_all_measures (si);
          break;
        case KEYSIG:
          /* Doesn't automatically delete sibling key signatures, though
           * I probably will have it do so soon */
          delete_object_helper (si);
          staff_beams_and_stems_dirs ((DenemoStaff *) si->currentstaff->data);
          staff_show_which_accidentals ((DenemoStaff *) si->currentstaff->data);
          find_xes_in_all_measures (si);
          break;
        case TIMESIG:
          delete_object_helper (si);
#if 0
          //do not do this, as this is a primitive and should only delete one object
          /* For time signature changes remove from all other staffs 
           * if in the conventional, first, position */
          for (curstaff = si->thescore; curstaff; curstaff = curstaff->next)
            {
              curmeasure = g_list_nth (staff_first_measure_node (curstaff), si->currentmeasurenum - 1);
              if (curmeasure && curmeasure->data)
                {
                  DenemoObject *first_obj = ((objnode *) curmeasure->data)->data;
                  //g_debug("Deleting object of type %s\n", DenemoObjTypeNames[first_obj->type]);
                  if (first_obj && first_obj->type == TIMESIG)
                    {
                      remove_object (curmeasure, (objnode *) curmeasure->data);
                      staff_beams_and_stems_dirs ((DenemoStaff *) curstaff->data);
                    }
                }
            }
#endif
          reset_cursor_stats (si);
          find_xes_in_all_measures (si);
          break;
        case STEMDIRECTIVE:
          delete_object_helper (si);
          staff_beams_and_stems_dirs ((DenemoStaff *) si->currentstaff->data);
          find_xes_in_all_measures (si);
          break;
        case DYNAMIC:
          delete_object_helper (si);

          break;
        case LILYDIRECTIVE:
          delete_object_helper (si);
          //displayhelper (gui);
          break;
        case GRACE_START:
        case GRACE_END:
          delete_object_helper (si);

          break;
        case LYRIC:
        case FIGURE:
          delete_object_helper (si);

          break;
        case BARLINE:
          //    case COMMENT:
        case MEASUREBREAK:
          break;
          // XXX: unhandled... 
        case STAFFBREAK:
        case FAKECHORD:
        case PARTIAL:
          break;
        }
      si->markstaffnum = 0;
    }
  if (!si->undo_guard)
    {
      get_position (si, &undo->position);
      undo->action = ACTION_DELETE;
      update_undo_info (si, undo);
    }

  displayhelper (Denemo.project);
  score_status(Denemo.project, TRUE);
}

/**
 * Insert cloned chordobject into the 
 * score
 * @param si - pointer to the scoreinfo structure
 * @return none
 */
void
insertclone (DenemoProject * gui)
{
  DenemoMovement *si = gui->movement;
  declarecurmudelaobj;

  if (curmudelaobj && curmudelaobj->type == CHORD)
    object_insert (gui, dnm_clone_object (curmudelaobj));
}


void
tolastobject (DenemoProject * gui)
{
  while (gui->movement->currentobject && (gui->movement->currentobject->next))
    {
      gui->movement->currentobject = gui->movement->currentobject->next;
      gui->movement->cursor_x++;
    }
}


/* Make note tied/untied */
void
toggle_tie (G_GNUC_UNUSED GtkAction * action, G_GNUC_UNUSED DenemoScriptParam * param)
{
  DenemoProject *gui = Denemo.project;
  DenemoMovement *si = gui->movement;
  DenemoObject *curmudelaobj = (DenemoObject *) (gui->movement->currentobject ? gui->movement->currentobject->data : NULL);
  if (curmudelaobj && curmudelaobj->type == CHORD && ((((chord *) curmudelaobj->object)->notes) ||  ((chord *) curmudelaobj->object)->is_tied))
    {
      store_for_undo_change (si, curmudelaobj);
      ((chord *) curmudelaobj->object)->is_tied ^= 1;
      draw_score_area();
    }
  score_status (gui, TRUE);
}




/**
 * Move cursor to the end of the score  extending the selection if extend_selection is TRUE
 * @param param - pointer to a script parameter structure
 * @return none
 */
static void
gotoend (gpointer param, gboolean extend_selection)
{
  DenemoProject *gui = Denemo.project;
  if (extend_selection && !gui->movement->markstaffnum)
    set_mark (NULL, NULL);
  gui->movement->currentmeasurenum = g_list_length (((DenemoStaff *) gui->movement->currentstaff->data)->measures);
  setcurrents (gui->movement);
  if (extend_selection)
    calcmarkboundaries (gui->movement);
  tolastobject (gui);
  if (extend_selection)
    cursorright (NULL, param);
  else
    movecursorright (NULL, param);
  //refresh cached values, eg current timesig
  update_drawing_cache ();
  find_leftmost_allcontexts (gui->movement);  //FIXME is this done in displayhelper?
  displayhelper (gui);
}

/**
 * Move the cursor to the beginning of the score extending the selection if extend_selection is TRUE
 * @param param - pointer to a script parameter structure
 * @return none
*/
static void
gotohome (gboolean extend_selection)
{
  DenemoProject *gui = Denemo.project;
  if (extend_selection && !gui->movement->markstaffnum)
    set_mark (NULL, NULL);
  gui->movement->currentmeasurenum = gui->movement->leftmeasurenum = 1;
  displayhelper (gui);
  setcurrents (gui->movement);
  if (extend_selection)
    calcmarkboundaries (gui->movement);
  find_leftmost_allcontexts (gui->movement);

  if(!Denemo.non_interactive){
    //refresh cached values, eg current timesig
    update_drawing_cache ();
  }
}


/**
 * Move the cursor to the beginning of the score, extending the selection if any. 
 * @param action - Gtk Action event
 * @param 
 * @return none
*/
void
tohome (G_GNUC_UNUSED GtkAction * action, G_GNUC_UNUSED DenemoScriptParam * param)
{
  gotohome (TRUE);
}


/**
 * Move the cursor to the end of the score, extending the selection if any. 
 * @param action - Gtk Action event
 * @param 
 * @return none
*/
void
toend (G_GNUC_UNUSED GtkAction * action, DenemoScriptParam * param)
{
  gotoend (param, TRUE);
}




/**
 * Move the cursor to the beginning of the staff, without extending the selection if any. 
 * @param action - Gtk Action event
 * @param 
 * @return none
*/
void
movetostart (G_GNUC_UNUSED GtkAction * action, DenemoScriptParam * param)
{
  gotohome (FALSE);
  if (param)
    param->status = TRUE;
}

/**
 * Move the cursor to the end of the staff, without extending the selection if any. 
 * @param action - Gtk Action event
 * @param 
 * @return none
*/
void
movetoend (G_GNUC_UNUSED GtkAction * action, DenemoScriptParam * param)
{
  gotoend (param, FALSE);
  if (param)
    param->status = TRUE;
}



/**
 * Insert stem directive,  absolute stemdirection for the 
 * entire staff or until a new stem directive in added
 * 
 * @param action Gtk Action event
 * @param gui pointer to the DenemoProject structure
 * @return none
 */
void
stem_directive_insert (G_GNUC_UNUSED GtkAction * action, G_GNUC_UNUSED DenemoScriptParam * param)
{
  DenemoProject *gui = Denemo.project;
  object_insert (gui, dnm_stem_directive_new (DENEMO_STEMBOTH));
  /* This sets beams and stem directions in the measure, but that's
   * not sufficient */

  displayhelper (gui);


}

/**
 * Toggle start_slur flag for the current chord
 * 
 * @return none
 */
void
toggle_begin_slur (GtkAction* action, DenemoScriptParam* param)
{
  DenemoMovement *si = Denemo.project->movement;
  declarecurmudelaobj;

  if (curmudelaobj && curmudelaobj->type == CHORD)
    {
      store_for_undo_change (si, curmudelaobj);


      ((chord *) curmudelaobj->object)->slur_begin_p = !((chord *) curmudelaobj->object)->slur_begin_p;


    }
  displayhelper (Denemo.project);
  score_status(Denemo.project, TRUE);
}

/**
 *  Force a cautionary accidental
 * @param si pointer to the scoreinfo structure
 * @return none
 */
void
caution (DenemoMovement * si)
{
  declarecurmudelaobj;

  forceaccidentals (curmudelaobj);
  find_xes_in_measure (si, si->currentmeasurenum, si->cursortime1, si->cursortime2);
}

/**
 * Toggle end_slur flag for the current chord
 * @return none
 */
void
toggle_end_slur (GtkAction* action, DenemoScriptParam * param)
{
  DenemoMovement *si = Denemo.project->movement;
  declarecurmudelaobj;

  if (curmudelaobj && curmudelaobj->type == CHORD)
    {
      store_for_undo_change (si, curmudelaobj);


      ((chord *) curmudelaobj->object)->slur_end_p = !((chord *) curmudelaobj->object)->slur_end_p;


    }
  displayhelper (Denemo.project);
  score_status(Denemo.project, TRUE);
}

/**
 * Toggle start crescendo flag for current chord
 * @param si pointer to the scoreinfo structure
 * @return none
 */
void
toggle_start_crescendo (GtkAction* action, DenemoScriptParam * param)
{
  DenemoMovement *si = (DenemoMovement *) Denemo.project->movement;
  declarecurmudelaobj;

  if (curmudelaobj && curmudelaobj->type == CHORD)
    {
      ((chord *) curmudelaobj->object)->crescendo_begin_p = !((chord *) curmudelaobj->object)->crescendo_begin_p;


    }
  displayhelper (Denemo.project);
  score_status(Denemo.project, TRUE);
}

/**
 * Toggle end crescendo flag for current chord
 * @param si pointer to the scoreinfo structure
 * @return none
 */
void
toggle_end_crescendo (GtkAction* action, DenemoScriptParam * param)
{
  DenemoMovement *si = (DenemoMovement *) Denemo.project->movement;
  declarecurmudelaobj;

  if (curmudelaobj && curmudelaobj->type == CHORD)
    {
      ((chord *) curmudelaobj->object)->crescendo_end_p = !((chord *) curmudelaobj->object)->crescendo_end_p;


    }
  displayhelper (Denemo.project);
  score_status(Denemo.project, TRUE);
}

/**
 * Toggle start diminuendo flag for current chord
 * @param si pointer to the scoreinfo structure
 * @return none
 */
void
toggle_start_diminuendo (GtkAction* action, DenemoScriptParam * param)
{
  DenemoMovement *si = (DenemoMovement *) Denemo.project->movement;
  declarecurmudelaobj;

  if (curmudelaobj && curmudelaobj->type == CHORD)
    {
      ((chord *) curmudelaobj->object)->diminuendo_begin_p = !((chord *) curmudelaobj->object)->diminuendo_begin_p;


    }
  displayhelper (Denemo.project);
  score_status(Denemo.project, TRUE);
}

/**
 * Toggle end diminuendo flag for current chord
 * @param si pointer to the scoreinfo structure
 * @return none
 */
void
toggle_end_diminuendo (GtkAction* action, DenemoScriptParam * param)
{
  DenemoMovement *si = (DenemoMovement *) Denemo.project->movement;
  declarecurmudelaobj;

  if (curmudelaobj && curmudelaobj->type == CHORD)
    {
      ((chord *) curmudelaobj->object)->diminuendo_end_p = !((chord *) curmudelaobj->object)->diminuendo_end_p;


    }
  displayhelper (Denemo.project);
  score_status(Denemo.project, TRUE);
}



/**
 * Autosave timeout function - saves the current score after the current
 * timeout has expired
 * @param si pointer to the scoreinfo structure
 * @return none
 */
gboolean
auto_save_document_timeout (DenemoProject * gui)
{
  /* first check that this timer has not been left running after destruction of the gui */
  if (g_list_find (Denemo.projects, gui) == NULL)
    {
      //do not do this, it causes denemo to hang. warningdialog (_("Timer left running"));
      return FALSE;             /* turns off the timer */
    }
  DenemoMovement *si = gui->movement;
  g_message ("Autosaving");
  if (!gui->autosavename)
    {
      g_warning ("gui->autosavename not set");
      /*gui->autosavename = g_string_new (dir); */
      gui->autosavename = g_string_new (get_user_data_dir (TRUE));
      if (si->lily_file)
        gui->autosavename = g_string_append (gui->autosavename, "/autosave.ly");
      else
        gui->autosavename = g_string_append (gui->autosavename, "/autosave.denemo");
    }
  //g_debug ("Auto save file name %s\n", gui->autosavename->str);
  if (si->lily_file)
    {
      exportlilypond (gui->autosavename->str, gui, TRUE);
    }
  else
    {
      exportXML (gui->autosavename->str, gui);
    }

  return TRUE;
}
