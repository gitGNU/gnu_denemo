/* lilydirectives.c 
 * Implements lilydirectives which are not notes 
 *
 * for Denemo, a gtk+ frontend to GNU Lilypond
 * Richard Shann 2009, 2010, 2011
 * A Tee  (c) 2000-2005
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <denemo/denemo.h>
#include "lilydirectives.h"
#include "chordops.h"
#include "calculatepositions.h"
#include "commandfuncs.h"
#include "contexts.h"
#include "dialogs.h"
#include "draw.h"
#include "objops.h"
#include "staffops.h"
#include "utils.h"
#include "prefops.h"
#include "view.h"
#if GTK_MAJOR_VERSION==3
#include <gdk/gdkkeysyms-compat.h>      //FIXME Look for something more gtk3 like
#endif

static DenemoDirective *get_movementcontrol_directive (gchar * tag);
static DenemoDirective *get_score_directive (gchar * tag);

static GHashTable *action_scripts;
static void
action_script_table_insert (gchar * name, gchar * script)
{
  if (!action_scripts)
    action_scripts = g_hash_table_new_full (g_str_hash, g_str_equal, g_free, g_free);
  g_hash_table_insert (action_scripts, g_strdup (name), script);
}

void
set_action_script_for_tag (gchar * tag, gchar * script)
{
  action_script_table_insert (tag, script);
}


gchar *
get_action_script (gchar * name)
{
  if (action_scripts)
    return (gchar *) g_hash_table_lookup (action_scripts, name);
  return NULL;
}


static void
gtk_menu_item_set_label_text (GtkMenuItem * item, gchar * text)
{
  GtkWidget *label = (GtkWidget *) gtk_bin_get_child (GTK_BIN (item));
  if (label)
    gtk_label_set_text (GTK_LABEL (label), text);
}




static void
toggle_locked (GtkWidget * widget, gboolean * locked)
{
  //g_print("Called with %d\n", *locked);
  *locked = !*locked;
}

/* lookup a directive tagged with TAG in a list DIRECTIVES and return it.
   if TAG is NULL return the first directive
   else return NULL
   * If TAG has two lines the first only is matched, while the second is
   * interpreted as a number selecting which matching directive to return */
static DenemoDirective *
find_directive (GList * directives, gchar * tag)
{
  DenemoDirective *directive = NULL;
  if (tag)
    {
      GList *g;
      gchar *newline;
      gint number = 0;          //number of matching directive required 1 is first matching
      gint count = 0;           //count of directives with matching name
      if (*tag == '\n')
        return NULL;

      for (newline = tag; *newline; newline++)
        {
          if (*newline == '\n')
            {
              number = atoi (newline + 1);
              if (number)
                *newline = 0;
              break;
            }
        }

      for (g = directives; g; g = g->next)
        {
          directive = (DenemoDirective *) g->data;
          if (directive->tag && number ? g_str_has_prefix (directive->tag->str, tag) : !strcmp (tag, directive->tag->str))
            {
              if (number == 0)
                return directive;
              count++;
              if (number == count)
                {
                  if (newline != tag)
                    *newline = '\n';
                  return directive;
                }
            }
          directive = NULL;
        }
    }
  else
    directive = (DenemoDirective *) directives->data;
  return directive;
}

static DenemoDirective *
find_directive_number (GList * directives, gint num)
{
  return g_list_nth_data (directives, num - 1);
}

static gboolean
delete_directive (GList ** directives, gchar * tag)
{
  DenemoDirective *directive = NULL;
  if (tag)
    {
      GList *g;
      for (g = *directives; g; g = g->next)
        {
          directive = (DenemoDirective *) g->data;
          if (directive->tag && !strcmp (tag, directive->tag->str))
            {
              *directives = g_list_remove (*directives, directive);
              free_directive (directive);
              score_status (Denemo.gui, TRUE);
              displayhelper (Denemo.gui);
              return TRUE;
            }
        }
    }
  return FALSE;
}

/* free a list of directives and set to NULL */
void
delete_directives (GList ** directives)
{
  DenemoDirective *directive = NULL;    //FIXME use free_directives
  if (directives)
    while (*directives)
      {
        directive = (DenemoDirective *) (*directives)->data;
        *directives = g_list_remove (*directives, directive);
        free_directive (directive);
      }
}



static DenemoDirective *
new_directive (gchar * tag)
{
  DenemoDirective *directive = (DenemoDirective *) g_malloc0 (sizeof (DenemoDirective));
  if (tag)
    directive->tag = g_string_new (tag);
  return directive;
}


static DenemoObject *
get_object (void)
{
  DenemoGUI *gui = Denemo.gui;
  DenemoScore *si = gui->si;
  return (DenemoObject *) si->currentobject ? (DenemoObject *) si->currentobject->data : NULL;
}

typedef enum attach_type
{ ATTACH_NOTE, ATTACH_CHORD } attach_type;
/**
 * Denemo directive attach or edit.  
if interactive: Allows user to attach a lilypond directive 
else attache the passed strings as lilypond directive
attachment is to chord ( attach is ATTACH_CHORD) or to the note at the cursor
 */
static void
attach_directive (attach_type attach, gchar * postfix, gchar * prefix, gchar * display, gchar * tag, gboolean interactive)
{
  gchar *prefixstring = NULL, *postfixstring = NULL, *displaystring = NULL;
  DenemoGUI *gui = Denemo.gui;
  DenemoScore *si = gui->si;
  note *curnote = NULL;
  DenemoObject *curObj = get_object ();
  if (curObj == NULL)
    {
      if (interactive)
        warningdialog (_("You must put the cursor on a chord to attach LilyPond"));     //FIXME find a note and ask
      return;
    }
  chord *thechord = NULL;
  thechord = (chord *) curObj->object;
  if (curObj->type != CHORD)
    {
      if (interactive)
        warningdialog (_("You must put the cursor on a chord to attach LilyPond"));
      return;
    }

  curnote = findnote (curObj, gui->si->cursor_y);
  if (attach == ATTACH_NOTE && (curnote == NULL))
    {
      if (interactive)
        warningdialog (_("You must put the cursor on a note to attach LilyPond to the note"));  //FIXME find a note and ask
      return;
    }

  // setup directive to be data from thechord->directives or curnote->directives which has matching tag, or first if tag is NULL.
  DenemoDirective *directive = NULL;
  switch (attach)
    {
    case ATTACH_CHORD:
      if (thechord->directives == NULL)
        {
          directive = new_directive (tag);
          thechord->directives = g_list_append (NULL, directive);
        }
      else
        {
          directive = find_directive (thechord->directives, tag);
          if (directive == NULL)
            {
              if (tag)
                {
                  directive = new_directive (tag);
                  thechord->directives = g_list_append (thechord->directives, directive);
                }
            }
        }
      break;
    case ATTACH_NOTE:
      if (curnote->directives == NULL)
        {
          directive = new_directive (tag);
          curnote->directives = g_list_append (NULL, directive);
        }
      else
        {
          directive = find_directive (curnote->directives, tag);
          if (directive == NULL)
            {
              if (tag)
                {
                  directive = new_directive (tag);
                  curnote->directives = g_list_append (curnote->directives, directive);
                }
            }
        }
      break;
    default:
      g_warning (_("Error in attach type"));
      return;
    }

  if (interactive)
    {
      if (directive->postfix)
        postfixstring = directive->postfix->str;
      if (directive->prefix)
        prefixstring = directive->prefix->str;
      if (directive->display)
        displaystring = directive->display->str;

      prefixstring = string_dialog_entry (gui, _("Attach LilyPond"), _("Give text to place before the note"), prefixstring);
      postfixstring = string_dialog_entry (gui, curnote ? _("Attach LilyPond to Note") : _("Attach LilyPond to Chord"), curnote ? _("Give LilyPond text to postfix to note of chord") : _("Give LilyPond text to postfix to chord"), postfixstring);
      displaystring = string_dialog_entry (gui, _("Attach LilyPond"), _("Give Display text if required"), displaystring);
    }
  else
    {                           //not interactive
      if (prefix)
        prefixstring = g_strdup (prefix);
      if (postfix)
        postfixstring = g_strdup (postfix);
      if (display)
        displaystring = g_strdup (display);
    }

#define STRINGASSIGN(field, val) \
     if(val && *val) {\
     if(directive->field) g_string_assign(directive->field, val);\
     else directive->field=g_string_new(val);}
  STRINGASSIGN (postfix, postfixstring);
  STRINGASSIGN (prefix, prefixstring);
  STRINGASSIGN (display, displaystring);

#undef STRINGASSIGN

  score_status (gui, TRUE);
  displayhelper (gui);
  g_free (postfixstring);
  g_free (displaystring);
  g_free (prefixstring);
}

static void
create_directives (GList ** directives, gchar * tag)
{
  *directives = g_list_append (NULL, new_directive (tag));
}

static void
get_lily_parameter (gchar * query, DenemoScriptParam * param)
{
  DenemoObject *curObj = (DenemoObject *) Denemo.gui->si->currentobject ? (DenemoObject *) Denemo.gui->si->currentobject->data : NULL;
  param->status = curObj && curObj->type == LILYDIRECTIVE;
#define ASSIGN_PARAM(field)  if(!strcmp(#field, query))\
  g_string_assign(param->string, lilyobj->field->str);
  if (param->status)
    {
      lilydirective *lilyobj = (lilydirective *) curObj->object;
      ASSIGN_PARAM (postfix);
      ASSIGN_PARAM (display);
      if (!strcmp ("minpixels", query))
        g_string_printf (param->string, "%d", curObj->minpixelsalloted);
    }
#undef ASSIGN_PARAM
}


static void
insert_lily_directive (gchar * postfix, gchar * display, gboolean locked, gint minpixels)
{
  DenemoGUI *gui = Denemo.gui;
  DenemoScore *si = gui->si;
  DenemoObject *lily;
  lilydirective *lilyobj = NULL;        /* a lily directive object */
  DenemoObject *curObj = (DenemoObject *) si->currentobject ? (DenemoObject *) si->currentobject->data : NULL;
  if (postfix == NULL)
    postfix = "";
  gboolean is_new = FALSE;
  if (curObj && curObj->type == LILYDIRECTIVE)
    {
      g_string_assign ((lilyobj = (lilydirective *) curObj->object)->postfix, postfix);
      setpixelmin (curObj);     //curObj->minpixelsalloted = minpixels;
    }
  else
    {
      lily = lily_directive_new (postfix);
      is_new = TRUE;
      lilyobj = (lilydirective *) lily->object;
      setpixelmin (lily);       //lily->minpixelsalloted = minpixels;// g_print("min pixels %d\n", lily->minpixelsalloted);
    }
  if (lilyobj)
    {
      lilyobj->locked = locked;
      if (*postfix == '%')
        {                       //append newline if directive starts with a LilyPond comment indicator
          g_string_append (lilyobj->postfix, "\n");
        }
      if (display)
        {
          if (lilyobj->display)
            g_string_assign (lilyobj->display, display);
          else
            lilyobj->display = g_string_new (display);
        }
    }
  if (is_new)
    object_insert (gui, lily);
  score_status (gui, TRUE);
  displayhelper (gui);
}


/* Run a dialog to get a lily directive from the user
 the values returned must be freed by the caller */
static gboolean
get_lily_directive (gchar ** directive, gchar ** display, gboolean * locked)
{
  DenemoGUI *gui = Denemo.gui;
  GtkToggleButton *button = NULL;
  button = (GtkToggleButton *) gtk_check_button_new_with_label ("locked");
  g_signal_connect (button, "toggled", G_CALLBACK (toggle_locked), locked);
  if (*locked)
    gtk_toggle_button_set_active (button, *locked), *locked = TRUE;     //FIXME how is this supposed to be done?
  *directive = string_dialog_entry_with_widget (gui, _("Insert LilyPond"), _("Give LilyPond text to insert"), *directive, GTK_WIDGET (button));
  if (!*directive)
    return FALSE;
  *display = string_dialog_entry (gui, _("Insert LilyPond"), _("Give Display text if required"), *display);
  return TRUE;
}

/* return the directive tagged tag if present at cursor postion
 if tag is NULL, return any directive at current position*/
static DenemoDirective *
get_standalone_directive (gchar * tag)
{
  DenemoObject *curObj = (DenemoObject *) Denemo.gui->si->currentobject ? (DenemoObject *) Denemo.gui->si->currentobject->data : NULL;
  if (curObj && curObj->type == LILYDIRECTIVE)
    {
      DenemoDirective *ret = (DenemoDirective *) curObj->object;
      if (tag == NULL)
        return ret;
      if (*tag == 0)
        return ret;
      if (ret && ret->tag && strcmp (tag, ret->tag->str))
        ret = NULL;
      return ret;
    }
  return NULL;
}

/**
 * DEPRECATED - only called from InsertLilyDirective action, not the general Get/Put directive functions
 * Denemo directive insert/Edit.  Allows user to insert a Denemo directive 
 * before the current cursor position
 * or edit the current Denemo directive
 */
void
standalone_directive (GtkAction * action, DenemoScriptParam * param)
{
  DenemoGUI *gui = Denemo.gui;
  GET_4PARAMS (action, param, directive, postfix, display, minpixels);
  //g_print("query is %s\n", query);
  if (directive)
    postfix = directive;        //support for simpler syntax directive=xxx instead of postfix=xxx
  if (query)
    {
      get_lily_parameter (*query ? query : "postfix", param);
      return;
    }
  gboolean locked = FALSE;
  if (postfix && !display)
    display = postfix;
  if (action)
    {
      DenemoObject *curObj = (DenemoObject *) gui->si->currentobject ? (DenemoObject *) gui->si->currentobject->data : NULL;
      if (curObj && curObj->type == LILYDIRECTIVE)
        {
          lilydirective *lilyobj = (lilydirective *) curObj->object;
          postfix = lilyobj->postfix ? lilyobj->postfix->str : NULL;
          display = lilyobj->display ? lilyobj->display->str : NULL;
        }
      if (get_lily_directive (&postfix, &display, &locked))
        insert_lily_directive (postfix, display, locked, 8);
    }
  else
    {
      insert_lily_directive (postfix, display, locked, minpixels ? atoi (minpixels) : 8);
      g_print ("display helper for %s\n", postfix);
      displayhelper (gui);
    }
}

/**
 * callback for AttachLilyToNote (command name is historical)
 * note_directive DenemoDirective attach to chord.  The DenemoDirective is tagged with TAG
 * and attached the note below cursor in current chord
 * if one tagged with TAG already exists, it edits it with the passed values. 
 * Only postfix, prefix and display can be set with this call (for backward compatibility)
 */
void
note_directive (GtkAction * action, DenemoScriptParam * param)
{
  DenemoGUI *gui = Denemo.gui;
  GET_4PARAMS (action, param, postfix, display, prefix, tag);
  attach_directive (ATTACH_NOTE, postfix, prefix, display, tag, action != NULL);
}

/**
 * callback for AttachLilyToChord (command name is historical)
 * chord_directive DenemoDirective attach to chord.  The DenemoDirective is tagged with TAG
 * and attached the current chord
 * if one tagged with TAG already exists, it edits it with the passed values. 
 * Only postfix, prefix and display can be set with this call (for backward compatibility)
 */
void
chord_directive (GtkAction * action, DenemoScriptParam * param)
{
  DenemoGUI *gui = Denemo.gui;
  GET_4PARAMS (action, param, postfix, display, prefix, tag);
  attach_directive (ATTACH_CHORD, postfix, prefix, display, tag, action != NULL);
}



static DenemoObject *
get_chordobject (void)
{
  chord *thechord = NULL;
  DenemoObject *curObj = get_object ();
  if (curObj == NULL)
    return NULL;
  thechord = (chord *) curObj->object;
  if (curObj->type != CHORD)
    {
      return NULL;
    }
  return curObj;
}

static chord *
get_chord (void)
{
  chord *thechord = NULL;
  DenemoObject *curObj = get_chordobject ();
  if (curObj == NULL)
    return NULL;
  return (chord *) curObj->object;
}

static DenemoLilyControl *
get_score (void)
{
  return &Denemo.gui->lilycontrol;
}


static note *
get_note (void)
{
  DenemoGUI *gui = Denemo.gui;
  note *curnote = NULL;
  chord *thechord = NULL;
  DenemoObject *curObj = get_chordobject ();
  if (curObj == NULL)
    return NULL;
  return findnote (curObj, gui->si->cursor_y);
}


static DenemoStaff *
get_staff (void)
{
  if (Denemo.gui->si->currentstaff == NULL)
    return NULL;
  return Denemo.gui->si->currentstaff->data;
}

#define get_voice get_staff


//block for new type of directive
static clef *
get_clef (void)
{
  clef *ret = NULL;
  DenemoObject *curObj = get_object ();
  if (curObj && curObj->type == CLEF)
    {
      ret = ((clef *) curObj->object);
    }
  else
    {
      DenemoStaff *curstaff = get_staff ();
      if (curstaff)
        ret = &curstaff->clef;
    }
  return ret;
}

static DenemoDirective *
get_clef_directive (gchar * tag)
{
  clef *curclef = get_clef ();
  if (curclef == NULL || (curclef->directives == NULL))
    return NULL;
  return find_directive (curclef->directives, tag);
}

gboolean
delete_clef_directive (gchar * tag)
{
  clef *curclef = get_clef ();
  if (curclef == NULL || (curclef->directives == NULL))
    return FALSE;
  DenemoDirective *directive = get_clef_directive (tag);
  if (directive == NULL)
    return FALSE;
  return delete_directive (&curclef->directives, tag);
}

// end of block for new type of directive


static keysig *
get_keysig (void)
{
  keysig *ret = NULL;
  DenemoObject *curObj = get_object ();
  if (curObj && curObj->type == KEYSIG)
    {
      ret = ((keysig *) curObj->object);
    }
  else
    {
      DenemoStaff *curstaff = get_staff ();
      if (curstaff)
        ret = &curstaff->keysig;
    }
  return ret;
}

static DenemoDirective *
get_keysig_directive (gchar * tag)
{
  keysig *curkeysig = get_keysig ();
  if (curkeysig == NULL || (curkeysig->directives == NULL))
    return NULL;
  return find_directive (curkeysig->directives, tag);
}

gboolean
delete_keysig_directive (gchar * tag)
{
  keysig *curkeysig = get_keysig ();
  if (curkeysig == NULL || (curkeysig->directives == NULL))
    return FALSE;
  DenemoDirective *directive = get_keysig_directive (tag);
  if (directive == NULL)
    return FALSE;
  return delete_directive (&curkeysig->directives, tag);
}

static timesig *
get_timesig (void)
{
  timesig *ret = NULL;
  DenemoObject *curObj = get_object ();
  if (curObj && curObj->type == TIMESIG)
    {
      ret = ((timesig *) curObj->object);
    }
  else
    {
      DenemoStaff *curstaff = get_staff ();
      if (curstaff)
        ret = &curstaff->timesig;
    }
  return ret;
}

static DenemoDirective *
get_timesig_directive (gchar * tag)
{
  timesig *curtimesig = get_timesig ();
  if (curtimesig == NULL || (curtimesig->directives == NULL))
    return NULL;
  return find_directive (curtimesig->directives, tag);
}

gboolean
delete_timesig_directive (gchar * tag)
{
  timesig *curtimesig = get_timesig ();
  if (curtimesig == NULL || (curtimesig->directives == NULL))
    return FALSE;
  DenemoDirective *directive = get_timesig_directive (tag);
  if (directive == NULL)
    return FALSE;
  return delete_directive (&curtimesig->directives, tag);
}

static tuplet *
get_tuplet (void)
{
  tuplet *ret = NULL;
  DenemoObject *curObj = get_object ();
  if (curObj && (curObj->type == TUPOPEN || curObj->type == TUPCLOSE))
    {
      ret = ((tuplet *) curObj->object);
    }
  return ret;
}

static DenemoDirective *
get_tuplet_directive (gchar * tag)
{
  tuplet *curtuplet = get_tuplet ();
  if (curtuplet == NULL || (curtuplet->directives == NULL))
    return NULL;
  return find_directive (curtuplet->directives, tag);
}

gboolean
delete_tuplet_directive (gchar * tag)
{
  tuplet *curtuplet = get_tuplet ();
  if (curtuplet == NULL || (curtuplet->directives == NULL))
    return FALSE;
  DenemoDirective *directive = get_tuplet_directive (tag);
  if (directive == NULL)
    return FALSE;
  return delete_directive (&curtuplet->directives, tag);
}

static stemdirective *
get_stemdirective (void)
{
  stemdirective *ret = NULL;
  DenemoObject *curObj = get_object ();
  if (curObj && (curObj->type == STEMDIRECTIVE))
    {
      ret = ((stemdirective *) curObj->object);
    }
  return ret;
}

static DenemoDirective *
get_stemdirective_directive (gchar * tag)
{
  stemdirective *curstemdirective = get_stemdirective ();
  if (curstemdirective == NULL || (curstemdirective->directives == NULL))
    return NULL;
  return find_directive (curstemdirective->directives, tag);
}

gboolean
delete_stemdirective_directive (gchar * tag)
{
  stemdirective *curstemdirective = get_stemdirective ();
  if (curstemdirective == NULL || (curstemdirective->directives == NULL))
    return FALSE;
  DenemoDirective *directive = get_stemdirective_directive (tag);
  if (directive == NULL)
    return FALSE;
  return delete_directive (&curstemdirective->directives, tag);
}

static scoreheader *
get_scoreheader (void)
{
  return &Denemo.gui->scoreheader;
}

static DenemoDirective *
get_scoreheader_directive (gchar * tag)
{
  scoreheader *curscoreheader = get_scoreheader ();
  if (curscoreheader == NULL || (curscoreheader->directives == NULL))
    return NULL;
  return find_directive (curscoreheader->directives, tag);
}

gboolean
delete_scoreheader_directive (gchar * tag)
{
  scoreheader *curscoreheader = get_scoreheader ();
  if (curscoreheader == NULL || (curscoreheader->directives == NULL))
    return FALSE;
  DenemoDirective *directive = get_scoreheader_directive (tag);
  if (directive == NULL)
    return FALSE;
  return delete_directive (&curscoreheader->directives, tag);
}


static paper *
get_paper (void)
{
  return &Denemo.gui->paper;
}

static DenemoDirective *
get_paper_directive (gchar * tag)
{
  paper *curpaper = get_paper ();
  if (curpaper == NULL || (curpaper->directives == NULL))
    return NULL;
  return find_directive (curpaper->directives, tag);
}

gboolean
delete_paper_directive (gchar * tag)
{
  paper *curpaper = get_paper ();
  if (curpaper == NULL || (curpaper->directives == NULL))
    return FALSE;
  DenemoDirective *directive = get_paper_directive (tag);
  if (directive == NULL)
    return FALSE;
  return delete_directive (&curpaper->directives, tag);
}

static layout *
get_layout (void)
{
  return &Denemo.gui->si->layout;
}

static DenemoDirective *
get_layout_directive (gchar * tag)
{
  layout *curlayout = get_layout ();
  if (curlayout == NULL || (curlayout->directives == NULL))
    return NULL;
  return find_directive (curlayout->directives, tag);
}

gboolean
delete_layout_directive (gchar * tag)
{
  layout *curlayout = get_layout ();
  if (curlayout == NULL || (curlayout->directives == NULL))
    return FALSE;
  DenemoDirective *directive = get_layout_directive (tag);
  if (directive == NULL)
    return FALSE;
  return delete_directive (&curlayout->directives, tag);
}


static movementcontrol *
get_movementcontrol (void)
{
  return &Denemo.gui->si->movementcontrol;
}

static DenemoDirective *
get_movementcontrol_directive (gchar * tag)
{
  movementcontrol *curmovementcontrol = get_movementcontrol ();
  if (curmovementcontrol == NULL || (curmovementcontrol->directives == NULL))
    return NULL;
  return find_directive (curmovementcontrol->directives, tag);
}

gboolean
delete_movementcontrol_directive (gchar * tag)
{
  movementcontrol *curmovementcontrol = get_movementcontrol ();
  if (curmovementcontrol == NULL || (curmovementcontrol->directives == NULL))
    return FALSE;
  DenemoDirective *directive = get_movementcontrol_directive (tag);
  if (directive == NULL)
    return FALSE;
  return delete_directive (&curmovementcontrol->directives, tag);
}


static header *
get_header (void)
{
  return &Denemo.gui->si->header;
}

static DenemoDirective *
get_header_directive (gchar * tag)
{
  header *curheader = get_header ();
  if (curheader == NULL || (curheader->directives == NULL))
    return NULL;
  return find_directive (curheader->directives, tag);
}

gboolean
delete_header_directive (gchar * tag)
{
  header *curheader = get_header ();
  if (curheader == NULL || (curheader->directives == NULL))
    return FALSE;
  DenemoDirective *directive = get_header_directive (tag);
  if (directive == NULL)
    return FALSE;
  return delete_directive (&curheader->directives, tag);
}


static DenemoDirective *
get_note_directive (gchar * tag)
{
  note *curnote = get_note ();
  if (curnote == NULL || (curnote->directives == NULL))
    return NULL;
  return find_directive (curnote->directives, tag);
}


DenemoDirective *
get_note_directive_number (gint num)
{
  note *curnote = get_note ();
  if (curnote == NULL || (curnote->directives == NULL))
    return NULL;
  return find_directive_number (curnote->directives, num);
}

static DenemoDirective *
get_chord_directive (gchar * tag)
{
  DenemoObject *curObj = get_chordobject ();
  if (curObj == NULL)
    return NULL;
  chord *thechord = (chord *) curObj->object;
  if (thechord->directives == NULL)
    return NULL;
  return find_directive (thechord->directives, tag);
}

static DenemoDirective *
get_object_directive (gchar * tag)
{
  DenemoObject *curObj = get_object ();
  if (curObj == NULL)
    return NULL;
  if (curObj->directives == NULL)
    return NULL;
  return find_directive (curObj->directives, tag);
}

gboolean
delete_object_directive (gchar * tag)
{
  DenemoObject *curObj = get_object ();
  if (curObj == NULL)
    return FALSE;
  if (curObj->directives == NULL)
    return FALSE;
  return delete_directive (&curObj->directives, tag);
}

static DenemoDirective *
get_score_directive (gchar * tag)
{

  return find_directive (Denemo.gui->lilycontrol.directives, tag);
}

static DenemoDirective *
get_staff_directive (gchar * tag)
{
  if (Denemo.gui->si->currentstaff == NULL)
    return NULL;
  DenemoStaff *curstaff = Denemo.gui->si->currentstaff->data;
  //FIXME return NULL if not primary staff
  if (curstaff == NULL || curstaff->staff_directives == NULL)
    return NULL;
  return find_directive (curstaff->staff_directives, tag);
}

static DenemoDirective *
get_voice_directive (gchar * tag)
{
  if (Denemo.gui->si->currentstaff == NULL)
    return NULL;
  DenemoStaff *curstaff = Denemo.gui->si->currentstaff->data;
  if (curstaff == NULL || curstaff->voice_directives == NULL)
    return NULL;
  return find_directive (curstaff->voice_directives, tag);
}

gboolean
delete_staff_directive (gchar * tag)
{
  if (Denemo.gui->si->currentstaff == NULL)
    return FALSE;
  DenemoStaff *curstaff = Denemo.gui->si->currentstaff->data;
  if (curstaff == NULL || curstaff->staff_directives == NULL)
    return FALSE;
  return delete_directive (&curstaff->staff_directives, tag);
}


gboolean
delete_initialclef_directive (gchar * tag)
{
  if (Denemo.gui->si->currentstaff == NULL)
    return FALSE;
  DenemoStaff *curstaff = Denemo.gui->si->currentstaff->data;
  if (curstaff == NULL || curstaff->clef.directives == NULL)
    return FALSE;
  return delete_directive (&curstaff->clef.directives, tag);
}


gboolean
delete_voice_directive (gchar * tag)
{
  if (Denemo.gui->si->currentstaff == NULL)
    return FALSE;
  DenemoStaff *curstaff = Denemo.gui->si->currentstaff->data;
  if (curstaff == NULL || curstaff->voice_directives == NULL)
    return FALSE;
  return delete_directive (&curstaff->voice_directives, tag);
}

gboolean
delete_note_directive (gchar * tag)
{
  note *curnote = get_note ();
  if (curnote == NULL || (curnote->directives == NULL))
    return FALSE;
  DenemoDirective *directive = get_note_directive (tag);
  if (directive == NULL)
    return FALSE;
  return delete_directive (&curnote->directives, tag);
}

gboolean
delete_chord_directive (gchar * tag)
{
  DenemoObject *curObj = get_chordobject ();
  if (curObj == NULL)
    return FALSE;
  chord *thechord = (chord *) curObj->object;
  if (thechord->directives == NULL)
    return FALSE;
  DenemoDirective *directive = get_chord_directive (tag);
  if (directive == NULL)
    return FALSE;
  return delete_directive (&thechord->directives, tag);
}

gboolean
delete_score_directive (gchar * tagname)
{
  DenemoDirective *directive = get_score_directive (tagname);
  if (directive == NULL)
    return FALSE;
  return delete_directive (&Denemo.gui->lilycontrol.directives, tagname);
}


#define GET_TAG_FUNC(what)\
gchar *\
what##_directive_get_tag(gchar *tag) {\
  DenemoDirective *directive = get_##what##_directive(tag);\
  if(directive && directive->tag)\
    return directive->tag->str;\
  else directive = NULL;/* get_##what##_directive(NULL)*/;	\
  if(directive && directive->tag)\
    return directive->tag->str;\
  return NULL;\
}



#define GET_STR_FIELD_FUNC(what, field)\
gchar *\
what##_directive_get_##field(gchar *tag) {\
  DenemoDirective *directive = get_##what##_directive(tag);\
  if(directive && directive->field)\
    return directive->field->str;\
  return NULL;\
}
//typdefs to make the macros defined below pick up the right structure for staff, voice and score as chord & note do
typedef DenemoStaff staff;
typedef DenemoStaff voice;
typedef DenemoLilyControl score;
typedef DenemoObject object;


     //note I think you cannot change the graphic once you have set it.
#define PUT_GRAPHIC_NAME(what, directives) gboolean \
what##_directive_put_graphic(gchar *tag, gchar *value) {\
  what *current = get_##what();\
  if(current==NULL) return FALSE;\
  if(Denemo.gui->si->currentobject)\
  store_for_undo_change (Denemo.gui->si, Denemo.gui->si->currentobject->data);\
  if(current->directives==NULL)\
       create_directives (&current->directives, tag);\
  DenemoDirective *directive = get_##what##_directive(tag);\
  if(directive==NULL){\
    directive=new_directive(tag);\
    current->directives = g_list_append(current->directives, directive);\
    }\
  loadGraphicItem(value, (DenemoGraphic **)&directive->graphic);\
  if(directive->graphic_name)\
     g_string_assign(directive->graphic_name, value);\
  else\
      directive->graphic_name = g_string_new(value);\
  return TRUE;\
}
#define PUT_GRAPHIC(what) PUT_GRAPHIC_NAME(what, directives)
PUT_GRAPHIC (chord);
PUT_GRAPHIC (note);

PUT_GRAPHIC (keysig) PUT_GRAPHIC (timesig) PUT_GRAPHIC (tuplet) PUT_GRAPHIC (stemdirective)
#define PUT_STR_FIELD_FUNC_NAME(what, field, name)\
gboolean \
what##_directive_put_##field(gchar *tag, gchar *value) {\
  what *current = get_##what();\
  if(current==NULL) return FALSE;\
  if(Denemo.gui->si->currentobject)\
  store_for_undo_change (Denemo.gui->si, Denemo.gui->si->currentobject->data);\
  if(current->name==NULL)\
       create_directives (&current->name, tag);\
  DenemoDirective *directive = get_##what##_directive(tag);\
  if(directive==NULL){\
    directive=new_directive(tag);\
    current->name = g_list_append(current->name, directive);\
    }\
  if(directive->field)\
    g_string_assign(directive->field, value);\
  else\
    directive->field = g_string_new(value);\
  widget_for_directive(directive, (void(*)())what##_directive_put_graphic);\
  g_object_set_data(G_OBJECT(directive->widget), "directives-pointer", &current->name);\
  return TRUE;\
}
#define PUT_STR_FIELD_FUNC(what, field) PUT_STR_FIELD_FUNC_NAME(what, field, directives)
//#define PUT_STR_FIELD_FUNCS(what, field) PUT_STR_FIELD_FUNC_NAME(what, field, staff_directives)
//#define PUT_STR_FIELD_FUNCV(what, field) PUT_STR_FIELD_FUNC_NAME(what, field, voice_directives)
  GET_TAG_FUNC (object);
GET_TAG_FUNC (standalone);
GET_TAG_FUNC (chord);
GET_TAG_FUNC (note);
GET_TAG_FUNC (staff);
GET_TAG_FUNC (voice);
GET_TAG_FUNC (score);
GET_TAG_FUNC (clef);
GET_TAG_FUNC (timesig);
GET_TAG_FUNC (tuplet);
GET_TAG_FUNC (stemdirective);
GET_TAG_FUNC (keysig);
GET_TAG_FUNC (scoreheader);
GET_TAG_FUNC (header);
GET_TAG_FUNC (paper);
GET_TAG_FUNC (layout);
GET_TAG_FUNC (movementcontrol);


#undef GET_TAG_FUNC




GET_STR_FIELD_FUNC (score, midibytes) GET_STR_FIELD_FUNC (movementcontrol, midibytes) GET_STR_FIELD_FUNC (note, midibytes) GET_STR_FIELD_FUNC (chord, midibytes) GET_STR_FIELD_FUNC (staff, midibytes) GET_STR_FIELD_FUNC (voice, midibytes) GET_STR_FIELD_FUNC (standalone, midibytes) PUT_STR_FIELD_FUNC (note, midibytes) PUT_STR_FIELD_FUNC (chord, midibytes)
//cloned for grob
  GET_STR_FIELD_FUNC (score, grob) GET_STR_FIELD_FUNC (movementcontrol, grob) GET_STR_FIELD_FUNC (note, grob) GET_STR_FIELD_FUNC (chord, grob) GET_STR_FIELD_FUNC (staff, grob) GET_STR_FIELD_FUNC (voice, grob) GET_STR_FIELD_FUNC (clef, grob) GET_STR_FIELD_FUNC (timesig, grob) GET_STR_FIELD_FUNC (keysig, grob) GET_STR_FIELD_FUNC (tuplet, grob) GET_STR_FIELD_FUNC (stemdirective, grob) GET_STR_FIELD_FUNC (standalone, grob) PUT_STR_FIELD_FUNC (score, grob)
//PUT_STR_FIELD_FUNC(staff, grob)
//PUT_STR_FIELD_FUNC(voice, grob)
  PUT_STR_FIELD_FUNC (note, grob) PUT_STR_FIELD_FUNC (chord, grob) PUT_STR_FIELD_FUNC (clef, grob) PUT_STR_FIELD_FUNC (timesig, grob) PUT_STR_FIELD_FUNC (keysig, grob) PUT_STR_FIELD_FUNC (tuplet, grob) PUT_STR_FIELD_FUNC (stemdirective, grob)
//PUT_STR_FIELD_FUNC(standalone, grob)
//end of clone for grob
  GET_STR_FIELD_FUNC (chord, prefix)
GET_STR_FIELD_FUNC (chord, postfix)
GET_STR_FIELD_FUNC (chord, display)
PUT_STR_FIELD_FUNC (chord, prefix)
PUT_STR_FIELD_FUNC (chord, postfix)
PUT_STR_FIELD_FUNC (chord, display)
GET_STR_FIELD_FUNC (note, prefix)
GET_STR_FIELD_FUNC (note, postfix)
GET_STR_FIELD_FUNC (note, display)
PUT_STR_FIELD_FUNC (note, prefix)
PUT_STR_FIELD_FUNC (note, postfix)
PUT_STR_FIELD_FUNC (note, display) GET_STR_FIELD_FUNC (standalone, prefix) GET_STR_FIELD_FUNC (standalone, postfix) GET_STR_FIELD_FUNC (standalone, display) GET_STR_FIELD_FUNC (score, prefix) GET_STR_FIELD_FUNC (score, postfix) GET_STR_FIELD_FUNC (score, display) GET_STR_FIELD_FUNC (staff, prefix) GET_STR_FIELD_FUNC (staff, postfix) GET_STR_FIELD_FUNC (staff, display) GET_STR_FIELD_FUNC (voice, prefix) GET_STR_FIELD_FUNC (voice, postfix) GET_STR_FIELD_FUNC (voice, display)
#undef staff
#define PUT_INT_FIELD_FUNC_NAME(what, field, name)\
gboolean \
what##_directive_put_##field(gchar *tag, gint value) {\
  what *current = get_##what();\
  if(current==NULL) return FALSE;\
  if(Denemo.gui->si->currentobject)\
  store_for_undo_change (Denemo.gui->si, Denemo.gui->si->currentobject->data);\
  if(current->name==NULL)\
       create_directives (&current->name, tag);\
  DenemoDirective *directive = get_##what##_directive(tag);\
  if(directive==NULL){\
    directive=new_directive(tag);\
    current->name = g_list_append(current->name, directive);\
    }\
  directive->field = value;\
  widget_for_directive(directive, (void(*)())what##_directive_put_graphic);\
  g_object_set_data(G_OBJECT(directive->widget), "directives-pointer", &current->name);\
  return TRUE;\
}
#define PUT_INT_FIELD_FUNC(what, field)  PUT_INT_FIELD_FUNC_NAME(what, field, directives)
//#define PUT_INT_FIELD_FUNCS(what, field)  PUT_INT_FIELD_FUNC_NAME(what, field, staff_directives)
//#define PUT_INT_FIELD_FUNCV(what, field)  PUT_INT_FIELD_FUNC_NAME(what, field, voice_directives)
#define GET_INT_FIELD_FUNC(what, field)\
gint \
what##_directive_get_##field(gchar *tag) {\
  DenemoDirective *directive = get_##what##_directive(tag);\
  if(directive)\
    return directive->field;\
  return 0;\
}
#define GET_INT_GRAPHIC_FIELD_FUNC(what, field)\
gint \
what##_directive_get_##field(gchar *tag) {\
  DenemoDirective *directive = get_##what##_directive(tag);\
  if(directive)\
    return directive->graphic->field;\
  return 0;\
}
  PUT_GRAPHIC (object) PUT_INT_FIELD_FUNC (object, minpixels) GET_INT_FIELD_FUNC (object, minpixels)
  /* block which can be copied for new int fields */
  PUT_INT_FIELD_FUNC (note, minpixels) PUT_INT_FIELD_FUNC (chord, minpixels)
  //PUT_INT_FIELD_FUNCS(staff, minpixels)
  //PUT_INT_FIELD_FUNCV(voice, minpixels)
  //PUT_INT_FIELD_FUNC(score, minpixels)
  PUT_INT_FIELD_FUNC (clef, minpixels) PUT_INT_FIELD_FUNC (timesig, minpixels) PUT_INT_FIELD_FUNC (tuplet, minpixels) PUT_INT_FIELD_FUNC (stemdirective, minpixels) PUT_INT_FIELD_FUNC (keysig, minpixels)
  //PUT_INT_FIELD_FUNC(scoreheader, minpixels)
  //PUT_INT_FIELD_FUNC(header, minpixels)
  //PUT_INT_FIELD_FUNC(paper, minpixels)
  //PUT_INT_FIELD_FUNC(layout, minpixels)
  //PUT_INT_FIELD_FUNC(movementcontrol, minpixels)
  //standalone needs different code for "put" see STANDALONE_PUT* below
  GET_INT_FIELD_FUNC (note, minpixels)
GET_INT_FIELD_FUNC (chord, minpixels)
GET_INT_FIELD_FUNC (staff, minpixels)
GET_INT_FIELD_FUNC (voice, minpixels)
GET_INT_FIELD_FUNC (score, minpixels) GET_INT_FIELD_FUNC (clef, minpixels) GET_INT_FIELD_FUNC (keysig, minpixels) GET_INT_FIELD_FUNC (timesig, minpixels) GET_INT_FIELD_FUNC (tuplet, minpixels) GET_INT_FIELD_FUNC (stemdirective, minpixels) GET_INT_FIELD_FUNC (scoreheader, minpixels) GET_INT_FIELD_FUNC (header, minpixels) GET_INT_FIELD_FUNC (paper, minpixels) GET_INT_FIELD_FUNC (layout, minpixels) GET_INT_FIELD_FUNC (movementcontrol, minpixels) GET_INT_FIELD_FUNC (standalone, minpixels)
  /* end block which can be copied for new int fields */
  PUT_INT_FIELD_FUNC (note, override)
PUT_INT_FIELD_FUNC (chord, override)
GET_INT_FIELD_FUNC (note, override)
GET_INT_FIELD_FUNC (chord, override)
GET_INT_FIELD_FUNC (staff, override)
GET_INT_FIELD_FUNC (voice, override)
GET_INT_FIELD_FUNC (score, override)
PUT_INT_FIELD_FUNC (note, x)
PUT_INT_FIELD_FUNC (chord, x)
GET_INT_FIELD_FUNC (note, x)
GET_INT_FIELD_FUNC (chord, x)
GET_INT_FIELD_FUNC (staff, x)
GET_INT_FIELD_FUNC (voice, x)
PUT_INT_FIELD_FUNC (note, y)
PUT_INT_FIELD_FUNC (chord, y)
GET_INT_FIELD_FUNC (note, y)
GET_INT_FIELD_FUNC (chord, y)
GET_INT_FIELD_FUNC (staff, y)
GET_INT_FIELD_FUNC (voice, y)
PUT_INT_FIELD_FUNC (note, tx)
PUT_INT_FIELD_FUNC (chord, tx)
GET_INT_FIELD_FUNC (note, tx)
GET_INT_FIELD_FUNC (chord, tx)
GET_INT_FIELD_FUNC (staff, tx)
GET_INT_FIELD_FUNC (voice, tx)
PUT_INT_FIELD_FUNC (note, ty)
PUT_INT_FIELD_FUNC (chord, ty)
GET_INT_FIELD_FUNC (note, ty)
GET_INT_FIELD_FUNC (chord, ty)
GET_INT_FIELD_FUNC (staff, ty)
GET_INT_FIELD_FUNC (voice, ty)
PUT_INT_FIELD_FUNC (note, gx)
PUT_INT_FIELD_FUNC (chord, gx)
GET_INT_FIELD_FUNC (note, gx)
GET_INT_FIELD_FUNC (chord, gx)
GET_INT_FIELD_FUNC (staff, gx)
GET_INT_FIELD_FUNC (voice, gx)
PUT_INT_FIELD_FUNC (note, gy)
PUT_INT_FIELD_FUNC (chord, gy)
GET_INT_FIELD_FUNC (note, gy)
GET_INT_FIELD_FUNC (chord, gy)
GET_INT_FIELD_FUNC (staff, gy)
GET_INT_FIELD_FUNC (voice, gy) GET_INT_FIELD_FUNC (standalone, override) GET_INT_FIELD_FUNC (standalone, x) GET_INT_FIELD_FUNC (standalone, y) GET_INT_FIELD_FUNC (standalone, tx) GET_INT_FIELD_FUNC (standalone, ty) GET_INT_FIELD_FUNC (standalone, gx) GET_INT_FIELD_FUNC (standalone, gy) GET_INT_FIELD_FUNC (score, x) GET_INT_FIELD_FUNC (score, y) GET_INT_FIELD_FUNC (score, tx) GET_INT_FIELD_FUNC (score, ty) GET_INT_FIELD_FUNC (score, gx) GET_INT_FIELD_FUNC (score, gy)
  /* width and height of graphic (if any), read only */
  GET_INT_GRAPHIC_FIELD_FUNC (note, width)
GET_INT_GRAPHIC_FIELD_FUNC (chord, width) GET_INT_GRAPHIC_FIELD_FUNC (staff, width) GET_INT_GRAPHIC_FIELD_FUNC (voice, width) GET_INT_GRAPHIC_FIELD_FUNC (standalone, width) GET_INT_GRAPHIC_FIELD_FUNC (score, width) GET_INT_GRAPHIC_FIELD_FUNC (note, height) GET_INT_GRAPHIC_FIELD_FUNC (chord, height) GET_INT_GRAPHIC_FIELD_FUNC (staff, height) GET_INT_GRAPHIC_FIELD_FUNC (voice, height) GET_INT_GRAPHIC_FIELD_FUNC (standalone, height) GET_INT_GRAPHIC_FIELD_FUNC (score, height)
/* return a full path to an editscript for directive or NULL if there is none */
     static gchar *
     get_editscript_filename (gchar * tag)
{
  gchar *basename = g_strconcat (tag, ".scm", NULL);
  gchar *filename = g_build_filename (locatedotdenemo (), "actions", "editscripts", basename, NULL);
  if (!g_file_test (filename, G_FILE_TEST_EXISTS))
    {
      g_free (filename);
      filename = g_build_filename (get_data_dir (), "actions", "editscripts", basename, NULL);
      if (!g_file_test (filename, G_FILE_TEST_EXISTS))
        {
          g_free (filename);
          filename = g_build_filename (locatedotdenemo (), "download", "actions", "editscripts", basename, NULL);
          if (!g_file_test (filename, G_FILE_TEST_EXISTS))
            {
              g_free (filename);
              g_free (basename);
              return NULL;
            }
        }
    }
  g_free (basename);
  return filename;
}


/*
instead of the "activate"  "button-release-event" signal

gboolean            user_function                      (GtkWidget      *widget,
                                                        GdkEventButton *event,
                                                        gpointer        user_data)      : Run Last
                                                        * PROBLEM cannot use gtk_widget_activate ... d-DirectiveActivate ...
the look at event to see if left or right button pressed
and allow advanced edit if right button.

*/
static gboolean text_edit_directive_by_fn (DenemoDirective * directive, gpointer fn);

static void
button_callback (GtkWidget * widget, GdkEventButton * event, DenemoDirective * directive)
{
  // !!!!! clicking on a staff tools menu item comes thru here - but if you break gdb here as the menu item is still up your mouse is grabbed.
  gboolean left = TRUE;
  if (event != NULL)
    left = !((event->button != 1));
  if (left && (directive->override & DENEMO_OVERRIDE_EDITOR))
    {
      GtkWidget *texteditor = (GtkWidget *) g_object_get_data (G_OBJECT (directive->widget), DENEMO_TEXTEDITOR_TAG);
      if (texteditor)
        {
          //FIXME position at cursor if a toplevel window
          gtk_widget_show_all (gtk_widget_get_toplevel (texteditor));
          gtk_window_present (GTK_WINDOW (gtk_widget_get_toplevel (texteditor)));
        }
    }
  else
    {
      gchar *script = get_action_script (directive->tag->str);
      if (left && script)
        {
          stage_undo (Denemo.gui->si, ACTION_STAGE_END);        //undo is a queue so this is the end :)
          call_out_to_guile (script);
          stage_undo (Denemo.gui->si, ACTION_STAGE_START);
        }
      else
        {
          if (left && (directive->override & DENEMO_OVERRIDE_TAGEDIT))
            script = get_editscript_filename (directive->tag->str);
          else
            script = NULL;
          if (script)
            execute_script_file (script);
          else
            {
              /* if there is an action of this tag with scheme script, run it again
                 else do text edit of the directives fields
               */
              GtkAction *action;
              if (left && ((action = lookup_action_from_name ((gchar *) directive->tag->str)) != NULL) && (directive->override & DENEMO_OVERRIDE_TAGEDIT))
                gtk_action_activate (action);
              else
                {
                  if (left && action)
                    {
                      gchar *name = (gchar *) gtk_action_get_name (action);
                      gint idx = lookup_command_from_name (Denemo.map, name);
                      if (idx > 0)
                        {
                          gchar *label = (gchar *) lookup_label_from_idx (Denemo.map, idx);
                          if (confirm (label, _("Repeat the command?")))
                            gtk_action_activate (action);
                        }
                    }
                  else
                    {
                      gpointer fn = (widget != NULL) ? g_object_get_data (G_OBJECT (widget), "fn") : NULL;
                      if (fn)
                        {
                          gboolean delete = !text_edit_directive_by_fn (directive, fn);
                          if (delete)
                            {
                              GList **directives = (GList **) g_object_get_data (G_OBJECT (widget), "directives-pointer");
                              if (directives)
                                delete_directive (directives, directive->tag->str);
                              else
                                g_warning (_("Could not get directives list to delete from"));
                            }
                        }
                    }
                }
            }
        }
    }
}


static void
button_activate_callback (GtkWidget * w, DenemoDirective * d)
{
  button_callback (w, NULL, d);
}

/* return a GtkTextView which has been installed inside a scrolled window */
static GtkWidget *
create_text_window (void)
{
  GtkWidget *textview = gtk_text_view_new ();
  GtkWidget *w = gtk_window_new (GTK_WINDOW_TOPLEVEL);
  gtk_window_set_title (GTK_WINDOW (w), _("Denemo Editor:Newline to update, Esc for Advanced Edit"));
  gtk_window_set_default_size (GTK_WINDOW (w), 600, 400);
  gtk_window_set_position (GTK_WINDOW (w), GTK_WIN_POS_MOUSE);
  g_signal_connect (G_OBJECT (w), "delete-event", G_CALLBACK (gtk_widget_hide_on_delete), w);
  GtkWidget *main_vbox = gtk_vbox_new (FALSE, 1);
  gtk_container_add (GTK_CONTAINER (w), main_vbox);

  GtkWidget *sw = gtk_scrolled_window_new (NULL, NULL);
  gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (sw), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
  gtk_box_pack_start (GTK_BOX (main_vbox), sw, TRUE, TRUE, 0);
  gtk_container_add (GTK_CONTAINER (sw), textview);
  return textview;
}

static void
assign_text (GtkWidget * w, gchar * text)
{
  GtkTextBuffer *textbuffer = gtk_text_view_get_buffer (GTK_TEXT_VIEW (w));
  if (textbuffer)
    gtk_text_buffer_set_text (textbuffer, text, -1);
}

static gchar *
get_label_text (DenemoDirective * directive, gchar * text)
{
  if (directive->override & DENEMO_OVERRIDE_MARKUP)
    return g_strdup (text);
  return g_markup_escape_text (text, -1);
}

/* create a label. 
Use the display string up to the first newline, if it is long enough
else use tag
*/
static void
set_directive_graphic_label (DenemoDirective * directive)
{
  gchar *value;
  if (directive->display && directive->display->len > 0)
    value = get_label_text (directive, directive->display->str);
  else
    value = get_label_text (directive, directive->tag->str);
  gchar *c;
  for (c = value; *c; c++)
    if (*c == '\n')
      {
        *c = 0;
        break;
      }
  if (GTK_IS_MENU_ITEM (directive->widget))
    gtk_menu_item_set_label_text ((GtkMenuItem *) directive->widget, value);
  else
    gtk_label_set_markup ((GtkLabel *) gtk_bin_get_child (GTK_BIN (directive->widget)), value);
  g_free (value);
}

static gboolean text_edit_directive (DenemoDirective * directive, gchar * what);

static
editor_keypress (GtkWidget * w, GdkEventKey * event, DenemoDirective * directive)
{
  GtkTextIter startiter, enditer;
  GtkTextBuffer *textbuffer = gtk_text_view_get_buffer (GTK_TEXT_VIEW (w));
  gtk_text_buffer_get_start_iter (textbuffer, &startiter);
  gtk_text_buffer_get_end_iter (textbuffer, &enditer);
  gchar *text = gtk_text_buffer_get_text (textbuffer, &startiter, &enditer, FALSE);
  if (directive->display)
    g_string_assign (directive->display, text);
  else
    directive->display = g_string_new (text);
  //if the GdkEventKey is newline, run the editscript for the directive

  //FIXME use switch
  if (event->keyval == GDK_Escape)
    if (!text_edit_directive (directive, "unknown"))
      {
        /* I have used "unknown" here because we would need to get the name e.g. "score" "movementcontrol" etc from fn, but this is only used for the create a script thing...
           g_object_get_data(G_OBJECT(directive->widget), "fn") 
         */
        GList **directives_ptr = g_object_get_data (G_OBJECT (directive->widget), "directives-pointer");
        if (directives_ptr)
          delete_directive (g_object_get_data (G_OBJECT (directive->widget), "directives-pointer"), directive->tag->str);
        else
          warningdialog (_("Cannot delete via this mechanism, sorry"));
        return TRUE;
      }
  if (event->keyval == GDK_Return)
    {
      gchar *filename = get_editscript_filename (directive->tag->str);
      if (filename)
        execute_script_file (filename);
      set_directive_graphic_label (directive);
    }
  return TRUE;
}

static void
attach_textedit_widget (DenemoDirective * directive)
{
  if (directive->override & DENEMO_OVERRIDE_EDITOR)
    {
      GtkWidget *texteditor = create_text_window ();
      //    g_signal_connect (G_OBJECT (texteditor), "key-press-event",
      //                G_CALLBACK (editor_keypress), directive);
      g_signal_connect_after (G_OBJECT (texteditor), "key-release-event", G_CALLBACK (editor_keypress), directive);
      if (directive->display == NULL)
        directive->display = g_string_new ("");
      assign_text (texteditor, directive->display->str);
      // g_object_set_data(texteditor, "gstring", directive->display);
      g_object_set_data (G_OBJECT (directive->widget), DENEMO_TEXTEDITOR_TAG, texteditor);
    }
}

/*
  widget_for_directive()
  if directive does not have widget:

     creates a widget (button or menu depending on fn) for editing/actioning directive, point directive->widget to it and attach a callback to edit/action this directive, passing fn as data to it (to say what sort of directive it is) or the directive itself (for actionscripts/editscripts).

if directive is non-DenemoObject directive it  places the widget in the appropriate buttonbox/menu, the directives attached to DenemoObjects have menus created dynamically. (fn gives the type of directive: it determines where the widget goes (score or movement level, DenemoGUI or DenemoScore respectively, or in staff or voice menu))

     set  the label for the widget from directive->display or the tag if no display text
     set  the visibility for the widget from directive->override
*/

void
widget_for_directive_menu (DenemoDirective * directive, void fn (), GtkMenu * menu)
{
  GtkWidget *box;
  gchar *value = "";
  //FIXME we don't need value now...
  if (directive->override & DENEMO_OVERRIDE_EDITOR)
    {
      value = directive->tag->str;
    }
  else if (directive->display)
    value = directive->display->str;
  value = get_label_text (directive, value);
  if ((directive->widget == NULL))
    {
      //FIXME at this point you could allow the user to specify a custom button box for his directive - some property of the directive saying which button box it should be in. We could even allow the directive to create a toolitem of a toolbar or menuitem on a menu bar???

      if (fn == (void (*)()) score_directive_put_graphic || fn == (void (*)()) scoreheader_directive_put_graphic || fn == (void (*)()) paper_directive_put_graphic || fn == (void (*)()) layout_directive_put_graphic)
        box = Denemo.gui->buttonbox;
      else if (fn == (void (*)()) movementcontrol_directive_put_graphic || fn == (void (*)()) header_directive_put_graphic)
        box = Denemo.gui->si->buttonbox;
      else
        box = NULL;

      if ((fn == (void (*)()) staff_directive_put_graphic) || (fn == (void (*)()) voice_directive_put_graphic))
        {
          //g_print("Doing the staff or voice case");
          directive->widget = GTK_WIDGET (gtk_menu_item_new_with_label (value));        //WARNING _with_label is important
          attach_textedit_widget (directive);
          g_signal_connect (G_OBJECT (directive->widget), "button-release-event", G_CALLBACK (button_callback), directive);
          gtk_menu_shell_append (GTK_MENU_SHELL (menu), GTK_WIDGET (directive->widget));
        }
      else if (box)
        {
          //g_print("Doing the score and movement cases starting from %p", directive->widget);
          directive->widget = GTK_WIDGET (gtk_button_new_with_label (value));
          gchar *tooltip;
          const gchar *label = get_label_for_command(directive->tag->str);
          const gchar *help = get_tooltip_for_command(directive->tag->str);
          if(label)
          tooltip = g_strdup_printf (_("Command: %s.\n(%s)"
          "\nLeft click to run the command or right click for further options"), label, help?help:"");
          else
						tooltip = g_strdup_printf (_("This button was created for the Denemo Directive whose tag is %s."
          " Usually you click on it to alter the setting made or perform the action it is labelled with"), directive->tag->str);   //FIXME enable scripters to pass a tooltip in???
          gtk_widget_set_tooltip_text (directive->widget, tooltip);
          g_free (tooltip);

          {
            GtkWidget *label = gtk_bin_get_child (GTK_BIN (directive->widget));
            gtk_label_set_use_markup (GTK_LABEL (label), TRUE);
          }
          attach_textedit_widget (directive);
          g_signal_connect (G_OBJECT (directive->widget), /*"activate" we want to use gtk_widget_activate on this */ "button-release-event", G_CALLBACK (button_callback), directive);


          g_signal_connect (G_OBJECT (directive->widget), "activate", G_CALLBACK (button_activate_callback), directive);



          if (box)
            {
              gtk_box_pack_start (GTK_BOX (box), GTK_WIDGET (directive->widget), FALSE, TRUE, 0);
              gtk_widget_show (box);
            }
        }
      else
        {
          directive->widget = gtk_menu_item_new_with_label (value);
          attach_textedit_widget (directive);
          g_signal_connect (G_OBJECT (directive->widget), "button-release-event", G_CALLBACK (button_callback), directive);
        }
      g_object_set_data (G_OBJECT (directive->widget), "directive", (gpointer) directive);
      g_object_set_data (G_OBJECT (directive->widget), "fn", (gpointer) fn);
      //GTK_WIDGET_UNSET_FLAGS(directive->widget, GTK_CAN_FOCUS);
      gtk_widget_set_can_focus (directive->widget, FALSE);
    }                           //end of no widget

  (directive->override & DENEMO_OVERRIDE_GRAPHIC) ? gtk_widget_show (GTK_WIDGET (directive->widget)) : gtk_widget_hide (GTK_WIDGET (directive->widget));

  // here handle the case where widget has a GtkTextView editing the text in value
  if (directive->display)
    {
      GtkWidget *texteditor = (GtkWidget *) g_object_get_data (G_OBJECT (directive->widget), DENEMO_TEXTEDITOR_TAG);
      if (texteditor)
        assign_text (texteditor, directive->display->str);
    }
  set_directive_graphic_label (directive);
  g_free (value);
}

void
widget_for_directive (DenemoDirective * directive, void fn ())
{
  GtkMenu *menu = NULL;
  if (Denemo.gui->si)
    {
      if (fn == (void (*)()) staff_directive_put_graphic)
        {
          menu = ((DenemoStaff *) Denemo.gui->si->currentstaff->data)->staffmenu;
        }
      if (fn == (void (*)()) voice_directive_put_graphic)
        {
          menu = ((DenemoStaff *) Denemo.gui->si->currentstaff->data)->voicemenu;
        }
    }
  widget_for_directive_menu (directive, fn, menu);
}

void
widget_for_staff_directive (DenemoDirective * directive, GtkMenu * menu)
{
  return widget_for_directive_menu (directive, (void (*)()) staff_directive_put_graphic, menu);
}

void
widget_for_voice_directive (DenemoDirective * directive, GtkMenu * menu)
{
  return widget_for_directive_menu (directive, (void (*)()) voice_directive_put_graphic, menu);
}

void
widget_for_movementcontrol_directive (DenemoDirective * directive)
{
  return widget_for_directive_menu (directive, (void (*)()) movementcontrol_directive_put_graphic, NULL);
}

void
widget_for_header_directive (DenemoDirective * directive)
{
  return widget_for_directive_menu (directive, (void (*)()) header_directive_put_graphic, NULL);
}

void
widget_for_layout_directive (DenemoDirective * directive)
{
  return widget_for_directive_menu (directive, (void (*)()) layout_directive_put_graphic, NULL);
}

// create a directive for non-DenemoObject directive #what
// assigning the string VALUE to the field ##field
// also create a button or menuitem ( if it does not already exist) as the directive->widget, this will be used to edit/action the directive
// Compare this with the macros above which create the what##_directive_put_##field() without calling widget_for_directive() and so do not create a widget in the graphic field, except via the user setting graphic_name.

#define PUT_GRAPHIC_WIDGET_STR(field, what, name) \
gboolean \
what##_directive_put_##field(gchar *tag, gchar *value) {\
  what *current = get_##what();\
  if(current==NULL) return FALSE;\
  take_snapshot();     \
  if(current->name==NULL)\
       create_directives (&current->name, tag);\
  DenemoDirective *directive = get_##what##_directive(tag);\
  if(directive==NULL){\
    directive=new_directive(tag);\
    current->name = g_list_append(current->name, directive);\
    }\
  if(directive->field)\
    g_string_assign(directive->field, value);\
  else\
    directive->field = g_string_new(value);\
  widget_for_directive(directive, (void(*)())what##_directive_put_graphic);\
  g_object_set_data(G_OBJECT(directive->widget), "directives-pointer", (gpointer)&current->name); \
  return TRUE;\
}


#define PUT_GRAPHIC_WIDGET_INT(field, what, name)\
gboolean \
what##_directive_put_##field(gchar *tag, gint value) {\
  what *current = get_##what();\
  if(current==NULL) return FALSE;\
  take_snapshot();		 \
  if(current->name==NULL)\
       create_directives (&current->name, tag);\
  DenemoDirective *directive = get_##what##_directive(tag);\
  if(directive==NULL){\
    directive=new_directive(tag);\
    current->name = g_list_append(current->name, directive);\
    }\
  directive->field = value;\
  widget_for_directive(directive, (void(*)())what##_directive_put_graphic);\
  g_object_set_data(G_OBJECT(directive->widget), "directives-pointer", &current->name);\
  return TRUE;\
}


//As the above (for string and int) but for the graphic name field
//FIXME this is just storing the graphic name, any bitmap of that name could be placed on the button/menu item as an icon
#define PUT_GRAPHIC_WIDGET_GRAPHIC(what, name) gboolean \
what##_directive_put_graphic(gchar *tag, gchar *value) {\
  what *current = get_##what();\
  if(current==NULL) return FALSE;\
  take_snapshot();		\
  if(current->name==NULL)\
       create_directives (&current->name, tag);\
  DenemoDirective *directive = get_##what##_directive(tag);\
  if(directive==NULL){\
    directive=new_directive(tag);\
    current->name = g_list_append(current->name, directive);\
    }\
  if(directive->graphic_name==NULL) \
    directive->graphic_name = g_string_new(value);\
  else\
    g_string_assign(directive->graphic_name, value);\
  widget_for_directive(directive, (void(*)())what##_directive_put_graphic);\
  g_object_set_data(G_OBJECT(directive->widget), "directives-pointer", &current->name);\
  return (gboolean)directive;\
}

PUT_GRAPHIC_WIDGET_GRAPHIC (score, directives)
PUT_GRAPHIC_WIDGET_GRAPHIC (scoreheader, directives)
PUT_GRAPHIC_WIDGET_GRAPHIC (header, directives)
PUT_GRAPHIC_WIDGET_GRAPHIC (paper, directives)
PUT_GRAPHIC_WIDGET_GRAPHIC (layout, directives)
PUT_GRAPHIC_WIDGET_GRAPHIC (movementcontrol, directives)
PUT_GRAPHIC_WIDGET_GRAPHIC (staff, staff_directives)
PUT_GRAPHIC_WIDGET_GRAPHIC (voice, voice_directives)
PUT_GRAPHIC_WIDGET_STR (display, score, directives)
PUT_GRAPHIC_WIDGET_STR (display, scoreheader, directives)
PUT_GRAPHIC_WIDGET_STR (display, header, directives)
PUT_GRAPHIC_WIDGET_STR (display, paper, directives)
PUT_GRAPHIC_WIDGET_STR (display, layout, directives)
PUT_GRAPHIC_WIDGET_STR (display, movementcontrol, directives)
PUT_GRAPHIC_WIDGET_STR (display, staff, staff_directives)
PUT_GRAPHIC_WIDGET_STR (display, voice, voice_directives)
PUT_GRAPHIC_WIDGET_STR (prefix, score, directives)
PUT_GRAPHIC_WIDGET_STR (prefix, scoreheader, directives)
PUT_GRAPHIC_WIDGET_STR (prefix, header, directives)
PUT_GRAPHIC_WIDGET_STR (prefix, paper, directives)
PUT_GRAPHIC_WIDGET_STR (prefix, layout, directives)
PUT_GRAPHIC_WIDGET_STR (prefix, movementcontrol, directives)
PUT_GRAPHIC_WIDGET_STR (prefix, staff, staff_directives)
PUT_GRAPHIC_WIDGET_STR (prefix, voice, voice_directives)
PUT_GRAPHIC_WIDGET_STR (postfix, score, directives)
PUT_GRAPHIC_WIDGET_STR (postfix, scoreheader, directives)
PUT_GRAPHIC_WIDGET_STR (postfix, header, directives)
PUT_GRAPHIC_WIDGET_STR (postfix, paper, directives)
PUT_GRAPHIC_WIDGET_STR (postfix, layout, directives)
PUT_GRAPHIC_WIDGET_STR (postfix, movementcontrol, directives)
PUT_GRAPHIC_WIDGET_STR (postfix, staff, staff_directives)
PUT_GRAPHIC_WIDGET_STR (postfix, voice, voice_directives)
PUT_GRAPHIC_WIDGET_STR (midibytes, score, directives)
PUT_GRAPHIC_WIDGET_STR (midibytes, scoreheader, directives)
PUT_GRAPHIC_WIDGET_STR (midibytes, header, directives)
PUT_GRAPHIC_WIDGET_STR (midibytes, paper, directives)
PUT_GRAPHIC_WIDGET_STR (midibytes, layout, directives)
PUT_GRAPHIC_WIDGET_STR (midibytes, movementcontrol, directives)
PUT_GRAPHIC_WIDGET_STR (midibytes, staff, staff_directives)
PUT_GRAPHIC_WIDGET_STR (midibytes, voice, voice_directives)
PUT_GRAPHIC_WIDGET_INT (minpixels, score, directives)
PUT_GRAPHIC_WIDGET_INT (minpixels, scoreheader, directives)
PUT_GRAPHIC_WIDGET_INT (minpixels, header, directives)
PUT_GRAPHIC_WIDGET_INT (minpixels, paper, directives)
PUT_GRAPHIC_WIDGET_INT (minpixels, layout, directives)
PUT_GRAPHIC_WIDGET_INT (minpixels, movementcontrol, directives)
PUT_GRAPHIC_WIDGET_INT (minpixels, staff, staff_directives)
PUT_GRAPHIC_WIDGET_INT (minpixels, voice, voice_directives)
PUT_GRAPHIC_WIDGET_INT (x, score, directives)
PUT_GRAPHIC_WIDGET_INT (x, scoreheader, directives)
PUT_GRAPHIC_WIDGET_INT (x, header, directives)
PUT_GRAPHIC_WIDGET_INT (x, paper, directives)
PUT_GRAPHIC_WIDGET_INT (x, layout, directives)
PUT_GRAPHIC_WIDGET_INT (x, movementcontrol, directives)
PUT_GRAPHIC_WIDGET_INT (x, staff, staff_directives)
PUT_GRAPHIC_WIDGET_INT (x, voice, voice_directives)
PUT_GRAPHIC_WIDGET_INT (y, score, directives)
PUT_GRAPHIC_WIDGET_INT (y, scoreheader, directives)
PUT_GRAPHIC_WIDGET_INT (y, header, directives)
PUT_GRAPHIC_WIDGET_INT (y, paper, directives)
PUT_GRAPHIC_WIDGET_INT (y, layout, directives)
PUT_GRAPHIC_WIDGET_INT (y, movementcontrol, directives)
PUT_GRAPHIC_WIDGET_INT (y, staff, staff_directives)
PUT_GRAPHIC_WIDGET_INT (y, voice, voice_directives)
PUT_GRAPHIC_WIDGET_INT (tx, score, directives)
PUT_GRAPHIC_WIDGET_INT (tx, scoreheader, directives)
PUT_GRAPHIC_WIDGET_INT (tx, header, directives)
PUT_GRAPHIC_WIDGET_INT (tx, paper, directives)
PUT_GRAPHIC_WIDGET_INT (tx, layout, directives)
PUT_GRAPHIC_WIDGET_INT (tx, movementcontrol, directives)
PUT_GRAPHIC_WIDGET_INT (tx, staff, staff_directives)
PUT_GRAPHIC_WIDGET_INT (tx, voice, voice_directives)
PUT_GRAPHIC_WIDGET_INT (ty, score, directives)
PUT_GRAPHIC_WIDGET_INT (ty, scoreheader, directives)
PUT_GRAPHIC_WIDGET_INT (ty, header, directives)
PUT_GRAPHIC_WIDGET_INT (ty, paper, directives)
PUT_GRAPHIC_WIDGET_INT (ty, layout, directives)
PUT_GRAPHIC_WIDGET_INT (ty, movementcontrol, directives)
PUT_GRAPHIC_WIDGET_INT (ty, staff, staff_directives)
PUT_GRAPHIC_WIDGET_INT (ty, voice, voice_directives)
PUT_GRAPHIC_WIDGET_INT (gx, score, directives)
PUT_GRAPHIC_WIDGET_INT (gx, scoreheader, directives)
PUT_GRAPHIC_WIDGET_INT (gx, header, directives)
PUT_GRAPHIC_WIDGET_INT (gx, paper, directives)
PUT_GRAPHIC_WIDGET_INT (gx, layout, directives)
PUT_GRAPHIC_WIDGET_INT (gx, movementcontrol, directives)
PUT_GRAPHIC_WIDGET_INT (gx, staff, staff_directives)
PUT_GRAPHIC_WIDGET_INT (gx, voice, voice_directives)
PUT_GRAPHIC_WIDGET_INT (gy, score, directives)
PUT_GRAPHIC_WIDGET_INT (gy, scoreheader, directives)
PUT_GRAPHIC_WIDGET_INT (gy, header, directives)
PUT_GRAPHIC_WIDGET_INT (gy, paper, directives)
PUT_GRAPHIC_WIDGET_INT (gy, layout, directives)
PUT_GRAPHIC_WIDGET_INT (gy, movementcontrol, directives)
PUT_GRAPHIC_WIDGET_INT (gy, staff, staff_directives)
PUT_GRAPHIC_WIDGET_INT (gy, voice, voice_directives)
PUT_GRAPHIC_WIDGET_INT (override, score, directives) PUT_GRAPHIC_WIDGET_INT (override, scoreheader, directives) PUT_GRAPHIC_WIDGET_INT (override, header, directives) PUT_GRAPHIC_WIDGET_INT (override, paper, directives) PUT_GRAPHIC_WIDGET_INT (override, layout, directives) PUT_GRAPHIC_WIDGET_INT (override, movementcontrol, directives) PUT_GRAPHIC_WIDGET_INT (override, staff, staff_directives) PUT_GRAPHIC_WIDGET_INT (override, voice, voice_directives)
#undef PUT_GRAPHIC_WIDGET_STR
#undef PUT_GRAPHIC_WIDGET_INT
  gboolean
standalone_directive_put_graphic (gchar * tag, gchar * value)
{
  DenemoDirective *directive = get_standalone_directive (tag);
  if (directive && directive->graphic)
    {

      // directive->graphic = NULL; FIXME should we do this...
      //g_string_free(directive->graphic_name, TRUE);
    }
  if (!directive)
    {
      DenemoObject *obj = lily_directive_new (" ");
      directive = (DenemoDirective *) obj->object;
      directive->tag = g_string_new (tag);
      object_insert (Denemo.gui, obj);
      displayhelper (Denemo.gui);
    }
  if (loadGraphicItem (value, &directive->graphic))
    {
      if (directive->graphic_name)
        g_string_assign (directive->graphic_name, value);
      else
        directive->graphic_name = g_string_new (value);
      return TRUE;
    }
  else
    {
      directive->graphic = NULL;
      directive->graphic_name = NULL;
      return FALSE;
    }
}



#define STANDALONE_PUT_STR_FIELD_FUNC(field)\
gboolean \
standalone_directive_put_##field(gchar *tag, gchar *value) {\
  DenemoDirective *directive = get_standalone_directive(tag);\
  if(directive && directive->field){\
    store_for_undo_change (Denemo.gui->si, Denemo.gui->si->currentobject->data);\
    g_string_assign(directive->field, value);}				\
  else if(directive)\
    directive->field = g_string_new(value);\
  else {\
	DenemoObject *obj = lily_directive_new (" ");\
        directive = (DenemoDirective*)obj->object;\
        directive->tag = g_string_new(tag);\
        directive->field = g_string_new(value);\
	object_insert(Denemo.gui, obj);\
	displayhelper(Denemo.gui);\
   }\
  return TRUE;\
}

STANDALONE_PUT_STR_FIELD_FUNC (prefix);
STANDALONE_PUT_STR_FIELD_FUNC (postfix);
STANDALONE_PUT_STR_FIELD_FUNC (display);
STANDALONE_PUT_STR_FIELD_FUNC (midibytes);

STANDALONE_PUT_STR_FIELD_FUNC (grob);





#define STANDALONE_PUT_INT_FIELD_FUNC(field)\
gboolean \
standalone_directive_put_##field(gchar *tag, gint value) {\
  DenemoDirective *directive = get_standalone_directive(tag);\
  if(directive){\
    store_for_undo_change (Denemo.gui->si, Denemo.gui->si->currentobject->data);\
    directive->field = value;}\
  else {\
        DenemoObject *obj = lily_directive_new (" ");\
        directive = (DenemoDirective*)obj->object;\
        directive->tag = g_string_new(tag);\
        directive->field = value;\
	object_insert(Denemo.gui, obj);\
   }\
  return TRUE;\
}

//STANDALONE_PUT_INT_FIELD_FUNC(minpixels); special case
STANDALONE_PUT_INT_FIELD_FUNC (x);
STANDALONE_PUT_INT_FIELD_FUNC (y);
STANDALONE_PUT_INT_FIELD_FUNC (tx);
STANDALONE_PUT_INT_FIELD_FUNC (ty);
STANDALONE_PUT_INT_FIELD_FUNC (gx);
STANDALONE_PUT_INT_FIELD_FUNC (gy);

STANDALONE_PUT_INT_FIELD_FUNC (override);


gboolean
standalone_directive_put_minpixels (gchar * tag, gint value)
{
  DenemoDirective *directive = get_standalone_directive (tag);
  if (directive)
    {
      directive->minpixels = value;     //This field is not actually useful for standalone directives.
      DenemoObject *obj = get_object ();
      store_for_undo_change (Denemo.gui->si, obj);
      obj->minpixelsalloted = value;
    }
  else
    {
      DenemoObject *obj = lily_directive_new (" ");
      directive = (DenemoDirective *) obj->object;
      directive->tag = g_string_new (tag);
      obj->minpixelsalloted = directive->minpixels = value;
      object_insert (Denemo.gui, obj);
    }
  return TRUE;
}



static gboolean
tag_choice (GtkWidget * widget, DenemoDirective ** response)
{
  if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (widget)))
    *response = g_object_get_data (G_OBJECT (widget), "choice");
  return TRUE;
}

static void
tag_none (GtkWidget * widget, DenemoDirective ** response)
{
  *response = NULL;
}

#define UNKNOWN_TAG "<Unknown Tag>"

/* pack radio buttons for directive choice */
static gint
pack_buttons (GtkWidget * vbox, GList * directives, DenemoDirective ** response)
{
  GList *g;
  gint count;
  GtkWidget *widget = NULL, *widget2;
  for (count = 0, g = directives; g; g = g->next)
    {
      DenemoDirective *directive = (DenemoDirective *) g->data;
      if (directive->tag == NULL)
        directive->tag = g_string_new (UNKNOWN_TAG);
      count++;
      if (*response == NULL)
        *response = directive;
      if (widget == NULL)
        {
          widget = gtk_radio_button_new_with_label (NULL, directive->tag->str); //FIXME get_label_for_tag() and get_tooltip_for_tag() here!!!
          g_object_set_data (G_OBJECT (widget), "choice", directive);
          g_signal_connect (G_OBJECT (widget), "toggled", G_CALLBACK (tag_choice), response);
          gtk_box_pack_start (GTK_BOX (vbox), widget, FALSE, TRUE, 0);
        }
      else
        {
          widget2 = gtk_radio_button_new_with_label_from_widget (GTK_RADIO_BUTTON (widget), directive->tag->str);
          g_object_set_data (G_OBJECT (widget2), "choice", directive);
          g_signal_connect (G_OBJECT (widget2), "toggled", G_CALLBACK (tag_none), response);
          gtk_box_pack_start (GTK_BOX (vbox), widget2, FALSE, TRUE, 0);
        }
    }
  return count;
}


/* let the user choose from a list of directives */
static DenemoDirective *
select_directive (gchar * instr, GList * directives)
{

  GtkWidget *dialog = gtk_dialog_new_with_buttons (_("Select Directive"),
                                                   GTK_WINDOW (Denemo.window),
                                                   (GtkDialogFlags) (GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT),
                                                   GTK_STOCK_OK, GTK_RESPONSE_ACCEPT,
                                                   GTK_STOCK_CANCEL, GTK_RESPONSE_REJECT,
                                                   NULL);


  GtkWidget *vbox = gtk_vbox_new (FALSE, 8);
  GtkWidget *content_area = gtk_dialog_get_content_area (GTK_DIALOG (dialog));
  gtk_container_add (GTK_CONTAINER (content_area), vbox);

  DenemoDirective *response = NULL;

/*                                 
	void                user_function                      (GtkDialog *arg0,
                                                        gpointer   user_data)      : Action
	The ::close signal is a keybinding signal which gets emitted when the user uses a keybinding to close the dialog.
	The default binding for this signal is the Escape key. 
*/
  g_signal_connect (G_OBJECT (dialog), "close", G_CALLBACK (tag_none), &response);



  GList *g;
  gint count;                   //count tagged directives
  GtkWidget *widget, *widget2;
  widget = gtk_label_new (instr);
  gtk_box_pack_start (GTK_BOX (vbox), widget, FALSE, TRUE, 0);
  count = pack_buttons (vbox, directives, &response);

  if (count > 0)
    {
      gtk_widget_show_all (dialog);
      if (gtk_dialog_run (GTK_DIALOG (dialog)) == GTK_RESPONSE_REJECT)
        {
          response = NULL;
        }
    }
  gtk_widget_destroy (dialog);
  //if(response && response->tag)
  //g_print("Came back with response %s\n", response->tag->str);
  return response;
}


/* let the user choose from the directives at the cursor */
static void
user_select_directive_at_cursor (gchar ** what, GList *** pdirectives, DenemoDirective ** pdirective)
{
  *pdirectives = NULL;
  *pdirective = get_standalone_directive (NULL);

  if (*pdirective)
    return;                     //FIXME is this needed???? a return will be done anyway

  {
    gchar *instr = _("Select a directive attached to the tuplet marker");
    tuplet *curtuplet = get_tuplet ();
    if (curtuplet && curtuplet->directives)
      {
        *pdirectives = &curtuplet->directives;
        *what = "tuplet";
        *pdirective = select_directive (instr, **pdirectives);
      }

  }
  {
    gchar *instr = _("Select a directive attached to the stemdir marker");
    stemdirective *curstemdir = get_stemdirective ();
    if (curstemdir && curstemdir->directives)
      {
        *pdirectives = &curstemdir->directives;
        *what = "stemdir";
        *pdirective = select_directive (instr, **pdirectives);
      }

  }

  gchar *name = NULL;
  note *curnote = get_note ();
  if (curnote != NULL)
    {
      name = mid_c_offsettolily (curnote->mid_c_offset, curnote->enshift);
      if (curnote->mid_c_offset == Denemo.gui->si->cursor_y)
        if (curnote->directives)
          {
            *pdirectives = &curnote->directives;
            *what = "note";
            gchar *instr = g_strdup_printf (_("Select a directive attached to the note \"%s\""), name);
            *pdirective = select_directive (instr, **pdirectives);
            g_free (instr);
            if (*pdirective)
              {
                g_free (name);
                return;
              }
          }
    }


  {
    // not exactly on a note, offer any chord directives
    gchar *instr = _("Select a directive attached to the chord");
    chord *curchord = get_chord ();
    if (curchord && curchord->directives)
      {
        *pdirectives = &curchord->directives;
        *what = "chord";
        *pdirective = select_directive (instr, **pdirectives);
      }
  }
  if (*pdirective == NULL && curnote)   //try nearest note
    if (curnote->directives && curnote->mid_c_offset != Denemo.gui->si->cursor_y)
      {
        *pdirectives = &curnote->directives;
        *what = "note";
        gchar *instr = g_strdup_printf (_("Select a directive attached to the note \"%s\""), name);
        *pdirective = select_directive (instr, **pdirectives);
        g_free (instr);
        if (*pdirective && (g_list_length (**pdirectives) == 1))
          {
            /* seek confirmation of the choice of this directive since it is on a note not pointed at and
               has been chosen automatically. */
            gchar *name = mid_c_offsettolily (curnote->mid_c_offset, curnote->enshift);
            gchar *msg = g_strdup_printf (_("Select the directive %s on note \"%s\"?"), (*pdirective)->tag->str, name);

            if (!confirm (_("Select Directive"), msg))
              *pdirective = NULL;
            g_free (name);
            g_free (msg);
          }
      }
  g_free (name);
  return;
}


static void
populate_menu_for_directive (GtkWidget * menu, DenemoDirective * directive)
{
  GtkWidget *item = gtk_menu_item_new_with_label (directive->tag->str);
  gtk_menu_shell_prepend (GTK_MENU_SHELL (menu), GTK_WIDGET (item));
  g_signal_connect (G_OBJECT (item), "activate", G_CALLBACK (button_activate_callback), directive);
  gtk_widget_show (item);

}

static void
populate_menu_for_directives (GtkWidget * menu, GList * directives)
{
  g_object_set_data (G_OBJECT (menu), "directives", directives);
  //g_print("setting directives %p for menu %p\n", directives, menu);
  for (; directives; directives = directives->next)
    {
      populate_menu_for_directive (menu, directives->data);
    }
}

/* callback for deactivate signal installed at startup on the NoteEditPopup menu
   it removes the menu items for the specific note 
 */
gboolean
unpopulate_menu (GtkWidget * menu)
{
  GList *directives = g_object_get_data (G_OBJECT (menu), "directives");
  //g_print("removing directives %p for menu %p\n", directives, menu);
  for (; directives; directives = directives->next)
    {
      DenemoDirective *directive = directives->data;
      //g_print("now remove %p\n", directive->widget);
      if (directive->widget)
        gtk_container_remove (GTK_CONTAINER (menu), directive->widget);
    }
  g_object_set_data (G_OBJECT (menu), "directives", NULL);
  return FALSE;
}


void
edit_object (GtkAction * action, DenemoScriptParam * param)
{
  DenemoObject *obj = get_object ();
  if (obj == NULL)
    {
      warningmessage (_("No object here to edit"));
      return;
    }
  switch (obj->type)
    {
    case LILYDIRECTIVE:
      edit_object_directive (action, param);
      return;
    case CLEF:
      {
        popup_menu ("/ClefEditPopup");
      }
      return;
    case KEYSIG:
      {
        popup_menu ("/KeyEditPopup");
      }
      return;
    case TIMESIG:
      {
        popup_menu ("/TimeEditPopup");
      }
      return;
    case CHORD:
      {
        gboolean found = FALSE;
        GList *directives = ((chord *) obj->object)->directives;
        if (directives)
          found = TRUE;
        note *curnote = findnote (obj, Denemo.gui->si->cursor_y);
        if (curnote && (curnote->mid_c_offset == Denemo.gui->si->cursor_y))
          directives = curnote->directives;
        if (directives)
          found = TRUE;
        if (found)
          popup_menu ("/NoteEditPopupDirectives");
        else
          popup_menu ("/NoteEditPopup");
      }
      return;

    case STEMDIRECTIVE:
      {
        GList *directives = ((stemdirective *) obj->object)->directives;
        if (directives)
          {
            GtkWidget *menu = gtk_menu_new ();
            populate_menu_for_directives (menu, directives);
            gtk_menu_popup (GTK_MENU (menu), NULL, NULL, NULL, NULL, 0, gtk_get_current_event_time ());
          }
        else
          {
            infodialog (_("Nothing to edit on this stem directive - use controls in Staffs->Voices menu"));
          }
      }
      return;
    case TUPOPEN:
      popup_menu ("/TupletPopup");
      infodialog (_("This marks the start of a tuplet (that is triplets etc) - after the notes there should be a corresponding end of tuplet object.\nSee the Notes/Rests->Tuplets for control over how tuplets print"));
      return;
    case TUPCLOSE:
      popup_menu ("/TupletPopup");
      infodialog (_("This marks the end of a tuplet (that is triplets etc) - it should come in the same measure as the tuplet start marker.\nSee the Notes/Rests->Tuplets for control over how tuplets print"));
      return;
    default:
      warningdialog (_("No method for editing this type of object"));
      return;
    }
}

static gboolean
set_gstring (GtkWidget * widget, GdkEventKey * event, GString * gstr)
{
  g_string_assign (gstr, (gchar *) gtk_entry_get_text (GTK_ENTRY (widget)));
  return TRUE;
}

static gboolean
set_int (GtkSpinButton * widget, gint * val)
{
  *val = gtk_spin_button_get_value_as_int (widget);
  return TRUE;
}

static gboolean
set_uint (GtkSpinButton * widget, guint * val)
{
  *val = (guint) gtk_spin_button_get_value (widget);
  return TRUE;
}


static gchar *
quote_scheme (gchar * s)
{
  GString *dest = g_string_new ("");
  gchar *c;
  for (c = s; *c; c++)
    {
      if (*c == '"' || *c == '\\')
        g_string_append_c (dest, '\\');
      g_string_append_c (dest, *c);
    }
  return g_string_free (dest, FALSE);
}

static void
create_script (DenemoDirective * directive, gchar * what)
{
  GString *scheme = g_string_new (directive->tag->str);
  g_string_prepend (scheme, ";;;");
  g_string_append (scheme, "\n");
  if (what == NULL)
    {
      what = "standalone";
      g_string_append_printf (scheme, "(d-DirectivePut-standalone \"%s\")", directive->tag->str);
    }
#define ADD_TEXT(field)\
if(directive->field && directive->field->len)\
  {gchar *quote = quote_scheme(directive->field->str);\
   g_string_append_printf(scheme, "(d-DirectivePut-%s-%s \"%s\" \"%s\")\n",\
       what, #field, directive->tag->str, quote);\
   g_free(quote);}
  ADD_TEXT (prefix);
  ADD_TEXT (postfix);
  ADD_TEXT (display);
  ADD_TEXT (graphic_name);
  ADD_TEXT (midibytes);
#undef ADD_TEXT
#define ADD_INTTEXT(field)\
if(directive->field)\
  g_string_append_printf(scheme, "(d-DirectivePut-%s-%s \"%s\" %d)\n",\
       what, #field, directive->tag->str, directive->field);
  ADD_INTTEXT (minpixels);
  ADD_INTTEXT (override);
  ADD_INTTEXT (x);
  ADD_INTTEXT (y);
  ADD_INTTEXT (tx);
  ADD_INTTEXT (ty);
  ADD_INTTEXT (gx);
  ADD_INTTEXT (gy);

#undef ADD_INTTEXT
  g_string_append (scheme, "(d-RefreshDisplay)\n;;;End of scheme script");
  // quote_scheme(scheme);
  //g_print("Scheme is %s\n", scheme->str);
  appendSchemeText (scheme->str);
  g_string_free (scheme, TRUE);
}


/* callback to get an upload script of name tag */
static void
upload_edit_script_cb (GtkWidget * widget, gchar * tag)
{
  gchar *filename = get_editscript_filename (tag);
  if (filename)
    {
      GError *error = NULL;
      gchar *script;
      if (g_file_get_contents (filename, &script, NULL, &error))
        upload_edit_script (tag, script);
      g_free (script);
      g_free (filename);
    }
}

/* callback to get an edit script of name tag into the Scheme Script window */
static void
get_edit_script (GtkWidget * widget, gchar * tag)
{
  gchar *filename = get_editscript_filename (tag);
  if (filename)
    {
      GError *error = NULL;
      gchar *script;
      if (g_file_get_contents (filename, &script, NULL, &error))
        appendSchemeText (script);
      else
        g_warning (_("Could not get contents of %s\n"), filename);
      g_free (script);
      g_free (filename);
    }
}


/* callback to save the scheme script text buffer as an edit script of name tag in the user's local denemo directory */
static void
put_edit_script (GtkWidget * widget, gchar * tag)
{
  gchar *tagscm = g_strconcat (tag, ".scm", NULL);
  gchar *filename = g_build_filename (locatedotdenemo (), "actions", "editscripts", tagscm, NULL);
  if ((!g_file_test (filename, G_FILE_TEST_EXISTS)) || confirm (_("There is already an edit script for this tag"), _("Do you want to replace it?")))
    {
      gchar *scheme = (gchar *) getSchemeText ();
      if (scheme && *scheme)
        {
          FILE *fp = fopen (filename, "w");
          if (fp)
            {
              fprintf (fp, "%s", scheme);
              fclose (fp);
              infodialog (_("Wrote edit script file to ~/.denemo/editscripts"));
            }
          g_free (scheme);
        }
    }
  g_free (tagscm);
  g_free (filename);
}

static gboolean
activate_directive (DenemoDirective * directive, gchar * what)
{
  gboolean ret = TRUE;
  if (directive->widget && GTK_IS_WIDGET (directive->widget))
    {
      g_print ("activate\n");
      gtk_widget_activate (directive->widget);
      //g_signal_emit!!!!!!!!!!!!!!! what do we do!!!!!!!!!!!(directive->widget, "button-release-event");
      return TRUE;
    }
  return FALSE;
}

/* text_edit_directive
   textually edit the directive via a dialog.
   return FALSE if the user requests deletion of the directive.
*/
static gboolean
text_edit_directive (DenemoDirective * directive, gchar * what)
{
  gboolean ret = TRUE;
#define CREATE_SCRIPT (2)
  DenemoDirective *clone = clone_directive (directive); //for reset

  GtkWidget *dialog = gtk_dialog_new_with_buttons (_("Low Level Denemo Directive Edit"),
                                                   GTK_WINDOW (Denemo.window),
                                                   (GtkDialogFlags) (GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT),
                                                   GTK_STOCK_OK, GTK_RESPONSE_ACCEPT,
                                                   GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
                                                   NULL);
  gtk_dialog_add_button (GTK_DIALOG (dialog), _("Delete Directive"), GTK_RESPONSE_REJECT);
  gtk_dialog_add_button (GTK_DIALOG (dialog), _("Create Script"), CREATE_SCRIPT);



  GtkWidget *vbox = gtk_vbox_new (FALSE, 8);
  GtkWidget *content_area = gtk_dialog_get_content_area (GTK_DIALOG (dialog));
  gtk_container_add (GTK_CONTAINER (content_area), vbox);

  GtkWidget *hbox;
  GString *entrycontent = g_string_new ("");
  GtkWidget *entrywidget;
  GtkWidget *label;
  GtkWidget *button;
#define TEXTENTRY(thelabel, field) \
  GtkWidget *field;\
  hbox = gtk_hbox_new (FALSE, 8);\
  gtk_box_pack_start (GTK_BOX (vbox), hbox, FALSE, TRUE, 0);\
  label = gtk_label_new (_(thelabel));\
  gtk_misc_set_alignment (GTK_MISC (label), 1, 0.5);\
  gtk_box_pack_start (GTK_BOX (hbox), label, FALSE, FALSE, 0);\
  entrywidget = gtk_entry_new ();\
  g_string_sprintf (entrycontent, "%s", directive->field?directive->field->str:"");\
  gtk_entry_set_text (GTK_ENTRY (entrywidget), entrycontent->str);\
  gtk_box_pack_start (GTK_BOX (hbox), entrywidget, TRUE, TRUE, 0);\
  if(directive->field==NULL) directive->field=g_string_new("");\
  g_signal_connect(G_OBJECT(entrywidget), "key-release-event", G_CALLBACK(set_gstring), directive->field);\
  g_string_assign(entrycontent, "");

#define NEWINTENTRY(thelabel, field)\
  hbox = gtk_hbox_new (FALSE, 8);\
  gtk_box_pack_start (GTK_BOX (vbox), hbox, FALSE, TRUE, 0);\
  label = gtk_label_new (_(thelabel));\
  gtk_misc_set_alignment (GTK_MISC (label), 1, 0.5);\
  gtk_box_pack_start (GTK_BOX (hbox), label, FALSE, FALSE, 0);\
  entrywidget = gtk_spin_button_new_with_range (0.0, (gdouble)G_MAXUINT, 1.0);\
  gtk_spin_button_set_value (GTK_SPIN_BUTTON (entrywidget), directive->field);\
  gtk_box_pack_start (GTK_BOX (hbox), entrywidget, TRUE, TRUE, 0);\
  g_signal_connect(G_OBJECT(entrywidget), "value-changed", G_CALLBACK(set_int), &directive->field);
#define NEWUINTENTRY(thelabel, field)\
  hbox = gtk_hbox_new (FALSE, 8);\
  gtk_box_pack_start (GTK_BOX (vbox), hbox, FALSE, TRUE, 0);\
  label = gtk_label_new (_(thelabel));\
  gtk_misc_set_alignment (GTK_MISC (label), 1, 0.5);\
  gtk_box_pack_start (GTK_BOX (hbox), label, FALSE, FALSE, 0);\
  entrywidget = gtk_spin_button_new_with_range (0.0, (gdouble)G_MAXUINT, 1.0);\
  gtk_spin_button_set_value (GTK_SPIN_BUTTON (entrywidget), *(guint*)&directive->field);\
  gtk_box_pack_start (GTK_BOX (hbox), entrywidget, TRUE, TRUE, 0);\
  g_signal_connect(G_OBJECT(entrywidget), "value-changed", G_CALLBACK(set_uint), &directive->field);
#define ADDINTENTRY(thelabel, fieldx, fieldy)\
  label = gtk_label_new (_(thelabel));\
  gtk_misc_set_alignment (GTK_MISC (label), 1, 0.5);\
  gtk_box_pack_start (GTK_BOX (hbox), label, FALSE, FALSE, 0);\
  label = gtk_label_new (_(" x:"));\
  gtk_misc_set_alignment (GTK_MISC (label), 1, 0.5);\
  gtk_box_pack_start (GTK_BOX (hbox), label, FALSE, FALSE, 0);\
  entrywidget = gtk_spin_button_new_with_range (-(gdouble)G_MAXINT, (gdouble)G_MAXINT, 1.0);\
  gtk_spin_button_set_value (GTK_SPIN_BUTTON (entrywidget), directive->fieldx);\
  gtk_box_pack_start (GTK_BOX (hbox), entrywidget, TRUE, TRUE, 0);\
  g_signal_connect(G_OBJECT(entrywidget), "value-changed", G_CALLBACK(set_int), &directive->fieldx);\
  label = gtk_label_new (_(" y:"));\
  gtk_misc_set_alignment (GTK_MISC (label), 1, 0.5);\
  gtk_box_pack_start (GTK_BOX (hbox), label, FALSE, FALSE, 0);\
  entrywidget = gtk_spin_button_new_with_range (-(gdouble)G_MAXINT, (gdouble)G_MAXINT, 1.0);\
  gtk_spin_button_set_value (GTK_SPIN_BUTTON (entrywidget), directive->fieldy);\
  gtk_box_pack_start (GTK_BOX (hbox), entrywidget, TRUE, TRUE, 0);\
  g_signal_connect(G_OBJECT(entrywidget), "value-changed", G_CALLBACK(set_int), &directive->fieldy);

  TEXTENTRY (_("Postfix"), postfix);
  TEXTENTRY (_("Prefix"), prefix);
  TEXTENTRY (_("Display text"), display);
  ADDINTENTRY (_("Text Position"), tx, ty);
  TEXTENTRY (_("Graphic"), graphic_name);
  ADDINTENTRY (_("Graphic Position"), gx, gy);
  TEXTENTRY (_("Tag"), tag);
  TEXTENTRY (_("LilyPond Grob Name"), grob);
  TEXTENTRY (_("MidiBytes"), midibytes);
  NEWINTENTRY (_("Override Mask"), override);
  NEWINTENTRY (_("Minimum pixel width"), minpixels);
  NEWUINTENTRY (_("Only Applies to Layout"), y);
  NEWUINTENTRY (_("Ignored by Layout"), x);
#undef TEXTENTRY
  hbox = gtk_hbox_new (FALSE, 8);
  gtk_box_pack_start (GTK_BOX (vbox), hbox, FALSE, TRUE, 0);
  button = gtk_button_new_with_label (_("Get Edit Script"));
  gtk_box_pack_start (GTK_BOX (hbox), button, TRUE, TRUE, 0);
  g_signal_connect (G_OBJECT (button), "clicked", G_CALLBACK (get_edit_script), directive->tag->str);
  button = gtk_button_new_with_label (_("Put Edit Script"));
  gtk_box_pack_start (GTK_BOX (hbox), button, TRUE, TRUE, 0);
  g_signal_connect (G_OBJECT (button), "clicked", G_CALLBACK (put_edit_script), directive->tag->str);
#ifdef UPLOAD_TO_DENEMO_DOT_ORG
//disabled until website can take uploading again
  button = gtk_button_new_with_label (_("Upload Edit Script"));
  gtk_box_pack_start (GTK_BOX (hbox), button, TRUE, TRUE, 0);
  g_signal_connect (G_OBJECT (button), "clicked", G_CALLBACK (upload_edit_script_cb), directive->tag->str);
#endif
  button = gtk_check_button_new_with_label (_("Show Current Script"));
  gtk_box_pack_start (GTK_BOX (hbox), button, TRUE, TRUE, 0);
  gtk_activatable_set_related_action (GTK_ACTIVATABLE (button), gtk_ui_manager_get_action (Denemo.ui_manager, "/MainMenu/ViewMenu/ToggleScript"));

  gtk_widget_show_all (dialog);
  gint response = gtk_dialog_run (GTK_DIALOG (dialog));
  // g_print("Got response %d\n", response);


  if (response == GTK_RESPONSE_CANCEL || response == GTK_RESPONSE_DELETE_EVENT || response == GTK_RESPONSE_REJECT)
    {
      GtkWidget *ref_widget = directive->widget;        //not cloned
      directive->widget = NULL; //prevent any button being destroyed
      free_directive_data (directive);
      memcpy (directive, clone, sizeof (DenemoDirective));
      directive->widget = ref_widget;
      if (response == GTK_RESPONSE_REJECT)
        {
          ret = FALSE;          //that is it may be deleted, we ensure it has not been changed first,as the tag is used for delelet
        }
    }
  else
    {
      clone->widget = NULL;     //prevent any button being destroyed FIXME ???
      free_directive (clone);
      score_status (Denemo.gui, TRUE);
    }
#define REMOVEEMPTIES(field)\
if(directive->field && directive->field->len==0) g_string_free(directive->field, TRUE), directive->field=NULL;
  REMOVEEMPTIES (postfix);
  REMOVEEMPTIES (prefix);
  REMOVEEMPTIES (display);
  REMOVEEMPTIES (graphic_name);

  //REMOVEEMPTIES(tag); don't allow NULL tag
#undef REMOVEEMPTIES

  if (directive->tag && directive->tag->len == 0)
    directive->tag = g_string_new (UNKNOWN_TAG);
  if (directive->widget)
    {
      if (GTK_IS_WIDGET (directive->widget))
        widget_for_directive (directive, NULL /* no need to pass fn in as it is only needed if there is not widget, g_object_get_data(directive->widget, "fn") */ );
    }
  if (directive->graphic_name && directive->graphic)
    {
      loadGraphicItem (directive->graphic_name->str, (DenemoGraphic **) & directive->graphic);
    }
  gtk_widget_destroy (dialog);
  if (response == CREATE_SCRIPT)
    create_script (directive, what);    //g_print("(d-DirectivePut-%s \"%s\")\n", what, directive->tag->str);
  return ret;
}

#define TEXT_EDIT_IF(what)\
  if(fn == (void(*)())what##_directive_put_graphic)\
    return text_edit_directive(directive, #what);
static gboolean
text_edit_directive_by_fn (DenemoDirective * directive, gpointer fn)
{
  TEXT_EDIT_IF (note);
  TEXT_EDIT_IF (chord);
  TEXT_EDIT_IF (staff);
  TEXT_EDIT_IF (voice);
  TEXT_EDIT_IF (score);
  TEXT_EDIT_IF (clef);
  TEXT_EDIT_IF (timesig);
  TEXT_EDIT_IF (tuplet);
  TEXT_EDIT_IF (stemdirective);
  TEXT_EDIT_IF (keysig);
  TEXT_EDIT_IF (scoreheader);
  TEXT_EDIT_IF (header);
  TEXT_EDIT_IF (paper);
  TEXT_EDIT_IF (layout);
  TEXT_EDIT_IF (movementcontrol);
  TEXT_EDIT_IF (standalone);
  return TRUE;
}

#undef TEXT_EDIT_IF
/* allow edit of a directive, either via script or textually if no script exists 
   return FALSE if user confirms a request to delete the directive
*/
static gboolean
edit_directive (DenemoDirective * directive, gchar * what)
{
  gboolean ret = TRUE;
  gchar *filename = get_editscript_filename (directive->tag->str);
  if (filename == NULL)
    {
      GtkAction *action = lookup_action_from_name (directive->tag->str);
      if (action && (Denemo.keyboard_state != GDK_MOD2_MASK /*NumLock */ ))
        {                       //FIXME this should be detecting shift click surely????
          DenemoScriptParam param;
          gchar *paramvar;
          param.string = g_string_new ("edit");
          g_print ("Script can look for params \"edit\" - a string to catch this\n");
          activate_script (action, &param);
          g_string_free (param.string, TRUE);
        }
      else
        {
          ret = (text_edit_directive (directive, what) || !confirm (_("Directive Delete"), _("Are you sure you want to delete the directive?")));
          score_status (Denemo.gui, TRUE);
        }
      return ret;
    }
  GError *error = (GError *) execute_script_file (filename);
  if (error)
    g_warning ("%s", error->message);
  g_free (filename);
  return ret;
}


/**
 * callback for EditDirective on directive attached to an object.
 */
void
edit_object_directive (GtkAction * action, DenemoScriptParam * param)
{
  //g_print("Edit directive called\n");
  DenemoDirective *directive;
  GList **directives;
  gchar *what = NULL;
  user_select_directive_at_cursor (&what, &directives, &directive);
  //g_print("Got directive %p in list %p\n", directive, directives);
  if (directive == NULL)
    {
			if(*directives!=NULL)
				infodialog(_("Cancelled"));
			else
				warningdialog(_("Use the ObjectMenu to modify this object - there are no directives here"));
      return;
    }
  if (directive->tag == NULL)
    directive->tag = g_string_new (UNKNOWN_TAG);
  if (!(param ? text_edit_directive (directive, what) : edit_directive (directive, what)))
    {
      if (directives && *directives)
        {
          delete_directive (directives, directive->tag->str);
        }
      else
        {                       //standalone directive
          dnm_deleteobject (Denemo.gui->si);
        }
    }
  if (Denemo.gui->si->currentobject)    //for standalone directive
    setpixelmin (Denemo.gui->si->currentobject->data);
}

/**
 * callback for DeleteDirective 
 */
void
delete_chord_or_note_directive (GtkAction * action, DenemoScriptParam * param)
{
  //g_print("Edit directive called\n");
  DenemoDirective *directive;
  GList **directives;
  gchar *what = NULL;
  user_select_directive_at_cursor (&what, &directives, &directive);
  //g_print("Got directive %p in list %p\n", directive, directives);
  if (directives == NULL)
    {
      warningdialog (_("No directives here"));
      return;
    }
  if (directive == NULL)
    {
      warningdialog (_("No directive selected"));
      return;
    }
  if (directive->tag == NULL)
    directive->tag = g_string_new (UNKNOWN_TAG);
  if (confirm (_("Directive Delete"), _("Are you sure you want to delete the directive?")))
    delete_directive (directives, directive->tag->str);
  else
    warningdialog (_("Operation cancelled"));
}



static DenemoDirective *
select_score_directive (void)
{
  if (Denemo.gui->lilycontrol.directives == NULL)
    return NULL;
  return select_directive (_("Select a score directive"), Denemo.gui->lilycontrol.directives);
}

static DenemoDirective *
select_scoreheader_directive (void)
{
  if (Denemo.gui->scoreheader.directives == NULL)
    return NULL;
  return select_directive (_("Select a score header block directive"), Denemo.gui->scoreheader.directives);
}

static DenemoDirective *
select_paper_directive (void)
{
  if (Denemo.gui->paper.directives == NULL)
    return NULL;
  return select_directive (_("Select a score paper block directive"), Denemo.gui->paper.directives);
}


static DenemoDirective *
select_header_directive (void)
{
  if (Denemo.gui->si->header.directives == NULL)
    return NULL;
  return select_directive (_("Select a movement header block directive"), Denemo.gui->si->header.directives);
}

static DenemoDirective *
select_layout_directive (void)
{
  if (Denemo.gui->si->layout.directives == NULL)
    return NULL;
  return select_directive (_("Select a movement layout block directive"), Denemo.gui->si->layout.directives);
}

static DenemoDirective *
select_movementcontrol_directive (void)
{
  if (Denemo.gui->si->movementcontrol.directives == NULL)
    return NULL;
  return select_directive (_("Select a movement control directive"), Denemo.gui->si->movementcontrol.directives);
}

static DenemoDirective *
select_clef_directive (void)
{
  clef *curclef = get_clef ();
  if (curclef == NULL || curclef->directives == NULL)
    return NULL;
  return select_directive (_("Select a clef directive"), curclef->directives);
}

static DenemoDirective *
select_keysig_directive (void)
{
  keysig *curkeysig = get_keysig ();
  if (curkeysig == NULL || curkeysig->directives == NULL)
    return NULL;
  return select_directive (_("Select a key signature directive"), curkeysig->directives);
}

static DenemoDirective *
select_timesig_directive (void)
{
  timesig *curtimesig = get_timesig ();
  if (curtimesig == NULL || curtimesig->directives == NULL)
    return NULL;
  return select_directive (_("Select a time signature directive"), curtimesig->directives);
}

static DenemoDirective *
select_tuplet_directive (void)
{
  tuplet *curtuplet = get_tuplet ();
  if (curtuplet == NULL || curtuplet->directives == NULL)
    return NULL;
  return select_directive (_("Select a time signature directive"), curtuplet->directives);
}

static DenemoDirective *
select_stemdirective_directive (void)
{
  stemdirective *curstemdirective = get_stemdirective ();
  if (curstemdirective == NULL || curstemdirective->directives == NULL)
    return NULL;
  return select_directive (_("Select a time signature directive"), curstemdirective->directives);
}

static DenemoDirective *
select_staff_directive (void)
{
  if (Denemo.gui->si->currentstaff == NULL)
    return NULL;
  DenemoStaff *curstaff = Denemo.gui->si->currentstaff->data;
  //FIXME return NULL if not primary staff
  if (curstaff == NULL || curstaff->staff_directives == NULL)
    return NULL;
  return select_directive (_("Select a staff directive"), curstaff->staff_directives);
}

static DenemoDirective *
select_voice_directive (void)
{
  if (Denemo.gui->si->currentstaff == NULL)
    return NULL;
  DenemoStaff *curstaff = Denemo.gui->si->currentstaff->data;
  if (curstaff == NULL || curstaff->voice_directives == NULL)
    return NULL;
  return select_directive (_("Select a voice directive"), curstaff->voice_directives);
}


/**
 * callback for EditVoiceDirective 
 */
void
edit_voice_directive (GtkAction * action, DenemoScriptParam * param)
{
  //g_print("Edit directive called\n");
  DenemoDirective *directive = select_voice_directive ();
  //g_print("Got directive %p\n", directive);
  if (directive == NULL)
    return;
  if (directive->tag == NULL)
    directive->tag = g_string_new (UNKNOWN_TAG);
  if (!edit_directive (directive, "voice"))
    delete_voice_directive (directive->tag->str);
  score_status (Denemo.gui, TRUE);
}

/**
 * callback for EditStaffDirective 
 */
void
edit_staff_directive (GtkAction * action, DenemoScriptParam * param)
{
  //g_print("Edit directive called\n");
  DenemoDirective *directive = select_staff_directive ();
  //g_print("Got directive %p\n", directive);
  if (directive == NULL)
    return;
  if (directive->tag == NULL)
    directive->tag = g_string_new (UNKNOWN_TAG);
  if (!edit_directive (directive, "staff"))
    delete_staff_directive (directive->tag->str);
  score_status (Denemo.gui, TRUE);
}

/**
 * callback for EditClefDirective 
 */
void
edit_clef_directive (GtkAction * action, DenemoScriptParam * param)
{
  //g_print("Edit directive called\n");
  DenemoDirective *directive = select_clef_directive ();
  //g_print("Got directive %p\n", directive);
  if (directive == NULL)
    return;
  if (directive->tag == NULL)
    directive->tag = g_string_new (UNKNOWN_TAG);
  if (!edit_directive (directive, "clef"))
    delete_clef_directive (directive->tag->str);
  score_status (Denemo.gui, TRUE);
}

/**
 * callback for EditKeysigDirective 
 */
void
edit_keysig_directive (GtkAction * action, DenemoScriptParam * param)
{
  //g_print("Edit directive called\n");
  DenemoDirective *directive = select_keysig_directive ();
  //g_print("Got directive %p\n", directive);
  if (directive == NULL)
    return;
  if (directive->tag == NULL)
    directive->tag = g_string_new (UNKNOWN_TAG);
  if (!edit_directive (directive, "keysig"))
    delete_keysig_directive (directive->tag->str);
  score_status (Denemo.gui, TRUE);
}


/**
 * callback for EditTimesigDirective 
 */
void
edit_timesig_directive (GtkAction * action, DenemoScriptParam * param)
{
  //g_print("Edit directive called\n");
  DenemoDirective *directive = select_timesig_directive ();
  //g_print("Got directive %p\n", directive);
  if (directive == NULL)
    return;
  if (directive->tag == NULL)
    directive->tag = g_string_new (UNKNOWN_TAG);
  if (!edit_directive (directive, "timesig"))
    delete_timesig_directive (directive->tag->str);
  score_status (Denemo.gui, TRUE);
}

/**
 * callback for EditTupletDirective 
 */
void
edit_tuplet_directive (GtkAction * action, DenemoScriptParam * param)
{
  //g_print("Edit directive called\n");
  DenemoDirective *directive = select_tuplet_directive ();
  //g_print("Got directive %p\n", directive);
  if (directive == NULL)
    return;
  if (directive->tag == NULL)
    directive->tag = g_string_new (UNKNOWN_TAG);
  if (!edit_directive (directive, "tuplet"))
    delete_tuplet_directive (directive->tag->str);
  score_status (Denemo.gui, TRUE);
}

/**
 * callback for EditStemdirectiveDirective 
 */
void
edit_stemdirective_directive (GtkAction * action, DenemoScriptParam * param)
{
  //g_print("Edit directive called\n");
  DenemoDirective *directive = select_stemdirective_directive ();
  //g_print("Got directive %p\n", directive);
  if (directive == NULL)
    return;
  if (directive->tag == NULL)
    directive->tag = g_string_new (UNKNOWN_TAG);
  if (!edit_directive (directive, "stemdirective"))
    delete_stemdirective_directive (directive->tag->str);
  score_status (Denemo.gui, TRUE);
}

/**
 * callback for EditScoreDirective 
 */
void
edit_score_directive (GtkAction * action, DenemoScriptParam * param)
{
#define ScoreDirectives  _("ScoreDirectives")
#define ScoreHeaderBlockDirectives  _("Score Header Block Directives")
#define ScorePaperBlockDirectives  _("Score Paper Block Directives")
#define HeaderBlockDirectives  _("Movement Header Block Directives")
#define LayoutBlockDirectives  _("Layout Block Directives")

#define STRINGAPPEND(field)  g_string_append_printf(options,"%s%c", field,'\0')
  GString *options = g_string_new ("");
  gchar *option;
  if (Denemo.gui->lilycontrol.directives)
    STRINGAPPEND (ScoreDirectives);
  if (Denemo.gui->scoreheader.directives)
    STRINGAPPEND (ScoreHeaderBlockDirectives);
  if (Denemo.gui->paper.directives)
    STRINGAPPEND (ScorePaperBlockDirectives);
  if (Denemo.gui->si->header.directives)
    STRINGAPPEND (HeaderBlockDirectives);
  if (Denemo.gui->si->layout.directives)
    STRINGAPPEND (LayoutBlockDirectives);

  if (strlen (options->str) != options->len)
    {
      option = get_option (options->str, options->len);
      if (option == NULL)
        {
          g_string_free (options, TRUE);
          return;
        }
    }
  else
    option = options->str;
#define EDITTYPE(type, what)\
  if(!strcmp(option, type)) {\
    DenemoDirective *directive = select_##what##_directive();\
    if(directive==NULL)\
      return;\
    if(directive->tag == NULL)\
      directive->tag = g_string_new(UNKNOWN_TAG);\
    if(!edit_directive(directive, #what))\
      delete_##what##_directive(directive->tag->str);\
  score_status (Denemo.gui, TRUE);\
  }


  EDITTYPE (ScoreDirectives, score);
  EDITTYPE (ScoreHeaderBlockDirectives, scoreheader);
  EDITTYPE (ScorePaperBlockDirectives, paper);
  EDITTYPE (HeaderBlockDirectives, header);
  EDITTYPE (LayoutBlockDirectives, layout);

  //  g_print("option was %s\n",option);
  g_string_free (options, TRUE);
#undef EDITTYPE
#undef STRINGAPPEND
}



/**
 * callback for EditMovementDirective 
 */
void
edit_movement_directive (GtkAction * action, DenemoScriptParam * param)
{
#define LayoutDirectives  _("Layout Directives")
#define MovementDirectives  _("Movement Directives")
#define HeaderBlockDirectives  _("Movement Header Block Directives")

#define STRINGAPPEND(field)  g_string_append_printf(options,"%s%c", field,'\0')
  GString *options = g_string_new ("");
  gchar *option;
  if (Denemo.gui->si->layout.directives)
    STRINGAPPEND (LayoutDirectives);
  if (Denemo.gui->si->movementcontrol.directives)
    STRINGAPPEND (MovementDirectives);

  if (Denemo.gui->si->header.directives)
    STRINGAPPEND (HeaderBlockDirectives);

  if (strlen (options->str) != options->len)
    {
      option = get_option (options->str, options->len);
      if (option == NULL)
        {
          g_string_free (options, TRUE);
          return;
        }
    }
  else
    option = options->str;
#define EDITTYPE(type, what)\
  if(!strcmp(option, type)) {\
    DenemoDirective *directive = select_##what##_directive();\
    if(directive==NULL)\
      return;\
    if(directive->tag == NULL)\
      directive->tag = g_string_new(UNKNOWN_TAG);\
    if(!edit_directive(directive, #what))\
      delete_##what##_directive(directive->tag->str);\
  score_status (Denemo.gui, TRUE);\
  }



  EDITTYPE (HeaderBlockDirectives, header);
  EDITTYPE (LayoutDirectives, layout);
  EDITTYPE (MovementDirectives, movementcontrol);

  g_string_free (options, TRUE);
#undef EDITTYPE
#undef STRINGAPPEND
}




/* block which can be copied for type of directive (minpixels is done as sample for new int fields */
PUT_INT_FIELD_FUNC (clef, x)
PUT_INT_FIELD_FUNC (clef, y)
PUT_INT_FIELD_FUNC (clef, tx)
PUT_INT_FIELD_FUNC (clef, ty)
PUT_INT_FIELD_FUNC (clef, gx)
PUT_INT_FIELD_FUNC (clef, gy)
PUT_INT_FIELD_FUNC (clef, override)
GET_INT_FIELD_FUNC (clef, x)
GET_INT_FIELD_FUNC (clef, y) GET_INT_FIELD_FUNC (clef, tx) GET_INT_FIELD_FUNC (clef, ty) GET_INT_FIELD_FUNC (clef, gx) GET_INT_FIELD_FUNC (clef, gy) GET_INT_FIELD_FUNC (clef, override) GET_INT_GRAPHIC_FIELD_FUNC (clef, width) GET_INT_GRAPHIC_FIELD_FUNC (clef, height) PUT_GRAPHIC (clef) PUT_STR_FIELD_FUNC (clef, prefix) PUT_STR_FIELD_FUNC (clef, postfix) PUT_STR_FIELD_FUNC (clef, display) GET_STR_FIELD_FUNC (clef, prefix) GET_STR_FIELD_FUNC (clef, postfix) GET_STR_FIELD_FUNC (clef, display)
/* end block which can be copied for type of directive */
  PUT_INT_FIELD_FUNC (keysig, x)
PUT_INT_FIELD_FUNC (keysig, y)
PUT_INT_FIELD_FUNC (keysig, tx)
PUT_INT_FIELD_FUNC (keysig, ty)
PUT_INT_FIELD_FUNC (keysig, gx)
PUT_INT_FIELD_FUNC (keysig, gy)
PUT_INT_FIELD_FUNC (keysig, override)
GET_INT_FIELD_FUNC (keysig, x)
GET_INT_FIELD_FUNC (keysig, y)
GET_INT_FIELD_FUNC (keysig, tx)
GET_INT_FIELD_FUNC (keysig, ty)
GET_INT_FIELD_FUNC (keysig, gx)
GET_INT_FIELD_FUNC (keysig, gy)
GET_INT_FIELD_FUNC (keysig, override)
GET_INT_GRAPHIC_FIELD_FUNC (keysig, width)
GET_INT_GRAPHIC_FIELD_FUNC (keysig, height)
PUT_STR_FIELD_FUNC (keysig, prefix)
PUT_STR_FIELD_FUNC (keysig, postfix)
PUT_STR_FIELD_FUNC (keysig, display)
GET_STR_FIELD_FUNC (keysig, prefix)
GET_STR_FIELD_FUNC (keysig, postfix)
GET_STR_FIELD_FUNC (keysig, display)
PUT_INT_FIELD_FUNC (timesig, x)
PUT_INT_FIELD_FUNC (timesig, y)
PUT_INT_FIELD_FUNC (timesig, tx)
PUT_INT_FIELD_FUNC (timesig, ty)
PUT_INT_FIELD_FUNC (timesig, gx)
PUT_INT_FIELD_FUNC (timesig, gy)
PUT_INT_FIELD_FUNC (timesig, override)
GET_INT_FIELD_FUNC (timesig, x)
GET_INT_FIELD_FUNC (timesig, y)
GET_INT_FIELD_FUNC (timesig, tx)
GET_INT_FIELD_FUNC (timesig, ty)
GET_INT_FIELD_FUNC (timesig, gx)
GET_INT_FIELD_FUNC (timesig, gy)
GET_INT_FIELD_FUNC (timesig, override)
GET_INT_GRAPHIC_FIELD_FUNC (timesig, width)
GET_INT_GRAPHIC_FIELD_FUNC (timesig, height)
PUT_STR_FIELD_FUNC (timesig, prefix)
PUT_STR_FIELD_FUNC (timesig, postfix)
PUT_STR_FIELD_FUNC (timesig, display)
GET_STR_FIELD_FUNC (timesig, prefix)
GET_STR_FIELD_FUNC (timesig, postfix)
GET_STR_FIELD_FUNC (timesig, display)
PUT_INT_FIELD_FUNC (tuplet, x)
PUT_INT_FIELD_FUNC (tuplet, y)
PUT_INT_FIELD_FUNC (tuplet, tx)
PUT_INT_FIELD_FUNC (tuplet, ty)
PUT_INT_FIELD_FUNC (tuplet, gx)
PUT_INT_FIELD_FUNC (tuplet, gy)
PUT_INT_FIELD_FUNC (tuplet, override)
GET_INT_FIELD_FUNC (tuplet, x)
GET_INT_FIELD_FUNC (tuplet, y)
GET_INT_FIELD_FUNC (tuplet, tx)
GET_INT_FIELD_FUNC (tuplet, ty)
GET_INT_FIELD_FUNC (tuplet, gx)
GET_INT_FIELD_FUNC (tuplet, gy)
GET_INT_FIELD_FUNC (tuplet, override)
GET_INT_GRAPHIC_FIELD_FUNC (tuplet, width)
GET_INT_GRAPHIC_FIELD_FUNC (tuplet, height)
PUT_STR_FIELD_FUNC (tuplet, prefix)
PUT_STR_FIELD_FUNC (tuplet, postfix)
PUT_STR_FIELD_FUNC (tuplet, display)
GET_STR_FIELD_FUNC (tuplet, prefix)
GET_STR_FIELD_FUNC (tuplet, postfix)
GET_STR_FIELD_FUNC (tuplet, display)
PUT_INT_FIELD_FUNC (stemdirective, x)
PUT_INT_FIELD_FUNC (stemdirective, y)
PUT_INT_FIELD_FUNC (stemdirective, tx)
PUT_INT_FIELD_FUNC (stemdirective, ty)
PUT_INT_FIELD_FUNC (stemdirective, gx)
PUT_INT_FIELD_FUNC (stemdirective, gy)
PUT_INT_FIELD_FUNC (stemdirective, override)
GET_INT_FIELD_FUNC (stemdirective, x)
GET_INT_FIELD_FUNC (stemdirective, y)
GET_INT_FIELD_FUNC (stemdirective, tx)
GET_INT_FIELD_FUNC (stemdirective, ty)
GET_INT_FIELD_FUNC (stemdirective, gx)
GET_INT_FIELD_FUNC (stemdirective, gy)
GET_INT_FIELD_FUNC (stemdirective, override)
GET_INT_GRAPHIC_FIELD_FUNC (stemdirective, width)
GET_INT_GRAPHIC_FIELD_FUNC (stemdirective, height)
PUT_STR_FIELD_FUNC (stemdirective, prefix)
PUT_STR_FIELD_FUNC (stemdirective, postfix)
PUT_STR_FIELD_FUNC (stemdirective, display)
GET_STR_FIELD_FUNC (stemdirective, prefix)
GET_STR_FIELD_FUNC (stemdirective, postfix)
GET_STR_FIELD_FUNC (stemdirective, display)
GET_INT_FIELD_FUNC (scoreheader, x)
GET_INT_FIELD_FUNC (scoreheader, y)
GET_INT_FIELD_FUNC (scoreheader, tx)
GET_INT_FIELD_FUNC (scoreheader, ty)
GET_INT_FIELD_FUNC (scoreheader, gx)
GET_INT_FIELD_FUNC (scoreheader, gy)
GET_INT_FIELD_FUNC (scoreheader, override)
GET_INT_GRAPHIC_FIELD_FUNC (scoreheader, width)
GET_INT_GRAPHIC_FIELD_FUNC (scoreheader, height)
GET_STR_FIELD_FUNC (scoreheader, prefix)
GET_STR_FIELD_FUNC (scoreheader, postfix)
GET_STR_FIELD_FUNC (scoreheader, display)
GET_INT_FIELD_FUNC (header, x)
GET_INT_FIELD_FUNC (header, y)
GET_INT_FIELD_FUNC (header, tx)
GET_INT_FIELD_FUNC (header, ty)
GET_INT_FIELD_FUNC (header, gx)
GET_INT_FIELD_FUNC (header, gy)
GET_INT_FIELD_FUNC (header, override)
GET_INT_GRAPHIC_FIELD_FUNC (header, width)
GET_INT_GRAPHIC_FIELD_FUNC (header, height)
GET_STR_FIELD_FUNC (header, prefix)
GET_STR_FIELD_FUNC (header, postfix)
GET_STR_FIELD_FUNC (header, display)
GET_INT_FIELD_FUNC (paper, x)
GET_INT_FIELD_FUNC (paper, y)
GET_INT_FIELD_FUNC (paper, tx)
GET_INT_FIELD_FUNC (paper, ty)
GET_INT_FIELD_FUNC (paper, gx)
GET_INT_FIELD_FUNC (paper, gy)
GET_INT_FIELD_FUNC (paper, override)
GET_INT_GRAPHIC_FIELD_FUNC (paper, width)
GET_INT_GRAPHIC_FIELD_FUNC (paper, height)
GET_STR_FIELD_FUNC (paper, prefix)
GET_STR_FIELD_FUNC (paper, postfix)
GET_STR_FIELD_FUNC (paper, display)
GET_INT_FIELD_FUNC (layout, x)
GET_INT_FIELD_FUNC (layout, y)
GET_INT_FIELD_FUNC (layout, tx)
GET_INT_FIELD_FUNC (layout, ty)
GET_INT_FIELD_FUNC (layout, gx)
GET_INT_FIELD_FUNC (layout, gy)
GET_INT_FIELD_FUNC (layout, override)
GET_INT_GRAPHIC_FIELD_FUNC (layout, width)
GET_INT_GRAPHIC_FIELD_FUNC (layout, height)
GET_STR_FIELD_FUNC (layout, prefix)
GET_STR_FIELD_FUNC (layout, postfix)
GET_STR_FIELD_FUNC (layout, display)
GET_INT_FIELD_FUNC (movementcontrol, x)
GET_INT_FIELD_FUNC (movementcontrol, y) GET_INT_FIELD_FUNC (movementcontrol, tx) GET_INT_FIELD_FUNC (movementcontrol, ty) GET_INT_FIELD_FUNC (movementcontrol, gx) GET_INT_FIELD_FUNC (movementcontrol, gy) GET_INT_FIELD_FUNC (movementcontrol, override) GET_INT_GRAPHIC_FIELD_FUNC (movementcontrol, width) GET_INT_GRAPHIC_FIELD_FUNC (movementcontrol, height) GET_STR_FIELD_FUNC (movementcontrol, prefix) GET_STR_FIELD_FUNC (movementcontrol, postfix) GET_STR_FIELD_FUNC (movementcontrol, display)
#undef STANDALONE_PUT_INT_FIELD_FUNC
#undef PUT_GRAPHIC
#undef PUT_INT_FIELD_FUNC
#undef GET_INT_FIELD_FUNC
#undef PUT_STR_FIELD_FUNC
#undef GET_STR_FIELD_FUNC
     gchar *
     get_scoretitle (void)
{
  gchar *scoretitle = NULL;
  GList *first = Denemo.gui->movements;
  if (first)
    {
      DenemoScore *si = (DenemoScore *) first->data;
      if (si)
        {
          DenemoDirective *directive = find_directive (si->header.directives, "Movement-title");
          if (directive && directive->display)
            scoretitle = directive->display->str;
        }
    }
  return scoretitle;
}

#define ACTIVATE_DIRECTIVE(what)\
gboolean activate_##what##_directive(gchar *tag) {\
  DenemoDirective *directive = get_##what##_directive(tag);\
  if(directive)\
    return activate_directive(directive, #what);\
  return FALSE;\
}
#define TEXT_EDIT_DIRECTIVE(what)\
gboolean text_edit_##what##_directive(gchar *tag) {\
  DenemoDirective *directive = get_##what##_directive(tag);\
  if(directive)\
    return text_edit_directive(directive, #what);\
  return FALSE;\
}\
ACTIVATE_DIRECTIVE(what)

TEXT_EDIT_DIRECTIVE (note);
TEXT_EDIT_DIRECTIVE (chord);
TEXT_EDIT_DIRECTIVE (staff);
TEXT_EDIT_DIRECTIVE (voice);
TEXT_EDIT_DIRECTIVE (score);
TEXT_EDIT_DIRECTIVE (clef);
TEXT_EDIT_DIRECTIVE (timesig);
TEXT_EDIT_DIRECTIVE (tuplet);
TEXT_EDIT_DIRECTIVE (stemdirective);
TEXT_EDIT_DIRECTIVE (keysig);
TEXT_EDIT_DIRECTIVE (scoreheader);
TEXT_EDIT_DIRECTIVE (header);
TEXT_EDIT_DIRECTIVE (paper);
TEXT_EDIT_DIRECTIVE (layout);
TEXT_EDIT_DIRECTIVE (movementcontrol);
TEXT_EDIT_DIRECTIVE (standalone);


#undef TEXT_EDIT_DIRECTIVE

#define GET_NTH_TAG(what, name)\
gchar *get_nth_##what##_tag(gint n) {\
  what *current = get_##what();\
  if(current==NULL) return NULL;\
   GList *g = g_list_nth(current->name, n);\
  if(g==NULL) return NULL;\
  DenemoDirective *directive = (DenemoDirective *)g->data;\
  return directive->tag->str;}

GET_NTH_TAG (note, directives);
GET_NTH_TAG (chord, directives);
GET_NTH_TAG (staff, staff_directives);
GET_NTH_TAG (voice, voice_directives);
GET_NTH_TAG (score, directives);
GET_NTH_TAG (clef, directives);
GET_NTH_TAG (timesig, directives);
GET_NTH_TAG (tuplet, directives);
GET_NTH_TAG (stemdirective, directives);
GET_NTH_TAG (keysig, directives);
GET_NTH_TAG (scoreheader, directives);
GET_NTH_TAG (header, directives);
GET_NTH_TAG (paper, directives);
GET_NTH_TAG (layout, directives);
GET_NTH_TAG (movementcontrol, directives);
#undef GET_NTH_TAG
