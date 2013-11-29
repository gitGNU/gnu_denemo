#include "scheme-callbacks.h"
#include "commandfuncs.h"
#include "selectops.h"
#include "scoreops.h"
#include "graceops.h"
#include "view.h"
#include "pitchentry.h"
#include "lilydirectives.h"
#include "playback.h"
#include "audiointerface.h"
#include "generated/scheme_cb.h"

SCM 
scheme_call_callback (SCM optional, callback_function callback) {
  gboolean query=FALSE;
  DenemoScriptParam param;
  GString *gstr=NULL;
  int length;
  char *str=NULL;
  
  if(scm_is_string(optional)){
    str = scm_to_locale_stringn(optional, (size_t *)&length);
    gstr = g_string_new_len(str, length);
    if(!strncmp("query",str,5)) query = TRUE;  
  }
  param.string = gstr;
  param.status = FALSE;

  callback (NULL, &param);
  if(param.status && query) 
    return scm_from_locale_string (gstr->str);  
  if(gstr) 
    g_string_free(gstr, TRUE);
  return SCM_BOOL(param.status);
}

SCM ReturnValue = SCM_BOOL_F;
static void
set_return_value (SCM val)
{
  ReturnValue = val;
}

static gboolean
scm_is_list (SCM scm)
{
  return scm_is_true (scm_list_p (scm));
}

SCM
scheme_popup_menu (SCM list)
{

  GtkWidget *menu = gtk_menu_new ();
  set_return_value (SCM_BOOL_F);
  if (scm_is_list (list))
    {
      gint i;
      gint length = scm_to_int (scm_length (list));
      for (i = 0; i < length; i++)
        {
          SCM el = scm_list_ref (list, scm_from_int (i));
          if (scm_is_pair (el))
            {
              gchar *label = NULL;
              gchar *tooltip = NULL;
              SCM sym;
              //g_print("Note that %d is value and %d stringp\n", scm_pair_p(el), scm_string_p(el));
              if (scm_is_string (scm_car (el)))
                {

                  label = scm_to_locale_string (scm_car (el));
                  sym = scm_cdr (el);


                }
              else if (scm_is_pair (scm_car (el)))
                {
                  label = scm_to_locale_string (scm_caar (el));
                  tooltip = scm_to_locale_string (scm_cdar (el));
                  sym = scm_cdr (el);
                }
              if (label)
                {
                  GtkWidget *item = gtk_menu_item_new_with_label (label);
                  if (tooltip)
                    gtk_widget_set_tooltip_text (item, tooltip);
                  gtk_menu_shell_append (GTK_MENU_SHELL (menu), item);
                  g_signal_connect_swapped (G_OBJECT (item), "activate", G_CALLBACK (set_return_value), sym);
                }
              else
                {
                  set_return_value (SCM_BOOL_F);
                  break;
                }
            }
          else if (scm_is_string (el))
            {
              GtkWidget *item = gtk_menu_item_new_with_label (scm_to_locale_string (el));
              gtk_menu_shell_append (GTK_MENU_SHELL (menu), item);
              g_signal_connect_swapped (G_OBJECT (item), "activate", G_CALLBACK (set_return_value), el);
            }
          else
            {
              set_return_value (SCM_BOOL_F);
              break;
            }
        }
      gtk_widget_show_all (menu);
      g_signal_connect (menu, "selection-done", gtk_main_quit, NULL);
      gtk_menu_popup (GTK_MENU (menu), NULL, NULL, NULL, NULL, 0, gtk_get_current_event_time ());
      gtk_main ();

    }
  return ReturnValue;
}

SCM
scheme_create_palette_button (SCM palette, SCM lbl, SCM tltp, SCM scrp) 
{
	SCM ret;
	gchar *name = scm_to_locale_string (palette);
	gchar *label = scm_to_locale_string (lbl);
	gchar *tooltip = scm_to_locale_string (tltp);
	gchar *script = scm_to_locale_string (scrp);
	
	DenemoPalette *pal = create_palette (name, FALSE, TRUE);

	ret = palette_add_button (pal, label, tooltip, script)?SCM_BOOL_T:SCM_BOOL_F;
	gtk_widget_show_all (gtk_widget_get_parent(pal->box));
	free(name);
	free(label);
	free(tooltip);
	free(script);
	return ret;
}	
SCM
scheme_set_palette_shape (SCM palette, SCM horizontal, SCM limit)
{
	gchar *name = scm_to_locale_string (palette);
	gboolean horiz = scm_is_true (horizontal);
	gint lim = scm_to_int (limit);
	
	DenemoPalette *pal = create_palette (name, FALSE, horiz);
	free(name);
	if (lim>0) {
		pal->limit = lim;
		pal->rows = horiz;
		repack_palette (pal);
		return SCM_BOOL_T;
	}
 return SCM_BOOL_F;
}
SCM
scheme_show_palettes (SCM option) 
{
	if(scm_is_true (option)) 
		{
		gchar *name;
		if(scm_is_string (option))
		   {
			name = scm_to_locale_string (option);
			DenemoPalette *pal = get_palette (name);
				if(pal)
				{
					gtk_widget_show (gtk_widget_get_parent(pal->box));
					if(gtk_widget_get_visible (pal->box))
						gtk_widget_hide (pal->box);
					else
						gtk_widget_show_all (pal->box);
					return SCM_BOOL_T;
				}
				else
				{
					return SCM_BOOL_F;
				}
		  } else
		  {
			name = choose_palette_by_name (FALSE, TRUE);
		   if(name)
			{
				DenemoPalette *pal = get_palette (name);
				if(pal)
				{
					gtk_widget_show (gtk_widget_get_parent(pal->box));
					gtk_widget_show_all (pal->box);
					return SCM_BOOL_T;
				}
				else
				{
					return SCM_BOOL_F;
				}
			}
		  }
		} else
		{
		if(Denemo.palettes)
			{
			GList *g;
			for (g=Denemo.palettes;g;g=g->next)
				{
				DenemoPalette *pal = (DenemoPalette *) g->data;
				gtk_widget_show (gtk_widget_get_parent(pal->box));
				gtk_widget_show_all (pal->box);
				}	
			return SCM_BOOL_T;
			} else
			return SCM_BOOL_F;
		}
	return SCM_BOOL_F;
}
	
		
SCM
scheme_get_offset (void)
{
#ifndef USE_EVINCE
  g_debug("This feature requires denemo to be built with evince");
#else
  gdouble offsetx, offsety;
  if (get_offset (&offsetx, &offsety))
    {
      offsetx *= 100;
      offsety *= 100;
      offsetx = floor (offsetx);
      offsety = floor (offsety);
      offsetx /= 100;
      offsety /= 100;

      return scm_cons (scm_from_double (offsetx), scm_from_double (offsety));
    }
#endif
  return SCM_BOOL_F;
}

SCM
scheme_get_control_point (SCM pt)
{
#ifndef USE_EVINCE
  g_debug("This feature requires denemo to be built with evince");
#else
  if (scm_is_integer (pt))
    {
      gint which = scm_to_int (pt);
      if (which > 0 && which < 5)
        return SCM_BOOL (get_control_point (which));
    }
#endif
  return SCM_BOOL_F;
}

static void
prec (gdouble * val)
{
  *val = round ((*val) * 100.0) / 100;
}

SCM
scheme_get_curve (void)
{
#ifndef USE_EVINCE
  g_debug("This feature requires denemo to be built with evince");
#else
  gdouble x1, y1, x2, y2, x3, y3, x4, y4;
  if (get_curve (&x1, &y1, &x2, &y2, &x3, &y3, &x4, &y4))
    {
      prec (&x1);
      prec (&y1);
      prec (&x2);
      prec (&y2);
      prec (&x3);
      prec (&y3);
      prec (&x4);
      prec (&y4);
      return scm_list_n (scm_cons (scm_from_double (x1), scm_from_double (y1)), scm_cons (scm_from_double (x2), scm_from_double (y2)), scm_cons (scm_from_double (x3), scm_from_double (y3)), scm_cons (scm_from_double (x4), scm_from_double (y4)), SCM_UNDEFINED);
    }
#endif
  return SCM_BOOL_F;
}

SCM
scheme_get_positions (SCM is_slur)
{
#ifndef USE_EVINCE
  g_debug("This feature requires denemo to be built with evince");
#else
  gdouble neary, fary;
  if (get_positions (&neary, &fary, scm_is_true (is_slur)))
    {
      return scm_cons (scm_from_double (neary), scm_from_double (fary));
    }
#endif
  return SCM_BOOL_F;
}

SCM
scheme_get_new_target (void)
{
#ifndef USE_EVINCE
  g_debug("This feature requires denemo to be built with evince");
  return SCM_BOOL_F;
#else
  return scm_from_bool (get_new_target ());
#endif
}

SCM
scheme_get_new_point (void)
{
#ifndef USE_EVINCE
  g_debug("This feature requires denemo to be built with evince");
  return SCM_BOOL_F;
#else
  return scm_from_bool (get_new_point ());
#endif
}

SCM
scheme_get_reference_point (void)
{
#ifndef USE_EVINCE
  g_debug("This feature requires denemo to be built with evince");
  return SCM_BOOL_F;
#else
  return scm_from_bool (get_reference_point ());
#endif
}

SCM
scheme_get_target_info (void)
{
  DenemoScore *si = Denemo.gui->si;
  if (Denemo.gui->si->currentobject == NULL)
    return SCM_BOOL_F;
  SCM type = SCM_BOOL_F, grob = SCM_BOOL_F, tag = SCM_BOOL_F;
  switch (si->target.type)
    {
    case TARGET_NONE:
      type = SCM_BOOL_F;
      break;
    case TARGET_OBJECT:
      type = scm_from_locale_string ("Object");
      break;
    case TARGET_CHORD:
      type = scm_from_locale_string ("Chord");
      break;
    case TARGET_NOTE:
      type = scm_from_locale_string ("Note");
      break;
    case TARGET_SLUR:
      type = scm_from_locale_string ("Slur");
      break;
    case TARGET_TIE:
      type = scm_from_locale_string ("Tie");
      break;
    case TARGET_CRESC:
      type = scm_from_locale_string ("Cresc");
      break;
    case TARGET_DIM:
      type = scm_from_locale_string ("Dim");
      break;
    default:
      g_warning ("Unknown target type %d\n", si->target.type);
      type = SCM_BOOL_F;
      break;
    }

  if (si->target.type == TARGET_NOTE) {
    DenemoObject *obj = si->currentobject->data;
    chord *thechord = ((chord *) ((DenemoObject *) obj->object));
    if(thechord->figure)
      grob = scm_from_locale_string ("BassFigure");
  }

            
  if (si->target.directivenum || (si->target.type == TARGET_OBJECT))
    {
      DenemoDirective *directive = NULL;
      DenemoObject *obj = si->currentobject->data;
      if (si->target.type == TARGET_CHORD)
        {
          GList *directives = ((chord *) ((DenemoObject *) obj->object))->directives;
          if (directives)
            {
              directive = (DenemoDirective *) g_list_nth_data (directives, si->target.directivenum - 1);
            }
        }
      else if (si->target.type == TARGET_NOTE)
        {
          directive = get_note_directive_number (si->target.directivenum);
        }
      else if (si->target.type == TARGET_OBJECT)
        {
          if (obj->type == LILYDIRECTIVE)
            directive = (DenemoDirective *) obj->object;
        }
      if (directive)
        {
          if (directive->grob)
            {
              grob = scm_from_locale_string (directive->grob->str);
            }
          else
            grob = SCM_BOOL_F;
        }
      if (directive->tag)
        {
          tag = scm_from_locale_string (directive->tag->str);
        }
    }
  return scm_list_n (type, grob, tag, SCM_UNDEFINED);
}

SCM
scheme_http (SCM hname, SCM page, SCM other, SCM poststr)
{
  char *name = NULL, *thepage = NULL, *oth = NULL, *post = NULL;

  if (scm_is_string (hname))
    {
      name = scm_to_locale_string (hname);
    }
  if (scm_is_string (page))
    {
      thepage = scm_to_locale_string (page);
    }
  if (scm_is_string (other))
    {
      oth = scm_to_locale_string (other);
    }
  if (scm_is_string (poststr))
    {
      post = scm_to_locale_string (poststr);
    }

  if (name && thepage && post && oth)
    {
      gchar *ret = post_denemodotorg (name, thepage, oth, post);
      SCM scm = scm_from_locale_string (ret);
      g_free (ret);
      free (name);
      free (thepage);
      free (oth);
      free (post);
      return scm;
    }
  else
    {
      free (name);
      free (thepage);
      free (oth);
      free (post);
      return SCM_BOOL (FALSE);
    }
}

//FIXME inelegant!
static gint
interpret_lilypond_notename (gchar * x, gint * mid_c_offset, gint * enshift)
{
  // g_print("Mid c offset of %d\n", *x-'c');
  gchar *c;
  gint octave = -1;             /* middle c is c' */
  gint accs = 0;

  for (c = x + 1; *c; c++)
    {
      if (*c == 'i' && *(c + 1) == 's')
        {
          accs++;
          c++;;
        }
      else if (*c == 'e' && *(c + 1) == 's')
        {
          accs--;
          c++;
        }
      else if (*c == ',')
        {
          octave--;
        }
      else if (*c == '\'')
        {
          octave++;
        }
    }
  if (*x == 'a' || *x == 'b')
    octave++;

  *mid_c_offset = *x - 'c' + 7 * octave;
  *enshift = accs;
  return *mid_c_offset;
}

static gint
lilypond_to_enshift (gchar * enshift_name)
{
  gint enshift = 0;
  gchar *c;
  for (c = enshift_name; *c; c++)
    {
      if (*c == 'i' && *(c + 1) == 's')
        {
          enshift++;
          c++;
        }
      else if (*c == 'e' && *(c + 1) == 's')
        {
          enshift--;
          c++;
        }
    }
  return enshift;
}

/*
  execute init script local dir for menupath or fallback on system dir
*/
SCM
scheme_execute_init (gchar * menupath)
{
  gchar *filename = g_build_filename (get_user_data_dir (TRUE), COMMANDS_DIR, "menus", menupath, INIT_SCM, NULL);
  if (g_file_test (filename, G_FILE_TEST_EXISTS))
    {
      g_print ("About to load from %s\n", filename);
      eval_file_with_catch (filename);  //ret = scm_c_primitive_load(filename);
    }
  else
    {
      g_free (filename);
      filename = g_build_filename (get_system_data_dir (), COMMANDS_DIR, "menus", menupath, INIT_SCM, NULL);
      if (g_file_test (filename, G_FILE_TEST_EXISTS))
        {
          g_print ("About to load from %s\n", filename);
          eval_file_with_catch (filename);      //ret = scm_c_primitive_load(filename);
        }
      g_free (filename);
    }
  return SCM_BOOL (TRUE);
}

void
execute_init_scripts (gchar * menupath)
{
  (void) scheme_execute_init (menupath);
}

/* called by a script if it requires initialization
 the initialization script is expected to be in init.scm in the menupath of the action that invoked the script*/
SCM
scheme_initialize_script (SCM action_name)
{
  SCM ret;
  char *name;
  name = scm_to_locale_string (action_name);
  GtkAction *action = lookup_action_from_name (name);
  if (!action)
    {
      g_warning ("Non-existent action %s", name);
      return SCM_BOOL (FALSE);
    }
  gchar *menupath = g_object_get_data (G_OBJECT (action), "menupath");
  ret = scheme_execute_init (menupath);
  if (name)
    free (name);
  return ret;
}

/* pass in a path (from below menus) to a command script. Loads the command from .denemo or system
 *  if it can be found
 * It is used at startup in .denemo files like ReadingNoteNames.denemo
 * which executes
 (d-LoadCommand "MainMenu/Educational/ReadingNoteNames")
 * to ensure that the command it needs is in the command set.
 */
SCM
scheme_load_command (SCM command)
{
  gboolean ret;
  char *name;
  name = scm_to_locale_string (command);
  gchar *filename = g_build_filename (get_user_data_dir (TRUE), COMMANDS_DIR, "menus", name, NULL);
  ret = load_xml_keymap (filename);
  if (ret == FALSE)
    {
      g_free (filename);
      filename = g_build_filename (get_user_data_dir (TRUE), "download", COMMANDS_DIR, name, NULL);
      ret = load_xml_keymap (filename);
    }
  if (ret == FALSE)
    {
      g_free (filename);
      filename = g_build_filename (get_system_data_dir (), COMMANDS_DIR, name, NULL);
      ret = load_xml_keymap (filename);
    }
  if (name)
    free (name);
  if (filename)
    g_free (filename);
  return SCM_BOOL (ret);
}

SCM
scheme_activate_menu_item (SCM menupath)
{
  if (scm_is_string (menupath))
    {
      char *item;
      item = scm_to_locale_string (menupath);
      if (item)
        {
          gboolean ret = activate_action (item) ? TRUE : FALSE;
          free (item);
          return SCM_BOOL (ret);
        }
    }
  return SCM_BOOL_F;
}


SCM
scheme_hide_buttons (SCM hide)
{
  SCM ret = SCM_BOOL_F;
  GtkWidget *widget = Denemo.gui->buttonbox;
  if (GTK_IS_CONTAINER (widget))
    {
      ret = SCM_BOOL_T;
      if (scm_is_false (hide))
        gtk_container_foreach (GTK_CONTAINER (widget), (GtkCallback) gtk_widget_show, NULL);
      else
        gtk_container_foreach (GTK_CONTAINER (widget), (GtkCallback) gtk_widget_hide, NULL);
    }
  return ret;
}

SCM
scheme_destroy_buttons (void)
{
  SCM ret = SCM_BOOL_F;
  GtkWidget *widget = Denemo.gui->buttonbox;

  if (GTK_IS_CONTAINER (widget))
    {
      gtk_container_foreach (GTK_CONTAINER (widget), (GtkCallback) gtk_widget_destroy, NULL);
      ret = SCM_BOOL_T;
    }
  return ret;
}


/* hide all menus, leaving only the score titles, used for educational games */
SCM
scheme_hide_menus (SCM hide)
{
  if (Denemo.gui->view != DENEMO_MENU_VIEW)
    {
      activate_action ("/MainMenu/ViewMenu/" ToggleScoreTitles_STRING);
      ToggleReduceToDrawingArea (NULL, NULL);
      return SCM_BOOL (TRUE);
    }
  gboolean show = FALSE;
  if (scm_is_false (hide))
    show = TRUE;
  toggle_to_drawing_area (show);
  activate_action ("/MainMenu/ViewMenu/" ToggleScoreTitles_STRING);
  return SCM_BOOL (TRUE);
}

SCM
scheme_hide_window (SCM hide)
{
  gboolean show = FALSE;
  gboolean showing = gtk_widget_get_visible (Denemo.window);
  if (scm_is_false (hide))
    show = TRUE;
  if (show)
    gtk_widget_show (Denemo.window);
  else
    gtk_widget_hide (Denemo.window);
  return SCM_BOOL (showing == show);
}


/* when a script calls a command which is itself a script it comes through here */
SCM
scheme_script_callback (SCM script, SCM params)
{
  char *name = NULL;
  SCM ret = SCM_BOOL_F;
  if (scm_is_string (script))
    {
      name = scm_to_locale_string (script);
      if (name)
        {
          GtkAction *action = lookup_action_from_name (name);
          if (action && !is_action_name_builtin(name))
            {
              gchar *paramvar = g_strdup_printf ("%s::params", name);
              scm_c_define (paramvar, params);

              gchar *text = g_object_get_data (G_OBJECT (action), "scheme");
              if (text && *text)
                {
                  //undo is a queue so this is the end :)
                  stage_undo (Denemo.gui->si, ACTION_STAGE_END);
                  ret = SCM_BOOL (!call_out_to_guile (text));
                  stage_undo (Denemo.gui->si, ACTION_STAGE_START);
                }
              else
                ret = SCM_BOOL (activate_script (action, NULL));
              scm_c_define (paramvar, SCM_BOOL_F);
              g_free (paramvar);
            }
          if (name)
            free (name);
        }
    }
  return ret;
}

void
create_scheme_function_for_script (gchar * name)
{
  gchar *proc = g_strdup_printf ("(d-%s #:optional params)", name);
  gchar *value = g_strdup_printf ("(d-ScriptCallback \"%s\" params)", name);
  gchar *def = g_strdup_printf ("(define %s::params #f) (define* %s %s)", name, proc, value);

  //g_print("Defining %s\n", def);
  call_out_to_guile (def);
  g_free (proc);
  g_free (value);
  g_free (def);

  // define_scheme_literal_variable(proc, value, "A scheme procedure to call the script of that name");
}


SCM
scheme_debug_object (SCM optional)
{
  DenemoObject *curObj;

  if (!Denemo.gui || !(Denemo.gui->si) || !(Denemo.gui->si->currentobject) || !(curObj = Denemo.gui->si->currentobject->data))
    return SCM_BOOL (FALSE);
  g_print ("*************\nType = %d\nbasic_durinticks = %d\ndurinticks - %d\nstarttickofnextnote = %d\n***********\n", curObj->type, curObj->basic_durinticks, curObj->durinticks, curObj->starttickofnextnote);
  return SCM_BOOL (TRUE);
}

SCM
scheme_destroy_scheme_init (void)
{
	if(confirm(_("Destroying Customized Buttons"), _("Remove buttons and other customized scheme on startup?"))) 
	{
		destroy_local_scheme_init();
		return SCM_BOOL_T;
	}
return SCM_BOOL_F;
}
SCM
scheme_load_keybindings (SCM name)
{
  char *filename;
  if (scm_is_string (name))
    {
      filename = scm_to_locale_string (name);
      if (load_xml_keybindings (filename) == 0)
        {
          free (filename);
          return SCM_BOOL_T;
        }
      gchar *name = g_build_filename (get_user_data_dir (TRUE), COMMANDS_DIR, filename, NULL);
      if (load_xml_keybindings (name) == 0)
        {
          free (filename);
          //g_free(name); CHECKME
          return SCM_BOOL_T;
        }
      g_free (name);
      name = g_build_filename (get_user_data_dir (TRUE), "download", COMMANDS_DIR, filename, NULL);
      if (load_xml_keybindings (name) == 0)
        {
          //g_free(name); CHECKME
          return SCM_BOOL_T;
        }
      g_free (name);
      name = g_build_filename (get_system_data_dir (), COMMANDS_DIR, filename, NULL);
      if (load_xml_keybindings (name) == 0)
        {
          //g_free(name); CHECKME
          return SCM_BOOL_T;
        }
      g_free (name);
    }
  //if (name) g_free(name); CHECKME
  return SCM_BOOL_F;
}

SCM
scheme_save_keybindings (SCM name)
{
  char *filename;
  if (scm_is_string (name))
    {
      filename = scm_to_locale_string (name);
      if (save_xml_keybindings (filename) == 0)
        {
          if (filename)
            free (filename);
          return SCM_BOOL_T;
        }
    }
  return SCM_BOOL_F;
}

SCM
scheme_clear_keybindings (SCM optional)
{
  keymap_clear_bindings (Denemo.map);
  return SCM_BOOL_T;
}


SCM
scheme_load_commandset (SCM name)
{
  char *filename;
  if (scm_is_string (name))
    {
      filename = scm_to_locale_string (name);
      if (load_xml_keymap (filename) == 0)
        {
          if (filename)
            free (filename);
          return SCM_BOOL_T;
        }
    }
  return SCM_BOOL_F;
}

#ifdef _WITH_X11_
#if 1                           //GTK3 Test
SCM
scheme_user_screenshot (SCM type, SCM position)
{
  GList **sources;
  SCM ret = SCM_BOOL_F;
  gint pos = -1;
  if ((!SCM_UNBNDP (position)) && scm_is_integer (position))
    pos = scm_to_int (position);

  if (scm_is_false (type))
    sources = &Denemo.gui->si->sources;
  else
    sources = &((DenemoStaff *) Denemo.gui->si->currentstaff->data)->sources;
  scheme_hide_window (SCM_BOOL_T);
  GdkRectangle *rect = screenshot_find_rectangle ();
  if (rect)
    {
      //g_print("%d %d %d %d\n", rect->x, rect->y, rect->width, rect->height);
      GdkPixbuf *screenshot = screenshot_get_pixbuf (gdk_get_default_root_window (), rect);
      if (screenshot)
        {
          *sources = g_list_insert (*sources, GINT_TO_POINTER (screenshot), pos);       //-1 appends
          ret = SCM_BOOL_T;
        }
    }
  scheme_hide_window (SCM_BOOL_F);

  return ret;
}

SCM
scheme_delete_screenshot (SCM type)
{
  GList **sources;
  if (scm_is_false (type))
    sources = &Denemo.gui->si->sources;
  else
    sources = &((DenemoStaff *) Denemo.gui->si->currentstaff->data)->sources;
  if (*sources)
    {
      GList *g = g_list_nth (*sources, Denemo.gui->si->currentmeasurenum - 1);
      if (g)
        {
          *sources = g_list_remove_link (*sources, g);
          //FIXME free g->data and g
          return SCM_BOOL_T;
        }
    }
  return SCM_BOOL_F;
}
#endif
#endif //_WITH_X11_
SCM
scheme_push_clipboard (SCM optional)
{
  push_clipboard ();
  return SCM_BOOL_T;
}

SCM
scheme_pop_clipboard (SCM optional)
{
  if (pop_clipboard ())
    return SCM_BOOL_T;
  else
    return SCM_BOOL_F;
}

SCM
scheme_delete_selection (SCM optional)
{
  if ((!Denemo.gui->si) || (!Denemo.gui->si->markstaffnum))
    return SCM_BOOL_F;
  delete_selection ();
  return SCM_BOOL_T;
}

SCM
scheme_set_thumbnail_selection (SCM optional)
{
  if ((!Denemo.gui->si) || (!Denemo.gui->si->markstaffnum))
    return SCM_BOOL_F;
  if (Denemo.gui->si == Denemo.gui->movements->data)
    {
      memcpy (&Denemo.gui->thumbnail, &Denemo.gui->si->selection, sizeof (DenemoSelection));
      return SCM_BOOL_T;
    }
  return SCM_BOOL_F;
}

SCM
scheme_set_newbie (SCM optional)
{
  SCM ret = SCM_BOOL (Denemo.prefs.newbie);
  if (scm_is_true (optional))
    {
      Denemo.prefs.tooltip_timeout = Denemo.prefs.tooltip_browse_timeout = 0;
      Denemo.prefs.learning = 1;
      Denemo.prefs.newbie = 1;
    }
  else
    {
      Denemo.prefs.tooltip_timeout = Denemo.prefs.tooltip_browse_timeout = 2000;
      Denemo.prefs.newbie = 0;
      gtk_widget_set_tooltip_text (Denemo.scorearea, NULL);
    }
  return ret;
}

SCM
scheme_get_checksum (SCM str)
{
  SCM ret = SCM_BOOL_F;
  if (scm_is_string (str))
    {
      gchar *chk;
      gchar *thestring = scm_to_locale_string (str);
      chk = g_compute_checksum_for_string (G_CHECKSUM_MD5, thestring, -1);
      ret = scm_from_locale_string (chk);
      g_free (chk);
    }
  return ret;
}

SCM
scheme_create_thumbnail (SCM optional)
{
#ifndef USE_EVINCE
  g_debug("This feature requires denemo to be built with evince");
  return SCM_BOOL_F;
#else
  gboolean ret;
  if ((!SCM_UNBNDP (optional)) && scm_is_true (optional))
    ret = create_thumbnail (TRUE);
  else
    ret = create_thumbnail (FALSE);
  return SCM_BOOL (ret);
#endif
}

SCM
scheme_exit (SCM optional)
{
  exit (0);
}

SCM
scheme_create_layout (SCM name, SCM force)
{
  if (scm_is_string (name))
    {
      gchar *layout_name = scm_to_locale_string (name);
      if (create_custom_scoreblock (layout_name, scm_is_true (force)))
        return SCM_BOOL_T;
    }
  return SCM_BOOL_F;
}

SCM
scheme_lilypond_for_part (void)
{
  gint save = Denemo.gui->si->markstaffnum;
  Denemo.gui->si->markstaffnum = 0;
  if (!select_custom_layout_for_name (((DenemoStaff *) (Denemo.gui->si->currentstaff->data))->lily_name->str))
    generate_lilypond_part ();
  Denemo.gui->si->markstaffnum = save;
  return SCM_BOOL_T;
}

SCM
scheme_typeset_part (void)
{
#ifndef USE_EVINCE
  g_debug("This feature requires denemo to be built with evince");
  return SCM_BOOL_F;
#else
  typeset_part ();
  return SCM_BOOL_T;
#endif
}

SCM
scheme_reduce_layout_to_lilypond (void)
{
  make_scoreblock_editable ();
  return SCM_BOOL_T;
}

SCM
scheme_get_current_staff_layout_id (void)
{
  guint id;
  if (((DenemoStaff *) (Denemo.gui->si->currentstaff->data))->voicecontrol == DENEMO_PRIMARY)
  {
    id = get_layout_id_for_name(((DenemoStaff *) (Denemo.gui->si->currentstaff->data))->lily_name->str);
    return scm_from_int(id);
  }
  return SCM_BOOL_F;
}
SCM
scheme_get_layout_id (void)
{
  DenemoScoreblock *sb = selected_scoreblock ();
  if (sb)
    return scm_from_int (sb->id);
  return SCM_BOOL_F;
}
SCM
scheme_select_layout_id (SCM the_id)
{
  if (scm_is_integer (the_id))
    {
      gint id = scm_to_int (the_id);
      return SCM_BOOL (select_layout_id (id));
    }
  return SCM_BOOL_F;
}

SCM
scheme_select_default_layout (void)
{
  select_default_scoreblock ();
  return SCM_BOOL_T;
}

SCM
scheme_get_layout_name (void)
{
  DenemoScoreblock *sb = selected_scoreblock ();
  if (sb && sb->name)
    return scm_from_locale_string (sb->name);
  return SCM_BOOL_F;
}

SCM
scheme_select_next_layout (void)
{
  if (gtk_widget_get_visible (Denemo.gui->score_layout))
    {
      DenemoScoreblock *sb = get_next_scoreblock ();
      return sb ? SCM_BOOL_T : SCM_BOOL_F;
    }
  return SCM_BOOL_F;
}

SCM
scheme_select_first_layout (void)
{
  if (gtk_widget_get_visible (Denemo.gui->score_layout))
    {
      DenemoScoreblock *sb = get_first_scoreblock ();
      return sb ? SCM_BOOL_T : SCM_BOOL_F;
    }
  return SCM_BOOL_F;
}


SCM
scheme_select_next_custom_layout (void)
{
  return SCM_BOOL (iterate_custom_layout (FALSE));
}

SCM
scheme_select_first_custom_layout (void)
{
  return SCM_BOOL (iterate_custom_layout (TRUE));
}

SCM
scheme_open_source (SCM link)
{
  SCM ret = SCM_BOOL_F;
#ifndef USE_EVINCE
  g_debug("This feature requires denemo to be built with evince");
#else
  if (scm_is_string (link))
    {
      gchar *thestring = scm_to_locale_string (link);
      gchar *filename;
#ifdef G_OS_WIN32
		filename = thestring;
		(void) strtok (thestring+2, ":");//skip leading drive name on windows
#else      
       filename = strtok (thestring, ":");//will not work if filename contains ':' characters.
#endif
      if (filename)
        {
          gint x, y, page;
          
          gchar *xstr = strtok (NULL, ":");
          gchar *ystr = strtok (NULL, ":");
          gchar *pstr = strtok (NULL, ":");
          x = xstr ? atoi (xstr) : 0;
          y = ystr ? atoi (ystr) : 0;
          page = pstr ? atoi (pstr) : 0;
     
          if (open_source (filename, x, y, page))
            ret = SCM_BOOL_T;
        }
      if (thestring)
        free (thestring);
    }
#endif
  return ret;
}

SCM
scheme_open_source_file (SCM optional)
{
  if (open_source_file ())
    return SCM_BOOL_T;
  return SCM_BOOL_F;
}

SCM
scheme_export_recorded_audio (void)
{

  return SCM_BOOL (export_recorded_audio ());

}

SCM
scheme_open_source_audio_file (SCM optional)
{
	if(open_source_audio_file () && Denemo.gui->si->recording && Denemo.gui->si->recording->samplerate) {
		return scm_from_double (Denemo.gui->si->recording->nframes/(double)Denemo.gui->si->recording->samplerate);
	}
  return SCM_BOOL_F;
}

SCM
scheme_close_source_audio (SCM optional)
{
  return SCM_BOOL (close_source_audio ());
}

SCM
scheme_start_audio_play (SCM annotate)
{
  if (Denemo.gui->si->recording)
    {
      start_audio_playing (scm_is_true (annotate));
      return SCM_BOOL_T;
    }
  return SCM_BOOL_F;
}

SCM
scheme_set_audio_lead_in (SCM seconds)
{
  if (scm_is_real (seconds))
    {
      gdouble secs = scm_to_double (seconds);
      return SCM_BOOL (set_lead_in (secs));
    }
  return SCM_BOOL_F;
}

SCM
scheme_stop_audio_play (SCM annotate)
{
  if (audio_is_playing ())
    {
      stop_audio_playing ();
      return SCM_BOOL_T;
    }
  return SCM_BOOL_F;
}

SCM
scheme_audio_is_playing (void)
{
  return SCM_BOOL (audio_is_playing ());
}

SCM
scheme_next_audio_timing (SCM optional)
{
  if (Denemo.gui->si->recording)
    {
      gdouble timing = get_audio_timing ();
      if (timing > 0.0)
        return scm_from_double (timing);
    }
  return SCM_BOOL_F;
}

SCM
scheme_take_snapshot (SCM optional)
{
  return SCM_BOOL (take_snapshot ());
}

SCM
scheme_increase_guard (SCM optional)
{
  if (Denemo.gui->si->undo_guard++)
    return SCM_BOOL_F;
  return SCM_BOOL_T;
}

SCM
scheme_decrease_guard (SCM optional)
{
  if (Denemo.gui->si->undo_guard > 0)
    return SCM_BOOL (!--Denemo.gui->si->undo_guard);
  Denemo.gui->si->undo_guard = 0;
  return SCM_BOOL_T;
}

//From a script undo must undo only the modifications to the start of the script, and push another STAGE_END for the end of the actions that it will do after the invocation of undo. This function overrides the built-in undo called directly by the user.
SCM
scheme_undo (SCM optional)
{
  stage_undo (Denemo.gui->si, ACTION_STAGE_START);
  undowrapper (NULL, NULL);
  stage_undo (Denemo.gui->si, ACTION_STAGE_END);
  return SCM_BOOL_T;
}

//Break the script up for undo purposes
SCM
scheme_stage_for_undo (SCM optional)
{
  stage_undo (Denemo.gui->si, ACTION_STAGE_START);
  stage_undo (Denemo.gui->si, ACTION_STAGE_END);
  return SCM_BOOL_T;
}

SCM
scheme_get_last_change (SCM optional)
{
  SCM ret = SCM_BOOL_F;
  gchar *last = get_last_change (Denemo.gui->si);
  if (last)
    ret = scm_from_locale_string (last);
  g_free (last);
  return ret;
}




SCM
scheme_new_window (SCM optional)
{
  stage_undo (Denemo.gui->si, ACTION_STAGE_START);

  //gint current =  Denemo.gui->scorearea->allocation.width;
  newview (NULL, NULL);
  // Denemo.gui->scorearea->allocation.width = current;

  stage_undo (Denemo.gui->si, ACTION_STAGE_END);
  return SCM_BOOL_T;
}


SCM
scheme_zoom (SCM factor)
{
  if (scm_is_real (factor))
    Denemo.gui->si->zoom = scm_to_double (factor);
  else if (scm_is_string (factor))
    {
      char *name;
      name = scm_to_locale_string (factor);
      if (name)
        {
          Denemo.gui->si->zoom = atof (name);
          free (name);
        }
    }
  else
    {
      return scm_from_double (Denemo.gui->si->zoom);
    }
  scorearea_configure_event (Denemo.scorearea, NULL);
  if (Denemo.gui->si->zoom > 0.01)
    {
      return scm_from_int (Denemo.gui->si->zoom);
    }
  Denemo.gui->si->zoom = 1.0;
  return SCM_BOOL_F;
}

SCM
scheme_master_tempo (SCM factor)
{
  DenemoScore *si = Denemo.gui->si;
  gdouble request_time = get_time ();
  gdouble duration = request_time - si->tempo_change_time;
  si->start_player += duration * (1.0 - si->master_tempo);

  if (scm_is_real (factor))
    si->master_tempo = scm_to_double (factor);
  else if (scm_is_string (factor))
    {
      char *name;
      name = scm_to_locale_string (factor);
      if (name)
        {
          si->master_tempo = atof (name);
          free (name);
        }
    }
  else
    {
      return scm_from_double (si->master_tempo);
    }
  if (si->master_tempo < 0.0)
    si->master_tempo = 1.0;

  si->tempo_change_time = request_time;
  return scm_from_double (si->master_tempo);
}

SCM
scheme_movement_tempo (SCM bpm)
{
  DenemoScore *si = Denemo.gui->si;
  if (scm_is_real (bpm))
    si->tempo = scm_to_int (bpm);
  if (scm_is_string (bpm))
    {
      char *name;
      name = scm_to_locale_string (bpm);
      if (name)
        {
          si->tempo = atof (name);
          free (name);
        }
    }

  if (si->tempo < 1)
    si->tempo = 120;
  return scm_from_int (si->tempo);
}

SCM
scheme_master_volume (SCM factor)
{
  DenemoScore *si = Denemo.gui->si;
  if (scm_is_real (factor))
    si->master_volume = scm_to_double (factor);
  if (scm_is_string (factor))
    {
      char *name;
      name = scm_to_locale_string (factor);
      if (name)
        {
          si->master_volume = atof (name);
          free (name);
        }
    }
  if (si->master_volume < 0.0)
    si->master_volume = 1.0;
  return scm_from_double (si->master_volume);
}

SCM
scheme_staff_master_volume (SCM level)
{
  DenemoStaff *thestaff = (DenemoStaff *) Denemo.gui->si->currentstaff->data;
  if (scm_is_real (level))
    {
      gdouble master_volume = scm_to_double (level);
      thestaff->volume = (gint) (master_volume * 127);
      if (thestaff->volume > 127)
        thestaff->volume = 127;
      if (thestaff->volume < 0)
        thestaff->volume = 0;
    }
  return scm_from_double (thestaff->volume / 127.0);
}

SCM
scheme_get_midi_tuning (void)
{
  gchar *cents = get_cents_string ();
  SCM ret = scm_from_locale_string (cents);
  g_free (cents);
  return ret;
}

SCM
scheme_get_sharpest (void)
{
  gchar *name = get_sharpest ();
  SCM ret = scm_from_locale_string (name);
  g_free (name);
  return ret;
}

SCM
scheme_get_flattest (void)
{
  gchar *name = get_flattest ();
  SCM ret = scm_from_locale_string (name);
  g_free (name);
  return ret;
}

SCM
scheme_get_temperament (void)
{
  gchar *name = get_temperament_name ();
  SCM ret = scm_from_locale_string (name);
  g_free (name);
  return ret;
}

static SCM
ignore_handler (gchar * data SCM_UNUSED, SCM tag, SCM throw_args SCM_UNUSED)
{
  // g_warning("ignoring throw");
  return SCM_BOOL_F;
}

void
set_meantone_tuning (gint step)
{
  SCM thestep = scm_from_int (step);
  if (SCM_BOOL_F == scm_internal_catch (SCM_BOOL_T, (scm_t_catch_body) scm_c_lookup, (void *) "SetQuarterCommaMeanTone", (scm_t_catch_handler) ignore_handler, (void *) "whoops"))
    return;
  SCM func_symbol = scm_c_lookup ("SetQuarterCommaMeanTone");
  SCM func = scm_variable_ref (func_symbol);
  scm_call_1 (func, thestep);
}

SCM
scheme_set_enharmonic_position (SCM position)
{
  if (scm_is_integer (position))
    {
      gint pos = scm_to_int (position);
      set_enharmonic_position (pos);
      return SCM_BOOL_T;
    }
  return SCM_BOOL_F;
}

SCM
scheme_rewind_midi (SCM start)
{
  DenemoGUI *gui = Denemo.gui;
  double thetime = 0.0;
  SCM scm = SCM_BOOL_T;
  gint err;
  if ((gui->si->smf == NULL) || (gui->si->smfsync != gui->si->changecount))
    generate_midi ();
  if (scm_is_real (start))
    thetime = scm_to_double (start);
  if (thetime > 0.0)
    {
      err = smf_seek_to_seconds (gui->si->smf, thetime);
      if (err)
        scm = SCM_BOOL_F;
    }
  else
    smf_rewind (gui->si->smf);
  return scm;
}


SCM
scheme_next_midi_notes (SCM interval)
{
  SCM scm = scm_list_n (SCM_UNDEFINED);
  DenemoScore *si = Denemo.gui->si;
  if (scm_is_real (interval))
    {
      double margin = scm_to_double (interval);
      double start = -1.0;      //unset  
      smf_event_t *event = si->smf ? smf_peek_next_event (si->smf) : NULL;
      if (event)
        {
          while ((event = smf_peek_next_event (si->smf)))
            {
              gint key;
              if ((key = noteon_key (event)))
                {
                  if (start < 0.0)
                    start = event->time_seconds;
                  if ((event->time_seconds - start) < margin)
                    {
                      event = smf_get_next_event (si->smf);
                      scm = scm_cons (scm_from_int (key), scm);
                    }
                  else
                    {
                      break;
                    }
                }
              else
                {
                  event = smf_get_next_event (si->smf);
                }
            }
        }
      scm = scm_cons (scm, scm_from_double (start));
      return scm;
    }
  return SCM_BOOL_F;
}

SCM
scheme_get_midi_on_time (void)
{
  if (!(Denemo.gui->si->currentobject))
    return SCM_BOOL_F;
  DenemoObject *curobj = Denemo.gui->si->currentobject->data;
  if (!curobj->midi_events)
    return SCM_BOOL_F;
  return scm_from_double (get_midi_on_time (curobj->midi_events));
}

SCM
scheme_get_midi_off_time (void)
{
  if (!(Denemo.gui->si->currentobject))
    return SCM_BOOL_F;
  DenemoObject *curobj = Denemo.gui->si->currentobject->data;
  if (!curobj->midi_events)
    return SCM_BOOL_F;
  return scm_from_double (get_midi_off_time (curobj->midi_events));
}

SCM
scheme_restart_play (void)
{
  restart_play ();
  return SCM_BOOL_T;
}

static double convert_and_adjust (SCM time) {
	return scm_to_double (time) * get_playback_speed();
}
SCM
scheme_set_playback_interval (SCM start, SCM end)
{
  stop_midi_playback(NULL, NULL);
  if (scm_is_real (start) && scm_is_real (end))
    {
      Denemo.gui->si->start_time = convert_and_adjust (start);
      Denemo.gui->si->end_time = convert_and_adjust (end);
      set_start_and_end_objects_for_draw ();
      return SCM_BOOL_T;
    }
  if (scm_is_real (start))
    {
      Denemo.gui->si->start_time = convert_and_adjust (start);
      set_start_and_end_objects_for_draw ();
      return SCM_BOOL_T;
    }
  if (scm_is_real (end))
    {
      Denemo.gui->si->end_time = convert_and_adjust (end);
      set_start_and_end_objects_for_draw ();
      return SCM_BOOL_T;
    }
  if (scm_is_string (start) && scm_is_string (end))
    {
      char *name;
      name = scm_to_locale_string (start);
      if (name)
        {
          Denemo.gui->si->start_time = atof (name);
          free (name);
        }
      name = scm_to_locale_string (end);
      if (name)
        {
          Denemo.gui->si->end_time = atof (name);
          free (name);
        }
      set_start_and_end_objects_for_draw ();
      return SCM_BOOL_T;
    }
  if (scm_is_string (start))
    {
      char *name;
      name = scm_to_locale_string (start);
      if (name)
        {
          Denemo.gui->si->start_time = atof (name);
          free (name);
        }
      set_start_and_end_objects_for_draw ();
      return SCM_BOOL_T;
    }
  if (scm_is_string (end))
    {
      char *name;
      name = scm_to_locale_string (end);
      if (name)
        {
          Denemo.gui->si->end_time = atof (name);
          free (name);
        }
      set_start_and_end_objects_for_draw ();
      return SCM_BOOL_T;
    }
  return SCM_BOOL_F;
}

SCM
scheme_adjust_playback_start (SCM adj)
{
  SCM ret = SCM_BOOL_F;
  if (scm_is_real (adj))
    {
	  stop_midi_playback(NULL, NULL);
      Denemo.gui->si->start_time += convert_and_adjust (adj);
      if (Denemo.gui->si->start_time < 0.0)
        Denemo.gui->si->start_time = 0.0;
      else
        ret = SCM_BOOL_T;
    }
  set_start_and_end_objects_for_draw ();
  return ret;
}

SCM
scheme_adjust_playback_end (SCM adj)
{
  SCM ret = SCM_BOOL_F;
  if (scm_is_real (adj))
    {
	  stop_midi_playback(NULL, NULL);
      Denemo.gui->si->end_time += convert_and_adjust (adj);
      if (Denemo.gui->si->end_time < 0.0)
        Denemo.gui->si->end_time = 0.0;
      else
        ret = SCM_BOOL_T;
    }
  set_start_and_end_objects_for_draw ();
  return ret;
}


SCM
scheme_get_help (SCM command)
{
  char *name = NULL;
  if (scm_is_string (command))
    name = scm_to_locale_string (command);
  if (name == NULL)
    {
      return SCM_BOOL_F;
    }
  gint idx = lookup_command_from_name (Denemo.map, name);
  if (name)
    free (name);
  if (idx < 0)
    {
#if 0
      SCM help = scm_c_eval_string (g_strconcat ("Help-d-", name));
      return help;
#else
      return SCM_BOOL_F;
#endif
    }
  return scm_from_locale_string ((gchar *) lookup_tooltip_from_idx (Denemo.map, idx));
}

SCM
scheme_get_lily_version (SCM optional)
{
  gchar *version = get_lily_version_string ();
  return scm_from_locale_string (version);
}

SCM
scheme_check_lily_version (SCM check_version)
{
  char *version;
  if (scm_is_string (check_version))
    {
      version = scm_to_locale_string (check_version);
    }
  else
    {
      return SCM_BOOL_F;
    }
  gint result = check_lily_version (version);
  if (version)
    free (version);
  if (result > 0)
    {
      return SCM_BOOL_T;
    }
  else
    {
      return SCM_BOOL_F;
    }
}

SCM
scheme_get_id (SCM command)
{
  char *name;
  if (scm_is_string (command))
    {
      gint id;
      name = scm_to_locale_string (command);
      id = lookup_command_from_name (Denemo.map, name);
      if (name)
        free (name);
      if (id != -1)
        {
          return scm_from_int (id);
        }
    }
  return SCM_BOOL_F;
}

SCM
scheme_add_keybinding (SCM command, SCM binding)
{
  char *shortcut;
  char *name;
  gint id;
  gint old_id = -1;
  if (scm_is_string (binding))
    {
      shortcut = scm_to_locale_string (binding);
      if (scm_is_string (command))
        {
          name = scm_to_locale_string (command);
          old_id = add_keybinding_for_name (name, shortcut);
        }
      else if (scm_is_integer (command))
        {
          id = scm_to_int (command);
          if (id >= 0)
            old_id = add_keybinding_for_command (id, shortcut);
        }
      if (shortcut)
        free (shortcut);
      if (name)
        free (name);
    }
  if (old_id >= 0)
    {
      return scm_from_int (old_id);
    }
  else
    {
      return SCM_BOOL_F;
    }
}

SCM
scheme_get_label (SCM command)
{
  char *name;
  if (scm_is_string (command))
    {
      name = scm_to_locale_string (command);
    }
  else
    {
      return SCM_BOOL_F;
    }
  if (name == NULL)
    {
      return SCM_BOOL_F;
    }
  gint idx = lookup_command_from_name (Denemo.map, name);
  if (name)
    free (name);
  if (idx < 0)
    {
      return SCM_BOOL_F;
    }
  return scm_from_locale_string ((gchar *) lookup_label_from_idx (Denemo.map, idx));
}




SCM
scheme_get_menu_position (SCM command)
{
  char *name;
  SCM ret;
  gchar *menuposition = NULL;
  if (scm_is_string (command))
    {
      name = scm_to_locale_string (command);
    }
  else
    {
      return SCM_BOOL_F;
    }
  if (name == NULL)
    {
      return SCM_BOOL_F;
    }
    
   
    
  gint idx = lookup_command_from_name (Denemo.map, name);
  if (name)
    free (name);
  if (idx < 0)
    {
      return SCM_BOOL_F;
    }
  GtkAction *action = (GtkAction *) lookup_action_from_idx (Denemo.map, idx);
  if (action == NULL)
    return SCM_BOOL_F;
  menuposition = get_menu_position (g_object_get_data (G_OBJECT (action), "menupath"));

  if(menuposition && *menuposition) { 
	ret = scm_from_locale_string (menuposition);
	g_free(menuposition);
	} 
  return ret;
}


SCM
scheme_get_menu_path (SCM command)
{
  char *name;
  if (scm_is_string (command))
    {
      name = scm_to_locale_string (command);
    }
  else
    {
      return SCM_BOOL_F;
    }
  if (name == NULL)
    {
      return SCM_BOOL_F;
    }
  gint idx = lookup_command_from_name (Denemo.map, name);
  if (name)
    free (name);
  if (idx < 0)
    {
      return SCM_BOOL_F;
    }
  GtkAction *action = (GtkAction *) lookup_action_from_idx (Denemo.map, idx);
  if (action == NULL)
    return SCM_BOOL_F;
  gchar *menupath = g_object_get_data (G_OBJECT (action), "menupath");
  if (menupath == NULL)
    {
      return SCM_BOOL_F;
    }
  return scm_from_locale_string (menupath);
}

SCM
scheme_get_verse (SCM number)
{
  gchar *text = NULL;
  DenemoGUI *gui = Denemo.gui;
  if (scm_is_integer (number))
    {
      text = get_lyrics_for_verse_num (scm_to_int (number));

    }
  else
    {
      DenemoStaff *staff = (DenemoStaff *) gui->si->currentstaff->data;
      text = get_lyrics_for_current_verse (staff);
    }
  if (text)
    {
      SCM scm = scm_from_locale_string (text);
      //wrong!! g_free(text);
      return scm;
    }
  return SCM_BOOL_F;
}

SCM
scheme_put_verse (SCM verse)
{
  DenemoGUI *gui = Denemo.gui;
  DenemoStaff *staff = (DenemoStaff *) gui->si->currentstaff->data;
  if (scm_is_string (verse))
    {
      char *text;
      text = scm_to_locale_string (verse);
      gboolean ret = put_lyrics_for_current_verse (staff, text);
      if (text)
        free (text);
      return SCM_BOOL (ret);
    }
  return SCM_BOOL_F;
}

SCM
scheme_append_to_verse (SCM verse)
{
  DenemoGUI *gui = Denemo.gui;
  DenemoStaff *staff = (DenemoStaff *) gui->si->currentstaff->data;
  if (scm_is_string (verse))
    {
      char *text;
      text = scm_to_locale_string (verse);
      gboolean ret = append_lyrics_for_current_verse (staff, text);
      if (text)
        free (text);
      return SCM_BOOL (ret);
    }
  return SCM_BOOL_F;
}

/* write MIDI/Audio filter status */
SCM
scheme_input_filter_names (SCM filtername)
{
  char *name = NULL;

  if (scm_is_string (filtername))
    {
      name = scm_to_locale_string (filtername);
      if (name)
        {
          g_string_printf (Denemo.input_filters, "MIDI Input: %s", name);
          gtk_widget_show (Denemo.input_source);
          write_input_status ();
          free (name);
          return SCM_BOOL_T;
        }
    }
  else
    {
      gtk_widget_hide (Denemo.input_source);
    }
  return SCM_BOOL_F;
}

/* write a status label on bottom right of window*/
SCM
scheme_write_status (SCM filtername)
{
  char *name = NULL;

  if (scm_is_string (filtername))
    {
      name = scm_to_locale_string (filtername);
      if (name)
        {

          g_string_assign (Denemo.input_filters, name);
          gtk_widget_show (Denemo.input_source);
          write_input_status ();
          free (name);
          return SCM_BOOL_T;
        }
    }
  else
    {
      gtk_widget_hide (Denemo.input_source);
    }
  return SCM_BOOL_F;
}

SCM
scheme_goto_position (SCM movement, SCM staff, SCM measure, SCM object)
{
  gint movementnum, staffnum, measurenum, objectnum;
  if (scm_is_integer (movement))
    movementnum = scm_to_int (movement);
  else
    movementnum = g_list_index (Denemo.gui->movements, Denemo.gui->si) + 1;
  if (scm_is_integer (staff))
    staffnum = scm_to_int (staff);
  else
    staffnum = Denemo.gui->si->currentstaffnum;

  if (scm_is_integer (measure))
    measurenum = scm_to_int (measure);
  else
    measurenum = Denemo.gui->si->currentmeasurenum;

  if (scm_is_integer (object))
    objectnum = scm_to_int (object);
  else
    objectnum = 1 + Denemo.gui->si->cursor_x;
#if 0
  // 1 is ambiguous, either empty measure or object 1
  gboolean result = goto_movement_staff_obj (NULL, movementnum, staffnum, measurenum, objectnum);
  if (Denemo.gui->si->currentmeasure->data == NULL && objectnum == 1)
    return SCM_BOOL (goto_movement_staff_obj (NULL, movementnum, staffnum, measurenum, 0));
  gint numobjs = (Denemo.gui->si->currentmeasure->data) ? g_list_length (Denemo.gui->si->currentmeasure->data) : 0;
  if (objectnum == 1 + numobjs)
    Denemo.gui->si->cursor_appending = TRUE;
  write_status (Denemo.gui);
  if (objectnum > 1 + numobjs)
    return SCM_BOOL_F;
  return SCM_BOOL (result);
#endif
  gint origmvt = g_list_index (Denemo.gui->movements, Denemo.gui->si) + 1, origstaff = Denemo.gui->si->currentstaffnum, origmeas = Denemo.gui->si->currentmeasurenum, origpos = 1 + Denemo.gui->si->cursor_x;
  goto_movement_staff_obj (NULL, movementnum, staffnum, measurenum, objectnum);
  if ((movementnum == g_list_index (Denemo.gui->movements, Denemo.gui->si) + 1) && (staffnum == Denemo.gui->si->currentstaffnum) && (measurenum == Denemo.gui->si->currentmeasurenum) && (objectnum == 1 + Denemo.gui->si->cursor_x))
    return SCM_BOOL_T;
  else
    goto_movement_staff_obj (NULL, origmvt, origstaff, origmeas, origpos);

  return SCM_BOOL_F;
}

SCM
scheme_shift_cursor (SCM value)
{
  if (!scm_is_integer (value))
    return SCM_BOOL_F;
  gint shift = scm_to_int (value);
  Denemo.gui->si->cursor_y += shift;
  Denemo.gui->si->staffletter_y = offsettonumber (Denemo.gui->si->staffletter_y + shift);
  return SCM_BOOL_T;



}

SCM
scheme_mid_c_offsettoname (gint offset)
{
  gchar *notename = g_strdup_printf ("%c", mid_c_offsettoname (offset));
  SCM scm = scm_from_locale_string (notename);
  g_free (notename);
  return scm;
}

SCM
scheme_get_horizontal_position (void)
{
  return scm_from_int (1 + Denemo.gui->si->cursor_x);
}

SCM
scheme_set_object_display_width (SCM value)
{
  if (!scm_is_integer (value))
    return SCM_BOOL_F;
  if (Denemo.gui->si->currentobject)
    {
      DenemoObject *obj = Denemo.gui->si->currentobject->data;
      gint minpixels = scm_to_int (value);
      obj->minpixelsalloted = minpixels;
      return SCM_BOOL_T;
    }
  return SCM_BOOL_F;
}




SCM
scheme_get_movement (void)
{
  gint num = g_list_index (Denemo.gui->movements, Denemo.gui->si) + 1;
  return scm_from_int (num);
}

SCM
scheme_get_staff (void)
{
  gint num = Denemo.gui->si->currentstaffnum;
  return scm_from_int (num);
}

SCM
scheme_get_measure (void)
{
  gint num = Denemo.gui->si->currentmeasurenum;
  return scm_from_int (num);
}

SCM
scheme_get_cursor_note (SCM optional)
{
  DenemoGUI *gui = Denemo.gui;
  return scheme_mid_c_offsettoname (gui->si->cursor_y);
}

SCM
scheme_get_cursor_note_with_octave (SCM optional)
{
  DenemoGUI *gui = Denemo.gui;
  scm_from_locale_string (mid_c_offsettolily (gui->si->cursor_y, 0));
  return SCM_BOOL_T;
}


SCM
scheme_set_prefs (SCM xml)
{
  if (scm_is_string (xml))
    {
      char *xmlprefs;
      xmlprefs = scm_to_locale_string (xml);
      gint fail = readxmlprefsString (xmlprefs);
      if (xmlprefs)
        free (xmlprefs);
      return SCM_BOOL (!fail);
    }
  return SCM_BOOL (FALSE);
}


SCM
scheme_get_boolean_pref (SCM pref)
{
  gchar *prefname = NULL;
  gboolean val;
  if (scm_is_string (pref))
    {
      prefname = scm_to_locale_string (pref);
      val = get_bool_pref (prefname);
      free (prefname);
      return SCM_BOOL (val);
    }
  return SCM_BOOL_F;
}

SCM
scheme_get_int_pref (SCM pref)
{
  gchar *prefname = NULL;
  gint val;
  if (scm_is_string (pref))
    {
      prefname = scm_to_locale_string (pref);
      val = get_int_pref (prefname);
      free (prefname);
      return scm_from_int (val);
    }
  return SCM_BOOL_F;
}

SCM
scheme_get_string_pref (SCM pref)
{
  gchar *prefname = NULL;
  gchar *val;
  if (scm_is_string (pref))
    {
      prefname = scm_to_locale_string (pref);
      val = get_string_pref (prefname);
      free (prefname);
      if (val)
        return scm_from_locale_string (val);
    }
  return SCM_BOOL_F;
}

SCM
scheme_attach_quit_callback (SCM callback)
{
  DenemoGUI *gui = Denemo.gui;
  if (scm_is_string (callback))
    {
      char *scheme;
      scheme = scm_to_locale_string (callback);
      gui->callbacks = g_list_prepend (gui->callbacks, scheme);
      if (scheme)
        free (scheme);
    }
  return SCM_BOOL (TRUE);
}

SCM
scheme_detach_quit_callback (void)
{
  DenemoGUI *gui = Denemo.gui;
  if (gui->callbacks)
    {
      g_free (gui->callbacks->data);
      gui->callbacks = g_list_delete_link (gui->callbacks, gui->callbacks);
      return SCM_BOOL (TRUE);
    }
  else
    g_warning ("No callback registered");
  return SCM_BOOL (FALSE);
}

SCM
scheme_get_input_source (void)
{
  return scm_from_int (Denemo.gui->input_source);
}


SCM
scheme_chordize (SCM setting)
{
  DenemoGUI *gui = Denemo.gui;
  DenemoObject *curObj;
  chord *thechord;
  note *thenote;
  if (!Denemo.gui || !(Denemo.gui->si) || !(Denemo.gui->si->currentobject) || !(curObj = Denemo.gui->si->currentobject->data) || (curObj->type != CHORD) || !(thechord = (chord *) curObj->object) || !(thechord->notes) || !(thenote = (note *) thechord->notes->data))
    return SCM_BOOL (FALSE);
  if (SCM_BOOLP (setting))
    {
      gboolean val = scm_to_bool (setting);
      if (thechord->chordize != val)
        {
          thechord->chordize = val;
          score_status (gui, TRUE);
        }
    }
  return SCM_BOOL (TRUE);
}


SCM
scheme_get_note_name (SCM optional)
{
  //char *str=NULL;
  //if(scm_is_string(optional)){
  //str = scm_to_locale_stringn(optional, &length);
  //  }
  DenemoObject *curObj;
  chord *thechord;
  note *thenote;
  if (!Denemo.gui || !(Denemo.gui->si) || !(Denemo.gui->si->currentobject) || !(curObj = Denemo.gui->si->currentobject->data) || (curObj->type != CHORD) || !(thechord = (chord *) curObj->object) || !(thechord->notes) || !(thenote = (note *) thechord->notes->data))
    return SCM_BOOL (FALSE);
  else
    {
      return scheme_mid_c_offsettoname (thenote->mid_c_offset);
    }

}



//Insert rests to the value of the timesig and return the number of rests inserted.
SCM
scheme_put_whole_measure_rests (void)
{
  DenemoGUI *gui = Denemo.gui;
  SCM scm;
  if (!Denemo.gui || !(Denemo.gui->si))
    return SCM_MAKINUM (0);
  else
    {
      gint numerator = gui->si->cursortime1;    // staff->timesig.time1;
      gint denominator = gui->si->cursortime2;  //staff->timesig.time2;
      gboolean dot = TRUE;
      if (numerator % 3)
        dot = FALSE;
      else
        numerator = 2 * numerator / 3;
      gint length = (numerator * 4) / denominator;
      gchar *str = NULL;
      scm = SCM_MAKINUM (1);
      switch (length)
        {
        case 1:                // e.g.  2/8 timesig
          str = g_strdup_printf ("(d-InsertRest2)(d-MoveCursorLeft)%s", dot ? "(d-AddDot)" : "");
          break;
        case 2:
          str = g_strdup_printf ("(d-InsertRest1)(d-MoveCursorLeft)%s", dot ? "(d-AddDot)" : "");
          break;
        case 3:                // e.g. 9/8 timesig
          str = g_strdup_printf ("(d-InsertRest0)(d-InsertRest3)(d-MoveCursorLeft)(d-MoveCursorLeft)");
          scm = SCM_MAKINUM (2);
          break;
        case 4:
          str = g_strdup_printf ("(d-InsertRest0)(d-MoveCursorLeft)%s", dot ? "(d-AddDot)" : "");
          break;
        case 8:
          str = g_strdup_printf ("(d-InsertRest0)(d-InsertRest0)(d-MoveCursorLeft)%s", dot ? "(d-AddDot)" : "");
          scm = SCM_MAKINUM (2);
          break;
        default:
          g_warning ("Not implemented %d %s", length, dot ? "dotted" : "");
          scm = SCM_MAKINUM (0);
          break;
        }
      if (str)
        {
          call_out_to_guile (str);
        }
      g_free (str);
      return scm;
    }
}

SCM
scheme_get_dots (void)
{
  DenemoObject *curObj;
  chord *thechord;
  if (!Denemo.gui || !(Denemo.gui->si) || !(Denemo.gui->si->currentobject) || !(curObj = Denemo.gui->si->currentobject->data) || (curObj->type != CHORD) || !(thechord = (chord *) curObj->object))
    return SCM_BOOL_F;
  return scm_from_int (thechord->numdots);
}

SCM
scheme_get_note_base_duration (void)
{
  DenemoObject *curObj;
  chord *thechord;
  if (!Denemo.gui || !(Denemo.gui->si) || !(Denemo.gui->si->currentobject) || !(curObj = Denemo.gui->si->currentobject->data) || (curObj->type != CHORD) || !(thechord = (chord *) curObj->object))
    return SCM_BOOL_F;
  return scm_from_int (thechord->baseduration);
}

SCM
scheme_get_note_duration (void)
{
  DenemoObject *curObj;
  chord *thechord;
  gint duration;
  gint numdots = 0;
  gchar *str;

  if (!Denemo.gui || !(Denemo.gui->si) || !(Denemo.gui->si->currentobject) || !(curObj = Denemo.gui->si->currentobject->data) || (curObj->type != CHORD) || !(thechord = (chord *) curObj->object))
    return SCM_BOOL_F;
  if (thechord->baseduration >= 0)
    {
      duration = 1 << thechord->baseduration;
      str = g_strdup_printf ("%d", duration);
      if (thechord->numdots)
        {
          gchar *tmp = NULL;
          while (numdots++ < thechord->numdots)
            {
              tmp = g_strdup_printf ("%s" "%c", str, '.');
              g_free (str);
              str = tmp;
            }
        }

      SCM scm = scm_from_locale_string (str);
      g_free (str);
      return scm;
    }
  return SCM_BOOL_F;
}

SCM
scheme_set_duration_in_ticks (SCM duration)
{
  DenemoObject *curObj;
  gint thedur = 0;
  if (scm_is_integer (duration))
    {
      thedur = scm_to_int (duration);
    }
  if (!Denemo.gui || !(Denemo.gui->si) || !(Denemo.gui->si->currentobject) || !(curObj = Denemo.gui->si->currentobject->data))
    return SCM_BOOL_F;
  if (thedur > 0)
    {
      curObj->basic_durinticks = curObj->durinticks = thedur;
      if (curObj->type == CHORD)
        {
          ((chord *) curObj->object)->baseduration = -thedur;
          ((chord *) curObj->object)->numdots = 0;
        }
      objnode *prev = Denemo.gui->si->currentobject->prev;
      DenemoObject *prevObj = prev ? (DenemoObject *) prev->data : NULL;
      gint starttick = (prevObj ? prevObj->starttickofnextnote : 0);
      curObj->starttickofnextnote = starttick + thedur;
      return SCM_BOOL_T;
    }
  return SCM_BOOL_F;
}

SCM
scheme_get_onset_time (void)
{
  DenemoGUI *gui = Denemo.gui;
  DenemoObject *curObj;
  if ((Denemo.gui->si->currentobject) && (curObj = Denemo.gui->si->currentobject->data))
    if ((gui->si->smfsync == gui->si->changecount))
      {
        if (curObj->midi_events)
          {
            smf_event_t *event = (smf_event_t *) curObj->midi_events->data;
            gdouble time = event->time_seconds;
            return scm_from_double (time);
          }
      }
  return SCM_BOOL_F;
}

SCM
scheme_get_recorded_midi_tempo (SCM index)
{
	SCM scm = scm_list_n (SCM_UNDEFINED);
	if(scm_is_integer(index)) {	
	gint idx =scm_to_int (index);
	smf_tempo_t *tempo = get_recorded_midi_tempo (idx);
	if(tempo)
		{
			scm = scm_cons (scm_from_double (tempo->microseconds_per_quarter_note/1000000.0), scm);
			scm = scm_cons (scm_from_int (tempo->denominator), scm);
			scm = scm_cons (scm_from_int (tempo->numerator), scm);
			scm = scm_cons (scm_from_int (tempo->time_seconds), scm);
			return scm;
		}
	}
 return SCM_BOOL_F;
}

SCM
scheme_get_imported_midi_track (SCM index)
{
	if(scm_is_integer(index)) {	
		gint idx =scm_to_int (index);
		if(get_imported_midi_track (idx))
			return SCM_BOOL_F;
		}
 return SCM_BOOL_F;
}
SCM
scheme_get_imported_midi_tracks (void)
{ gint num = get_imported_midi_tracks ();
	if(num<0)
		return SCM_BOOL_F;
	else
		return scm_from_int (num);
}
SCM
scheme_get_recorded_midi_duration (void) {
	gdouble duration = get_recorded_midi_duration ();
	if (duration > 0.0)
		return scm_from_double (duration);
	return SCM_BOOL_F;
}

SCM
scheme_get_duration_in_ticks (void)
{
  DenemoObject *curObj;
  if (!Denemo.gui || !(Denemo.gui->si) || !(Denemo.gui->si->currentobject) || !(curObj = Denemo.gui->si->currentobject->data))
    return SCM_BOOL (FALSE);
  return scm_from_int (curObj->durinticks);
}

SCM
scheme_get_base_duration_in_ticks (void)
{
  DenemoObject *curObj;
  if (!Denemo.gui || !(Denemo.gui->si) || !(Denemo.gui->si->currentobject) || !(curObj = Denemo.gui->si->currentobject->data))
    return SCM_BOOL (FALSE);
  if (curObj->type == CHORD)
    return scm_from_int (((chord *) curObj->object)->baseduration >= 0 ?        /* (* (expt 2 (- 8 number)) 6) */
                         (int) pow (2.0, (8.0 - ((chord *) curObj->object)->baseduration)) * 6 : ((chord *) curObj->object)->baseduration);
  return SCM_BOOL (FALSE);
}


SCM
scheme_get_end_tick (void)
{
  DenemoObject *curObj;
  if (!Denemo.gui || !(Denemo.gui->si) || !(Denemo.gui->si->currentobject) || !(curObj = Denemo.gui->si->currentobject->data))
    return SCM_BOOL (FALSE);
  return scm_from_int (curObj->starttickofnextnote);
}


SCM
scheme_get_start_tick (void)
{
  DenemoObject *curObj;
  if (!Denemo.gui || !(Denemo.gui->si) || !(Denemo.gui->si->currentobject) || !(curObj = Denemo.gui->si->currentobject->data))
    return SCM_BOOL (FALSE);
  return scm_from_int (curObj->starttick);
}


SCM
scheme_get_measure_number (void)
{
  return scm_from_int (Denemo.gui->si->currentmeasurenum);
}




SCM
scheme_get_note (SCM count)
{
  gint index = 0;
  DenemoObject *curObj;
  chord *thechord;
  note *thenote;
  if (scm_is_integer (count))
    {
      index = scm_to_int (count) - 1;
      if (index < 0)
        return SCM_BOOL_F;
    }
  if (!Denemo.gui || !(Denemo.gui->si) || !(Denemo.gui->si->currentobject) || !(curObj = Denemo.gui->si->currentobject->data) || (curObj->type != CHORD) || !(thechord = (chord *) curObj->object) || !(thechord->notes) || !(thenote = (note *) g_list_nth_data (thechord->notes, index)))
    return SCM_BOOL_F;
  else
    {
      gchar *str = g_strdup_printf ("%s", mid_c_offsettolily (thenote->mid_c_offset, thenote->enshift));
      SCM scm = scm_from_locale_string (str);
      g_free (str);
      return scm;
    }

}

SCM
scheme_get_note_from_top (SCM count)
{
  gint index = 1;
  DenemoObject *curObj;
  chord *thechord;
  note *thenote;
  if (scm_is_integer (count))
    {
      index = scm_to_int (count);
      if (index < 1)
        return SCM_BOOL_F;
    }
  if (!Denemo.gui || !(Denemo.gui->si) || !(Denemo.gui->si->currentobject) || !(curObj = Denemo.gui->si->currentobject->data) || (curObj->type != CHORD) || !(thechord = (chord *) curObj->object) || !(thechord->notes))
    return SCM_BOOL_F;
  else
    {
      SCM scm;
      gint end = g_list_length (thechord->notes);
      index = end - index;
      if (index < 0)
        scm = SCM_BOOL_F;
      else
        {
          thenote = (note *) g_list_nth_data (thechord->notes, index);
          gchar *str = g_strdup_printf ("%s", mid_c_offsettolily (thenote->mid_c_offset, thenote->enshift));
          scm = scm_from_locale_string (str);
          g_free (str);
        }
      return scm;
    }

}

SCM
scheme_get_note_from_top_as_midi (SCM count)
{
  gint index = 1;
  DenemoObject *curObj;
  chord *thechord;
  note *thenote;
  if (scm_is_integer (count))
    {
      index = scm_to_int (count);
      if (index < 1)
        return SCM_BOOL_F;
    }
  if (!Denemo.gui || !(Denemo.gui->si) || !(Denemo.gui->si->currentobject) || !(curObj = Denemo.gui->si->currentobject->data) || (curObj->type != CHORD) || !(thechord = (chord *) curObj->object) || !(thechord->notes))
    return SCM_BOOL_F;
  else
    {
      SCM scm;
      gint end = g_list_length (thechord->notes);
      index = end - index;
      if (index < 0)
        scm = SCM_BOOL_F;
      else
        {
          thenote = (note *) g_list_nth_data (thechord->notes, index);
          gint midi = dia_to_midinote (thenote->mid_c_offset) + thenote->enshift;
          scm = scm_from_int (midi);
        }
      return scm;
    }

}


SCM
scheme_spell_check_midi_chord (SCM list)
{
  SCM scm;
  GList *notes = NULL;
  gboolean status;
  if (scm_is_list (list))
    {
      for (scm = list; !scm_is_null (scm); scm = scm_cdr (scm))
        {
          gint note = scm_to_int (scm_car (scm));
          notes = g_list_prepend (notes, GINT_TO_POINTER (note));
        }
      status = check_midi_intervals (notes);
      g_list_free (notes);
      return status ? SCM_BOOL_T : SCM_BOOL_F;
    }
  else
    {
      g_print ("Bad pitch spell list\n");
      return SCM_BOOL_F;
    }
}


SCM
scheme_get_cursor_note_as_midi (SCM optional)
{

  DenemoGUI *gui = Denemo.gui;
  gint midi = dia_to_midinote (gui->si->cursor_y);
  SCM scm = scm_from_int (midi);
  return scm;
}


SCM
scheme_get_note_as_midi (void)
{
  DenemoObject *curObj;
  chord *thechord;
  note *thenote;
  if (!Denemo.gui || !(Denemo.gui->si) || !(Denemo.gui->si->currentobject) || !(curObj = Denemo.gui->si->currentobject->data) || (curObj->type != CHORD) || !(thechord = (chord *) curObj->object) || !(thechord->notes) || !(thenote = (note *) thechord->notes->data))
    return scm_from_int (0);
  else
    {
      gint midi = dia_to_midinote (thenote->mid_c_offset) + thenote->enshift;
      SCM scm = scm_from_int (midi);
      return scm;
    }
}


SCM
scheme_get_notes (SCM optional)
{
  DenemoObject *curObj;
  chord *thechord;
  note *thenote;
  GString *str = g_string_new ("");
  SCM scm;

  if (!Denemo.gui || !(Denemo.gui->si) || !(Denemo.gui->si->currentobject) || !(curObj = Denemo.gui->si->currentobject->data) || (curObj->type != CHORD) || !(thechord = (chord *) curObj->object) || !(thechord->notes) || !(thenote = (note *) thechord->notes->data))
    return SCM_BOOL (FALSE);
  else
    {
      GList *g;
      for (g = thechord->notes; g; g = g->next)
        {
          thenote = (note *) g->data;
          gchar *name = mid_c_offsettolily (thenote->mid_c_offset, thenote->enshift);
          str = g_string_append (str, name);
          if (g->next)
            str = g_string_append (str, " ");
        }

      scm = scm_from_locale_string (g_string_free (str, FALSE));
      return scm;
    }
}

SCM
scheme_add_movement (SCM optional)
{
  append_blank_movement ();
  return SCM_BOOL_T;
}

SCM
scheme_get_prevailing_clef (SCM optional)
{
  gint theclef = find_prevailing_clef (Denemo.gui->si);
  //FIXME look at directives to see if it is overridden, e.g. drum clef
  const gchar *clefname = get_clef_name (theclef);
  if (clefname)
    return scm_from_locale_string (clefname);
  else
    return SCM_BOOL_F;
}

SCM
scheme_get_prevailing_clef_as_lilypond (SCM optional)
{
  const gchar *clefname = get_prevailing_clef_as_lilypond ();
  if (clefname)
    return scm_from_locale_string (clefname);
  else
    return SCM_BOOL_F;
}

SCM
scheme_get_prevailing_keysig_as_lilypond (SCM optional)
{
  const gchar *keysigname = get_prevailing_keysig_as_lilypond ();
  if (keysigname)
    return scm_from_locale_string (keysigname);
  else
    return SCM_BOOL_F;
}

SCM
scheme_get_prevailing_timesig_as_lilypond (SCM optional)
{
  const gchar *timesigname = get_prevailing_timesig_as_lilypond ();
  if (timesigname)
    return scm_from_locale_string (timesigname);
  else
    return SCM_BOOL_F;
}

SCM
scheme_get_prevailing_duration (SCM optional)
{
  return scm_from_int (get_prevailing_duration ());
}

SCM
scheme_get_prevailing_timesig (SCM optional)
{
  timesig *timesig = get_prevailing_context (TIMESIG);
  //FIXME look at directives to see if it is overridden, e.g. drum clef
  gchar *name = g_strdup_printf ("%d/%d", timesig->time1, timesig->time2);
  SCM ret = scm_from_locale_string (name);
  g_free (name);
  return ret;
}

SCM
scheme_get_prevailing_keysig (SCM optional)
{
  GString *str = g_string_new (" ");
  keysig *keysig = get_prevailing_context (KEYSIG);
  gint i;
  for (i = 0; i < 7; i++)
    g_string_append_printf (str, "%d ", keysig->accs[i]);
  return scm_from_locale_string (g_string_free (str, FALSE));
}

SCM
scheme_set_prevailing_keysig (SCM keyaccs)
{
  //keysigs have a field called "number" which determines how it is drawn, setting like this does not get a keysig drawn, nor does it affect lilypond output
  char *accs = NULL;
  if (scm_is_string (keyaccs))
    {
      accs = scm_to_locale_string (keyaccs);
    }
  if (!accs)
    {
      return SCM_BOOL_F;
    }
  keysig *keysig = get_prevailing_context (KEYSIG);
  sscanf (accs, "%d%d%d%d%d%d%d", keysig->accs + 0, keysig->accs + 1, keysig->accs + 2, keysig->accs + 3, keysig->accs + 4, keysig->accs + 5, keysig->accs + 6);
  showwhichaccidentalswholestaff ((DenemoStaff *) Denemo.gui->si->currentstaff->data);
  free (accs);
  displayhelper (Denemo.gui);   //score_status(Denemo.gui, TRUE);
  return SCM_BOOL_T;
}

SCM
scheme_increment_initial_keysig (SCM amount)
{
  DenemoStaff *curstaff = Denemo.gui->si->currentstaff->data;
  SCM ret = SCM_BOOL_F;
  gint inc = 1;
  if (scm_is_integer (amount))
    inc = scm_to_int (amount);
  keysig *sig = &curstaff->keysig;
  inc += sig->number;
  if (inc < 8 && inc > -8)
    {
      dnm_setinitialkeysig (curstaff, inc, curstaff->keysig.isminor);
      score_status (Denemo.gui, TRUE);
      ret = SCM_BOOL_T;
    }
  return ret;
}

SCM
scheme_increment_keysig (SCM amount)
{
  DenemoStaff *curstaff = Denemo.gui->si->currentstaff->data;
  DenemoObject *curObj = NULL;
  SCM ret = SCM_BOOL_F;
  gint inc = 1;
  if (scm_is_integer (amount))
    inc = scm_to_int (amount);
  keysig *sig = &curstaff->keysig;
  if ((Denemo.gui->si->currentobject) && (curObj = Denemo.gui->si->currentobject->data) && (curObj->type == KEYSIG))
    {
      sig = curObj->object;
    }

  inc += sig->number;
  if (inc < 8 && inc > -8)
    {
      if (sig == &curstaff->keysig)
        {
          dnm_setinitialkeysig (curstaff, inc, curstaff->keysig.isminor);
        }
      else
        {
          sig->number = inc;
          initkeyaccs (sig->accs, inc);
          set_basic_numticks (curObj);
          setpixelmin (curObj);
        }
      score_status (Denemo.gui, TRUE);
      displayhelper (Denemo.gui);
      ret = SCM_BOOL_T;
    }
  return ret;
}

SCM
scheme_cursor_to_note (SCM lilyname)
{
  DenemoGUI *gui = Denemo.gui;
  gint mid_c_offset;
  gint enshift;
  char *notename;

  if (scm_is_string (lilyname))
    {
      notename = scm_to_locale_string (lilyname);
      interpret_lilypond_notename (notename, &mid_c_offset, &enshift);
      gui->si->cursor_y = mid_c_offset;
      gui->si->staffletter_y = offsettonumber (gui->si->cursor_y);
      displayhelper (gui);
      if (notename)
        free (notename);
      return SCM_BOOL (TRUE);
    }
  else
    {
      return SCM_BOOL (FALSE);
    }
}

SCM
scheme_change_chord_notes (SCM lilynotes)
{
  DenemoGUI *gui = Denemo.gui;
  DenemoObject *curObj;
  chord *thechord;
  note *thenote;
  char *notename;
  gchar *chordnote;
  gint mid_c_offset;
  gint enshift;
  gint dclef;
  GList *g = NULL;
  GList *n = NULL;
  GList *directives = NULL;

  if (scm_is_string (lilynotes))
    {

      if (!Denemo.gui || !(Denemo.gui->si) || !(Denemo.gui->si->currentobject) || !(curObj = Denemo.gui->si->currentobject->data) || (curObj->type != CHORD) || !(thechord = (chord *) curObj->object) || !(thechord->notes) || !(thenote = (note *) thechord->notes->data))
        return SCM_BOOL (FALSE);

      else
        {
          /* delete all chord tones */
          while (thechord->notes)
            {
              thenote = thechord->notes->data;
              g = g_list_append (g, thenote->directives);
              thenote->directives = NULL;
              delete_chordnote (gui);
            }
          /* add changed tones */
          dclef = find_prevailing_clef (Denemo.gui->si);
          notename = scm_to_locale_string (lilynotes);
          chordnote = strtok (notename, " ");
          while (chordnote)
            {
              interpret_lilypond_notename (chordnote, &mid_c_offset, &enshift);
              dnm_addtone (curObj, mid_c_offset, enshift, dclef);
              chordnote = strtok (NULL, " ");
            }
          /* paste directives over */
          for (n = thechord->notes; n && g; n = n->next, g = g->next)
            {
              thenote = (note *) n->data;
              directives = (GList *) g->data;

              if (directives)
                thenote->directives = directives;

            }
          score_status (gui, TRUE);
          displayhelper (gui);
          if (notename)
            free (notename);
          return SCM_BOOL (TRUE);
        }
    }
  else
    return SCM_BOOL (FALSE);
}

SCM
scheme_get_user_input (SCM label, SCM prompt, SCM init, SCM modal)
{
  char *title, *instruction, *initial_value;

  if (scm_is_string (label))
    {
      title = scm_to_locale_string (label);
    }
  else
    title = strdup ("Input Required");
  if (scm_is_string (prompt))
    {
      instruction = scm_to_locale_string (prompt);
    }
  else
    instruction = strdup ("Give input: ");

  if (scm_is_string (init))
    {
      initial_value = scm_to_locale_string (init);
    }
  else
    initial_value = strdup (" ");

  gchar *ret = string_dialog_entry_with_widget_opt (Denemo.gui, title, instruction, initial_value, NULL, (modal == SCM_UNDEFINED) || scm_is_true (modal));
  SCM scm = ret ? scm_from_locale_string (ret) : SCM_BOOL_F;

  if (title)
    free (title);
  if (instruction)
    free (instruction);
  if (initial_value)
    free (initial_value);
  if (ret)
    g_free (ret);
  return scm;
}

static void
paste_snippet_lilypond (GtkWidget * button)
{
  DenemoGUI *gui = Denemo.gui;
  GtkWidget *hbox = gtk_widget_get_parent (button);
  GtkWidget *textbuffer = (GtkWidget *) g_object_get_data (G_OBJECT (hbox), "textbuffer");
  if (textbuffer)
    {
      RhythmPattern *r = (gui->currhythm) ? ((RhythmPattern *) gui->currhythm->data) : NULL;
      if (r)
        {
          const gchar *transpose, *clefname = get_prevailing_clef_as_lilypond (), *keysigname = get_prevailing_keysig_as_lilypond (), *timesigname = get_prevailing_timesig_as_lilypond ();
          extern gchar *score_directive_get_postfix (gchar * tagname);
          transpose = score_directive_get_postfix ("TransposeScorePrint");
          transpose = transpose ? transpose : "";
          gchar *text = g_strdup_printf ("§\\raise #6.0 \\score { %s { {%s}{%s}{%s} %s } \\layout {indent=0.0}}§", transpose, clefname, keysigname, timesigname, r->lilypond);
          gtk_text_buffer_insert_at_cursor (GTK_TEXT_BUFFER (textbuffer), text, -1 /*gint len */ );
          g_free (text);
        }
    }
  else
    {
      g_warning ("Denemo program error, widget hierarchy changed???");
    }
  GtkWidget *textview = (GtkWidget *) g_object_get_data (G_OBJECT (hbox), "textview");
  gtk_widget_grab_focus (textview);
}

#define SECTION_UTF8_STRING "§"
//#define SECTION_UTF8_STRING "\302\247"

static gchar *
create_lilypond_from_text (gchar * orig)
{
  gchar *text = g_strdup (orig);
  GString *ret = g_string_new ("");
  g_print ("looking at %s\n", text);
  gunichar section = g_utf8_get_char (SECTION_UTF8_STRING);
  gchar *this = g_utf8_strchr (text, -1, section);
  if (this)
    {
      gchar *start = g_utf8_next_char (this);
      *this = 0;
      if (*text)
        {
          g_string_append_printf (ret, "\\wordwrap-string #\"%s\"", g_strescape (text, "\"\\"));
        }
      gchar *end = g_utf8_strchr (start, -1, section);
      if (end == NULL)
        {
          g_warning ("Unbalanced § marks");
          g_string_free (ret, TRUE);
          return g_strdup ("%{error %}");
        }
      gchar *next = g_utf8_next_char (end);
      *end = 0;
      g_string_append_printf (ret, "%s %s", start, create_lilypond_from_text (next));
    }
  else
    {
      g_string_append_printf (ret, "\\wordwrap-string #\"%s\"", g_strescape (text, "\"\\"));
    }
  g_free (text);
  return g_string_free (ret, FALSE);
}

SCM
scheme_get_user_input_with_snippets (SCM label, SCM prompt, SCM init, SCM modal)
{
  char *title, *instruction, *initial_value;
  SCM scm;
  if (scm_is_string (label))
    {
      title = scm_to_locale_string (label);
    }
  else
    title = strdup ("Input Required");
  if (scm_is_string (prompt))
    {
      instruction = scm_to_locale_string (prompt);
    }
  else
    instruction = strdup ("Give input: ");

  if (scm_is_string (init))
    {
      initial_value = scm_to_locale_string (init);
    }
  else
    initial_value = strdup (" ");
  GtkWidget *hbox = gtk_hbox_new (FALSE, 8);
  GtkWidget *button = gtk_button_new_with_label (_("Paste Current Snippet"));
  gtk_widget_set_tooltip_text (button, _("Pastes the music captured in the currently selected Snippet into the text at the cursor. The music appears here in the LilyPond typesetter syntax between two markers (\302\247). It will print as typeset music embedded in the sentence you are writing.\nYou can edit the syntax, taking care to leave the markers in position. If you delete one marker be sure to delete the other.\n"));
// if(!Denemo.gui->rhythms)
  //   gtk_widget_set_sensitive(button, FALSE);
  g_signal_connect (button, "clicked", G_CALLBACK (paste_snippet_lilypond), NULL);
  gtk_box_pack_start (GTK_BOX (hbox), button, FALSE, TRUE, 0);
  button = gtk_button_new_with_label (_("Next Snippet"));
  gtk_widget_set_tooltip_text (button, _("Makes the next Snippet the one that can be pasted. To see the music snippets you need to check View → Snippets\nThe one selected is in bold black."));
  GtkAction *action = gtk_ui_manager_get_action (Denemo.ui_manager, "/ObjectMenu/NotesRests/SelectDuration/NextRhythm");
  if (action)
    g_signal_connect_swapped (button, "clicked", G_CALLBACK (gtk_action_activate), action);
  else
    gtk_widget_set_sensitive (button, FALSE);
// if(!Denemo.gui->rhythms)
//   gtk_widget_set_sensitive(button, FALSE);
  gtk_box_pack_start (GTK_BOX (hbox), button, FALSE, TRUE, 0);

  gchar *text = string_dialog_editor_with_widget_opt (Denemo.gui, title, instruction, initial_value, hbox, (modal == SCM_UNDEFINED) || scm_is_true (modal));
  if (text)
    {
      gchar *lilypond = create_lilypond_from_text (text);
      scm = scm_cons (scm_from_locale_string (text), scm_from_locale_string (lilypond));
      g_free (lilypond);
    }
  else
    scm = SCM_BOOL_F;

  if (title)
    free (title);
  if (instruction)
    free (instruction);
  if (initial_value)
    free (initial_value);
  if (text)
    g_free (text);

  return scm;
}

SCM
scheme_warningdialog (SCM msg)
{
  char *title;
  if (scm_is_string (msg))
    {
      title = scm_to_locale_string (msg);
    }
  else
    title = strdup ("Script generated warning");

  warningdialog (title);
  if (title)
    free (title);
  return msg;
}

SCM
scheme_infodialog (SCM msg)
{
  char *title;
  if (scm_is_string (msg))
    {
      title = scm_to_locale_string (msg);
      msg = SCM_BOOL (TRUE);
    }
  else
    {
      title = strdup (_("Script error, wrong parameter type to d-InfoDialog"));
      msg = SCM_BOOL (FALSE);
    }
  static GtkWidget *dialog;

  if (dialog)
    {
      gtk_widget_show (dialog);
      gtk_message_dialog_set_markup (GTK_MESSAGE_DIALOG (dialog), title);
    }
  else
    {
      dialog = infodialog (title);
      g_signal_connect (dialog, "delete-event", G_CALLBACK (gtk_widget_hide_on_delete), NULL);
    }
  if (*title)
    {
      gtk_widget_show (dialog);
    }
  else
    {
      gtk_widget_hide (dialog);
    }
  if (title)
    free (title);
  return msg;
}

SCM
scheme_progressbar (SCM msg)
{
  char *title = NULL;
  if (scm_is_string (msg))
    {
      title = scm_to_locale_string (msg);
      progressbar (title, NULL);
      msg = SCM_BOOL (TRUE);
    }
  else
    msg = SCM_BOOL (FALSE);
  if (title)
    free (title);
  return msg;
}

SCM
scheme_progressbar_stop (void)
{
  progressbar_stop ();
  return SCM_BOOL (TRUE);
}

SCM
scheme_typeset_for_script (SCM thescript)
{
  SCM ret = SCM_BOOL_F;
#ifndef USE_EVINCE
  g_debug("This feature requires denemo to be built with evince");
#else  
  if (scm_is_string (thescript))
    {
      gchar *script = scm_to_locale_string (thescript);
      if (typeset_for_script (script))
        ret = SCM_BOOL_T;
    }
#endif
  return ret;
}

SCM
scheme_print_typeset_pdf (void)
{
#ifndef USE_EVINCE
  g_debug("This feature requires denemo to be built with evince");
  return SCM_BOOL_F;
#else
  return print_typeset_pdf ()? SCM_BOOL_F : SCM_BOOL_T;
#endif
}

SCM
scheme_get_char (void)
{

  GdkEventKey event;
  gboolean success = intercept_scorearea_keypress (&event);
  if (success)
    {
      gchar *str = g_strdup_printf ("%c", success ? event.keyval : 0);
      SCM scm = scm_from_locale_string (str);
      g_free (str);
      return scm;
    }
  else
    return SCM_BOOL (FALSE);
}

SCM
scheme_get_keypress (void)
{
  GdkEventKey event;
  gboolean success = intercept_scorearea_keypress (&event);
  if (success)
    {
      gchar *str = dnm_accelerator_name (event.keyval, event.state);
      SCM scm = scm_from_locale_string (str);
      g_free (str);
      return scm;
    }
  else
    return SCM_BOOL (FALSE);
}

/* get last keypress that successfully invoked a command */
SCM
scheme_get_command_keypress (void)
{
  gchar *str = dnm_accelerator_name (Denemo.last_keyval, Denemo.last_keystate);
  SCM scm = scm_from_locale_string (str);
  g_free (str);
  return scm;
}



SCM
scheme_get_command (void)
{
  GdkEventKey event;
  GString *name = g_string_new ("");
  gboolean success = intercept_scorearea_keypress (&event);
  if (success)
    {
      gint cmd = lookup_command_for_keyevent (&event);
      //g_print("command %d for %x %x\n", cmd, event.keyval, event.state);
      if (cmd != -1)
        name = g_string_append (name, lookup_name_from_idx (Denemo.map, cmd));  //FIXME NULL?, memory leaks
      name = g_string_prepend (name, DENEMO_SCHEME_PREFIX);
    }
  SCM scm = success ? scm_from_locale_string (name->str) : SCM_BOOL (FALSE);
  g_string_free (name, TRUE);
  return scm;
}


gchar *
return_command (gchar * name, GdkEvent * event)
{
  return name;
}

/* listens for a shortcut and returns a command, or if keypresses are not shortcut returns #f */
SCM
scheme_get_command_from_user (void)
{

  GdkEventKey event;

  if (intercept_scorearea_keypress (&event))
    {
      gchar *command = process_key_event (&event, &return_command);
      if (command == NULL)
        return SCM_BOOL_F;
      if (*command == 0)
        {                       //can be two-key shortcut
          if (intercept_scorearea_keypress (&event))
            {
              command = process_key_event (&event, &return_command);
              if (command == NULL)
                return SCM_BOOL_F;
            }
          else
            return SCM_BOOL_F;
        }
      write_status (Denemo.gui);
      SCM scm = scm_from_locale_string (command);       //command is from lookup_name_from... functions, do not free.
      return scm;
    }
  return SCM_BOOL_F;
}


/*UNUSED
static void
get_drag_offset (GtkWidget * dialog, gint response_id, GtkLabel * label)
{
  g_object_set_data (G_OBJECT (dialog), "offset-response", (gpointer) (intptr_t) response_id);
  if (response_id < 0)
    gtk_main_quit ();
  gint offsetx, offsety;
  offsetx = (intptr_t) g_object_get_data (G_OBJECT (Denemo.printarea), "offsetx");
  offsety = (intptr_t) g_object_get_data (G_OBJECT (Denemo.printarea), "offsety");
  gchar *text = g_strdup_printf ("Offset now %d %d. Drag again in the print window to change\nOr click OK to apply the position shift", offsetx, offsety);
  gtk_label_set_text (label, text);
  g_free (text);
}*/

static void
get_drag_pad (GtkWidget * dialog, gint response_id, GtkLabel * label)
{
  g_object_set_data (G_OBJECT (dialog), "pad-response", (gpointer) (intptr_t) response_id);
  if (response_id < 0)
    gtk_main_quit ();
  gint padding;
  padding = (intptr_t) g_object_get_data (G_OBJECT (Denemo.printarea), "padding");
  gchar *text = g_strdup_printf ("Padding now %d. Drag again in the print window to change\nOr click OK to apply the padding to the graphical object belonging to the directive", padding);
  gtk_label_set_text (label, text);
  g_free (text);
}



/* return a string representing the relative font size the user wishes to use*/
SCM
scheme_get_relative_font_size (void)
{
  if (Denemo.printarea == NULL)
    return SCM_BOOL (FALSE);
  gchar *value = g_object_get_data (G_OBJECT (Denemo.printarea), "font-size");
  if (value)
    g_free (value);
  value = string_dialog_entry (Denemo.gui, "Font Size", "Give a value (+/-) to adjust font size by", "0");
  if (!value)
    value = g_strdup ("0");
  gchar *clean = g_strdup_printf ("%d", atoi (value));
  g_free (value);
  g_object_set_data (G_OBJECT (Denemo.printarea), "font-size", (gpointer) clean);
  return scm_from_locale_stringn (clean, strlen (clean));
}

void get_clipboard (GtkAction * action, DenemoScriptParam * param);
/* return a string from the X selection */
SCM
scheme_get_text_selection (void)
{
  SCM ret;
  DenemoScriptParam param;
  get_clipboard (NULL, &param);
  if (param.status)
    {
      ret = scm_from_locale_stringn (param.string->str, param.string->len);
      g_string_free (param.string, TRUE);
    }
  else
    ret = SCM_BOOL (FALSE);
  return ret;
}






/* return a string representing the padding desired for some lilypond graphic
 or #f if no printarea or user cancels*/
SCM
scheme_get_padding (void)
{
  SCM ret;
  if (Denemo.printarea == NULL)
    return SCM_BOOL (FALSE);
  if (g_object_get_data (G_OBJECT (Denemo.printarea), "pad-dialog"))
    {
      warningdialog (_("Already in a padding dialog"));
      return SCM_BOOL_F;
    }

  gint padding = (intptr_t) g_object_get_data (G_OBJECT (Denemo.printarea), "padding");

  GtkWidget *dialog = gtk_dialog_new_with_buttons ("Select Padding in Print Window",
                                                   GTK_WINDOW (Denemo.window),
                                                   (GtkDialogFlags) (GTK_DIALOG_DESTROY_WITH_PARENT),
                                                   GTK_STOCK_OK, GTK_RESPONSE_ACCEPT,
                                                   GTK_STOCK_CANCEL, GTK_RESPONSE_REJECT,
                                                   NULL);
  g_object_set_data (G_OBJECT (Denemo.printarea), "pad-dialog", (gpointer) dialog);
  GtkWidget *vbox = gtk_vbox_new (FALSE, 8);

  GtkWidget *content_area = gtk_dialog_get_content_area (GTK_DIALOG (dialog));
  gtk_container_add (GTK_CONTAINER (content_area), vbox);

  gchar *text = g_strdup_printf ("Current padding is %d\nUse right click in print window to change this\nClick OK to apply the padding to the music item drawn by the directive", padding);
  GtkWidget *label = gtk_label_new (text);
  g_free (text);
  gtk_box_pack_start (GTK_BOX (vbox), label, FALSE, TRUE, 0);
  gtk_widget_show_all (dialog);

  gint val;

  g_signal_connect (dialog, "response", G_CALLBACK (get_drag_pad), label);
  gtk_widget_show_all (dialog);
  gtk_main ();
  padding = (intptr_t) g_object_get_data (G_OBJECT (Denemo.printarea), "padding");
  val = (intptr_t) g_object_get_data (G_OBJECT (dialog), "pad-response");
  g_object_set_data (G_OBJECT (Denemo.printarea), "pad-dialog", NULL);
  gtk_widget_destroy (dialog);
  if (val == GTK_RESPONSE_ACCEPT)
    {
      gchar *pad = g_strdup_printf ("%d", padding / 10);
      ret = scm_from_locale_string (pad);
      g_free (pad);
    }
  else
    ret = SCM_BOOL (FALSE);
  return ret;
}



/* create a dialog with the options & return the one chosen, of #f if
   the user cancels
*/
SCM
scheme_get_option (SCM options)
{
  gchar *response = NULL;
  size_t length;
  //gchar *str=NULL;
  if (scm_is_string (options))
    {
      char *str_unterm;
      str_unterm = scm_to_locale_stringn (options, &length);
      response = get_option (str_unterm, length);       //returns NULL or a pointer to a location in str_unterm
      //g_print("Got %p holding %s\n", response, response);
      if (response)
        response = g_strdup (response);
      if (str_unterm)
        free (str_unterm);
    }
  if (response)
    {
      SCM ret = scm_from_locale_stringn (response, strlen (response));
      //g_print("Freeing %p holding %s\n", response, response);
      g_free (response);        //FIXME the g_strdup above is not needed?
      return ret;
      //return scm_from_locale_stringn (response, strlen(response));
    }
  else
    {
      return SCM_BOOL_F;
    }
}


/* Scheme interface to DenemoDirectives (formerly LilyPond directives attached to notes/chords) */


SCM
scheme_lock_directive (SCM lock)
{
  DenemoObject *curObj;
  DenemoDirective *directive;
  if (!Denemo.gui || !(Denemo.gui->si) || !(Denemo.gui->si->currentobject) || !(curObj = Denemo.gui->si->currentobject->data) || (curObj->type != LILYDIRECTIVE) || !(directive = (DenemoDirective *) curObj->object))
    return SCM_BOOL (FALSE);
  directive->locked = scm_is_true (lock);
  return SCM_BOOL_T;
}


/* store the script to be invoked as an action for a directive tagged with tag */
SCM
scheme_set_action_script_for_tag (SCM tag, SCM script)
{
  if (scm_is_string (tag))
    {
      char *the_tag;
      the_tag = scm_to_locale_string (tag);
      if (scm_is_string (script))
        {
          char *the_script;
          the_script = scm_to_locale_string (script);
          gchar *stored_script = g_strdup (the_script); //FIXME
          free (the_script);
          set_action_script_for_tag (the_tag, stored_script);
          if (the_tag)
            free (the_tag);
          return SCM_BOOL (TRUE);
        }
      if (the_tag)
        free (the_tag);
    }
  return SCM_BOOL (FALSE);
}



#define GET_NTH_TAG(what)\
 SCM scheme_##what##_directive_get_nth_tag(SCM index) {\
  gint n;\
  if(!scm_is_integer(index))\
     return SCM_BOOL_F;\
    n = scm_to_int(index);\
  extern gchar *get_nth_##what##_tag (gint n);\
  gchar *val = get_nth_##what##_tag (n);\
  if(val) return scm_from_locale_stringn (val, strlen(val));\
  return SCM_BOOL_F;\
}
GET_NTH_TAG (chord);
GET_NTH_TAG (note);
GET_NTH_TAG (staff);
GET_NTH_TAG (voice);
GET_NTH_TAG (score);
GET_NTH_TAG (clef);
GET_NTH_TAG (timesig);
GET_NTH_TAG (tuplet);
GET_NTH_TAG (stemdirective);
GET_NTH_TAG (keysig);
GET_NTH_TAG (scoreheader);
GET_NTH_TAG (header);
GET_NTH_TAG (paper);
GET_NTH_TAG (layout);
GET_NTH_TAG (movementcontrol);
#undef GET_NTH_TAG








#define GET_TAG_FN_DEF(what)\
 SCM scheme_##what##_directive_get_tag(SCM tag) {\
  char *tagname;\
  if(!scm_is_string(tag))\
     tagname = NULL;\
  else { \
    tagname = scm_to_locale_string(tag);\
  } \
  extern gchar *what##_directive_get_tag (gchar *tagname);\
  gchar *val = (gchar*)what##_directive_get_tag ((gchar*)tagname);\
  if(val){\
    SCM ret = scm_from_locale_stringn (val, strlen(val));\
    if(tagname) free(tagname);\
    return ret;\
  }\
  if(tagname) free(tagname);\
  return SCM_BOOL(FALSE);\
}

GET_TAG_FN_DEF (object);
GET_TAG_FN_DEF (standalone);
GET_TAG_FN_DEF (chord);
GET_TAG_FN_DEF (note);
GET_TAG_FN_DEF (staff);
GET_TAG_FN_DEF (voice);
GET_TAG_FN_DEF (score);
GET_TAG_FN_DEF (clef);
GET_TAG_FN_DEF (timesig);
GET_TAG_FN_DEF (tuplet);
GET_TAG_FN_DEF (stemdirective);
GET_TAG_FN_DEF (keysig);
GET_TAG_FN_DEF (scoreheader);
GET_TAG_FN_DEF (header);
GET_TAG_FN_DEF (paper);
GET_TAG_FN_DEF (layout);
GET_TAG_FN_DEF (movementcontrol);
#undef GET_TAG_FN_DEF
#define ACTIVATE_FN_DEF(what)\
 SCM scheme_activate_##what##_directive(SCM tag) {\
  if(!scm_is_string(tag)){\
    return SCM_BOOL(FALSE);\
  }\
  char *tagname;\
  tagname = scm_to_locale_string(tag);\
  extern gboolean activate_##what##_directive (gchar *tagname);\
  gboolean ret = activate_##what##_directive (tagname);\
  if(tagname) g_free(tagname);\
  return SCM_BOOL(ret);\
}

#define EDIT_FN_DEF(what)\
 SCM scheme_text_edit_##what##_directive(SCM tag) {\
  if(!scm_is_string(tag)){\
    return SCM_BOOL(FALSE);\
  }\
  char *tagname;\
  tagname = scm_to_locale_string(tag);\
  extern gboolean text_edit_##what##_directive (gchar *tagname);\
  gboolean ret = text_edit_##what##_directive (tagname);\
  if(tagname) g_free(tagname);\
  return SCM_BOOL(ret);\
}
#define DELETE_FN_DEF(what)\
 SCM scheme_delete_##what##_directive(SCM tag) {\
  if(!scm_is_string(tag)){\
    return SCM_BOOL(FALSE);\
  }\
  char *tagname;\
  tagname = scm_to_locale_string(tag);\
  extern gboolean delete_##what##_directive (gchar *tagname);\
  gboolean ret = delete_##what##_directive (tagname);\
  if(tagname) free(tagname);\
  return SCM_BOOL(ret);\
}
#define EDIT_DELETE_FN_DEF(what)\
EDIT_FN_DEF(what)\
DELETE_FN_DEF(what)\
ACTIVATE_FN_DEF(what)

EDIT_FN_DEF (standalone) EDIT_DELETE_FN_DEF (note) EDIT_DELETE_FN_DEF (chord) EDIT_DELETE_FN_DEF (staff) EDIT_DELETE_FN_DEF (voice) EDIT_DELETE_FN_DEF (score)
#define GETFUNC_DEF(what, field)\
SCM scheme_##what##_directive_get_##field(SCM tag) {\
  if(!scm_is_string(tag)){\
    return SCM_BOOL(FALSE);\
  }\
  char *tagname;\
  tagname = scm_to_locale_string(tag);\
  extern gchar* what##_directive_get_##field(gchar *tagname);\
  gchar *value = (gchar*)what##_directive_get_##field((gchar*)tagname);\
  if(tagname) free(tagname);\
  if(value){\
    return scm_from_locale_string(value);\
  }\
  return SCM_BOOL(FALSE);\
}
#define PUTFUNC_DEF(what, field)\
SCM scheme_##what##_directive_put_##field(SCM tag, SCM value) {\
  if((!scm_is_string(tag))||(!scm_is_string(value)))\
     return SCM_BOOL(FALSE);\
  char *tagname;\
  tagname = scm_to_locale_string(tag);\
  char *valuename;\
  valuename = scm_to_locale_string(value);\
  extern gboolean what##_directive_put_##field (gchar *tagname, gchar *valuename);\
  gboolean ret = what##_directive_put_##field ((gchar*)tagname, (gchar*)valuename);\
  if(tagname) free(tagname);\
  if(valuename) free(valuename);\
  return SCM_BOOL(ret);\
}
//block to clone for new GString entries in DenemoDirective
  GETFUNC_DEF (note, display);
GETFUNC_DEF (chord, display);
GETFUNC_DEF (standalone, display);
GETFUNC_DEF (staff, display);
GETFUNC_DEF (voice, display);
GETFUNC_DEF (score, display);
GETFUNC_DEF (movementcontrol, display);
PUTFUNC_DEF (note, display);
PUTFUNC_DEF (chord, display);
PUTFUNC_DEF (standalone, display);
PUTFUNC_DEF (staff, display);
PUTFUNC_DEF (voice, display);
PUTFUNC_DEF (score, display);
PUTFUNC_DEF (movementcontrol, display);
// end of block to clone ??? there are now stem tuplet and keysigs as well - see grob
GETFUNC_DEF (note, grob);
GETFUNC_DEF (chord, grob);
GETFUNC_DEF (standalone, grob);
GETFUNC_DEF (staff, grob);
GETFUNC_DEF (voice, grob);
GETFUNC_DEF (score, grob);
/*UNUSED
  GETFUNC_DEF (movementcontrol, grob);
  */
GETFUNC_DEF (clef, grob);
GETFUNC_DEF (timesig, grob);
GETFUNC_DEF (tuplet, grob);
GETFUNC_DEF (stemdirective, grob);
GETFUNC_DEF (keysig, grob);
PUTFUNC_DEF (note, grob);
PUTFUNC_DEF (chord, grob);
PUTFUNC_DEF (standalone, grob);
//PUTFUNC_DEF(staff, grob)
//PUTFUNC_DEF(voice, grob)
PUTFUNC_DEF (score, grob)
//PUTFUNC_DEF(movementcontrol, grob)
  PUTFUNC_DEF (clef, grob)
PUTFUNC_DEF (timesig, grob)
PUTFUNC_DEF (tuplet, grob)
PUTFUNC_DEF (stemdirective, grob)
PUTFUNC_DEF (keysig, grob)
GETFUNC_DEF (note, midibytes)
GETFUNC_DEF (chord, midibytes)
GETFUNC_DEF (standalone, midibytes) 
GETFUNC_DEF (staff, midibytes) 
GETFUNC_DEF (voice, midibytes) 
GETFUNC_DEF (score, midibytes) 
GETFUNC_DEF (movementcontrol, midibytes) 
PUTFUNC_DEF (note, midibytes) 
PUTFUNC_DEF (chord, midibytes) 
PUTFUNC_DEF (standalone, midibytes) 
PUTFUNC_DEF (staff, midibytes) 
PUTFUNC_DEF (voice, midibytes) 
PUTFUNC_DEF (score, midibytes) 
PUTFUNC_DEF (movementcontrol, midibytes) 
GETFUNC_DEF (note, prefix) 
GETFUNC_DEF (note, postfix) 
PUTFUNC_DEF (note, prefix)
  //PUTFUNC_DEF(clef, prefix)
  PUTFUNC_DEF (note, postfix)
GETFUNC_DEF (score, prefix)
GETFUNC_DEF (score, postfix)
PUTFUNC_DEF (score, prefix)
PUTFUNC_DEF (score, postfix)
PUTFUNC_DEF (staff, prefix) 
PUTFUNC_DEF (voice, prefix) 
GETFUNC_DEF (staff, prefix) 
GETFUNC_DEF (voice, prefix) 
PUTFUNC_DEF (staff, postfix) 
PUTFUNC_DEF (voice, postfix) 
GETFUNC_DEF (staff, postfix) 
GETFUNC_DEF (voice, postfix) 
GETFUNC_DEF (chord, prefix) 
GETFUNC_DEF (chord, postfix) 
PUTFUNC_DEF (chord, prefix) 
PUTFUNC_DEF (chord, postfix) 
GETFUNC_DEF (standalone, prefix) 
GETFUNC_DEF (standalone, postfix) 
PUTFUNC_DEF (standalone, prefix) 
PUTFUNC_DEF (standalone, postfix)
#define INT_PUTFUNC_DEF(what, field)\
SCM scheme_##what##_directive_put_##field(SCM tag, SCM value) {\
  if((!scm_is_string(tag))||(!scm_is_integer(value))){\
    return SCM_BOOL(FALSE);\
  }\
  char *tagname;\
  tagname = scm_to_locale_string(tag);\
  gint valuename = scm_to_int(value);\
  extern  gboolean  what##_directive_put_##field (gchar *tag, gint value);\
  gboolean ret = what##_directive_put_##field ((gchar*)tagname, valuename);\
  if(tagname) free(tagname);\
  return SCM_BOOL(ret);\
}
#define INT_GETFUNC_DEF(what, field)\
SCM scheme_##what##_directive_get_##field(SCM tag) {\
  if(!scm_is_string(tag)){\
    return SCM_BOOL(FALSE);\
  }\
  char *tagname;\
  tagname = scm_to_locale_string(tag);\
  extern gint what##_directive_get_##field (gchar *tag);\
  gint ret = what##_directive_get_##field ((gchar*)tagname);\
  if(tagname) free(tagname);\
  return scm_from_int(ret);\
}
#define PUTGRAPHICFUNC_DEF(what)\
SCM scheme_##what##_directive_put_graphic(SCM tag, SCM value) {\
  if((!scm_is_string(tag))||(!scm_is_string(value))){\
    return SCM_BOOL(FALSE);\
  }\
  char *tagname;\
  tagname = scm_to_locale_string(tag);\
  char *valuename;\
  valuename = scm_to_locale_string(value);\
  gboolean ret = what##_directive_put_graphic ((gchar*)tagname, (gchar*)valuename);\
  if(tagname) free(tagname);\
  if(valuename) free(valuename);\
  return SCM_BOOL(ret);\
}
  PUTGRAPHICFUNC_DEF (note);
PUTGRAPHICFUNC_DEF (chord);
PUTGRAPHICFUNC_DEF (standalone);
PUTGRAPHICFUNC_DEF (staff);
PUTGRAPHICFUNC_DEF (voice);
PUTGRAPHICFUNC_DEF (score);


     //block to copy for new int field in directive
INT_PUTFUNC_DEF (note, minpixels)
INT_PUTFUNC_DEF (chord, minpixels)
INT_PUTFUNC_DEF (standalone, minpixels)
INT_PUTFUNC_DEF (staff, minpixels)
INT_PUTFUNC_DEF (voice, minpixels)
INT_PUTFUNC_DEF (score, minpixels)
INT_PUTFUNC_DEF (clef, minpixels)
INT_PUTFUNC_DEF (timesig, minpixels)
INT_PUTFUNC_DEF (tuplet, minpixels)
INT_PUTFUNC_DEF (stemdirective, minpixels)
INT_PUTFUNC_DEF (keysig, minpixels)
INT_PUTFUNC_DEF (scoreheader, minpixels)
INT_PUTFUNC_DEF (header, minpixels)
INT_PUTFUNC_DEF (paper, minpixels)
INT_PUTFUNC_DEF (layout, minpixels)
INT_PUTFUNC_DEF (movementcontrol, minpixels)
INT_GETFUNC_DEF (note, minpixels)
INT_GETFUNC_DEF (chord, minpixels)
INT_GETFUNC_DEF (standalone, minpixels)
INT_GETFUNC_DEF (staff, minpixels) INT_GETFUNC_DEF (voice, minpixels) INT_GETFUNC_DEF (score, minpixels) INT_GETFUNC_DEF (clef, minpixels) INT_GETFUNC_DEF (timesig, minpixels) INT_GETFUNC_DEF (tuplet, minpixels) INT_GETFUNC_DEF (stemdirective, minpixels) INT_GETFUNC_DEF (keysig, minpixels) INT_GETFUNC_DEF (scoreheader, minpixels) INT_GETFUNC_DEF (header, minpixels) INT_GETFUNC_DEF (paper, minpixels) INT_GETFUNC_DEF (layout, minpixels) INT_GETFUNC_DEF (movementcontrol, minpixels)
  //end block to ocpy for new int field in directive
  INT_PUTFUNC_DEF (note, override)
INT_PUTFUNC_DEF (chord, override)
INT_PUTFUNC_DEF (standalone, override)
INT_PUTFUNC_DEF (staff, override)
INT_PUTFUNC_DEF (voice, override)
INT_PUTFUNC_DEF (score, override)
INT_GETFUNC_DEF (note, override)
INT_GETFUNC_DEF (chord, override)
INT_GETFUNC_DEF (standalone, override)
INT_GETFUNC_DEF (staff, override)
INT_GETFUNC_DEF (voice, override)
INT_GETFUNC_DEF (score, override)
INT_PUTFUNC_DEF (note, y)
INT_PUTFUNC_DEF (chord, y)
INT_PUTFUNC_DEF (standalone, y)
INT_GETFUNC_DEF (note, y)
INT_GETFUNC_DEF (chord, y)
INT_GETFUNC_DEF (standalone, y)
INT_PUTFUNC_DEF (note, x)
INT_PUTFUNC_DEF (chord, x)
INT_PUTFUNC_DEF (standalone, x)
INT_GETFUNC_DEF (note, x)
INT_GETFUNC_DEF (chord, x)
INT_GETFUNC_DEF (standalone, x)
INT_PUTFUNC_DEF (note, ty)
INT_PUTFUNC_DEF (chord, ty)
INT_PUTFUNC_DEF (standalone, ty)
INT_GETFUNC_DEF (note, ty)
INT_GETFUNC_DEF (chord, ty)
INT_GETFUNC_DEF (standalone, ty)
INT_PUTFUNC_DEF (note, tx)
INT_PUTFUNC_DEF (chord, tx)
INT_PUTFUNC_DEF (standalone, tx)
INT_GETFUNC_DEF (note, tx)
INT_GETFUNC_DEF (chord, tx)
INT_GETFUNC_DEF (standalone, tx)
INT_PUTFUNC_DEF (note, gy)
INT_PUTFUNC_DEF (chord, gy)
INT_PUTFUNC_DEF (standalone, gy)
INT_GETFUNC_DEF (note, gy)
INT_GETFUNC_DEF (chord, gy)
INT_GETFUNC_DEF (standalone, gy)
INT_PUTFUNC_DEF (note, gx)
INT_PUTFUNC_DEF (chord, gx)
INT_PUTFUNC_DEF (standalone, gx)
INT_GETFUNC_DEF (note, gx)
INT_GETFUNC_DEF (chord, gx)
INT_GETFUNC_DEF (standalone, gx)
INT_GETFUNC_DEF (note, width)
INT_GETFUNC_DEF (chord, width)
INT_GETFUNC_DEF (standalone, width)
INT_GETFUNC_DEF (note, height)
INT_GETFUNC_DEF (chord, height)
INT_GETFUNC_DEF (standalone, height)
INT_GETFUNC_DEF (score, x) INT_GETFUNC_DEF (score, y) INT_GETFUNC_DEF (score, tx) INT_GETFUNC_DEF (score, ty) INT_GETFUNC_DEF (score, gx) INT_GETFUNC_DEF (score, gy) INT_GETFUNC_DEF (score, width) INT_GETFUNC_DEF (score, height) INT_PUTFUNC_DEF (score, x) INT_PUTFUNC_DEF (score, y) INT_PUTFUNC_DEF (score, tx) INT_PUTFUNC_DEF (score, ty) INT_PUTFUNC_DEF (score, gx) INT_PUTFUNC_DEF (score, gy) INT_GETFUNC_DEF (object, minpixels) INT_PUTFUNC_DEF (object, minpixels) DELETE_FN_DEF (object)
  // block to copy for new type of directive, !!minpixels is done in block to copy for new fields!!
  GETFUNC_DEF (clef, prefix) GETFUNC_DEF (clef, postfix) GETFUNC_DEF (clef, display) PUTFUNC_DEF (clef, prefix) PUTFUNC_DEF (clef, postfix) PUTFUNC_DEF (clef, display) PUTGRAPHICFUNC_DEF (clef);

INT_PUTFUNC_DEF (clef, x) INT_PUTFUNC_DEF (clef, y) INT_PUTFUNC_DEF (clef, tx) INT_PUTFUNC_DEF (clef, ty) INT_PUTFUNC_DEF (clef, gx) INT_PUTFUNC_DEF (clef, gy) INT_PUTFUNC_DEF (clef, override) INT_GETFUNC_DEF (clef, x) INT_GETFUNC_DEF (clef, y) INT_GETFUNC_DEF (clef, tx) INT_GETFUNC_DEF (clef, ty) INT_GETFUNC_DEF (clef, gx) INT_GETFUNC_DEF (clef, gy) INT_GETFUNC_DEF (clef, override) INT_GETFUNC_DEF (clef, width) INT_GETFUNC_DEF (clef, height) EDIT_DELETE_FN_DEF (clef)
  // end block
  GETFUNC_DEF (timesig, prefix) GETFUNC_DEF (timesig, postfix) GETFUNC_DEF (timesig, display) PUTFUNC_DEF (timesig, prefix) PUTFUNC_DEF (timesig, postfix) PUTFUNC_DEF (timesig, display) PUTGRAPHICFUNC_DEF (timesig);

INT_PUTFUNC_DEF (timesig, x)
INT_PUTFUNC_DEF (timesig, y)
INT_PUTFUNC_DEF (timesig, tx)
INT_PUTFUNC_DEF (timesig, ty)
INT_PUTFUNC_DEF (timesig, gx)
INT_PUTFUNC_DEF (timesig, gy)
INT_PUTFUNC_DEF (timesig, override)
INT_GETFUNC_DEF (timesig, x)
INT_GETFUNC_DEF (timesig, y) INT_GETFUNC_DEF (timesig, tx) INT_GETFUNC_DEF (timesig, ty) INT_GETFUNC_DEF (timesig, gx) INT_GETFUNC_DEF (timesig, gy) INT_GETFUNC_DEF (timesig, override) INT_GETFUNC_DEF (timesig, width) INT_GETFUNC_DEF (timesig, height) EDIT_DELETE_FN_DEF (timesig) GETFUNC_DEF (tuplet, prefix) GETFUNC_DEF (tuplet, postfix) GETFUNC_DEF (tuplet, display) PUTFUNC_DEF (tuplet, prefix) PUTFUNC_DEF (tuplet, postfix) PUTFUNC_DEF (tuplet, display) PUTGRAPHICFUNC_DEF (tuplet);

INT_PUTFUNC_DEF (tuplet, x)
INT_PUTFUNC_DEF (tuplet, y)
INT_PUTFUNC_DEF (tuplet, tx)
INT_PUTFUNC_DEF (tuplet, ty)
INT_PUTFUNC_DEF (tuplet, gx)
INT_PUTFUNC_DEF (tuplet, gy)
INT_PUTFUNC_DEF (tuplet, override)
INT_GETFUNC_DEF (tuplet, x)
INT_GETFUNC_DEF (tuplet, y)
INT_GETFUNC_DEF (tuplet, tx) INT_GETFUNC_DEF (tuplet, ty) INT_GETFUNC_DEF (tuplet, gx) INT_GETFUNC_DEF (tuplet, gy) INT_GETFUNC_DEF (tuplet, override) INT_GETFUNC_DEF (tuplet, width) INT_GETFUNC_DEF (tuplet, height) EDIT_DELETE_FN_DEF (tuplet) GETFUNC_DEF (stemdirective, prefix) GETFUNC_DEF (stemdirective, postfix) GETFUNC_DEF (stemdirective, display) PUTFUNC_DEF (stemdirective, prefix) PUTFUNC_DEF (stemdirective, postfix) PUTFUNC_DEF (stemdirective, display) PUTGRAPHICFUNC_DEF (stemdirective);

INT_PUTFUNC_DEF (stemdirective, x)
INT_PUTFUNC_DEF (stemdirective, y)
INT_PUTFUNC_DEF (stemdirective, tx)
INT_PUTFUNC_DEF (stemdirective, ty)
INT_PUTFUNC_DEF (stemdirective, gx)
INT_PUTFUNC_DEF (stemdirective, gy)
INT_PUTFUNC_DEF (stemdirective, override)
INT_GETFUNC_DEF (stemdirective, x)
INT_GETFUNC_DEF (stemdirective, y)
INT_GETFUNC_DEF (stemdirective, tx)
INT_GETFUNC_DEF (stemdirective, ty) INT_GETFUNC_DEF (stemdirective, gx) INT_GETFUNC_DEF (stemdirective, gy) INT_GETFUNC_DEF (stemdirective, override) INT_GETFUNC_DEF (stemdirective, width) INT_GETFUNC_DEF (stemdirective, height) EDIT_DELETE_FN_DEF (stemdirective) GETFUNC_DEF (keysig, prefix) GETFUNC_DEF (keysig, postfix) GETFUNC_DEF (keysig, display) PUTFUNC_DEF (keysig, prefix) PUTFUNC_DEF (keysig, postfix) PUTFUNC_DEF (keysig, display) PUTGRAPHICFUNC_DEF (keysig);

INT_PUTFUNC_DEF (keysig, x)
INT_PUTFUNC_DEF (keysig, y)
INT_PUTFUNC_DEF (keysig, tx)
INT_PUTFUNC_DEF (keysig, ty)
INT_PUTFUNC_DEF (keysig, gx)
INT_PUTFUNC_DEF (keysig, gy)
INT_PUTFUNC_DEF (keysig, override)
INT_GETFUNC_DEF (keysig, x)
INT_GETFUNC_DEF (keysig, y)
INT_GETFUNC_DEF (keysig, tx) INT_GETFUNC_DEF (keysig, ty) INT_GETFUNC_DEF (keysig, gx) INT_GETFUNC_DEF (keysig, gy) INT_GETFUNC_DEF (keysig, override) INT_GETFUNC_DEF (keysig, width) INT_GETFUNC_DEF (keysig, height) EDIT_DELETE_FN_DEF (keysig) GETFUNC_DEF (scoreheader, prefix) GETFUNC_DEF (scoreheader, postfix) GETFUNC_DEF (scoreheader, display) PUTFUNC_DEF (scoreheader, prefix) PUTFUNC_DEF (scoreheader, postfix) PUTFUNC_DEF (scoreheader, display) PUTGRAPHICFUNC_DEF (scoreheader);

INT_PUTFUNC_DEF (scoreheader, x)
INT_PUTFUNC_DEF (scoreheader, y)
INT_PUTFUNC_DEF (scoreheader, tx)
INT_PUTFUNC_DEF (scoreheader, ty)
INT_PUTFUNC_DEF (scoreheader, gx)
INT_PUTFUNC_DEF (scoreheader, gy)
INT_PUTFUNC_DEF (scoreheader, override)
INT_GETFUNC_DEF (scoreheader, x)
INT_GETFUNC_DEF (scoreheader, y)
INT_GETFUNC_DEF (scoreheader, tx) INT_GETFUNC_DEF (scoreheader, ty) INT_GETFUNC_DEF (scoreheader, gx) INT_GETFUNC_DEF (scoreheader, gy) INT_GETFUNC_DEF (scoreheader, override) INT_GETFUNC_DEF (scoreheader, width) INT_GETFUNC_DEF (scoreheader, height) EDIT_DELETE_FN_DEF (scoreheader) GETFUNC_DEF (header, prefix) GETFUNC_DEF (header, postfix) GETFUNC_DEF (header, display) PUTFUNC_DEF (header, prefix) PUTFUNC_DEF (header, postfix) PUTFUNC_DEF (header, display) PUTGRAPHICFUNC_DEF (header);

INT_PUTFUNC_DEF (header, x)
INT_PUTFUNC_DEF (header, y)
INT_PUTFUNC_DEF (header, tx)
INT_PUTFUNC_DEF (header, ty)
INT_PUTFUNC_DEF (header, gx)
INT_PUTFUNC_DEF (header, gy)
INT_PUTFUNC_DEF (header, override)
INT_GETFUNC_DEF (header, x) 
INT_GETFUNC_DEF (header, y) 
INT_GETFUNC_DEF (header, tx) 
INT_GETFUNC_DEF (header, ty) 
INT_GETFUNC_DEF (header, gx) 
INT_GETFUNC_DEF (header, gy) 
INT_GETFUNC_DEF (header, override) 
INT_GETFUNC_DEF (header, width) 
INT_GETFUNC_DEF (header, height) 
EDIT_DELETE_FN_DEF (header) 
GETFUNC_DEF (paper, prefix) 
GETFUNC_DEF (paper, postfix) 
GETFUNC_DEF (paper, display) 
PUTFUNC_DEF (paper, prefix) 
PUTFUNC_DEF (paper, postfix) 
PUTFUNC_DEF (paper, display) 
PUTGRAPHICFUNC_DEF (paper);

INT_PUTFUNC_DEF (paper, x)
INT_PUTFUNC_DEF (paper, y)
INT_PUTFUNC_DEF (paper, tx)
INT_PUTFUNC_DEF (paper, ty)
INT_PUTFUNC_DEF (paper, gx)
INT_PUTFUNC_DEF (paper, gy)
INT_PUTFUNC_DEF (paper, override)
INT_GETFUNC_DEF (paper, x) 
INT_GETFUNC_DEF (paper, y) 
INT_GETFUNC_DEF (paper, tx) 
INT_GETFUNC_DEF (paper, ty) 
INT_GETFUNC_DEF (paper, gx) 
INT_GETFUNC_DEF (paper, gy) 
INT_GETFUNC_DEF (paper, override) 
INT_GETFUNC_DEF (paper, width) 
INT_GETFUNC_DEF (paper, height) 
EDIT_DELETE_FN_DEF (paper) 
GETFUNC_DEF (layout, prefix) 
GETFUNC_DEF (layout, postfix) 
GETFUNC_DEF (layout, display) 
PUTFUNC_DEF (layout, prefix) 
PUTFUNC_DEF (layout, postfix) 
PUTFUNC_DEF (layout, display) 
PUTGRAPHICFUNC_DEF (layout);

INT_PUTFUNC_DEF (layout, x)
INT_PUTFUNC_DEF (layout, y)
INT_PUTFUNC_DEF (layout, tx)
INT_PUTFUNC_DEF (layout, ty)
INT_PUTFUNC_DEF (layout, gx)
INT_PUTFUNC_DEF (layout, gy)
INT_PUTFUNC_DEF (layout, override)
INT_GETFUNC_DEF (layout, x) 
INT_GETFUNC_DEF (layout, y) 
INT_GETFUNC_DEF (layout, tx) 
INT_GETFUNC_DEF (layout, ty) 
INT_GETFUNC_DEF (layout, gx) 
INT_GETFUNC_DEF (layout, gy) 
INT_GETFUNC_DEF (layout, override) 
INT_GETFUNC_DEF (layout, width) 
INT_GETFUNC_DEF (layout, height) 
EDIT_DELETE_FN_DEF (layout) 
GETFUNC_DEF (movementcontrol, prefix) 
GETFUNC_DEF (movementcontrol, postfix) 
PUTFUNC_DEF (movementcontrol, prefix) 
PUTFUNC_DEF (movementcontrol, postfix) 
PUTGRAPHICFUNC_DEF (movementcontrol);

INT_PUTFUNC_DEF (movementcontrol, x)
INT_PUTFUNC_DEF (movementcontrol, y)
INT_PUTFUNC_DEF (movementcontrol, tx)
INT_PUTFUNC_DEF (movementcontrol, ty)
INT_PUTFUNC_DEF (movementcontrol, gx)
INT_PUTFUNC_DEF (movementcontrol, gy) 
INT_PUTFUNC_DEF (movementcontrol, override) 
INT_GETFUNC_DEF (movementcontrol, x) 
INT_GETFUNC_DEF (movementcontrol, y) 
INT_GETFUNC_DEF (movementcontrol, tx) 
INT_GETFUNC_DEF (movementcontrol, ty) 
INT_GETFUNC_DEF (movementcontrol, gx) 
INT_GETFUNC_DEF (movementcontrol, gy) 
INT_GETFUNC_DEF (movementcontrol, override) 
INT_GETFUNC_DEF (movementcontrol, width) 
INT_GETFUNC_DEF (movementcontrol, height) 
EDIT_DELETE_FN_DEF (movementcontrol)
     
SCM
scheme_put_text_clipboard (SCM optional)
{
  size_t length;
  char *str = NULL;
  if (scm_is_string (optional))
    {
      str = scm_to_locale_stringn (optional, &length);
      GtkClipboard *clipboard = gtk_clipboard_get (GDK_SELECTION_CLIPBOARD);
      gtk_clipboard_set_text (clipboard, str, length);
      if (str)
        free (str);
      return SCM_BOOL (TRUE);
    }
  return SCM_BOOL (FALSE);
}



SCM
scheme_get_username (void)
{
  return scm_from_locale_string (Denemo.prefs.username->str);
}

SCM
scheme_get_password (void)
{
  return scm_from_locale_string (Denemo.prefs.password->str);
}

SCM
scheme_set_midi_capture (SCM setting)
{
  gboolean prev;
  prev = set_midi_capture ((setting != SCM_BOOL_F));
  return prev ? SCM_BOOL_T : SCM_BOOL_F;
}

SCM
scheme_get_keyboard_state (void)
{
  return scm_from_int (Denemo.keyboard_state);
}

SCM
scheme_set_midi_thru (SCM set)
{
  SCM ret = scm_from_int (Denemo.keyboard_state);
  if (scm_is_true (set))
    Denemo.keyboard_state = GDK_SHIFT_MASK;
  else
    Denemo.keyboard_state = 0;
  set_midi_in_status ();
  return ret;
}

SCM
scheme_get_recorded_midi_on_tick (void)
{
  smf_track_t *track = Denemo.gui->si->recorded_midi_track;
  if (track)
    {
#define MIDI_NOTEOFF		0x80
#define MIDI_NOTEON		0x90
      smf_event_t *event = smf_track_get_next_event (track);
      if (event)
        switch (event->midi_buffer[0] & 0xF0)
          {
          case MIDI_NOTEON:
            return scm_from_int (event->time_pulses);
          case MIDI_NOTEOFF:
            return scm_from_int (-event->time_pulses);
          default:
            return SCM_BOOL_F;
          }
    }
  return SCM_BOOL_F;
}

SCM
scheme_get_recorded_midi_note (void)
{
  smf_track_t *track = Denemo.gui->si->recorded_midi_track;
  if (track)
    {
      smf_event_t *event = NULL;
      if (track->next_event_number > 0 && (track->next_event_number <= track->events_array->len))
        event = g_ptr_array_index (track->events_array, track->next_event_number - 1);
      if (event)
        switch (event->midi_buffer[0] & 0xF0)
          {
          case MIDI_NOTEON:
          case MIDI_NOTEOFF:
            return scm_from_int (event->midi_buffer[1]);
          default:
            return SCM_BOOL_F;
          }
    }
  return SCM_BOOL_F;
}

SCM
scheme_rewind_recorded_midi (void)
{
  smf_track_t *track = Denemo.gui->si->recorded_midi_track;
  if (track)
    {
      if (track->smf == NULL)
        {
          if (Denemo.gui->si->smf)
            {
              smf_add_track (Denemo.gui->si->smf, track);
              smf_rewind (Denemo.gui->si->smf);
            }
          else
            return SCM_BOOL_F;
        }
      smf_rewind (track->smf);
      return SCM_BOOL_T;
    }
  return SCM_BOOL_F;
}

SCM
scheme_get_note_for_midi_key (SCM scm)
{
  gint notenum = 0, offset, enshift, octave;
  if (scm_is_integer (scm))
    notenum = scm_to_int (scm);
  if (notenum > 0 && notenum < 256)
    {
      notenum2enharmonic (notenum, &offset, &enshift, &octave);
      gchar *name = mid_c_offsettolily (offset + 7 * octave, enshift);
      return scm_from_locale_string (name);
    }
  return SCM_BOOL_F;
}

SCM
scheme_get_midi (void)
{
  gint midi;
  gboolean success = intercept_midi_event (&midi);
  if (!success)
    midi = 0;                   /* scripts should detect this impossible value and take action */
  else
    Denemo.gui->last_source = INPUTMIDI;
  gchar *buf = (gchar *) & midi;
  *buf &= 0xF0;                 //do not return channel info

  SCM scm = scm_from_int (midi);
  return scm;
}

//Simulates a midi event, with no capture by any calling scheme script unless midi==0
SCM
scheme_put_midi (SCM scm)
{
  gchar buf[3];
  gint midi = scm_to_int (scm);

  buf[0] = midi & 0xFF;
  buf[1] = (midi >> 8) & 0xFF;
  buf[2] = (midi >> 16) & 0xFF;
  //g_print("got %x\nbreaks as %x %x %x\n", midi&0xFFFFFF, buf[0], buf[1], buf[2]);
  if (midi)
    {
      gboolean capture = set_midi_capture (FALSE);      //Turn off any capturing
      process_midi_event (buf);
      set_midi_capture (capture);       //Restore any capturing that might be on
    }
  else
    process_midi_event (buf);
  return SCM_BOOL (TRUE);
}

SCM
scheme_output_midi (SCM scm)
{
  gchar buf[3];
  gint midi = scm_to_int (scm);

  buf[0] = midi & 0xFF;
  buf[1] = (midi >> 8) & 0xFF;
  buf[2] = (midi >> 16) & 0xFF;
  play_adjusted_midi_event (buf);
  return SCM_BOOL_T;
}


/* outputs a midibytes string to MIDI out. Format of midibytes as in DenemoDirective->midibytes */
SCM
scheme_output_midi_bytes (SCM input)
{
  char *next;
  gint i, numbytes;
  gint channel;
  gint volume;
  if (!scm_is_string (input))
    {
      return SCM_BOOL_F;
    }
  DenemoStaff *curstaffstruct = (DenemoStaff *) Denemo.gui->si->currentstaff->data;
  channel = get_midi_channel (curstaffstruct);
  volume = curstaffstruct->volume;
  char *string_input;
  string_input = scm_to_locale_string (input);
  gchar *bytes = substitute_midi_values (string_input, channel, volume);

  for (i = 0, next = bytes; *next; next++)
    {
      i++;
      if (*next == 0)
        break;
    }
  numbytes = i;
  unsigned char *buffer = (unsigned char *) g_malloc0 (numbytes);
  for (i = 0, next = bytes; i < numbytes; i++, next++)
    buffer[i] = (unsigned char) strtol (next, &next, 0);
  g_free (bytes);
  //g_print("\nbuffer[0] = %x buffer[1] = %x buffer[2] = %x\n", buffer[0], buffer[1], buffer[2]);

  play_midi_event (DEFAULT_BACKEND, curstaffstruct->midi_port, buffer);

  if (string_input)
    free (string_input);
  return SCM_BOOL (TRUE);
}

SCM
scheme_create_timebase (SCM optional)
{
  DenemoScore *si = Denemo.gui->si;
  if (si->smfsync != si->changecount)
    {
      exportmidi (NULL, si, 0, 0);
      return SCM_BOOL_T;
    }
  return SCM_BOOL_F;
}

SCM
scheme_pending_midi (SCM scm)
{
  if (scm_is_integer (scm))
    {
      guint key = scm_to_int (scm);
      g_queue_push_head (Denemo.gui->pending_midi, GINT_TO_POINTER (key));
      return SCM_BOOL_T;
    }
  else
    return SCM_BOOL_F;
}

SCM
scheme_play_midi_note (SCM note, SCM volume, SCM channel, SCM duration)
{
  guint vol = scm_to_int (volume);
  gint key = scm_to_int (note);
  gint chan = scm_to_int (channel);
  gint dur = scm_to_int (duration);

  //g_print("Playing %x at %f volume, %d channel for %dms\n", key, vol/255.0, channel, dur);
  play_note (DEFAULT_BACKEND, 0 /*port */ , chan, key, dur, vol);
  return SCM_BOOL (TRUE);
}

SCM
scheme_play_midikey (SCM scm)
{
  guint midi = scm_to_int (scm);
  gint key = (midi >> 8) & 0xFF;
  gint channel = midi & 0xF;
  gint volume = ((midi >> 16) & 0x7F);
  //g_print("Playing %x at %f volume, %d channel\n", key, (double)volume, channel);
  play_note (DEFAULT_BACKEND, 0 /*port */ , channel, key, 1000 /*duration */ , volume);
  //g_usleep(200000);
  return SCM_BOOL (TRUE);
}

//Insert a rest without setting the prevailing duration
SCM
scheme_put_rest (SCM optional_duration)
{
  gint duration;
  if (scm_is_integer (optional_duration))
    {
      duration = scm_to_int (optional_duration);
    }
  else
    {
      duration = get_prevailing_duration ();
    }
  if ((duration < 0) || (duration > 7))
    return SCM_BOOL_F;

  dnm_insertchord (Denemo.gui, duration, 0, TRUE);
  displayhelper (Denemo.gui);   //without this a call to d-AddVoice causes a crash as the chord length info has not been updated
  return SCM_BOOL_T;
}

//Insert a rest in the given (or prevailing duration) and set the prevailing duration
SCM
scheme_insert_rest (SCM optional)
{
  SCM ret = scheme_put_rest (optional);
  if (scm_is_integer (optional))
    {
      gint duration = scm_to_int (optional);
      highlight_duration (Denemo.gui, duration);
    }
  return ret;
}


SCM
scheme_toggle_playalong (void)
{
  pb_playalong (get_playalong_button ());
  return SCM_BOOL (Denemo.gui->midi_destination | MIDIPLAYALONG);
}

SCM
scheme_toggle_conduct (void)
{
  pb_conduct (get_conduct_button ());
  return SCM_BOOL (Denemo.gui->midi_destination | MIDICONDUCT);
}

SCM
scheme_midi_record (void)
{
  pb_record (get_record_button ());
  return SCM_BOOL (Denemo.gui->midi_destination | MIDIRECORD);
}

SCM
scheme_get_marked_midi_note (void)
{
 SCM scm = SCM_BOOL_F;
 DenemoGUI *gui = Denemo.gui;
 DenemoScore *si = gui->si;
 if(si->recording && (si->recording->type == DENEMO_RECORDING_MIDI) && si->marked_onset) {
	 GList *marked = si->marked_onset;
	 DenemoRecordedNote *thenote = (DenemoRecordedNote*)marked->data;
	 gchar *name = mid_c_offsettolily (thenote->mid_c_offset, thenote->enshift);
	 gchar *str = g_strdup_printf ("%s", mid_c_offsettolily (thenote->mid_c_offset + 7*thenote->octave, thenote->enshift));
     scm = scm_from_locale_string (str);
 }
 return scm;
}
SCM
scheme_advance_marked_midi (void)
{
 SCM scm = SCM_BOOL_F;
 DenemoGUI *gui = Denemo.gui;
 DenemoScore *si = gui->si;
 if(si->recording && (si->recording->type == DENEMO_RECORDING_MIDI) && si->marked_onset) {
	 si->marked_onset = si->marked_onset->next;
	 if(si->marked_onset)
		scm = SCM_BOOL_T;
 }
 return scm;
}



typedef struct cb_scheme_and_id
{
  char *scheme_code;
  gint id;
} cb_scheme_and_id;

static gboolean
scheme_callback_one_shot_timer (cb_scheme_and_id * scheme)
{
  char *scheme_code = scheme->scheme_code;
  if (scheme->id == Denemo.gui->id)
    call_out_to_guile (scheme_code);
  else
    g_warning ("Timer missed for gui %d\n", scheme->id);
  g_free (scheme);
  free (scheme_code);
  return FALSE;
}

SCM
scheme_one_shot_timer (SCM duration_amount, SCM callback)
{
  char *scheme_code;
  scheme_code = scm_to_locale_string (callback);
  gint duration = scm_to_int (duration_amount);
  cb_scheme_and_id *scheme = g_malloc (sizeof (cb_scheme_and_id));
  scheme->scheme_code = scheme_code;
  scheme->id = Denemo.gui->id;
  g_timeout_add (duration, (GSourceFunc) scheme_callback_one_shot_timer, GINT_TO_POINTER (scheme));
  return SCM_BOOL (TRUE);
}

static gboolean
scheme_callback_timer (cb_scheme_and_id * scheme)
{
  char *scheme_code = scheme->scheme_code;
  if (scheme->id == Denemo.gui->id)
    call_out_to_guile (scheme_code);
  else
    g_warning ("Timer missed for gui %d\n", scheme->id);

  return TRUE;                  //continue to call
}


SCM
scheme_timer (SCM duration_amount, SCM callback)
{
  char *scheme_code;
  if (scm_is_string (callback))
    {
      scheme_code = scm_to_locale_string (callback);    //FIXME check that type of callback is tring
      gint duration = scm_to_int (duration_amount);
      //g_print("setting timer for %s after %d ms", scheme_code, duration);
      cb_scheme_and_id *scheme = g_malloc (sizeof (cb_scheme_and_id));
      scheme->scheme_code = scheme_code;
      scheme->id = Denemo.gui->id;
      g_timeout_add (duration, (GSourceFunc) scheme_callback_timer, GINT_TO_POINTER (scheme));
      //if(scheme_code) free(scheme_code);
      return scm_from_int (GPOINTER_TO_INT (scheme));   //FIXME pointer may not fit in int
    }
  else
    return SCM_BOOL_F;
}

SCM
scheme_kill_timer (SCM id)
{
  if (scm_is_integer (id))
    {
      //FIXME the int may not be large enough for a pointer
      cb_scheme_and_id *scheme = GINT_TO_POINTER (scm_to_int (id));
      if (scheme)
        {
          g_source_remove_by_user_data (scheme);
          free (scheme->scheme_code);
          g_free (scheme);
          return SCM_BOOL_T;
        }
    }
  return SCM_BOOL_F;
}




SCM
scheme_bass_figure (SCM bass, SCM harmony)
{
  SCM ret = SCM_BOOL_F;
  gboolean status = FALSE;
  gint bassnum = scm_to_int (bass);
  gint harmonynum = scm_to_int (harmony);
  gchar *interval = determine_interval (bassnum, harmonynum, &status);
  if (interval)
    {
      ret = scm_cons (status ? SCM_BOOL_T : SCM_BOOL_F, scm_from_locale_string (interval));
      g_free (interval);
    }
  return ret;
}

SCM
scheme_has_figures (SCM optional)
{
  return SCM_BOOL (((DenemoStaff *) Denemo.gui->si->currentstaff->data)->hasfigures);
}



//badly named:
SCM
scheme_put_note_name (SCM optional)
{
  DenemoObject *curObj;
  chord *thechord;
  note *thenote;
  if (!Denemo.gui || !(Denemo.gui->si) || !(Denemo.gui->si->currentobject) || !(curObj = Denemo.gui->si->currentobject->data) || (curObj->type != CHORD) || !(thechord = (chord *) curObj->object) || !(thechord->notes) || !(thenote = (note *) thechord->notes->data))
    return SCM_BOOL (FALSE);
  else
    {
      char *str = NULL;
      if (scm_is_string (optional))
        {
          str = scm_to_locale_string (optional);
          gint mid_c_offset;
          gint enshift;
          interpret_lilypond_notename (str, &mid_c_offset, &enshift);
          //g_print("note %s gives %d and %d\n", str, mid_c_offset, enshift);
          modify_note (thechord, mid_c_offset, enshift, find_prevailing_clef (Denemo.gui->si));
          if (str)
            free (str);
          return SCM_BOOL (TRUE);
        }
    }
  return SCM_BOOL (FALSE);
}

SCM
scheme_set_accidental (SCM optional)
{
  DenemoObject *curObj;
  chord *thechord;
  note *thenote;
  if (!Denemo.gui || !(Denemo.gui->si) || !(Denemo.gui->si->currentobject) || !(curObj = Denemo.gui->si->currentobject->data) || (curObj->type != CHORD) || !(thechord = (chord *) curObj->object) || !(thechord->notes) || !(thenote = (note *) thechord->notes->data))
    return SCM_BOOL (FALSE);
  else
    {
      GList *g;
      for (g = thechord->notes; g; g = g->next)
        {
          thenote = (note *) g->data;
          if (thenote->mid_c_offset == Denemo.gui->si->cursor_y)
            break;
        }
      if (g == NULL)
        return SCM_BOOL_F;
      DenemoScore *si = Denemo.gui->si;
      char *str = NULL;

      if (scm_is_string (optional))
        {
          str = scm_to_locale_string (optional);
          thenote->enshift = lilypond_to_enshift (str);
        }
      else if (scm_is_integer (optional))
        thenote->enshift = scm_to_int (optional);
      else
        thenote->enshift = 0;
      if ((thenote->enshift < -2) || (thenote->enshift > 2))
        thenote->enshift = 0;
      showwhichaccidentals ((objnode *) si->currentmeasure->data, si->curmeasurekey, si->curmeasureaccs);
      //  find_xes_in_measure (si, si->currentmeasurenum, si->cursortime1,
      //                      si->cursortime2); causes a crash, si is not passed correctly, why???
      //thenote->mid_c_offset = interpret_lilypond_notename(str);
      displayhelper (Denemo.gui);
      if (str)
        free (str);
      return SCM_BOOL (TRUE);
    }
}





//create a putnote here that takes a duration and numdots and note name, inserts a chord and calls the scheme_put_note_name above - this can be done via script at present, e.g. (d-C) (d-Change3) (d-AddDot) (d-PutNoteName "eis''")


//Puts a note into the chord at the cursor PARAM lily is a string representation of the note
SCM
scheme_insert_note_in_chord (SCM lily)
{
  DenemoGUI *gui = Denemo.gui;
  DenemoObject *curObj;
  if (!Denemo.gui || !(Denemo.gui->si) || !(Denemo.gui->si->currentobject) || !(curObj = Denemo.gui->si->currentobject->data) )
    return SCM_BOOL_F;
#ifdef INSERT_NOTE_IN_CHORD_WORKS_ON_PREVIOUS_CHORD
  if(curObj->type != CHORD) {
    objnode *theobj = Denemo.gui->si->currentobject;
    while(theobj->prev)
      {
        theobj = theobj->prev;
        curObj = theobj->data;
        if(curObj->type ==CHORD)
          break;
      }
    if(curObj->type !=CHORD)
      return SCM_BOOL_F;
  }
#else
   if(curObj->type !=CHORD)
      return SCM_BOOL_F;
#endif
      
  char *str = NULL;
  if (scm_is_string (lily))
    {
      str = scm_to_locale_string (lily);
      gint mid_c_offset;
      gint enshift;
      interpret_lilypond_notename (str, &mid_c_offset, &enshift);

      //g_print("note %s gives %d and %d\n", str, mid_c_offset, enshift);
      addtone (curObj, mid_c_offset, enshift, find_prevailing_clef (Denemo.gui->si));
      score_status (gui, TRUE);
      displayhelper (Denemo.gui);
      if (str)
        free (str);
      return SCM_BOOL_T;
    }
  return SCM_BOOL (FALSE);
}


//return the number of objects in the copybuffer at staff m
SCM
scheme_get_clip_objects (SCM m)
{
  gint staff = scm_to_int (m);
  gint num = get_clip_objs (staff);
  if (num == -1)
    return SCM_BOOL_F;
  else
    return scm_from_int (num);
}

//return the type of the nth object in the copybuffer
SCM
scheme_get_clip_obj_type (SCM m, SCM n)
{
  gint value = scm_to_int (n);
  gint staff = scm_to_int (m);
  DenemoObjType type = get_clip_obj_type (staff, value);
  if (type == -1)
    return SCM_BOOL_F;
  else
    return scm_from_int (type);
}


//insert the nth object from the denemo copybuffer
SCM
scheme_put_clip_obj (SCM m, SCM n)
{
  gint value = scm_to_int (n);
  gint staff = scm_to_int (m);
  return SCM_BOOL (insert_clip_obj (staff, value));
}

SCM
scheme_adjust_xes (SCM optional)
{
  find_xes_in_all_measures (Denemo.gui->si);
  return SCM_BOOL_T;
}

static gint
flash_cursor (void)
{
  gtk_widget_queue_draw (Denemo.scorearea);
 // draw_score (NULL); what was this for?????
  return TRUE;
}

SCM
scheme_highlight_cursor (SCM optional)
{
  static gint id;
  SCM ret = SCM_BOOL_T;
  gboolean old_value = Denemo.prefs.cursor_highlight;
  if (scm_is_bool(optional))
   {
	  Denemo.prefs.cursor_highlight = scm_is_true (optional);
	  ret =  old_value?SCM_BOOL_T:SCM_BOOL_F;  
  } else
  {
  Denemo.prefs.cursor_highlight = !Denemo.prefs.cursor_highlight;
  }
  if (id && !Denemo.prefs.cursor_highlight)
    {
      g_source_remove (id);
      id = 0;
    }
  else if (Denemo.prefs.cursor_highlight)
    id = g_timeout_add (500, (GSourceFunc) flash_cursor, NULL);
  //g_print("Cursor highlighting %d id %d", Denemo.prefs.cursor_highlight, id);
  return ret;
}

SCM
scheme_get_type (SCM optional)
{
  DenemoObject *curObj;
  if (!Denemo.gui || !(Denemo.gui->si) || !(Denemo.gui->si->currentobject) || !(curObj = Denemo.gui->si->currentobject->data) || !(DENEMO_OBJECT_TYPE_NAME (curObj)))
    return scm_from_locale_string ("None");
  if (Denemo.gui->si->cursor_appending)
    return scm_from_locale_string ("Appending");
  return scm_from_locale_string (DENEMO_OBJECT_TYPE_NAME (curObj));
}

SCM
scheme_get_lilypond (SCM optional)
{
  DenemoGUI *gui = Denemo.gui;
  DenemoObject *curObj;
  if (!Denemo.gui || !(Denemo.gui->si) || !(Denemo.gui->si->currentobject) || !(curObj = Denemo.gui->si->currentobject->data) || !(DENEMO_OBJECT_TYPE_NAME (curObj)))
    return SCM_BOOL_F;
//g_print("Before %d %d\n", gui->lilysync, gui->changecount);

  if (gui->lilysync != gui->changecount)
    refresh_lily_cb (NULL, Denemo.gui);
//g_print("After %d %d\n", gui->lilysync, gui->changecount);
  if (curObj->lilypond)
    return scm_from_locale_string (curObj->lilypond);
  return SCM_BOOL_F;
}

SCM
scheme_get_tuplet (SCM optional)
{
  DenemoObject *curObj;
  if (!Denemo.gui || !(Denemo.gui->si) || !(Denemo.gui->si->currentobject) || !(curObj = Denemo.gui->si->currentobject->data) || (curObj->type != TUPOPEN))
    return SCM_BOOL_F;
  GString *ratio = g_string_new ("");
  g_string_printf (ratio, "%d/%d", ((tupopen *) curObj->object)->numerator, ((tupopen *) curObj->object)->denominator);
  return scm_from_locale_string (g_string_free (ratio, FALSE));
}

SCM
scheme_set_tuplet (SCM ratio)
{
  DenemoObject *curObj;
  if (!Denemo.gui || !(Denemo.gui->si) || !(Denemo.gui->si->currentobject) || !(curObj = Denemo.gui->si->currentobject->data) || (curObj->type != TUPOPEN))
    {
      return SCM_BOOL_F;
    }
  char *theratio;
  theratio = scm_to_locale_string (ratio);
  sscanf (theratio, "%d/%d", &((tupopen *) curObj->object)->numerator, &((tupopen *) curObj->object)->denominator);
  //g_print("Set %d/%d\n", (((tupopen*)curObj->object)->numerator), (((tupopen*)curObj->object)->denominator));
  free (theratio);
  if (((tupopen *) curObj->object)->denominator)
    {
      return SCM_BOOL_T;
    }
  ((tupopen *) curObj->object)->denominator = 1;
  return SCM_BOOL_F;
}

SCM
scheme_set_background (SCM color)
{
  if (scm_is_integer (color))
    {
      gint value = scm_to_int (color);
      Denemo.color = value;
      gtk_widget_queue_draw (Denemo.scorearea);
      draw_score (NULL);
      return SCM_BOOL_T;
    }
  return SCM_BOOL_F;
}


SCM
scheme_get_nonprinting (SCM optional)
{
  DenemoObject *curObj;
  if (!Denemo.gui || !(Denemo.gui->si) || !(Denemo.gui->si->currentobject) || !(curObj = Denemo.gui->si->currentobject->data) || curObj->isinvisible)
    return SCM_BOOL_T;
  return SCM_BOOL_F;
}

SCM
scheme_set_nonprinting (SCM optional)
{
  DenemoObject *curObj;
  if (!Denemo.gui || !(Denemo.gui->si) || !(Denemo.gui->si->currentobject) || !(curObj = Denemo.gui->si->currentobject->data))
    return SCM_BOOL_F;
  if (scm_is_false (optional))
    curObj->isinvisible = FALSE;
  else
    curObj->isinvisible = TRUE;
  return SCM_BOOL_T;
}

SCM
scheme_is_grace (SCM optional)
{
  DenemoObject *curObj;
  chord *thechord;
  if (!Denemo.gui || !(Denemo.gui->si) || !(Denemo.gui->si->currentobject) || !(curObj = Denemo.gui->si->currentobject->data) || (curObj->type != CHORD) || !(thechord = (chord *) curObj->object) || !(thechord->is_grace))
    return SCM_BOOL_F;
  return SCM_BOOL_T;
}

SCM
scheme_is_tied (SCM optional)
{
  DenemoObject *curObj;
  chord *thechord;
  if (!Denemo.gui || !(Denemo.gui->si) || !(Denemo.gui->si->currentobject) || !(curObj = Denemo.gui->si->currentobject->data) || (curObj->type != CHORD) || !(thechord = (chord *) curObj->object) || !(thechord->is_tied))
    return SCM_BOOL_F;
  return SCM_BOOL_T;
}


SCM
scheme_is_slur_start (SCM optional)
{
  DenemoObject *curObj;
  chord *thechord;
  if (!Denemo.gui || !(Denemo.gui->si) || !(Denemo.gui->si->currentobject) || !(curObj = Denemo.gui->si->currentobject->data) || (curObj->type != CHORD) || !(thechord = (chord *) curObj->object) || !(thechord->slur_begin_p))
    return SCM_BOOL_F;
  return SCM_BOOL_T;
}

SCM
scheme_is_slur_end (SCM optional)
{
  DenemoObject *curObj;
  chord *thechord;
  if (!Denemo.gui || !(Denemo.gui->si) || !(Denemo.gui->si->currentobject) || !(curObj = Denemo.gui->si->currentobject->data) || (curObj->type != CHORD) || !(thechord = (chord *) curObj->object) || !(thechord->slur_end_p))
    return SCM_BOOL_F;
  return SCM_BOOL_T;
}

SCM
scheme_is_cresc_start (SCM optional)
{
  DenemoObject *curObj;
  chord *thechord;
  if (!Denemo.gui || !(Denemo.gui->si) || !(Denemo.gui->si->currentobject) || !(curObj = Denemo.gui->si->currentobject->data) || (curObj->type != CHORD) || !(thechord = (chord *) curObj->object) || !(thechord->crescendo_begin_p))
    return SCM_BOOL_F;
  return SCM_BOOL_T;
}

SCM
scheme_is_cresc_end (SCM optional)
{
  DenemoObject *curObj;
  chord *thechord;
  if (!Denemo.gui || !(Denemo.gui->si) || !(Denemo.gui->si->currentobject) || !(curObj = Denemo.gui->si->currentobject->data) || (curObj->type != CHORD) || !(thechord = (chord *) curObj->object) || !(thechord->crescendo_end_p))
    return SCM_BOOL_F;
  return SCM_BOOL_T;
}

SCM
scheme_is_dim_start (SCM optional)
{
  DenemoObject *curObj;
  chord *thechord;
  if (!Denemo.gui || !(Denemo.gui->si) || !(Denemo.gui->si->currentobject) || !(curObj = Denemo.gui->si->currentobject->data) || (curObj->type != CHORD) || !(thechord = (chord *) curObj->object) || !(thechord->diminuendo_begin_p))
    return SCM_BOOL_F;
  return SCM_BOOL_T;
}

SCM
scheme_is_dim_end (SCM optional)
{
  DenemoObject *curObj;
  chord *thechord;
  if (!Denemo.gui || !(Denemo.gui->si) || !(Denemo.gui->si->currentobject) || !(curObj = Denemo.gui->si->currentobject->data) || (curObj->type != CHORD) || !(thechord = (chord *) curObj->object) || !(thechord->diminuendo_end_p))
    return SCM_BOOL_F;
  return SCM_BOOL_T;
}

SCM
scheme_is_in_selection (void)
{
  return SCM_BOOL (in_selection (Denemo.gui->si));
}

SCM
scheme_is_appending (void)
{
  return SCM_BOOL (Denemo.gui->si->cursor_appending);
}




SCM
scheme_clear_clipboard (SCM optional)
{
  clearbuffer ();
  return SCM_BOOL (TRUE);
}

SCM
scheme_get_staffs_in_clipboard (SCM optional)
{
  gint num = get_staffs_in_clipboard ();
  if (num)
    return scm_from_int (num);
  return SCM_BOOL_F;
}


SCM
scheme_get_measures_in_staff (SCM optional)
{
  gint num = g_list_length (((DenemoStaff *) Denemo.gui->si->currentstaff->data)->measures);
  return scm_from_int (num);
}

SCM
scheme_staff_to_voice (SCM optional)
{
  SCM ret = SCM_BOOL_F;
  if (Denemo.gui->si->currentstaff->prev && (((DenemoStaff *) Denemo.gui->si->currentstaff->data)->voicecontrol == DENEMO_PRIMARY))
    {
      ((DenemoStaff *) Denemo.gui->si->currentstaff->data)->voicecontrol |= DENEMO_SECONDARY;
      setcurrentprimarystaff (Denemo.gui->si);
      ret = SCM_BOOL_T;
      gtk_widget_queue_draw (Denemo.scorearea);
      score_status (Denemo.gui, TRUE);
      draw_score (NULL);
    }
  return ret;
}

SCM
scheme_voice_to_staff (SCM optional)
{
  SCM ret = SCM_BOOL_F;
  if (((DenemoStaff *) Denemo.gui->si->currentstaff->data)->voicecontrol & DENEMO_SECONDARY)
    {
      ((DenemoStaff *) Denemo.gui->si->currentstaff->data)->voicecontrol = DENEMO_PRIMARY;
      setcurrentprimarystaff (Denemo.gui->si);
      ret = SCM_BOOL_T;
      score_status (Denemo.gui, TRUE);
      gtk_widget_queue_draw (Denemo.scorearea);
    }
  return ret;
}

SCM
scheme_is_voice (void)
{
return SCM_BOOL ((((DenemoStaff *) Denemo.gui->si->currentstaff->data)->voicecontrol & DENEMO_SECONDARY));
}
/* shifts the note at the cursor by the number of diatonic steps passed in */
SCM
scheme_diatonic_shift (SCM optional)
{
  DenemoGUI *gui = Denemo.gui;
  DenemoObject *curObj;
  chord *thechord;
  note *thenote;
  if (!Denemo.gui || !(Denemo.gui->si) || !(Denemo.gui->si->currentobject) || !(curObj = Denemo.gui->si->currentobject->data) || (curObj->type != CHORD) || !(thechord = (chord *) curObj->object) || !(thechord->notes) || !(thenote = (note *) thechord->notes->data))
    {
      return SCM_BOOL (FALSE);
    }
  else
    {
      char *str = NULL;
      if (scm_is_string (optional))
        {
          str = scm_to_locale_string (optional);
          gint shift;
          sscanf (str, "%d", &shift);
//     g_print("note shift %s ie %d\n", str, shift);
          modify_note (thechord, thenote->mid_c_offset + shift, gui->si->curmeasureaccs[offsettonumber (thenote->mid_c_offset + shift)], find_prevailing_clef (Denemo.gui->si));
          free (str);
        }
    }
  return SCM_BOOL (FALSE);
}

/* moves currentobject to next object by calling cursorright.
   Steps over barlines (i.e. cursor_appending).
   returns TRUE if currentobject is different after than before doing the call
*/
SCM
scheme_next_object (void)
{
  return SCM_BOOL (cursor_to_next_object (FALSE, FALSE));
}

/* moves currentobject to prev object by calling cursorleft.
   Steps over barlines (i.e. cursor_appending).
   returns TRUE if currentobject is different after than before doing the call
*/
SCM
scheme_prev_object (void)
{
  return SCM_BOOL (cursor_to_prev_object (FALSE, FALSE));
}


/* moves currentobject to next object in measure, if any
   returns TRUE if currentobject is different after than before doing the call
*/
SCM
scheme_next_object_in_measure (void)
{
  return SCM_BOOL (cursor_to_next_object (TRUE, FALSE));
}

/* moves currentobject to previous object in measure, if any
   returns TRUE if currentobject is different after than before doing the call
*/
SCM
scheme_prev_object_in_measure (void)
{
  return SCM_BOOL (cursor_to_prev_object (TRUE, FALSE));
}


SCM
scheme_refresh_display (SCM optional)
{
  displayhelper (Denemo.gui);
  //done in displayhelper write_status(Denemo.gui);
  return SCM_BOOL (TRUE);
}



SCM
scheme_set_saved (SCM optional)
{
  //scm_is_bool(optional) &&
  if (scm_is_false (optional))
    score_status (Denemo.gui, TRUE);
  else
    score_status (Denemo.gui, FALSE);
  return SCM_BOOL (TRUE);
}

SCM
scheme_get_saved (SCM optional)
{
  return SCM_BOOL (!Denemo.gui->notsaved);
}

SCM
scheme_mark_status (SCM optional)
{
  return SCM_BOOL (mark_status ());

}







/* moves currentobject to next object in the selection.
   Steps over barlines (i.e. cursor_appending).
 returns TRUE if currentobject is different after than before the call
*/
SCM
scheme_next_selected_object (SCM optional)
{
  return SCM_BOOL (cursor_to_next_selected_object ());
}

/* moves currentobject to previous object in the selection.
   Steps over barlines (i.e. cursor_appending).
 returns TRUE if currentobject is different after than before the call
*/
SCM
scheme_prev_selected_object (SCM optional)
{
  return SCM_BOOL (cursor_to_prev_selected_object ());
}




SCM
scheme_next_standalone_directive (SCM optional)
{
  return SCM_BOOL (cursor_to_next_standalone_directive ());
}

SCM
scheme_prev_standalone_directive (SCM optional)
{
  return SCM_BOOL (cursor_to_prev_standalone_directive ());
}

SCM
scheme_next_standalone_directive_in_measure (SCM optional)
{
  return SCM_BOOL (cursor_to_next_standalone_in_measure ());
}

SCM
scheme_prev_standalone_directive_in_measure (SCM optional)
{
  return SCM_BOOL (cursor_to_prev_standalone_in_measure ());
}


SCM
scheme_next_chord (SCM optional)
{
  DenemoPosition pos;
  get_position (Denemo.gui->si, &pos);
  gboolean ret = cursor_to_next_chord ();
  if (!ret)
    goto_movement_staff_obj (NULL, -1, pos.staff, pos.measure, pos.object);
  return SCM_BOOL (ret);
}

SCM
scheme_prev_chord (SCM optional)
{
  DenemoPosition pos;
  get_position (Denemo.gui->si, &pos);
  gboolean ret = cursor_to_prev_chord ();
  if (!ret)
    goto_movement_staff_obj (NULL, -1, pos.staff, pos.measure, pos.object);
  return SCM_BOOL (ret);
}


SCM
scheme_next_chord_in_measure (SCM optional)
{
  return SCM_BOOL (cursor_to_next_chord_in_measure ());
}

SCM
scheme_prev_chord_in_measure (SCM optional)
{
  return SCM_BOOL (cursor_to_prev_chord_in_measure ());
}




SCM
scheme_next_note (SCM optional)
{
  return SCM_BOOL (cursor_to_next_note ());
}

SCM
scheme_prev_note (SCM optional)
{
  return SCM_BOOL (cursor_to_prev_note ());
}

void
update_scheme_snippet_ids (void)
{
  DenemoGUI *gui = Denemo.gui;
  GList *g;
  gint i;
  for (g = gui->rhythms, i = 1; g; g = g->next, i++)
    {
      RhythmPattern *r = (RhythmPattern *) g->data;
      if (r->name)
        {
          gchar *command = g_strdup_printf ("(define Snippet::%s %d)", r->name, i);
          call_out_to_guile (command);
          g_free (command);
        }
    }
}

SCM
scheme_create_snippet_from_object (SCM name)
{
  if (scm_is_string (name))
    {
      char *str;
      str = scm_to_locale_string (name);
      if (Denemo.gui->si->currentobject)
        {
          DenemoObject *clonedobj = dnm_clone_object (Denemo.gui->si->currentobject->data);
          RhythmPattern *r = (RhythmPattern *) g_malloc0 (sizeof (RhythmPattern));
          install_button_for_pattern (r, str);
          r->clipboard = g_list_append (NULL, g_list_append (NULL, clonedobj));
          append_rhythm (r, NULL);
          RhythmElement *relement = (RhythmElement *) g_malloc0 (sizeof (RhythmElement));
          //relement->icon = str; was wrong, must be NULL for a singleton.
          r->name = str;
          r->rsteps = g_list_append (NULL, relement);
          r->rsteps->prev = r->rsteps->next = r->rsteps;        //make list circular
          SCM ret = scm_from_int (insert_pattern_in_toolbar (r));
          update_scheme_snippet_ids ();
          if (str)
            free (str);
          return ret;
        }
      if (str)
        free (str);
    }
  return SCM_BOOL_F;
}

SCM
scheme_select_snippet (SCM number)
{
  if (scm_is_integer (number))
    {
      gint position = scm_to_int (number);
      GList *g = g_list_nth (Denemo.gui->rhythms, position - 1);
      if (g)
        {
          RhythmPattern *r = g->data;
          if (r)
            {

              select_rhythm_pattern (r);

              return SCM_BOOL_T;
            }
        }
    }

  return SCM_BOOL_F;
}

SCM
scheme_insert_snippet (SCM number)
{
  if (scm_is_integer (number))
    {
      gint position = scm_to_int (number);
      GList *g = g_list_nth (Denemo.gui->rhythms, position - 1);
      if (g)
        {
          RhythmPattern *r = g->data;
          if (r)
            {

              select_rhythm_pattern (r);
              insert_note_following_pattern (Denemo.gui);

              return SCM_BOOL_T;
            }
        }
    }
  return SCM_BOOL_F;
}



SCM
scheme_locate_dotdenemo (SCM optional)
{
  const gchar *dotdenemo = get_user_data_dir (TRUE);
  if (!dotdenemo)
    return SCM_BOOL (FALSE);
  SCM scm = scm_from_locale_string (dotdenemo);
  return scm;
}