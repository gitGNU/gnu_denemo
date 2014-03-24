/* exportxml.cpp
 * Functions for exporting what Denemo's working on to an XML file (adapted
 * somewhat from exportabc.c)
 *
 * for Denemo, a gtk+ frontend to GNU Lilypond
 * (c) 2001, 2002 Eric Galluzzo
 */

#include "config.h"
#include <denemo/denemo.h>
#include "core/exportxml.h"
#include "source/source.h"
#include "core/utils.h"
#include "command/lyric.h"
#include "command/lilydirectives.h"
#include "ui/texteditors.h"
#include "export/xmldefs.h"
#include "command/scorelayout.h"
#include "audio/pitchentry.h"
#include <stdlib.h>
#include <string.h>

/* libxml includes: for libxml2 this should be <libxml/tree.h> */
#include <libxml/tree.h>


#define XML_COMPRESSION_RATIO 3


/* The list of export handlers */
/*static GList *sExportHandlers = NULL;*/

/**
 * The map from staves, voices, chords, etc. to XML IDs
 *
 * FIXME: This won't work for multi-threaded applications!  I should really
 *        encapsulate these in a nice DenemoXMLContext struct or some such
 *        thing (or does libxml already provide generating unique IDs?) that I
 *        pass around.
 */
static gint sNextXMLID = 0;
static GHashTable *sStructToXMLIDMap = NULL;


/**
 * Free the value (a string) of the given key/value pair.  For use with
 * g_hash_table_foreach.
 */
static void
freeHashTableValue (G_GNUC_UNUSED gpointer key, gpointer value, G_GNUC_UNUSED gpointer userData)
{
  g_free (value);
}

/**
 * Determine the key signature as it should
 * be written to the XML file
 */
static void
determineKeySignature (gint number, gboolean isMinor, gchar ** baseNote, gchar ** baseAcc, gchar ** mode)
{
  if (isMinor)
    {
      number += 3;
      *mode = "minor";
    }
  else
    {
      *mode = "major";
    }

  switch (number)
    {
    case -7:
      *baseNote = "C";
      *baseAcc = "flat";
      break;
    case -6:
      *baseNote = "G";
      *baseAcc = "flat";
      break;
    case -5:
      *baseNote = "D";
      *baseAcc = "flat";
      break;
    case -4:
      *baseNote = "A";
      *baseAcc = "flat";
      break;
    case -3:
      *baseNote = "E";
      *baseAcc = "flat";
      break;
    case -2:
      *baseNote = "B";
      *baseAcc = "flat";
      break;
    case -1:
      *baseNote = "F";
      *baseAcc = NULL;
      break;
    case 0:
      *baseNote = "C";
      *baseAcc = NULL;
      break;
    case 1:
      *baseNote = "G";
      *baseAcc = NULL;
      break;
    case 2:
      *baseNote = "D";
      *baseAcc = NULL;
      break;
    case 3:
      *baseNote = "A";
      *baseAcc = NULL;
      break;
    case 4:
      *baseNote = "E";
      *baseAcc = NULL;
      break;
    case 5:
      *baseNote = "B";
      *baseAcc = NULL;
      break;
    case 6:
      *baseNote = "F";
      *baseAcc = "sharp";
      break;
    case 7:
      *baseNote = "C";
      *baseAcc = "sharp";
      break;
    case 8:
      *baseNote = "G";
      *baseAcc = "sharp";
      break;
    case 9:
      *baseNote = "D";
      *baseAcc = "sharp";
      break;
    case 10:
      *baseNote = "A";
      *baseAcc = "sharp";
      break;
    default:
      if (isMinor)
        number -= 3;
      g_warning ("Unknown key signature with %d %s, using C major", abs (number), number < 0 ? "flats" : "sharps");
      *baseNote = "C";
      *baseAcc = NULL;
      *mode = "major";
      break;
    }
}

/**
 * Determine the clef as it should
 * be written to the XML file
 */
static void
determineClef (gint type, gchar ** clefName)
{
  switch (type)
    {
    case DENEMO_TREBLE_CLEF:
      *clefName = "treble";
      break;
    case DENEMO_BASS_CLEF:
      *clefName = "bass";
      break;
    case DENEMO_ALTO_CLEF:
      *clefName = "alto";
      break;
    case DENEMO_G_8_CLEF:
      *clefName = "treble-8vb";
      break;
    case DENEMO_F_8_CLEF:
      *clefName = "bass-8vb";
      break;
    case DENEMO_TENOR_CLEF:
      *clefName = "tenor";
      break;
    case DENEMO_SOPRANO_CLEF:
      *clefName = "soprano";
      break;
    case DENEMO_FRENCH_CLEF:
      *clefName = "french";
      break;
    default:
      g_warning ("Unknown clef type %d, using treble", type);
      *clefName = "treble";
      break;
    }
}



/**
 * Determine the duration as it should be written to the XML file.  duration
 * is the Denemo duration, and the XML duration is returned in durationName.
 */
static void
determineDuration (gint duration, gchar ** durationName)
{
  switch (duration)
    {
    case 0:
      *durationName = "whole";
      break;
    case 1:
      *durationName = "half";
      break;
    case 2:
      *durationName = "quarter";
      break;
    case 3:
      *durationName = "eighth";
      break;
    case 4:
      *durationName = "sixteenth";
      break;
    case 5:
      *durationName = "thirty-second";
      break;
    case 6:
      *durationName = "sixty-fourth";
      break;
    case 7:
      *durationName = "one-hundred-twenty-eighth";
      break;
    default:
      if (duration < 0)
        *durationName = "whole";
      else
        {
          g_warning ("Unknown note duration 1/%d, using quarter", 1 << duration);
          *durationName = "quarter";
        }
      break;
    }
}


/**
 * return a child node of parent, holding the passed name and integer.
 */
static xmlNodePtr
newXMLIntChild (xmlNodePtr parent, xmlNsPtr ns, const xmlChar * name, gint content)
{
  gchar *integer = g_strdup_printf ("%d", content);
  xmlNodePtr child = xmlNewChild (parent, ns, name, (xmlChar *) integer);
  g_free (integer);
  return child;
}

/**
 * 
Set a prop of the parent, holding the passed name and integer.
 */
static void
newXMLIntProp (xmlNodePtr parent, const xmlChar * name, gint content)
{
  gchar *integer = g_strdup_printf ("%d", content);
  xmlSetProp (parent, name, (xmlChar *) integer);
  g_free (integer);
}




/**
 * Return a newly allocated unique XML ID string which must be freed by the
 * caller when it is no longer needed.
 */
static gchar *
newXMLID ()
{
  /* Allocate enough space for "id2000000000" + '\0'. */

  gchar *result = g_new (char, 13);
  sprintf (result, "id%d", sNextXMLID++);
  return result;
}


/**
 * Return a unique XML ID for the given pointer, and register it in the global
 * structure -> XML ID map.  If it already exists in the map, return its XML
 * ID; otherwise, create a new XML ID and return that.
 */
static gchar *
getXMLID (gpointer ptr)
{
  gchar *xmlID = (gchar *) g_hash_table_lookup (sStructToXMLIDMap, ptr);
  if (xmlID == NULL)
    g_hash_table_insert (sStructToXMLIDMap, ptr, (xmlID = newXMLID ()));
  return xmlID;
}


/**
 * Output a fraction (<numerator>, <denominator>) as a child of the given
 * node.
 */
static void
newXMLFraction (xmlNodePtr parent, xmlNsPtr ns, gint num, gint denom)
{
  newXMLIntChild (parent, ns, (xmlChar *) "numerator", num);
  newXMLIntChild (parent, ns, (xmlChar *) "denominator", denom);
}

static void
newThumbnailElem (xmlNodePtr curElem, xmlNsPtr ns, DenemoSelection * thumbnail, gchar * type)
{
  xmlNodePtr thumbElem = xmlNewChild (curElem, ns, (xmlChar *) type, NULL);

  newXMLIntChild (thumbElem, ns, (xmlChar *) "first-staff", thumbnail->firststaffmarked);
  newXMLIntChild (thumbElem, ns, (xmlChar *) "last-staff", thumbnail->laststaffmarked);
  newXMLIntChild (thumbElem, ns, (xmlChar *) "first-measure", thumbnail->firstmeasuremarked);
  newXMLIntChild (thumbElem, ns, (xmlChar *) "last-measure", thumbnail->lastmeasuremarked);
  newXMLIntChild (thumbElem, ns, (xmlChar *) "first-obj", thumbnail->firstobjmarked);
  newXMLIntChild (thumbElem, ns, (xmlChar *) "last-obj", thumbnail->lastobjmarked);
}

static void
newSourceFileElem (xmlNodePtr curElem, xmlNsPtr ns, DenemoProject * gui)
{
#ifndef USE_EVINCE  
  g_debug("This feature requires denemo to be built with evince");
#else
  if (source_position (&gui->source_x, &gui->source_y, &gui->source_width, &gui->source_height, &gui->source_scale))
    {
      xmlNodePtr sourceElem = xmlNewChild (curElem, ns, (xmlChar *) "sourcefile", NULL);
      newXMLIntChild (sourceElem, ns, (xmlChar *) "x", gui->source_x);
      newXMLIntChild (sourceElem, ns, (xmlChar *) "y", gui->source_y);
      newXMLIntChild (sourceElem, ns, (xmlChar *) "width", gui->source_width);
      newXMLIntChild (sourceElem, ns, (xmlChar *) "height", gui->source_height);
      newXMLIntChild (sourceElem, ns, (xmlChar *) "scale", gui->source_scale);
    }
#endif
}

static void
newVersesElem (xmlNodePtr curElem, xmlNsPtr ns, GList * verses, gchar * type)
{
  xmlNodePtr versesElem = xmlNewChild (curElem, ns, (xmlChar *) type, NULL);
  for (; verses; verses = verses->next)
    {
      GtkWidget *textview = verses->data;
      xmlNewTextChild (versesElem, ns, (xmlChar *) "verse", (xmlChar *) get_text_from_view (textview));
    }
}

static void
newDirectivesElem (xmlNodePtr objElem, xmlNsPtr ns, GList * g, gchar * type)
{
  xmlNodePtr directivesElem = NULL;
  for (; g; g = g->next)
    {
      DenemoDirective *directive = (DenemoDirective *) g->data;

/*      hhmmmm the set_action_script_for_tag is not modifying the directive - just associating a script with the tag. It is when we try to invoke the directive that we find there is an action script */
/* We Decline to store any directive that has a tag for which an action script is defined in this run of denemo 
*  this implies people need to be careful in the tags they use for action scripts*/
      if (directive->tag && get_action_script (directive->tag->str))
        continue;
      if (directivesElem == NULL)
        directivesElem = xmlNewChild (objElem, ns, (xmlChar *) type, NULL);
      xmlNodePtr directiveElem = xmlNewChild (directivesElem, ns, (xmlChar *) "directive", NULL);
#define DO_DIREC(field)  if (directive->field \
                   && directive->field->len)\
                      xmlNewTextChild (directiveElem, ns, (xmlChar *) #field,\
                     (xmlChar *) directive->field->str);
#define DO_INTDIREC(field)   if(directive->field) newXMLIntChild (directiveElem, ns, (xmlChar *) #field,\
                             directive->field);
      DO_DIREC (tag);
      DO_DIREC (prefix);
      DO_DIREC (postfix);
      DO_DIREC (display);
      DO_DIREC (midibytes);
      DO_DIREC (grob);
      DO_DIREC (data);
      DO_DIREC (graphic_name);

      DO_INTDIREC (minpixels);
      DO_INTDIREC (x);
      DO_INTDIREC (y);
      DO_INTDIREC (tx);
      DO_INTDIREC (ty);
      DO_INTDIREC (gx);
      DO_INTDIREC (gy);
      DO_INTDIREC (override);

#undef DO_DIREC
#undef DO_INTDIREC
    }
}



/**
 * Output a clef of the form:
 *
 *   <clef name="treble"/>
 *
 * as a child of the given node.
 */
static xmlNodePtr
newXMLClef (xmlNodePtr parent, xmlNsPtr ns, clef * clef)
{
  gchar *clefName = NULL;
  xmlNodePtr clefElem = NULL;

  determineClef (clef->type, &clefName);
  clefElem = xmlNewChild (parent, ns, (xmlChar *) "clef", NULL);
  xmlSetProp (clefElem, (xmlChar *) "name", (xmlChar *) clefName);
  if (clef->directives)
    newDirectivesElem (clefElem, ns, clef->directives, "directives");
  return clefElem;
}


/**
 * Output a key signature of the form:
 *
 *   <key-signature>
 *     <modal-key-signature note-name="B" accidental="flat" mode="major"/>
 *   <directives>
 *         ....
 *   </directives>
 *   </key-signature>
 *
 * as a child of the given node.
 */
static xmlNodePtr
newXMLKeySignature (xmlNodePtr parent, xmlNsPtr ns, keysig * keysig)
{
  gint keySig = keysig->number;
  gboolean isMinor = keysig->isminor;
  gchar *noteName = NULL, *accidental = NULL, *mode = NULL;
  xmlNodePtr keySigElem = NULL, modalKeySigElem = NULL;

  determineKeySignature (keySig, isMinor, &noteName, &accidental, &mode);
  keySigElem = xmlNewChild (parent, ns, (xmlChar *) "key-signature", NULL);
  modalKeySigElem = xmlNewChild (keySigElem, ns, (xmlChar *) "modal-key-signature", NULL);
  xmlSetProp (modalKeySigElem, (xmlChar *) "note-name", (xmlChar *) noteName);
  if (accidental != NULL)
    xmlSetProp (modalKeySigElem, (xmlChar *) "accidental", (xmlChar *) accidental);
  xmlSetProp (modalKeySigElem, (xmlChar *) "mode", (xmlChar *) mode);
  if (keysig->directives)
    newDirectivesElem (keySigElem, ns, keysig->directives, "directives");
  return keySigElem;
}


/**
 * Output a time signature of the form:
 *
 *   <time-signature>
 *     <simple-time-signature>
 *       <numerator>3</numerator>
 *       <denominator>4</denominator>
 *     </simple-time-signature>
 *   <directives>
 *         ....
 *   </directives>
 *   </time-signature>
 *
 * as a child of the given node.
 */
static xmlNodePtr
newXMLTimeSignature (xmlNodePtr parent, xmlNsPtr ns, timesig * timesig)
{
  xmlNodePtr timeSigElem = xmlNewChild (parent, ns, (xmlChar *) "time-signature", NULL);
  newXMLFraction (xmlNewChild (timeSigElem, ns, (xmlChar *) "simple-time-signature", NULL), ns, timesig->time1, timesig->time2);
  if (timesig->directives)
    newDirectivesElem (timeSigElem, ns, timesig->directives, "directives");
  return timeSigElem;
}


/**
 * Output a decoration of the form:
 *
 *   <decoration type="staccato"/>
 *
 * as a child of the given node.
 */
/*UNUSED
static xmlNodePtr
newXMLDecoration (xmlNodePtr parent, xmlNsPtr ns, gchar * decorationName)
{
  xmlNodePtr decorationElem = xmlNewChild (parent, ns, (xmlChar *) "decoration", NULL);
  xmlSetProp (decorationElem, (xmlChar *) "type", (xmlChar *) decorationName);
  return decorationElem;
}
*/

/**
 * Output an accidental of the form:
 *
 *   <accidental name="double-sharp" show="true"/>
 *
 * as a child of the given node.
 */
static xmlNodePtr
newXMLAccidental (xmlNodePtr parent, xmlNsPtr ns, gint enshift, gboolean show)
{
  xmlNodePtr accElem = xmlNewChild (parent, ns, (xmlChar *) "accidental", NULL);
  gchar *accName = NULL;
  switch (enshift)
    {
    case -2:
      accName = "double-flat";
      break;
    case -1:
      accName = "flat";
      break;
    case 0:
      accName = "natural";
      break;
    case 1:
      accName = "sharp";
      break;
    case 2:
      accName = "double-sharp";
      break;
    default:
      g_warning ("Illegal accidental shift %d: must be between -2 and 2; " "using 0", enshift);
      accName = "natural";
      break;
    }
  xmlSetProp (accElem, (xmlChar *) "name", (xmlChar *) accName);
  xmlSetProp (accElem, (xmlChar *) "show", (show ? (xmlChar *) "true" : (xmlChar *) "false"));
  return accElem;
}


/**
 * Output a stem directive of the form:
 *
 *   <stem-directive type="down"/>
 *
 * as a child of the given node.
 */
static xmlNodePtr
newXMLStemDirective (xmlNodePtr parent, xmlNsPtr ns, stemdirective * stem)
{
  xmlNodePtr stemElem = xmlNewChild (parent, ns, (xmlChar *) "stem-directive", NULL);
  gchar *stemName = NULL;
  switch (stem->type)
    {
    case DENEMO_STEMDOWN:
      stemName = "down";
      break;
    case DENEMO_STEMBOTH:
      stemName = "auto";
      break;
    case DENEMO_STEMUP:
      stemName = "up";
      break;
    default:
      g_warning ("Unknown stem directive type %d, using auto", stem->type);
      stemName = "auto";
      break;
    }
  xmlSetProp (stemElem, (xmlChar *) "type", (xmlChar *) stemName);
  if (stem->directives)
    newDirectivesElem (stemElem, ns, stem->directives, "directives");
  return stemElem;
}

/* staff (aka voice) properties */
static void
newVoiceProps (xmlNodePtr parentElem, xmlNsPtr ns, DenemoStaff * curStaffStruct)
{
  xmlNodePtr curElem = xmlNewChild (parentElem, ns, (xmlChar *) "voice-props", NULL);
  newXMLIntChild (curElem, ns, (xmlChar *) "number-of-lines", curStaffStruct->no_of_lines);
  newXMLIntChild (curElem, ns, (xmlChar *) "voice-control", curStaffStruct->voicecontrol);
  newXMLIntChild (curElem, ns, (xmlChar *) "transpose", curStaffStruct->transposition);
  xmlNewChild (curElem, ns, (xmlChar *) "instrument", (xmlChar *) curStaffStruct->midi_instrument->str);
  if (curStaffStruct->device_port->len)
    xmlNewChild (curElem, ns, (xmlChar *) "device-port", (xmlChar *) curStaffStruct->device_port->str);

  newXMLIntChild (curElem, ns, (xmlChar *) "volume", curStaffStruct->volume);
  newXMLIntChild (curElem, ns, (xmlChar *) "midi_prognum", curStaffStruct->midi_prognum);
  newXMLIntChild (curElem, ns, (xmlChar *) "midi_channel", curStaffStruct->midi_channel);

  newXMLIntChild (curElem, ns, (xmlChar *) "hasfigures", curStaffStruct->hasfigures);

  newXMLIntChild (curElem, ns, (xmlChar *) "hasfakechords", curStaffStruct->hasfakechords);

  if (curStaffStruct->verses)
    newVersesElem (curElem, ns, curStaffStruct->verses, "verses");

  if (curStaffStruct->staff_directives)
    newDirectivesElem (curElem, ns, curStaffStruct->staff_directives, "staff-directives");

  if (curStaffStruct->voice_directives)
    newDirectivesElem (curElem, ns, curStaffStruct->voice_directives, "voice-directives");
  if (curStaffStruct->clef.directives)
    newDirectivesElem (curElem, ns, curStaffStruct->clef.directives, "clef-directives");
}

/**
 * Output a notehead element of the form:
 *
 *   <note-head type="diamond"/>
 *
 * as a child of the given node.
 */
static xmlNodePtr
newXMLNoteHead (xmlNodePtr parent, xmlNsPtr ns, enum headtype noteHeadType)
{
  xmlNodePtr noteHeadElem = xmlNewChild (parent, ns, (xmlChar *) "note-head",
                                         NULL);
  gchar *headTypeName = NULL;
  switch (noteHeadType)
    {
    case DENEMO_NORMAL_NOTEHEAD:
      headTypeName = "normal";
      break;
    case DENEMO_CROSS_NOTEHEAD:
      headTypeName = "cross";
      break;
    case DENEMO_HARMONIC_NOTEHEAD:
      headTypeName = "harmonic";
      break;
    case DENEMO_DIAMOND_NOTEHEAD:
      headTypeName = "diamond";
      break;
    default:
      g_warning ("Unknown notehead type %d, using normal", noteHeadType);
      headTypeName = "normal";
      break;
    }
  xmlSetProp (noteHeadElem, (xmlChar *) "type", (xmlChar *) headTypeName);
  return noteHeadElem;
}

static void
outputSources (xmlNodePtr mvmntElem, xmlNsPtr ns, GList * sources)
{
  GList *g = sources;
  xmlNodePtr curElem = xmlNewChild (mvmntElem, ns, (xmlChar *) "sources", NULL);
  for (; g; g = g->next)
    {
      gsize len;
      GError *error = NULL;
      gchar *buf;
      gdk_pixbuf_save_to_buffer (g->data, &buf, &len, "png", &error, NULL);
      gchar *cdata = g_base64_encode ((guchar*) buf, len);
      g_free (buf);
      xmlNewChild (curElem, ns, (xmlChar*) "pixbuf", (xmlChar*) cdata);
      g_free (cdata);
      //??? xmlNodePtr xmlNewCDataBlock       (xmlDocPtr doc, const xmlChar *content, int len);

      //use gboolean  gdk_pixbuf_save_to_buffer(GdkPixbuf *pixbuf, gchar **buffer, gsize *buffer_size, const char *type, GError **error, ...);  
//g_base64_encode(guchar *buf, gsize len);
//g_base64_decode(guchar *buf, gsize* outlen);
//g_base64_decode_inplace(guchar *buf, gsize* outlen);//overwrites buf with decoded data.
    }
}


static void
outputAudio (xmlNodePtr mvmntElem, xmlNsPtr ns, DenemoRecording * audio)
{
  xmlNodePtr curElem = xmlNewChild (mvmntElem, ns, (xmlChar *) "audio", NULL);
  xmlNewChild (curElem, ns, (xmlChar *) "filename", (xmlChar*) audio->filename);
  newXMLIntChild (curElem, ns, (xmlChar *) "lead-in", audio->leadin);
}


static void
set_invisible (xmlNodePtr objElem, DenemoObject * curObj)
{
  if (curObj->isinvisible)
    xmlSetProp (objElem, (xmlChar *) "show", (xmlChar *) "false");
  else
    xmlSetProp (objElem, (xmlChar *) "show", (xmlChar *) "true");
}


/**
 * Export the given score (from measure start to measure end) as a "native"
 * Denemo XML file to the given file.
 */
gint
exportXML (gchar * thefilename, DenemoProject * gui)
{
  gint ret = 0;
  GString *filename = g_string_new (thefilename);
  xmlDocPtr doc;
  xmlNodePtr scoreElem, mvmntElem, stavesElem, voicesElem, voiceElem;
  xmlNodePtr measuresElem, measureElem, objElem, prevTieElem;
  xmlNodePtr curElem, parentElem;
  xmlNsPtr ns;
  staffnode *curStaff;
  DenemoStaff *curStaffStruct;
  gchar *staffXMLID = 0, *voiceXMLID;
  gchar *lastBeamStartXMLID, *chordXMLID, *noteXMLID;
  gchar *lastTupletStartXMLID;
  //gchar *clefname, *baseKeyName, *accidental;
  measurenode *curMeasure;
  objnode *curObjNode;
  DenemoObject *curObj;
  GList *curNoteNode;
  note *curNote;
  GList *slurElemStack;
  GList *crescElemStack;
  GList *diminElemStack;
  //gint numerator, denominator;
  gchar *durationType;
  static gchar *version_string;
  if (version_string == NULL)
    version_string = g_strdup_printf ("%d", CURRENT_XML_VERSION);
  /* Append .denemo onto the filename if necessary. */

  //if (strcmp (filename->str + filename->len - 7, ".denemo"))
  //  g_string_append (filename, ".denemo");

  /* Initialize score-wide variables. */

  sStructToXMLIDMap = g_hash_table_new (NULL, NULL);

  /* Create the XML document and output the root element. */

  doc = xmlNewDoc ((xmlChar *) "1.0");
  //xmlSetDocCompressMode (doc, XML_COMPRESSION_RATIO);
  doc->xmlRootNode = scoreElem = xmlNewDocNode (doc, NULL, (xmlChar *) "score", NULL);
  ns = xmlNewNs (doc->xmlRootNode, (xmlChar *) DENEMO_XML_NAMESPACE, NULL);
  xmlSetProp (scoreElem, (xmlChar *) "version", (xmlChar *) version_string);
  /* FIXME: Put comment here ("Denemo XML file generated by..."). */

  if (!Denemo.non_interactive && getNumCharsSchemeText ())
    {
      gchar *text = (gchar *) getSchemeText ();
      xmlNewTextChild (scoreElem, ns, (xmlChar *) "scheme", (xmlChar *) text);
      g_free (text);
    }

  if (gui->scoreheader.directives)
    newDirectivesElem (scoreElem, ns, gui->scoreheader.directives, "scoreheader-directives");
  if (gui->paper.directives)
    newDirectivesElem (scoreElem, ns, gui->paper.directives, "paper-directives");

  if (gui->thumbnail.firststaffmarked)
    newThumbnailElem (scoreElem, ns, &gui->thumbnail, "thumbnail");


  newSourceFileElem (scoreElem, ns, gui);

  /* lilycontrol for the whole musical score */


  parentElem = xmlNewChild (scoreElem, ns, (xmlChar *) "lilycontrol", NULL);
#define NEWCHILD(field) if(gui->lilycontrol.field->len) \
                       xmlNewTextChild (parentElem, ns, (xmlChar *) #field,\
                      (xmlChar *) gui->lilycontrol.field->str)
  NEWCHILD (papersize);
  NEWCHILD (lilyversion);

  newXMLIntChild (parentElem, ns, (xmlChar *) "fontsize", atoi (gui->lilycontrol.staffsize->str));
  newXMLIntChild (parentElem, ns, (xmlChar *) "orientation", gui->lilycontrol.orientation);

  if (gui->lilycontrol.directives)
    newDirectivesElem (parentElem, ns, gui->lilycontrol.directives, "score-directives");

  GList *custom;
  for (custom = g_list_last (gui->custom_scoreblocks); custom; custom = custom->prev)
    {
      GString *lilypond = (GString *) (((DenemoScoreblock *) custom->data)->lilypond);
      if (lilypond == NULL)
        refresh_lilypond ((DenemoScoreblock *) custom->data);
      lilypond = (GString *) (((DenemoScoreblock *) custom->data)->lilypond);
      if (lilypond)
        {
          xmlNodePtr scoreblockElem = xmlNewTextChild (scoreElem, ns, (xmlChar *) "custom_scoreblock", (xmlChar *) (lilypond->str));
          if (((DenemoScoreblock *) custom->data)->uri)
            xmlSetProp (scoreblockElem, (xmlChar *) "scoreblock_uri", (xmlChar *) ((DenemoScoreblock *) custom->data)->uri);
        }
      else
        g_warning ("Custom Scoreblock with no LilyPond text");
    }
  //  if(gui->custom_prolog && gui->custom_prolog->len)
  //   xmlNewChild (scoreElem, ns, "custom_prolog", (xmlChar *)gui->custom_prolog->str);
  gint movement_number = 1 + g_list_index (gui->movements, gui->movement);
  if (movement_number)
    newXMLIntChild (scoreElem, ns, (xmlChar *) "movement-number", movement_number);
  GList *g;
  for (g = gui->movements; g; g = g->next)
    {
      DenemoMovement *si = g->data;

      mvmntElem = xmlNewChild (scoreElem, ns, (xmlChar *) "movement", NULL);
      parentElem = xmlNewChild (mvmntElem, ns, (xmlChar *) "edit-info", NULL);
      newXMLIntChild (parentElem, ns, (xmlChar *) "staffno", si->currentstaffnum);
      newXMLIntChild (parentElem, ns, (xmlChar *) "measureno", si->currentmeasurenum);

      newXMLIntChild (parentElem, ns, (xmlChar *) "cursorposition", si->cursor_x - 1);
      newXMLIntChild (parentElem, ns, (xmlChar *) "tonalcenter", get_enharmonic_position ());

      newXMLIntChild (parentElem, ns, (xmlChar *) "zoom", (int) (100 * si->zoom));
      newXMLIntChild (parentElem, ns, (xmlChar *) "system-height", (int) (100 * si->system_height));

      newXMLIntChild (parentElem, ns, (xmlChar *) "page-zoom", (int) (100 * si->page_zoom));
      newXMLIntChild (parentElem, ns, (xmlChar *) "page-system-height", (int) (100 * si->page_system_height));
      if (si->page_width)
        newXMLIntChild (parentElem, ns, (xmlChar *) "page-width", si->page_width);
      if (si->page_height)
        newXMLIntChild (parentElem, ns, (xmlChar *) "page-height", si->page_height);
      if (si->measurewidth != DENEMO_INITIAL_MEASURE_WIDTH)
        newXMLIntChild (parentElem, ns, (xmlChar *) "measure-width", si->measurewidth);

      if (si->header.directives)
        {
          newDirectivesElem (mvmntElem, ns, si->header.directives, "header-directives");
        }
      if (si->layout.directives)
        {
          newDirectivesElem (mvmntElem, ns, si->layout.directives, "layout-directives");
        }
      if (si->movementcontrol.directives)
        {
          newDirectivesElem (mvmntElem, ns, si->movementcontrol.directives, "movementcontrol-directives");
        }

      // output si->sources
      if (si->sources)
        outputSources (mvmntElem, ns, si->sources);
      // output audio source
      if (si->recording)
        outputAudio (mvmntElem, ns, si->recording);

      parentElem = xmlNewChild (mvmntElem, ns, (xmlChar *) "score-info", NULL);
      curElem = xmlNewChild (parentElem, ns, (xmlChar *) "tempo", NULL);
      newXMLFraction (xmlNewChild (curElem, ns, (xmlChar *) "duration", NULL), ns, 1, 4);
      newXMLIntChild (curElem, ns, (xmlChar *) "bpm", si->tempo * si->master_tempo);

      stavesElem = xmlNewChild (mvmntElem, ns, (xmlChar *) "staves", NULL);
      for (curStaff = si->thescore; curStaff != NULL; curStaff = curStaff->next)
        {
          curStaffStruct = (DenemoStaff *) curStaff->data;
          if (!(curStaffStruct->voicecontrol & DENEMO_SECONDARY))
            {
              parentElem = xmlNewChild (stavesElem, ns, (xmlChar *) "staff", NULL);
              staffXMLID = getXMLID (curStaffStruct);
              xmlSetProp (parentElem, (xmlChar *) "id", (xmlChar *) staffXMLID);
              //  curElem = xmlNewChild (parentElem, ns,
              //                       (xmlChar *) "staff-info", NULL);



            }
        }




      /* Output each voice. These are the DenemoStaff objects */

      voicesElem = xmlNewChild (mvmntElem, ns, (xmlChar *) "voices", NULL);
      for (curStaff = si->thescore; curStaff != NULL; curStaff = curStaff->next)
        {
          curStaffStruct = (DenemoStaff *) curStaff->data;

          /* Initialize voice-wide variables. */

          prevTieElem = NULL;
          slurElemStack = NULL;
          crescElemStack = NULL;
          diminElemStack = NULL;
          lastBeamStartXMLID = NULL;
          lastTupletStartXMLID = NULL;

          /*
           * If this is a primary voice, find the ID of its staff, which applies
           * until the next primary voice we run across.
           */

          if (!(curStaffStruct->voicecontrol & DENEMO_SECONDARY))
            {
              staffXMLID = getXMLID (curStaffStruct);
            }

          voiceElem = xmlNewChild (voicesElem, ns, (xmlChar *) "voice", NULL);
          voiceXMLID = newXMLID ();
          xmlSetProp (voiceElem, (xmlChar *) "id", (xmlChar *) voiceXMLID);

          /* Nobody actually needs the voice ID right now, so we throw it away. */

          g_free (voiceXMLID);

          /*
           * Output the voice info (voice name and first measure number, which
           * currently is always 1.
           */

          parentElem = xmlNewChild (voiceElem, ns, (xmlChar *) "voice-info", NULL);
          xmlNewChild (parentElem, ns, (xmlChar *) "voice-name", (xmlChar *) curStaffStruct->denemo_name->str);
          newXMLIntChild (parentElem, ns, (xmlChar *) "first-measure-number", 1);

          /*
           * Output the initial voice parameters:
           *     - staff on which this voice resides
           *     - clef
           *     - key signature
           *     - time signature
           */

          parentElem = xmlNewChild (voiceElem, ns, (xmlChar *) "initial-voice-params", NULL);
          curElem = xmlNewChild (parentElem, ns, (xmlChar *) "staff-ref", NULL);
          xmlSetProp (curElem, (xmlChar *) "staff", (xmlChar *) staffXMLID);
          newXMLClef (parentElem, ns, &curStaffStruct->clef);
          newXMLKeySignature (parentElem, ns, &curStaffStruct->keysig);
          newXMLTimeSignature (parentElem, ns, &curStaffStruct->timesig);
// output here the stuff like device-port which are currently being done on the staff, because that staff is just a container, not a real Denemo staff
          newVoiceProps (voiceElem, ns, curStaffStruct);

          // output staff->sources
          if (curStaffStruct->sources)
            outputSources (parentElem, ns, curStaffStruct->sources);


          /* Write out the measures. */
          measuresElem = xmlNewChild (voiceElem, ns, (xmlChar *) "measures", NULL);
          for (curMeasure = curStaffStruct->measures; curMeasure != NULL; curMeasure = curMeasure->next)
            {

              measureElem = xmlNewChild (measuresElem, ns, (xmlChar *) "measure", NULL);

              for (curObjNode = (objnode *) curMeasure->data; curObjNode != NULL; curObjNode = curObjNode->next)
                {
                  curObj = (DenemoObject *) curObjNode->data;

                  switch (curObj->type)
                    {
                    case CHORD:
                      {
                        chord *thechord = (chord *) curObj->object;


                        /* Output the root element, "rest" or "chord". */

                        if ((thechord)->notes == NULL)
                          {
                            objElem = xmlNewChild (measureElem, ns, (xmlChar *) "rest", NULL);

                          }
                        else
                          objElem = xmlNewChild (measureElem, ns, (xmlChar *) "chord", NULL);
                        set_invisible (objElem, curObj);


                        chordXMLID = getXMLID (curObj);
                        xmlSetProp (objElem, (xmlChar *) "id", (xmlChar *) chordXMLID);

                        if (thechord->is_grace & GRACED_NOTE)
                          xmlSetProp (objElem, (xmlChar *) "grace", (xmlChar *) "true");
                        else if (thechord->is_grace & ACCIACCATURA)
                          xmlSetProp (objElem, (xmlChar *) "grace", (xmlChar *) "acciaccatura");

                        /* Output the duration. */

                        determineDuration ((thechord)->baseduration, &durationType);
                        parentElem = xmlNewChild (objElem, ns, (xmlChar *) "duration", NULL);
                        xmlSetProp (parentElem, (xmlChar *) "base", (xmlChar *) durationType);
                        if ((thechord)->numdots != 0)
                          newXMLIntChild (parentElem, ns, (xmlChar *) "dots", (thechord)->numdots);


                        if ((thechord)->chordize)
                          newXMLIntChild (objElem, ns, (xmlChar *) "chordize", TRUE);

                        /*Output Lyric */
                        if ((thechord)->lyric)
                          {
                            parentElem = xmlNewTextChild (objElem, ns, (xmlChar *) "lyric", (xmlChar *) (thechord)->lyric->str);
                            if ((thechord)->is_syllable)
                              xmlSetProp (parentElem, (xmlChar *) "extend", (xmlChar *) "true");
                            else
                              xmlSetProp (parentElem, (xmlChar *) "extend", (xmlChar *) "false");

                            if ((thechord)->center_lyric)
                              xmlSetProp (parentElem, (xmlChar *) "center", (xmlChar *) "true");
                            else
                              xmlSetProp (parentElem, (xmlChar *) "center", (xmlChar *) "false");

                          }
                        /*Output Figured Bass */
                        if ((thechord)->figure)
                          {
                            //DenemoObject *mud = (DenemoObject *) ((GList *) ((thechord)->figure)->data);
                            //chord *mych = (chord *) mud->object;

                            parentElem = xmlNewTextChild (objElem, ns, (xmlChar *) "figure", (xmlChar *) ((GString *) (thechord)->figure)->str);

                            // printf("\nfigure in exportxml == %s\n", ((GString *) (thechord)->figure)->str);
                          }
                        //(thechord)->is_figure = FALSE;
                        //     parentElem = xmlNewChild (objElem, ns, (xmlChar *) "figure",
                        //                              (GString *) mych->figure);

                        /*Output Fakechords */
                        if ((thechord)->fakechord && ((GString *) (thechord)->fakechord)->len)
                          {

                            GString *temp = g_string_new ("");
                            temp = g_string_append (temp, ((GString *) (thechord)->fakechord)->str);

                            parentElem = xmlNewTextChild (objElem, ns, (xmlChar *) "fakechord", (xmlChar *) ((GString *) temp)->str);
                          }



                        /* Output the DenemoDirectives on the chord */
                        if ((thechord)->directives)
                          {
                            newDirectivesElem (objElem, ns, (thechord)->directives, "directives");
                          }
                        if (thechord->baseduration < 0)
                          newXMLIntChild (objElem, ns, (xmlChar *) "ticks", -thechord->baseduration);


                        /*
                         *  Output Dynamic which is now part of note 
                         *
                         */
                        if ((thechord)->dynamics)
                          {
                            GString *string = (GString *) (thechord)->dynamics->data;
                            parentElem = xmlNewChild (objElem, ns, (xmlChar *) "dynamic", NULL);
                            xmlSetProp (parentElem, (xmlChar *) "name", (xmlChar *) string->str);
                          }
                        /*
                         * If this is the end of a slur, terminate the previous
                         * <slur> element.
                         */

                        if ((thechord)->slur_end_p)
                          {
                            if (slurElemStack == NULL)
                              {
                                g_warning ("Encountered slur end without a " "beginning");
                              }
                            else
                              {
                                xmlSetProp ((xmlNodePtr) slurElemStack->data, (xmlChar *) "to", (xmlChar *) chordXMLID);

                                /* Pop the top element off the stack. */

                                slurElemStack = g_list_remove (slurElemStack, slurElemStack->data);
                              }
                          }

                        /*
                         * If this is the end of a crescendo, terminate the previous
                         * <crescendo> element.
                         */

                        if ((thechord)->crescendo_end_p)
                          {
                            if (crescElemStack == NULL)
                              {
                                g_warning ("Encountered crescendo end without a " "beginning");
                              }
                            else
                              {
                                xmlSetProp ((xmlNodePtr) crescElemStack->data, (xmlChar *) "to", (xmlChar *) chordXMLID);

                                /* Pop the top element off the stack. */

                                crescElemStack = g_list_remove (crescElemStack, crescElemStack->data);
                              }
                          }

                        /*
                         * If this is the end of a diminuendo, terminate the previous
                         * <diminuendo> element.
                         */

                        if ((thechord)->diminuendo_end_p)
                          {
                            if (diminElemStack == NULL)
                              {
                                g_warning ("Encountered diminuendo end without a " "beginning");
                              }
                            else
                              {
                                xmlSetProp ((xmlNodePtr) diminElemStack->data, (xmlChar *) "to", (xmlChar *) chordXMLID);

                                /* Pop the top element off the stack. */

                                diminElemStack = g_list_remove (diminElemStack, diminElemStack->data);
                              }
                          }

                        /*
                         * Output a <slur> element (to be filled in by the end of
                         * the slur) if there's a slur beginning on this chord.
                         */

                        if ((thechord)->slur_begin_p)
                          {
                            parentElem = xmlNewChild (objElem, ns, (xmlChar *) "slurs", NULL);
                            curElem = xmlNewChild (parentElem, ns, (xmlChar *) "slur", NULL);

                            /* Push the <slur> element onto the slur stack. */

                            slurElemStack = g_list_prepend (slurElemStack, curElem);
                          }

                        /*
                         * Output a <crescendo> element (to be filled in by the end of
                         * the crescendo) if there's a crescendo beginning on this chord.
                         */

                        if ((thechord)->crescendo_begin_p)
                          {
                            parentElem = xmlNewChild (objElem, ns, (xmlChar *) "crescendos", NULL);
                            curElem = xmlNewChild (parentElem, ns, (xmlChar *) "crescendo", NULL);

                            /* Push the <crescendo> element onto the crescendo stack. */

                            crescElemStack = g_list_prepend (crescElemStack, curElem);
                          }

                        /*
                         * Output a <diminuendo> element (to be filled in by the end of
                         * the diminuendo) if there's a diminuendo beginning on this chord.
                         */

                        if ((thechord)->diminuendo_begin_p)
                          {
                            parentElem = xmlNewChild (objElem, ns, (xmlChar *) "diminuendos", NULL);
                            curElem = xmlNewChild (parentElem, ns, (xmlChar *) "diminuendo", NULL);

                            /* Push the <diminuendo> element onto the diminuendo stack. */

                            diminElemStack = g_list_prepend (diminElemStack, curElem);
                          }

                        /*
                         * If the previous chord was tied, fill in the <tie>
                         * element to point to this one.
                         */

                        if (prevTieElem != NULL)
                          {
                            xmlSetProp (prevTieElem, (xmlChar *) "to", (xmlChar *) chordXMLID);
                            prevTieElem = NULL;
                          }

                        /* Output a <tie> element if this chord is tied. */

                        if ((thechord)->is_tied)
                          {
                            prevTieElem = xmlNewChild (objElem, ns, (xmlChar *) "tie", NULL);
                          }




                        /* Output all the notes, if this isn't a rest. */

                        if ((thechord)->notes != NULL)
                          {
                            parentElem = xmlNewChild (objElem, ns, (xmlChar *) "notes", NULL);
                            for (curNoteNode = (thechord)->notes; curNoteNode != NULL; curNoteNode = curNoteNode->next)
                              {
                                curNote = (note *) curNoteNode->data;
                                curElem = xmlNewChild (parentElem, ns, (xmlChar *) "note", NULL);
                                noteXMLID = getXMLID (curNote);
                                xmlSetProp (curElem, (xmlChar *) "id", (xmlChar *) noteXMLID);
                                newXMLIntChild (curElem, ns, (xmlChar *) "middle-c-offset", curNote->mid_c_offset);
                                if (curNote->enshift != 0 || curNote->showaccidental)
                                  {
                                    newXMLAccidental (curElem, ns, curNote->enshift, curNote->showaccidental);
                                  }
                                if (curNote->noteheadtype != DENEMO_NORMAL_NOTEHEAD)
                                  {
                                    newXMLNoteHead (curElem, ns, curNote->noteheadtype);
                                  }

                                if (curNote->directives)
                                  {
                                    newDirectivesElem (curElem, ns, curNote->directives, "directives");

                                  }

                              }
                          }
                      }
                      break;

                    case TUPOPEN:
                      {
                        tuplet *theob = (tuplet *) curObj->object;
                        objElem = xmlNewChild (measureElem, ns, (xmlChar *) "tuplet-start", NULL);
                        /*
                         * not true? FIXME: This code does not yet handle nested tuplets.  For
                         *        that, we'd need a stack of "tuplet-start" IDs
                         *        instead of just a single "last ID."
                         */

                        lastTupletStartXMLID = getXMLID (curObj);
                        xmlSetProp (objElem, (xmlChar *) "id", (xmlChar *) lastTupletStartXMLID);
                        newXMLFraction (xmlNewChild (objElem, ns, (xmlChar *) "multiplier", NULL), ns, ((tupopen *) curObj->object)->numerator, ((tupopen *) curObj->object)->denominator);
                        if (theob->directives)
                          {
                            newDirectivesElem (objElem, ns, theob->directives, "directives");

                          }
                      }
                      break;

                    case TUPCLOSE:
                      {
                        tuplet *theob = (tuplet *) curObj->object;
                        objElem = xmlNewChild (measureElem, ns, (xmlChar *) "tuplet-end", NULL);
                        if (lastTupletStartXMLID == NULL)
                          {
                            g_warning ("Encountered nested tuplets or tuplet end " "without start");
                          }
                        else
                          {
                            xmlSetProp (objElem, (xmlChar *) "tuplet", (xmlChar *) lastTupletStartXMLID);
                            lastTupletStartXMLID = NULL;
                          }
                        if (theob->directives)
                          {
                            newDirectivesElem (objElem, ns, theob->directives, "directives");

                          }
                      }
                      break;

                    case CLEF:
                      objElem = newXMLClef (measureElem, ns, ((clef *) curObj->object));
                      set_invisible (objElem, curObj);
                      break;

                    case TIMESIG:
                      objElem = newXMLTimeSignature (measureElem, ns, (timesig *) curObj->object);
                      break;

                    case KEYSIG:
                      objElem = newXMLKeySignature (measureElem, ns, ((keysig *) curObj->object));
                      break;

                    case STEMDIRECTIVE:
                      {
                        stemdirective *theob = (stemdirective *) curObj->object;
                        objElem = newXMLStemDirective (measureElem, ns, theob);

                      }
                      break;

                    case GRACE_START:
                      //obsolete
                      break;

                    case GRACE_END:
                      //obsolete
                      break;
                    case LYRIC:
                      objElem = xmlNewTextChild (measureElem, ns, (xmlChar *) "lyric", (xmlChar *) ((lyric *) curObj->object)->lyrics->str);

                      if (((lyric *) curObj->object)->is_syllable)
                        xmlSetProp (objElem, (xmlChar *) "extend", (xmlChar *) "true");
                      else
                        xmlSetProp (objElem, (xmlChar *) "extend", (xmlChar *) "false");

                      if (((lyric *) curObj->object)->center_lyric)
                        xmlSetProp (objElem, (xmlChar *) "center", (xmlChar *) "true");
                      else
                        xmlSetProp (objElem, (xmlChar *) "center", (xmlChar *) "false");

                      break;
                    case LILYDIRECTIVE:
                      //FIXME this should really have been the tag saved here, but for backwards compatibility we use the postfix string.
                      if (((lilydirective *) curObj->object)->postfix && ((lilydirective *) curObj->object)->postfix->len)
                        objElem = xmlNewTextChild (measureElem, ns, (xmlChar *) "lily-directive", (xmlChar *) ((lilydirective *) curObj->object)->postfix->str);
                      else
                        objElem = xmlNewTextChild (measureElem, ns, (xmlChar *) "lily-directive", (xmlChar *) " ");

                      if (((lilydirective *) curObj->object)->locked)
                        xmlSetProp (objElem, (xmlChar *) "locked", (xmlChar *) "true");

#define SETSTRING_PROP(field)\
          if(((lilydirective *) curObj->object)->field && ((lilydirective *) curObj->object)->field->len)\
            xmlSetProp (objElem, (xmlChar *) #field,\
                  (xmlChar *) (((lilydirective *) curObj->object)->field->str));

                      SETSTRING_PROP (tag);
                      SETSTRING_PROP (display);
                      SETSTRING_PROP (midibytes);
                      SETSTRING_PROP (grob);
                      SETSTRING_PROP (data);
                      SETSTRING_PROP (graphic_name);
                      SETSTRING_PROP (prefix);  //postfix done above, for backward compatibility
#undef SETSTRING_PROP

#define SETINT_PROP(x)\
            if(((lilydirective *) curObj->object)->x) newXMLIntProp (objElem, (xmlChar *) #x, \
                   (((lilydirective *) curObj->object)->x));
                      SETINT_PROP (x);
                      SETINT_PROP (y);
                      SETINT_PROP (tx);
                      SETINT_PROP (ty);
                      SETINT_PROP (gx);
                      SETINT_PROP (gy);
                      SETINT_PROP (minpixels);
                      SETINT_PROP (override);
#undef SETINT_PROP
                      if (curObj->durinticks)
                        newXMLIntProp (objElem, (xmlChar*) "ticks", curObj->durinticks);
                      break;
                    case BARLINE:
                    case MEASUREBREAK:
                      g_warning ("Cannot yet handle DenemoObjects of type %d", curObj->type);
                      break;
                    default:
                      /* FIXME: First try handlers. */
                      g_warning ("Got a DenemoObject of unknown type %d", curObj->type);
                      break;
                    }           /* end switch on object type */
                }               /* end for each object in measure */


            }                   /* end for each measure in voice */

          /* Clean up voice-specific variables. */

          g_list_free (slurElemStack);
          g_list_free (crescElemStack);
          g_list_free (diminElemStack);
          g_free (lastBeamStartXMLID);
        }                       /* end for each voice in score */
    }                           // for each movement
  /* Save the file. */

  if (xmlSaveFormatFile (filename->str, doc, 1) < 0)
    {
      g_warning ("Could not save file %s", filename->str);
      ret = -1;
    }

  /* Clean up all the memory we've allocated. */

  xmlFreeDoc (doc);
  g_hash_table_foreach (sStructToXMLIDMap, freeHashTableValue, NULL);
  g_hash_table_destroy (sStructToXMLIDMap);
  sNextXMLID = 0;

  g_string_free (filename, TRUE);
  return ret;
}