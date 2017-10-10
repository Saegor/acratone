#include <alsa/asoundlib.h>

#include "init.h"
#include "midi.h"
#include "sound.h"

#define CARD 1
#define DEVICE 0
#define SUBDEVICE 0

int midiServer( )
{

 // OPEN MIDI INPUT
 char name[ 16 ];
 sprintf( name, "hw:%d,%d,%d", CARD, DEVICE, SUBDEVICE );
 snd_rawmidi_t * handle_in = NULL;
 int err = snd_rawmidi_open( &handle_in, NULL,
	name, SND_RAWMIDI_NONBLOCK );
 if ( err ) return EXIT_FAILURE;

 // STORE MIDI INPUT MESSAGE
 unsigned char byte = 0;

 // STORE ID OF PLAYED NOTES
 int midiTab[ 128 ];
 for ( int i = 0; i < 128; i++ ) {
  midiTab[ i ] = -1;
 }

 // LOOP
 while ( !params.exit ) {

  // STABILIZE
  usleep( 1000 );

  // READ
  snd_rawmidi_read( handle_in, &byte, 1 );

  // MUTEX LOCK
  pthread_mutex_lock( &mutex );

  // IF COMMAND BYTE
  if ( byte >= 0x80 ) {

   // NOTE ON
   if ( byte == 0x90 ) {
    snd_rawmidi_read( handle_in, &byte, 1 );
    int id = newNote( semitoneToFreq( byte ) );
    midiTab[ byte ] = id;

    // VELOCITY
    if ( id != -1 ) {
     snd_rawmidi_read( handle_in, &byte, 1 );
     setEnergy( id, ( 64 + byte ) / 192.0 );
    }
   }

   // NOTE OFF
   else if ( byte == 0x80 ) {
    snd_rawmidi_read( handle_in, &byte, 1 );
    if ( midiTab[ byte ] != -1 ) {
     dropNote( midiTab[ byte ] );
     midiTab[ byte ] = -1;
    }
   }
  }

  // MUTEX UNLOCK
  pthread_mutex_unlock( &mutex );
 }
 // MIDI EXIT
 snd_rawmidi_drain( handle_in );
 snd_rawmidi_close( handle_in );

 return EXIT_SUCCESS;
}
