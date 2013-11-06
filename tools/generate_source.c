/* 
 * generate_source.c
 *
 * Program for generating source code from the old unmenued commands
 *
 * for Denemo, a gtk+ frontend to GNU Lilypond
 * (C) 2007 Richard Shann
 *
 * License: this file may be used under the FSF GPL version 2
 */


#include <stdio.h>

#define NOTECHAR0 "\\xF0\\x9D\\x85\\x9D"
#define NOTECHAR1 "\\xF0\\x9D\\x85\\x9E"
#define NOTECHAR2 "\\xF0\\x9D\\x85\\x9F"
#define NOTECHAR3 "\\xF0\\x9D\\x85\\xA0"
#define NOTECHAR4 "\\xF0\\x9D\\x85\\xA1"
#define NOTECHAR5 "\\xF0\\x9D\\x85\\xA2"
#define NOTECHAR6 "\\xF0\\x9D\\x85\\xA3"
#define NOTECHAR7 "\\xF0\\x9D\\x85\\xA4"
#define NOTECHAR8 "\\xF0\\x9D\\x85\\xA5"

#define RESTCHAR0 "\\xF0\\x9D\\x84\\xBB"
#define RESTCHAR1 "\\xF0\\x9D\\x84\\xBC"
#define RESTCHAR2 "\\xF0\\x9D\\x84\\xBD"
#define RESTCHAR3 "\\xF0\\x9D\\x84\\xBE"
#define RESTCHAR4 "\\xF0\\x9D\\x84\\xBF"
#define RESTCHAR5 "\\xF0\\x9D\\x85\\x80"
#define RESTCHAR6 "\\xF0\\x9D\\x85\\x81"
#define RESTCHAR7 "\\xF0\\x9D\\x85\\x82"
#define RESTCHAR8 "\\xF0\\x9D\\x85\\x83"

#define REST0 "<span font_desc=\\\"Denemo\\\">"RESTCHAR0"</span>"
#define REST1 "<span font_desc=\\\"Denemo\\\">"RESTCHAR1"</span>"
#define REST2 "<span font_desc=\\\"Denemo\\\">"RESTCHAR2"</span>"
#define REST3 "<span font_desc=\\\"Denemo\\\">"RESTCHAR3"</span>"
#define REST4 "<span font_desc=\\\"Denemo\\\">"RESTCHAR4"</span>"
#define REST5 "<span font_desc=\\\"Denemo\\\">"RESTCHAR5"</span>"
#define REST6 "<span font_desc=\\\"Denemo\\\">"RESTCHAR6"</span>"
#define REST7 "<span font_desc=\\\"Denemo\\\">"RESTCHAR7"</span>"
#define REST8 "<span font_desc=\\\"Denemo\\\">"RESTCHAR8"</span>"

//char *NOTES[] = {NOTE0, NOTE1, NOTE2, NOTE3, NOTE4, NOTE5, NOTE6, NOTE7, NOTE8};
char *NOTECHARS[] = { NOTECHAR0, NOTECHAR1, NOTECHAR2, NOTECHAR3, NOTECHAR4, NOTECHAR5, NOTECHAR6, NOTECHAR7, NOTECHAR8 };

//char *RESTS[] = {REST0, REST1, REST2, REST3, REST4, REST5, REST6, REST7, REST8};
char *RESTCHARS[] = { RESTCHAR0, RESTCHAR1, RESTCHAR2, RESTCHAR3, RESTCHAR4, RESTCHAR5, RESTCHAR6, RESTCHAR7, RESTCHAR8 };

#define N_

#define CMD_CATEGORY_NAVIGATION		0
#define CMD_CATEGORY_NOTE_ENTRY		1
#define CMD_CATEGORY_REST_ENTRY		2
#define CMD_CATEGORY_ARTICULATION	3
#define CMD_CATEGORY_EDIT		4
#define CMD_CATEGORY_MEASURE		5
#define CMD_CATEGORY_STAFF		6
#define CMD_CATEGORY_PLAYBACK		7
#define CMD_CATEGORY_OTHER		8

#define CMD_CLASS(cat) (cat&0xFF)
#define CMD_CATEGORY_DIRECT		0x100   /* does not require a wrapper to the callback */
#define CMD_CATEGORY_BOOLEAN		0x200   /* call returns boolean */

char *catname[9] = { 
  N_("Navigation"),
  N_("Note entry"),
  N_("Rest entry"),
  N_("Articulation"),
  N_("Edit"),
  N_("Measure"),
  N_("Staff"),
  N_("Playback"),
  N_("Other")
};

struct name_and_function
{
  unsigned category;
  /** Command name */
  char *icon;
  //char *menu_label;
  char *tooltip;
  char *name;
  char *function;
  char *menu_label;
  char *initial_setting;        /*of radio/check items */
};

FILE *callbacks, *entries, *xml, *scheme, *scheme_cb, *register_commands;

#define ni denemo_commands[i].name
#define ii denemo_commands[i].icon
#define ml denemo_commands[i].menu_label
#define ti denemo_commands[i].tooltip
#define fi denemo_commands[i].function
#define mi denemo_commands[i].category

void parse_menu_commands(){
  #include "menu.c"

  int i;
  int n_denemo_commands = (sizeof (denemo_commands) / sizeof (struct name_and_function));

  for (i = 0; i < n_denemo_commands; i++)
    {
      if (fi != NULL)
        {
          if (!(mi & CMD_CATEGORY_DIRECT))
            {
              fprintf (callbacks, "/*%s %s*/\n", ni, fi);
              /*******************   create a callback for calling from a menuitem *******************/
              if (mi & CMD_CATEGORY_BOOLEAN)
                {
                  fprintf (callbacks, 
                           "static void %s_cb (GtkAction *action, DenemoScriptParam *param) {\n" "%s (param);\n%s}\n", fi, fi, (CMD_CLASS (mi) == CMD_CATEGORY_NAVIGATION) ? "gtk_widget_queue_draw(Denemo.scorearea);\n" : " displayhelper (Denemo.gui);\n score_status(gui, TRUE);\n");
                }
              else
                {
                  fprintf (callbacks, 
                           "static void %s_cb (GtkAction *action, gpointer param) {\n" 
                           "  DenemoGUI *gui = Denemo.gui;\n" 
                           "  %s (gui);\n"
                           "%s"
                           "}\n", 
                           fi, fi, CMD_CLASS (mi) == CMD_CATEGORY_NAVIGATION ? "gtk_widget_queue_draw(Denemo.scorearea);\n" : "  displayhelper (gui);\n" "  score_status(gui, TRUE);\n");
                }
            }

          /*******************   create a procedure d-<name> in scheme to call scheme_<name>  *******************/
          fprintf (scheme, "/*%s %s*/\n", ni, fi);
          fprintf (scheme, 
                   "SCM scheme_%s(SCM optional);\n"
                   "install_scm_function (0, NULL, \"d-%s\", scheme_%s);\n", ni, ni, ni);  // for direct callback via (scheme_xxx)

          /*******************   create a callback scheme_<name> for calling from a scheme procedure d-<name>  *******************/
          fprintf (scheme_cb, 
                   "SCM scheme_%s (SCM optional) {\n"
                   "  gboolean query=FALSE;\n"
                   "  DenemoScriptParam param;\n"
                   "  GString *gstr=NULL;\n"
                   "  int length;\n"
                   "  char *str=NULL;\n"
                   "  if(scm_is_string(optional)){\n"
                   "    str = scm_to_locale_stringn(optional, (size_t *)&length);\n"
                   "    gstr = g_string_new_len(str, length);\n"
                   "    if(!strncmp(\"query\",str,5)) query = TRUE;"
                   "  }\n"
                   "  param.string = gstr;\n"
                   "  param.status = FALSE;\n\n"
                   "  %s%s (NULL, &param);\n"
                   "  if(param.status && query) return scm_from_locale_string (gstr->str);"
                   "  if(gstr) g_string_free(gstr, TRUE);\n"
                   "  return SCM_BOOL(param.status);\n"
                   "}\n", 
                   ni, fi, !(mi & CMD_CATEGORY_DIRECT) ? "_cb" : "");
          /****************** install the command in the hash table of commands (keymap) **************/
          fprintf (register_commands, "register_command(\"%s\", _(\"%s\"), _(\"%s\"), %s);\n", ni, ml ? ml : ni, ti ? ti : ni, fi);
          /****************** install the command as an action in the menu system **************************/
          fprintf (entries, "{\"%s\", %s, N_(\"%s\"), \"\"," "N_(\"%s\")," "G_CALLBACK (%s%s)},\n", ni, ii ? ii : "NULL", ml ? ml : ni, ti ? ti : ni, fi, (mi & CMD_CATEGORY_DIRECT) ? "" : "_cb");
        }
      else                      //no callback function - a menu rather than a menu item. It still needs to be added as an action in the menu system.
        fprintf (entries, "{\"%s\", %s, N_(\"%s\"), \"\"," "N_(\"%s\")},\n", ni, ii ? ii : "NULL", ml ? ml : ni, ti ? ti : ni);
    }
}

int
main ()
{
  scheme_cb = fopen ("scheme_cb.h", "w");
  scheme = fopen ("scheme.h", "w");
  callbacks = fopen ("callbacks.h", "w");
  entries = fopen ("entries.h", "w");
  register_commands = fopen ("register_commands.h", "w");
  xml = fopen ("xml.fragment", "w");
  if (!callbacks || !entries || !xml || !scheme || !scheme_cb || !register_commands)
    return -1;
  fprintf (callbacks, "/******** generated automatically from generate_source. See generate_source.c */\n");
  fprintf (entries, "/******** generated automatically from generate_source. See generate_source.c */\n");

  parse_menu_commands ();
  
  int i;

  /* generate source for duration callbacks - these were intercepted when
     typed at the keyboard to set prevailing rhythm, so the callback has to
     include code for this */
  //     for(i=0;i<9;i++) {

  //     }

  for (i = 0; i < 9; i++)
    {
      /* callbacks for mode independent duration actions InsertRest0,1,2... InsertDur,ChangeDur0,1,2... SetDur0,1,2... */
      fprintf (callbacks,
               "static void InsertRest%d(GtkAction *action, gpointer param){\n"
               "  DenemoGUI *gui = Denemo.gui;\n"
               "  highlight_rest(gui, %d);\n"
               "  gint mode = gui->mode;\n"
               "  gui->mode = INPUTINSERT|INPUTREST;\n"
               "  insert_chord_%dkey(gui);\n"
               "  gui->mode = mode;\n"
               "  score_status(gui, TRUE);\n"
               "  displayhelper(gui);\n"
               "}\n"
               "/* UNUSED\n"
               "static void Dummy%d(GtkAction *action, gpointer param){\n"
               "  DenemoGUI *gui = Denemo.gui;\n"
               "  gint mode = gui->mode;\n"
               "  gboolean appending = gui->si->cursor_appending;\n"
               "  if(appending)\n"
               "    movecursorleft(NULL);\n"
               "  gui->mode = INPUTEDIT|INPUTREST;\n"
               "  insert_chord_%dkey(gui);\n"
               "  gui->mode = mode;\n"
               "  if(appending)\n"
               "    movecursorright(NULL);\n"
               "  score_status(gui, TRUE);\n"
               "  displayhelper(gui);\n"
               "}\n"
               "*/\n"
               "static void InsertDur%d(GtkAction *action, gpointer param){\n"
               "  DenemoGUI *gui = Denemo.gui;\n"
               "  highlight_duration(gui, %d);\n"
               "  gint mode = gui->mode;\n"
               "  gui->mode = INPUTINSERT|INPUTNORMAL;\n"
               "  insert_chord_%dkey(gui);\n"
               "  gui->mode = mode;\n"
               "  score_status(gui, TRUE);\n"
               "  displayhelper(gui);\n"
               "}\n"
               "static void ChangeDur%d(GtkAction *action, gpointer param){\n"
               "  DenemoGUI *gui = Denemo.gui;\n"
               "  gint mode = gui->mode;\n"
               "  gboolean appending = gui->si->cursor_appending;\n"
               "  if(appending)\n" "    movecursorleft(NULL);\n" "  gui->mode = INPUTEDIT|INPUTNORMAL;\n" "  insert_chord_%dkey(gui);\n" "  gui->mode = mode;\n" "  if(appending)\n" "    movecursorright(NULL);\n" "  score_status(gui, TRUE);\n" "  displayhelper(gui);\n" "}\n" "static void SetDur%d(GtkAction *action, gpointer param){\n" "  DenemoGUI *gui = Denemo.gui;\n" "  highlight_duration(gui, %d);\n" "//  displayhelper(gui);\n" "}\n", i, i, i, i, i, i, i, i, i, i, i, i);


      /* callbacks for mode sensitive  duration actions, Dur0,1,2 ... */
      fprintf (callbacks, "static void Dur%d  (GtkAction *action, gpointer param) {\n" "  DenemoGUI *gui = Denemo.gui;\n" " if(gui->mode&INPUTINSERT)\n" "   highlight_duration(gui, %d);\n" " else \n" " if( !(gui->mode&INPUTRHYTHM) && (gui->mode&INPUTEDIT) && (!gui->si->cursor_appending))\n" "   ChangeDur%d (action, param);\n" "else {\n" " insert_chord_%dkey(gui);\n" "   highlight_duration(gui, %d);\n" "  score_status(gui, TRUE);\n" " displayhelper(gui);\n" " }\n" "}\n", i, i, i, i, i);

      // fprintf(entries, "\n#define NOTECHAR%d \"%s\"\n", i,  NOTECHARS[i]);
      // fprintf(entries, "\n#define RESTCHAR%d \"%s\"\n", i, RESTCHARS[i]);
      fprintf (entries, "\n#define NOTE%d \"%s\"\n", i, NOTECHARS[i]);
      fprintf (entries, "\n#define REST%d \"%s\"\n", i, RESTCHARS[i]);


      /* menu_entries for the mode sensitive duration actions, Dur0,1,2 ... */
      fprintf (entries, "{\"%d\", \"NULL\", NOTE%d, NULL, \"Inserts a note at the cursor with duration \"NOTE%d\", or \\n(if appending) appends this duration\\nIf MIDI-in is active the note will be pitchless (displays yellow, percussion-sounding)\\n - the MIDI keyboard will provide the pitch. Changes prevailing duration.\",\n" "G_CALLBACK (Dur%d)},\n" "{\"Change%d\", \"NULL\", NOTE%d, NULL, \"Change current note to a \"NOTE%d,\n" "G_CALLBACK (ChangeDur%d)},\n"
               //"{\"ChangeRest%d\", NULL, \"Change duration\", NULL, \"Change duration of current rest\",\n"
               // "G_CALLBACK (ChangeRest%d)},\n"
               "{\"Insert%d\", NULL, \"Insert a \"NOTE%d\"\", NULL, \"Inserts a \"NOTE%d\" at cursor position\\nSets prevailing duration to \"NOTE%d,\n"
               "G_CALLBACK (InsertDur%d)},\n" "{\"InsertRest%d\", NULL, \"Insert a \"REST%d\"rest\", NULL, \"Inserts a rest at cursor position\\nSets prevailing duration to \"NOTE%d,\n" "G_CALLBACK (InsertRest%d)},\n" "{\"Set%d\", NULL, \"Set Duration to \"NOTE%d\"\", NULL, \"Sets prevailing duration to \"NOTE%d\" (subsequent notes entered will have this duration)\", \n" "G_CALLBACK (SetDur%d)},\n"
               /* ,i, i, i , i, i*/, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i);


    }


  for (i = 0; i < 9; i++)
    fprintf (xml, "<menuitem action=\"%d\"/>\n", i);
  for (i = 0; i < 9; i++)
    fprintf (xml, "<menuitem action=\"Change%d\"/>\n", i);
  for (i = 0; i < 9; i++)
    fprintf (xml, "<menuitem action=\"Insert%d\"/>\n", i);
  //for (i = 0; i < 9; i++)
    //fprintf(xml, "<menuitem action=\"ChangeRest%d\"/>\n", i);
  for (i = 0; i < 9; i++)
    fprintf (xml, "<menuitem action=\"InsertRest%d\"/>\n", i);
  
  for (i = 'A'; i <= 'G'; i++)
    fprintf (xml, "<menuitem action=\"Insert%c\"/>\n", i);
  for (i = 'A'; i <= 'G'; i++)
    fprintf (xml, "<menuitem action=\"ChangeTo%c\"/>\n", i);
  
  for (i = 0; i < 9; i++)
    fprintf (xml, "<menuitem action=\"Set%d\"/>\n", i);

  /* menu_entries for the mode    note name    */
  for (i = 'A'; i <= 'G'; i++)
    {
      fprintf (entries,
               " {\"Insert%c\", NULL, \"Insert %c\", NULL, \"Inserts note %c before note at cursor\\nCursor determines which octave\\nNote is inserted in the prevailing rhythm\",\n"
               "  G_CALLBACK (Insert%c)},\n"
               " {\"AddNote%c\", NULL, \"Insert %c After\", NULL, \"Inserts note %c after note at cursor\\nCursor determines which octave\\nNote is inserted in the prevailing rhythm\",\n"
               "  G_CALLBACK (AddNote%c)},\n"
               " {\"Add%c\", NULL, \"Add %c to Chord\", NULL, \"Adds note %c to chord at cursor\\nCursor determines which octave\",\n"
               "  G_CALLBACK (Add%c)},\n"
               "  {\"ChangeTo%c\", NULL, \"Change current note to %c\", NULL, \"Changes current note to the %c nearest cursor or (if no current note) inserts the note %c\\nCursor determines which octave\\nNote is inserted in the prevailing rhythm\",\n"
               "   G_CALLBACK (ChangeTo%c)},\n" "  {\"MoveTo%c\", NULL, \"Move cursor to step %c\", NULL, \"Moves the cursor to the %c nearest cursor\\nCurrent cursor position determines which octave.\",\n" "   G_CALLBACK (MoveTo%c)},\n", i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i);

    }

  for (i = 'A'; i <= 'G'; i++)
    {
      fprintf (callbacks, "static void ChangeTo%c(GtkAction *action, gpointer param){\n" "  DenemoGUI *gui = Denemo.gui;\n" "  gboolean appending = gui->si->cursor_appending;\n" "  if(appending)\n" "    movecursorleft(NULL); \n" "  gint mode = gui->mode;\n" "  gui->mode = INPUTEDIT|INPUTNORMAL;\n" "  go_to_%c_key(gui);\n" "  gui->mode = mode;\n" "  if(appending)\n" "    movecursorright(NULL);\n" "  score_status(gui, TRUE);\n" "  displayhelper(gui);\n" "}\n", i, i);
    }


  for (i = 'A'; i <= 'G'; i++)
    {
      fprintf (callbacks, "static void MoveTo%c(GtkAction *action, gpointer param){\n" "  DenemoGUI *gui = Denemo.gui;\n" "  gint mode = gui->mode;\n" "  gui->mode = INPUTCLASSIC|INPUTNORMAL;\n" "  go_to_%c_key(gui);\n" "  gui->mode = mode;\n" "  displayhelper(gui);\n" "}\n", i, i);
    }


  for (i = 'A'; i <= 'G'; i++)
    {

      fprintf (callbacks, "static void Insert%c(GtkAction *action, gpointer param){\n" "  DenemoGUI *gui = Denemo.gui;\n" "  gint mode = gui->mode;\n" "  gui->mode = INPUTINSERT|INPUTNORMAL;\n" "  go_to_%c_key(gui);\n" "  gui->mode = mode;\n" "  score_status(gui, TRUE);\n" "  displayhelper(gui);\n" "}\n", i, i);
    }
  for (i = 'A'; i <= 'G'; i++)
    {
      fprintf (callbacks, "static void AddNote%c(GtkAction *action, gpointer param){\n" "  DenemoGUI *gui = Denemo.gui;\n" "  movecursorright(NULL);\n" "  gint mode = gui->mode;\n" "  gui->mode = INPUTINSERT|INPUTNORMAL;\n" "  go_to_%c_key(gui);\n" "  gui->mode = mode;\n" "  movecursorleft(NULL);\n" "  score_status(gui, TRUE);\n" "  displayhelper(gui);\n" "}\n", i, i);
    }

  for (i = 'A'; i <= 'G'; i++)
    {

      fprintf (callbacks, "static void Add%c(GtkAction *action, gpointer param){\n" "  DenemoGUI *gui = Denemo.gui;\n" "  gint mode = gui->mode;\n" "  gui->mode = INPUTCLASSIC|INPUTNORMAL;\n" "  go_to_%c_key(gui);\n" "  add_tone_key(gui);\n" "  gui->mode = mode;\n" "  score_status(gui, TRUE);\n" "  displayhelper(gui);\n" "}\n", i, i);
    }

  for (i = 'A'; i <= 'G'; i++)
    {
      fprintf (register_commands, "register_command(\"Insert%c\", _(\"Insert %c\"),_(\"Inserts note %c before note at cursor\\nCursor determines which octave\\nNote is inserted in the prevailing rhythm\"),  Insert%c);\n", i, i, i, i);
      fprintf (scheme, "SCM scheme_Insert%c(SCM optional);\ninstall_scm_function (0, NULL, \"d-Insert%c\", scheme_Insert%c);\n", i, i, i);       // for direct callback via (scheme_xxx)
      fprintf (scheme_cb, "SCM scheme_Insert%c (SCM optional) {\nInsert%c (NULL, NULL);\nreturn SCM_BOOL(TRUE);\n}\n", i, i);
      fprintf (register_commands, "register_command(\"AddNote%c\", _(\"Insert %c After\"),_(\"Inserts note %c after note at cursor\\nCursor determines which octave\\nNote is inserted in the prevailing rhythm\"),  AddNote%c);\n", i, i, i, i);
      fprintf (scheme, "SCM scheme_AddNote%c(SCM optional);\ninstall_scm_function (0, NULL, \"d-AddNote%c\", scheme_AddNote%c);\n", i, i, i);    // for direct callback via (scheme_xxx)
      fprintf (scheme_cb, "SCM scheme_AddNote%c (SCM optional) {\nAddNote%c (NULL, NULL);\nreturn SCM_BOOL(TRUE);\n}\n", i, i);


      fprintf (register_commands, "register_command(\"Add%c\", _(\"Add %c\"),_(\"Adds note %c to the chord at cursor\\nCursor height determines which octave\"),  Add%c);\n", i, i, i, i);
      fprintf (scheme, "SCM scheme_Add%c(SCM optional);\ninstall_scm_function (0, NULL, \"d-Add%c\", scheme_Add%c);\n", i, i, i);        // for direct callback via (scheme_xxx)
      fprintf (scheme_cb, "SCM scheme_Add%c (SCM optional) {\nAdd%c (NULL, NULL);\nreturn SCM_BOOL(TRUE);\n}\n", i, i);



      fprintf (register_commands, "register_command(\"ChangeTo%c\", _(\"Change to %c\"),_(\"Changes note at cursor to nearest note %c\\nRhythm is unchanged\"),  ChangeTo%c);\n", i, i, i, i);
      fprintf (scheme, "SCM scheme_ChangeTo%c(SCM optional);\ninstall_scm_function (0, NULL, \"d-ChangeTo%c\", scheme_ChangeTo%c);\n", i, i, i); // for direct callback via (scheme_xxx)
      fprintf (scheme_cb, "SCM scheme_ChangeTo%c (SCM optional) {\nChangeTo%c (NULL, NULL);\nreturn SCM_BOOL(TRUE);\n}\n", i, i);


      fprintf (register_commands, "register_command(\"MoveTo%c\", _(\"Move to %c\"),_(\"Moves cursor to nearest note %c\"),  MoveTo%c);\n", i, i, i, i);
      fprintf (scheme, "SCM scheme_MoveTo%c(SCM optional);\ninstall_scm_function (0, NULL, \"d-MoveTo%c\", scheme_MoveTo%c);\n", i, i, i);       // for direct callback via (scheme_xxx)
      fprintf (scheme_cb, "SCM scheme_MoveTo%c (SCM optional) {\nMoveTo%c (NULL, NULL);\nreturn SCM_BOOL(TRUE);\n}\n", i, i);

    }



  for (i = 0; i < 9; i++)
    {
      /* registering commands for mode independent duration actions InsertRest0,1,2... ChangeRest0,1,2... InsertDur,ChangeDur0,1,2...
       *
       * !!! FIXME what is ChangeRestn???? seems to be Changen ... now dropped. */

      fprintf (register_commands, "register_command(\"%d\", _(\"Insert/Append a %s\"), _(\"When appending, appends a %s \\nWith the cursor on a note inserts a %s  before the current note\\nIf MIDI-in is active, the note will be pitchless (displays yellow, percussion-sounding)\\n - the MIDI keyboard will provide the pitch. Changes prevailing duration.\"), Dur%d);\n", i, NOTECHARS[i], NOTECHARS[i], NOTECHARS[i], i);

      fprintf (register_commands, "register_command(\"Change%d\", _(\"Change to %s\"), _(\"Change the current note to a %s\"), ChangeDur%d);\n", i, NOTECHARS[i], NOTECHARS[i], i);

      fprintf (register_commands, "register_command(\"Insert%d\", _(\"%s\"), _(\"Insert a %s\"), InsertDur%d);\n", i, NOTECHARS[i], NOTECHARS[i], i);

      fprintf (register_commands, "register_command(\"InsertRest%d\",  _(\"Insert a %s\") ,  _(\"Inserts a rest at cursor position\\nSets prevailing rhythm to %s\"), InsertRest%d);\n", i, RESTCHARS[i], NOTECHARS[i], i);

      //  fprintf(register_commands, 
      //    "register_command(Denemo.map, gtk_action_group_get_action(action_group, \"ChangeRest%d\"), \"ChangeRest%d\",  _(\"Change a %s\") ,  _(\"Changes a rest at cursor position\\nSets prevailing rhythm to %s\"), ChangeRest%d);\n", i, i, RESTCHARS[i], NOTECHARS[i], i);

      fprintf (register_commands, "register_command(\"Set%d\", _(\"Set Prevailing Duration to %s\"), _(\"Set the prevailing duration to %s (subsequent notes entered will have this duration)\"), SetDur%d);\n", i, NOTECHARS[i], NOTECHARS[i], i);

      fprintf (register_commands, "\n#undef NOTE%d\n", i);
      fprintf (register_commands, "\n#undef REST%d\n", i);
      fprintf (register_commands, "\n#undef NOTECHAR%d\n", i);
      fprintf (register_commands, "\n#undef RESTCHAR%d\n", i);

      fprintf (scheme, "/*%d */\n", i);

      fprintf (scheme, "SCM scheme_%d(SCM optional);\ninstall_scm_function (0, NULL, \"d-%d\", scheme_%d);\n", i, i, i); // for direct callback via (scheme_xxx)
      fprintf (scheme_cb, "SCM scheme_%d (SCM optional) {\nDur%d (NULL, NULL);\nreturn SCM_BOOL(TRUE);\n}\n", i, i);

      fprintf (scheme, "SCM scheme_InsertDur%d(SCM optional);\ninstall_scm_function (0, NULL, \"d-Insert%d\", scheme_InsertDur%d);\n", i, i, i); // for direct callback via (scheme_xxx)
      fprintf (scheme_cb, "SCM scheme_InsertDur%d (SCM optional) {\nInsertDur%d (NULL, NULL);\nreturn SCM_BOOL(TRUE);\n}\n", i, i);



      fprintf (scheme, "SCM scheme_ChangeDur%d(SCM optional);\ninstall_scm_function (0, NULL, \"d-Change%d\", scheme_ChangeDur%d);\n", i, i, i); // for direct callback via (scheme_xxx)
      fprintf (scheme_cb, "SCM scheme_ChangeDur%d (SCM optional) {\nChangeDur%d (NULL, NULL);\nreturn SCM_BOOL(TRUE);\n}\n", i, i);

      fprintf (scheme, "SCM scheme_SetDur%d(SCM optional);\ninstall_scm_function (0, NULL, \"d-Set%d\", scheme_SetDur%d);\n", i, i, i);  // for direct callback via (scheme_xxx)
      fprintf (scheme_cb, "SCM scheme_SetDur%d (SCM optional) {\nSetDur%d (NULL, NULL);\nreturn SCM_BOOL(TRUE);\n}\n", i, i);



      fprintf (scheme, "SCM scheme_InsertRest%d(SCM optional);\ninstall_scm_function (0, NULL, \"d-InsertRest%d\", scheme_InsertRest%d);\n", i, i, i);   // for direct callback via (scheme_xxx)
      fprintf (scheme_cb, "SCM scheme_InsertRest%d (SCM optional) {\nInsertRest%d (NULL, NULL);\nreturn SCM_BOOL(TRUE);\n}\n", i, i);

      // fprintf(scheme, "SCM scheme_ChangeRest%d(SCM optional);\ninstall_scm_function (0, NULL, \"d-ChangeRest%d\", scheme_ChangeRest%d);\n", i, i, i);// for direct callback via (scheme_xxx)
      // fprintf(scheme_cb, "SCM scheme_ChangeRest%d (SCM optional) {\nChangeRest%d (NULL, NULL);\nreturn SCM_BOOL(TRUE);\n}\n", i,  i);
    }



#ifdef GENERATE_XML_FRAGMENT
  fprintf (xml, "<menu action=\"AllOther\">\n");
  for (j = 0; j < 9; j++)
    {
      fprintf (xml, "<menu action=\"%s\">\n", catname[j]);
      for (i = 0; i < n_denemo_commands; i++)
        {
          if (mi == j)
            {
              fprintf (xml, "<menuitem action=\"%s\"/>\n", ni);
            }

        }
      fprintf (xml, "</menu>\n");
    }
  fprintf (xml, "</menu>\n");
#endif
  return 0;
}
