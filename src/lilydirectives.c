/* lilydirectives.cpp 
 * Implements lilydirectives which are not notes 
 *
 * for Denemo, a gtk+ frontend to GNU Lilypond
 * A Tee  (c) 2000-2005
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <denemo/denemo.h>
#include "chordops.h"
#include "calculatepositions.h"
#include "commandfuncs.h"
#include "contexts.h"
#include "dialogs.h"
#include "draw.h"
#include "objops.h"
#include "staffops.h"
#include "utils.h"



/**
 * If the curObj is a chord with a note(s)
 * return the first note at or below cursory, or the last note 
 * else return NULL
 */
static note *
findnote(DenemoObject *curObj, gint cursory) {
  note *curnote = NULL;
  if (curObj && curObj->type == CHORD && ((chord *) curObj->object)->notes ) {
    GList *notes = ((chord *) curObj->object)->notes;
    for(;notes; notes = notes->next){
      curnote =  (note*)notes->data;
      //g_print("comparing %d and %d\n", cursory, curnote->y);
      if(cursory <= curnote->mid_c_offset)
	break;
   }

  }
     return curnote;
}

static void  toggle_locked(GtkWidget *widget, gboolean *locked) {
  //g_print("Called with %d\n", *locked);
  *locked = !*locked;
}

/* lookup a directive tagged with TAG in a list DIRECTIVES and return it.
   if TAG is NULL return the first directive
   else return NULL */
static DenemoDirective *find_directive(GList *directives, gchar *tag) {
  DenemoDirective *directive = NULL;
  if(tag) {
    GList *g;
    for(g=directives;g;g=g->next){
      directive = (DenemoDirective *)g->data;
      if(directive->tag && !strcmp(tag, directive->tag->str))
	return directive;
      directive = NULL;
    }
  } else
    directive = (DenemoDirective *)directives->data;
  return directive;
}

static DenemoDirective*
new_directive(gchar *tag){
  DenemoDirective *directive = (DenemoDirective*)g_malloc0(sizeof(DenemoDirective));
  if(tag)
    directive->tag = g_string_new(tag);
  return directive;
}

static  DenemoObject *findobj(void) {
  DenemoGUI *gui = Denemo.gui;
  DenemoScore * si = gui->si;
  note *curnote = NULL;
  return (DenemoObject *) si->currentobject ?
    (DenemoObject *) si->currentobject->data : NULL;
  }

typedef enum attach_type {ATTACH_NOTE, ATTACH_CHORD} attach_type;
/**
 * Denemo directive attach or edit.  
if interactive: Allows user to attach a lilypond directive 
else attache the passed strings as lilypond directive
attachment is to chord ( attach is ATTACH_CHORD) or to the note at the cursor
 */
static void
attach_directive (attach_type attach, gchar *postfix, gchar *prefix, gchar *display, gchar *tag, gboolean interactive)
{
  gchar *prefixstring=NULL, *postfixstring=NULL, *displaystring=NULL;
  DenemoGUI *gui = Denemo.gui;
  DenemoScore * si = gui->si;
  note *curnote = NULL;
  DenemoObject *curObj = findobj();
  if(curObj==NULL) {  
    if(interactive)
      warningdialog("You must put the cursor on a chord to attach LilyPond");//FIXME find a note and ask
    return;
  }
  chord *thechord = NULL;
  thechord = (chord *)curObj->object;
  if(curObj->type!=CHORD) {  
    if(interactive)
      warningdialog("You must put the cursor on a chord to attach LilyPond");
    return;
  }
    
  curnote = findnote(curObj, gui->si->cursor_y);
  if(attach==ATTACH_NOTE && (curnote==NULL)) {  
    if(interactive)
      warningdialog("You must put the cursor on a note to attach LilyPond to the note");//FIXME find a note and ask
    return;
  }
 
  // setup directive to be data from thechord->directives or curnote->directives which has matching tag, or first if tag is NULL.
  DenemoDirective *directive=NULL;
    switch(attach) {
    case  ATTACH_CHORD:
      if(thechord->directives==NULL) {
	directive = new_directive(tag);
	thechord->directives = g_list_append(NULL, directive);
      } else {
	directive = find_directive(thechord->directives, tag);
	if(directive == NULL) {
	  if(tag) {
	    directive = new_directive(tag);
	    thechord->directives = g_list_append(thechord->directives, directive);
	  }
	}
      }
      break;
     case  ATTACH_NOTE:
      if(curnote->directives==NULL) {
	directive = new_directive(tag);
	curnote->directives = g_list_append(NULL, directive);
      } else {
	directive = find_directive(curnote->directives, tag);
	if(directive == NULL) {
	  if(tag) {
	    directive = new_directive(tag);
	    curnote->directives = g_list_append(curnote->directives, directive);
	  }
	}
      }
      break;
    default:
      g_warning("Error in attach type");
      return;
    }

  if(interactive) {
      if(directive->postfix)
	postfixstring = directive->postfix->str;
      if(directive->prefix)
	prefixstring = directive->prefix->str;
      if(directive->display)
	displaystring = directive->display->str;

    prefixstring = string_dialog_entry(gui, "Attach LilyPond", "Give text to place before the note", prefixstring);
    postfixstring = string_dialog_entry(gui, curnote?"Attach LilyPond to Note":"Attach LilyPond to Chord", curnote?"Give LilyPond text to postfix to note of chord":"Give LilyPond text to postfix to chord", postfixstring);
    displaystring =  string_dialog_entry(gui, "Attach LilyPond", "Give Display text if required", displaystring);
  } else {//not interactive
    if(prefix)
      prefixstring = g_strdup(prefix);
    if(postfix)
      postfixstring = g_strdup(postfix);
    if(display)
      displaystring = g_strdup(display);
  }

#define STRINGASSIGN(field, val) \
     if(val && *val) {\
     if(directive->field) g_string_assign(directive->field, val);\
     else directive->field=g_string_new(val);}
    STRINGASSIGN(postfix, postfixstring);
    STRINGASSIGN(prefix, prefixstring);
    STRINGASSIGN(display, displaystring);

#undef STRINGASSIGN

  score_status(gui, TRUE);
  displayhelper (gui);
  g_free(postfixstring);
  g_free(displaystring);
  g_free(prefixstring);
}

static void create_directives(GList **directives, gchar *tag) {
  *directives = g_list_append(NULL, new_directive(tag));
}
static void
get_lily_parameter(gchar *query, DenemoScriptParam *param) {
  DenemoObject *curObj = (DenemoObject *) Denemo.gui->si->currentobject ?
    (DenemoObject *) Denemo.gui->si->currentobject->data : NULL;
  param->status = curObj && curObj->type==LILYDIRECTIVE;
#define ASSIGN_PARAM(field)  if(!strcmp(#field, query))\
  g_string_assign(param->string, lilyobj->field->str);
  if(param->status)
    {
      lilydirective *lilyobj = (lilydirective *) curObj->object;
      ASSIGN_PARAM(directive);
      ASSIGN_PARAM(display);
      if(!strcmp("minpixels", query))
	g_string_printf(param->string, "%d", curObj->minpixelsalloted);
    }
#undef ASSIGN_PARAM
}


static
void insert_lily_directive(gchar *directive, gchar *display, gboolean locked, gint minpixels) {
  DenemoGUI *gui = Denemo.gui;
  DenemoScore *si = gui->si;
  DenemoObject *lily;
  lilydirective *lilyobj=NULL; /* a lily directive object */
  DenemoObject *curObj = (DenemoObject *) si->currentobject ?
    (DenemoObject *) si->currentobject->data : NULL;
  gboolean is_new = FALSE;
    if (curObj && curObj->type == LILYDIRECTIVE) {
      g_string_assign((lilyobj=(lilydirective *) curObj->object)->directive, directive);
      curObj->minpixelsalloted = minpixels;
    }  else {  
      lily = lily_directive_new (directive);
      is_new= TRUE;
      lilyobj = (lilydirective *) lily->object;
      lily->minpixelsalloted = minpixels;// g_print("min pixels %d\n", lily->minpixelsalloted);
    }
    if(lilyobj) {
      lilyobj->locked = locked;
      if(*directive=='%') {//append newline if directive starts with a LilyPond comment indicator
	g_string_append(lilyobj->directive,"\n");
      }
      if(display) {
	if(lilyobj->display)
	  g_string_assign(lilyobj->display, display);
	else
	  lilyobj->display = g_string_new(display);
      }
    }
    if(is_new)
      object_insert (gui, lily);
    score_status(gui, TRUE);
    displayhelper(gui);
}


/* Run a dialog to get a lily directive from the user
 the values returned must be freed by the caller */
static
gboolean get_lily_directive(gchar **directive, gchar **display, gboolean *locked) {
  DenemoGUI *gui = Denemo.gui;
  GtkToggleButton *button = NULL;
  button = (GtkToggleButton *)gtk_check_button_new_with_label("locked");
  g_signal_connect(button, "toggled",  G_CALLBACK (toggle_locked), locked);
  if(*locked)
    gtk_toggle_button_set_active (button, *locked), *locked=TRUE;//FIXME how is this supposed to be done?
  *directive = string_dialog_entry_with_widget(gui, "Insert LilyPond", "Give LilyPond text to insert", *directive, GTK_WIDGET(button));
  if(!*directive)
    return FALSE;
  *display =  string_dialog_entry(gui, "Insert LilyPond", "Give Display text if required", *display);
  return TRUE;
}
/**
 * Lilypond directive insert/Edit.  Allows user to insert a lilypond directive 
 * before the current cursor position
 * or edit the current lilypond directive
 */
void
standalone_directive (GtkAction *action, DenemoScriptParam * param)
{
  DenemoGUI *gui = Denemo.gui;
  GET_3PARAMS(action, param, directive, display, minpixels);
  //g_print("query is %s\n", query);
  if(query) {
    get_lily_parameter(*query?query:"directive", param);
    return;
  }
  gboolean locked = FALSE;
  if(directive && !display)
    display = directive;
  if(action) {
    DenemoObject *curObj = (DenemoObject *) gui->si->currentobject ?
      (DenemoObject *) gui->si->currentobject->data : NULL;
    if (curObj && curObj->type == LILYDIRECTIVE) {
      lilydirective *lilyobj=(lilydirective *) curObj->object;
      directive = lilyobj->directive? lilyobj->directive->str:NULL;
      display = lilyobj->display? lilyobj->display->str:NULL;
    }
    if(get_lily_directive(&directive, &display, &locked))
      insert_lily_directive(directive, display, locked, 8);
  } else {
    insert_lily_directive(directive, display, locked, minpixels?atoi(minpixels):8);
  }
}
/**
 * Lilypond directive attach to note.  Allows user to attach a lilypond directive 
 * to the current note
 * or edit it
 */
void
note_directive (GtkAction *action, DenemoScriptParam *param)
{
  DenemoGUI *gui = Denemo.gui;
  GET_4PARAMS(action, param, postfix, display, prefix, tag);
  attach_directive (ATTACH_NOTE, postfix, prefix, display, tag, action!=NULL);
}
/**
 * Lilypond directive attach to chord.  Allows user to attach a lilypond directive 
 * to the current chord
 * or edit it
 */
void
chord_directive (GtkAction *action, DenemoScriptParam *param)
{
  DenemoGUI *gui = Denemo.gui;
  GET_4PARAMS(action, param, postfix, display, prefix, tag);
  attach_directive (ATTACH_CHORD, postfix, prefix, display, tag, action!=NULL);
}


static DenemoObject *get_chordobject(void) {
  chord *thechord = NULL;
  DenemoObject *curObj = findobj();
  if(curObj==NULL)
    return NULL;
  thechord = (chord *)curObj->object;
  if(curObj->type!=CHORD) {  
    return NULL;
  }
  return curObj;
}
static chord *get_chord(void) {
  chord *thechord = NULL;
  DenemoObject *curObj = get_chordobject();
  if(curObj==NULL)
    return NULL;
  return (chord *)curObj->object;
}


static note *get_note(void) {
  DenemoGUI *gui = Denemo.gui;
  note *curnote = NULL;
  chord *thechord = NULL;
  DenemoObject *curObj = get_chordobject();
  if(curObj==NULL)
    return NULL;
  return findnote(curObj, gui->si->cursor_y);
}
static
DenemoDirective *get_note_directive(gchar *tag) {
  note *curnote = get_note();
  if(curnote==NULL || (curnote->directives==NULL))
    return NULL;
  return find_directive(curnote->directives, tag);
}

static
DenemoDirective *get_chord_directive(gchar *tag) {
DenemoObject *curObj = get_chordobject();
  if(curObj==NULL)
    return NULL;
  chord *thechord = (chord *)curObj->object;
  if(thechord->directives==NULL)
    return NULL;
  return find_directive(thechord->directives, tag);
}


#define GET_FIELD_FUNC(what, field)\
gchar *\
what##_directive_get_##field(gchar *tag) {\
  DenemoDirective *directive = get_##what##_directive(tag);\
  if(directive && directive->field)\
    return directive->field->str;\
  return NULL;\
}

#define PUT_FIELD_FUNC(what, field)\
gboolean \
what##_directive_put_##field(gchar *tag, gchar *value) {\
  DenemoDirective *directive = get_##what##_directive(tag);\
  if(directive && directive->field)\
    g_string_assign(directive->field, value);\
  else if(directive)\
    directive->field = g_string_new(value);\
  else {\
       what *current = get_##what();\
       if(current==NULL) return FALSE;\
       if(current->directives==NULL) {\
          create_directives (&current->directives, tag);\
          what##_directive_put_##field(tag, value);\
        }\
  }\
  return TRUE;\
}


GET_FIELD_FUNC(note, prefix)
GET_FIELD_FUNC(note, postfix)
GET_FIELD_FUNC(note, display)

PUT_FIELD_FUNC(note, prefix)
PUT_FIELD_FUNC(note, postfix)
PUT_FIELD_FUNC(note, display)

GET_FIELD_FUNC(chord, prefix)
GET_FIELD_FUNC(chord, postfix)
GET_FIELD_FUNC(chord, display)

PUT_FIELD_FUNC(chord, prefix)
PUT_FIELD_FUNC(chord, postfix)
PUT_FIELD_FUNC(chord, display)


#undef GET_FIELD_FUNC
#undef PUT_FIELD_FUNC

#define PUT_INT_FIELD_FUNC(what, field)\
gboolean \
what##_directive_put_##field(gchar *tag, gint value) {\
  DenemoDirective *directive = get_##what##_directive(tag);\
  if(directive)\
    directive->field = value;\
  else {\
       what *current = get_##what();\
       if(current==NULL) return FALSE;\
       if(current->directives==NULL) {\
          create_directives (&current->directives, tag);\
          what##_directive_put_##field(tag, value);\
        }\
  }\
  return TRUE;\
}

#define GET_INT_FIELD_FUNC(what, field)\
gint \
what##_directive_get_##field(gchar *tag) {\
  DenemoDirective *directive = get_##what##_directive(tag);\
  if(directive)\
    return directive->field;\
  return 0;\
}




     /* block which can be copied for new int fields */
PUT_INT_FIELD_FUNC(note, minpixels)
PUT_INT_FIELD_FUNC(chord, minpixels)
GET_INT_FIELD_FUNC(note, minpixels)
GET_INT_FIELD_FUNC(chord, minpixels)
  /* end block which can be copied for new int fields */

PUT_INT_FIELD_FUNC(note, x)
PUT_INT_FIELD_FUNC(chord, x)
GET_INT_FIELD_FUNC(note, x)
GET_INT_FIELD_FUNC(chord, x)

PUT_INT_FIELD_FUNC(note, y)
PUT_INT_FIELD_FUNC(chord, y)
GET_INT_FIELD_FUNC(note, y)
GET_INT_FIELD_FUNC(chord, y)


PUT_INT_FIELD_FUNC(note, tx)
PUT_INT_FIELD_FUNC(chord, tx)
GET_INT_FIELD_FUNC(note, tx)
GET_INT_FIELD_FUNC(chord, tx)

PUT_INT_FIELD_FUNC(note, ty)
PUT_INT_FIELD_FUNC(chord, ty)
GET_INT_FIELD_FUNC(note, ty)
GET_INT_FIELD_FUNC(chord, ty)


PUT_INT_FIELD_FUNC(note, gx)
PUT_INT_FIELD_FUNC(chord, gx)
GET_INT_FIELD_FUNC(note, gx)
GET_INT_FIELD_FUNC(chord, gx)

PUT_INT_FIELD_FUNC(note, gy)
PUT_INT_FIELD_FUNC(chord, gy)
GET_INT_FIELD_FUNC(note, gy)
GET_INT_FIELD_FUNC(chord, gy)

#undef PUT_INT_FIELD_FUNC
#undef GET_INT_FIELD_FUNC


#define PUT_GRAPHIC(what) gboolean \
what##_directive_put_graphic(gchar *tag, gchar *value) {\
  DenemoDirective *directive = get_##what##_directive(tag);\
  if(directive && directive->graphic)\
    g_object_unref(G_OBJECT(directive->graphic));\
  if(directive) {\
     loadGraphicItem(value, &directive->graphic, &directive->width, &directive->height);\
     directive->graphic_name = g_string_new(value);}\
  else {\
       what *current = get_##what();\
       if(current==NULL) return FALSE;\
       if(current->directives==NULL) {\
          create_directives (&current->directives, tag);\
          what##_directive_put_graphic(tag, value);\
        }\
  }\
  return TRUE;\
}
     PUT_GRAPHIC(chord);
     PUT_GRAPHIC(note);
#undef PUT_GRAPHIC
