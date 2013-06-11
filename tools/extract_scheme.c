//      extract_scheme.c
//      
//      Copyright 2012 Richard Shann <rshann@debian2>
//      
//      This program is free software; you can redistribute it and/or modify
//      it under the terms of the GNU General Public License as published by
//      the Free Software Foundation; either version 2 of the License, or
//      (at your option) any later version.
//      
//      This program is distributed in the hope that it will be useful,
//      but WITHOUT ANY WARRANTY; without even the implied warranty of
//      MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//      GNU General Public License for more details.
//      
//      You should have received a copy of the GNU General Public License
//      along with this program; if not, write to the Free Software
//      Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
//      MA 02110-1301, USA.

#include <stdio.h>
#include <glib.h>
#include <libxml/parser.h>
#include <libxml/tree.h>
#include <string.h>

static void
extract_scheme (xmlDocPtr doc, xmlNodePtr cur, gchar * filename)
{
  cur = cur->xmlChildrenNode;
  while (cur != NULL)
    {
      if (0 == xmlStrcmp (cur->name, (const xmlChar *) "scheme"))
        {
          gchar *scheme = (gchar*) xmlNodeListGetString (doc, cur->xmlChildrenNode, 1);
          if (scheme && *scheme)
            {
              scheme = g_strdup_printf (";;; Warning!!! This file is derived from those in actions/menus/... do not edit here\n%s", scheme);
              g_file_set_contents (filename, scheme, -1, NULL);
              g_free (scheme);
              g_print ("Written %s\n", filename);
            }
        }
      cur = cur->next;
    }
}

static gchar *usage = "export DENEMO_COMMANDSCRIPTS_DIR=/.../denemo/actions/commandscripts && extract_scheme commandfile_in_xml";
int
main (int argc, char **argv)
{
  if (argc != 2)
    {
      g_critical ("%s: wrong number of arguments %d instead of 1", usage, argc - 1);
      return -1;
    }
  gchar *filename = argv[1];
  const gchar *commandscripts = g_getenv ("DENEMO_COMMANDSCRIPTS_DIR");
  if (commandscripts == NULL)
    {
      g_critical ("%s: Environment variable DENEMO_COMMANDSCRIPTS_DIR not set. Set it to denemo/actions/commandscripts/", usage);
      return -1;
    }
  gint ret = 0;
  xmlDocPtr doc;
  xmlNodePtr rootElem;
  if (!g_file_test (commandscripts, G_FILE_TEST_EXISTS))
    {
      g_critical ("%s: directory %s commandscripts does not exist", usage, commandscripts);
      return -1;
    }
  if (!g_file_test (commandscripts, G_FILE_TEST_IS_DIR))
    {
      g_critical ("%s: directory %s commandscripts is not a directory", usage, commandscripts);
      return -1;
    }
  if (!g_file_test (filename, G_FILE_TEST_EXISTS))
    return ret;

  if (g_file_test (filename, G_FILE_TEST_IS_DIR))
    return ret;

  doc = xmlParseFile (filename);
  if (doc == NULL)
    {
      return ret;
    }
  rootElem = xmlDocGetRootElement (doc);
  if (rootElem == NULL)
    {
      xmlFreeDoc (doc);
      return ret;
    }
  if (xmlStrcmp (rootElem->name, (const xmlChar *) "Denemo"))
    {
      xmlFreeDoc (doc);
      return ret;
    }
  rootElem = rootElem->xmlChildrenNode;

  size_t s = strlen(filename);
  if(filename[s - 3] == 'x' && filename[s - 2 ] == 'm' && filename[s - 1] == 'l')
  {
    filename[s - 3] = 's';
    filename[s - 2] = 'c';
    filename[s - 1] = 'm';
  }
  else
    filename = g_strconcat(g_path_get_basename (filename), ".scm", NULL);
  filename = g_strconcat (commandscripts, "/", filename, NULL);
  while (rootElem != NULL)
    {
      if ((0 == xmlStrcmp (rootElem->name, (const xmlChar *) "merge")))
        {
          xmlNodePtr cur;
          for (cur = rootElem->xmlChildrenNode; cur != NULL; cur = cur->next)
            {
              if (0 == xmlStrcmp (cur->name, (const xmlChar *) "map"))
                {
                  xmlNodePtr ncur;
                  for (ncur = cur->xmlChildrenNode; ncur != NULL; ncur = ncur->next)
                    {
                      extract_scheme (doc, ncur, filename);
                    }
                }
            }
        }
      rootElem = rootElem->next;
    }
  xmlFreeDoc (doc);
  return ret;

}