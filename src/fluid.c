#ifdef _HAVE_FLUIDSYNTH_
#include <glib.h>
#include <fluidsynth.h>
#include "midi.h"
#include "smf.h"
#include "fluid.h"

#define MAX_NUMBER_OF_TRACKS    128
#define NOTE_OFF                0x80
#define NOTE_ON                 0x90
#define SYS_EXCLUSIVE_MESSAGE1  0xF0

fluid_settings_t* settings;
fluid_synth_t* synth;
fluid_audio_driver_t* adriver;
int sfont_id;
static gint timeout_id = 0, kill_id=0;

int fluidsynth_init()
{ 
  g_debug("\nStarting FLUIDSYNTH\n"); 
  /* Create the settings. */
  settings = new_fluid_settings();
  if (!settings){
    g_warning("\nFailed to create the settings\n");
    fluidsynth_shutdown();
    return 1;
  }

  /* Change the settings if necessary*/
  fluid_settings_setstr(settings, "audio.driver", Denemo.prefs.fluidsynth_audio_driver->str);
  fluid_settings_setstr(settings, "synth.reverb.active" , Denemo.prefs.fluidsynth_reverb == TRUE ? "yes":"no");
  fluid_settings_setstr(settings, "synth.chorus.active" , Denemo.prefs.fluidsynth_chorus == TRUE ? "yes":"no");
  
  /* Create the synthesizer. */
  synth = new_fluid_synth(settings);
  if (!synth){
    g_warning("\nFailed to create the settings\n");
    fluidsynth_shutdown();
    return 1;
  }

  /* Create the audio driver. */
  adriver = new_fluid_audio_driver(settings, synth);
  if (!adriver){
    g_warning("\nFailed to create the audio driver\n");
    fluidsynth_shutdown();
    return 1;
  }
  
  /* Load a SoundFont*/
  sfont_id = fluid_synth_sfload(synth, Denemo.prefs.fluidsynth_soundfont->str, 0);
  if (sfont_id == -1){
    g_warning("\nFailed to load the soundfont\n");
    fluidsynth_shutdown();
    return 1;
  }
  
  /* Select bank 0 and preset 0 in the SoundFont we just loaded on
  channel 0 */
  fluid_synth_program_select(synth, 0, sfont_id, 0, 0);

   return 0;
}                                                                                                                              
                    
void fluidsynth_shutdown()
{
  /* Clean up */
  g_debug("\nStopping FLUIDSYNTH\n");
  if (adriver)
    delete_fluid_audio_driver(adriver);
  if (synth)
    delete_fluid_synth(synth);
  if (settings)
    delete_fluid_settings(settings); 
  adriver = synth = settings = NULL;
}

void
fluidsynth_start_restart (void){
  if (synth){
    fluidsynth_shutdown();
    fluidsynth_init();
  }
  else {
    fluidsynth_init();
  }
}

static gboolean timer_callback(gpointer notenum){
  gint key = (gint) notenum;
  if (synth)
    fluid_synth_noteoff(synth, 0, key);
  return FALSE;
}

void fluid_playpitch(int key, int duration)
{
  /* Play a note */
  if (synth){
    fluid_synth_noteon(synth, 0, key, 80);
    g_timeout_add(duration, timer_callback, (gpointer) key); 
  }
}

void fluid_output_midi_event(unsigned char *buffer)
{
  if (synth){
    if ((buffer[0] & SYS_EXCLUSIVE_MESSAGE1) == NOTE_ON)
      fluid_synth_noteon(synth, 0, buffer[1], 80);
    if ((buffer[0] & SYS_EXCLUSIVE_MESSAGE1) == NOTE_OFF) 
      fluid_synth_noteoff(synth, 0, buffer[1]);
  }
}

gchar * fluidsynth_get_default_audio_driver()
{
  return fluid_settings_getstr_default(settings, "audio.driver");
}
#endif // _HAVE_FLUIDSYNTH_

