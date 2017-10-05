#include <alsa/asoundlib.h>

#include "init.h"
#include "sound.h"

#define VOICES ( 1 << 4 )
#define SAMPLE_RATE 44100
#define BUFFER ( 1 << 10 )

// NOTE STRUCT
typedef struct {
 float phase;
 float sample[ BUFFER ];
 float freq;
 float energy;
 int hold :1;
} note_t;

// THE BOOK OF NOTES
note_t * tab[ VOICES ] = { NULL };

// SMALLEST PERIOD
const float step = 1 / (float) SAMPLE_RATE;

// MAIN OUTPUT BUFFER
float bufferOut[ BUFFER ];

// GENERATORS
float phaseToSaw( float p ) { return 2 * p - 1; }
float phaseToSqu( float p ) { return p < .5 ? -1 : 1; }
float phaseToTri( float p ) { return p < .5 ? 4 * p - 1 : -4 * p + 3; }
float phaseToSin( float p ) { return sinf( 2 * M_PI * p ); }

// FREE NOTE
void freeNote( int id ) {
  // THROW ERROR IF NO ADRESS
  assert( tab[ id ] != NULL );
  // UNALLOCATE MEMORY
  free( tab[ id ] );
  // CLEAN THE BOOK
  tab[ id ] = NULL;
}

// SOUND SERVER
int soundServer( ) {

 // ALSA INIT
 snd_pcm_t * handle;
 int err = snd_pcm_open( &handle, "default", SND_PCM_STREAM_PLAYBACK, 0 );
 if ( err < 0 ) return EXIT_FAILURE;
 snd_pcm_set_params( handle, SND_PCM_FORMAT_FLOAT,
	SND_PCM_ACCESS_RW_INTERLEAVED, 1, SAMPLE_RATE, 0, 1 << 16 );

 // LOOP
 while ( !params.exit )
 {
  // MUTEX LOCK
  pthread_mutex_lock ( &mutex );

  // CLEAN MAIN BUFFER
  for ( int bf = 0; bf < BUFFER; bf++ ) bufferOut[ bf ] = 0.0;

  // FOR EACH NOTE
  for ( int id = 0; id < VOICES; id++ ) {
   if ( tab[ id ] != NULL )
   {
    note_t * n = tab[ id ];

    // FOR EACH SAMPLE
    for ( int bf = 0; bf < BUFFER; bf++ ) {

     // ENERGY UPDATE
     if ( !n->hold ) {
      if ( n->energy > 0.0 ) {
       n->energy -= n->energy/ ( (float) ( 1 << 12 ) );
      }
     }

     // PHASE UPDATE
     n->phase += n->freq * step;
     while ( n->phase >= 1 ) n->phase--;

     // WAVE GENERATION
     n->sample[ bf ] = n->energy * phaseToSin( n->phase );

     // MIX AND STORE INTO MAIN BUFFER
     bufferOut[ bf ] += n->sample[ bf ] / (float) VOICES;
    }

    // UNALLOCATE NOTES WITHOUT ENERGY
    if ( n->energy <= 0.05 && n->hold == 0 ) freeNote( id );
   }
  }

  // MUTEX UNLOCK
  pthread_mutex_unlock ( &mutex );

  // WRITE !
  snd_pcm_writei( handle, bufferOut, BUFFER );
 }
 // UNALLOCATE MEMORY OF LAST NOTES
 for ( int id = 0; id < VOICES; id++ ) {
  if ( tab[ id ] != NULL ) freeNote( id );
 }

 // EXIT ALSA
 snd_pcm_drain( handle );
 snd_pcm_close( handle );

 return EXIT_SUCCESS;
}

// NEW NOTE
int newNote( float freq )
{
 // SEARCH FOR EMPTY SPACE
 for ( int id = 0; id < VOICES; id++ ) {
  if ( tab[ id ] == NULL ) {

   // MEMORY REQUEST
   note_t * n = malloc( sizeof( note_t ) );

   // NOTE CONSTRUCTION
   n->freq = 	freq;
   n->hold = 	1;
   n->phase = 	0.0;
   n->energy = 	0.0;

   // STORE ADRESS
   tab[ id ] = n;
   return id;
  }
 }
 return -1;
}

// DROP NOTE
void dropNote( int id ) {
 assert( tab[ id ] != NULL );
 tab[ id ]->hold = 0;
}

// CONVERT SEMITONE -> FREQ
float semitoneToFreq( float s ) {
 return pow( 2, ( s - 69 ) / 12. ) * 440.0;
}

// CONVERT FREQ -> SEMITONE
float freqToSemitone( float f ) {
 return 12 * log2( f / 440.0 ) + 69;
}

// MODULO 12 (FOR FLOAT)
float mod12( float s ) {
 while( s >= 12 ) s -= 12;
 while( s < 0 ) s += 12;
 return s;
}

// SET ENERGY
void setEnergy( int id, float energy ) {
 if ( tab[ id ] != NULL ) tab[ id ]->energy = energy;
}

// GET FREQ
float getFreq( int id ) {
 if ( tab[ id ] != NULL ) return tab[ id ]->freq;
 else return -1;
}

// GET ENERGY
float getEnergy( int id ) {
 if ( tab[ id ] != NULL ) return tab[ id ]->energy;
 else return -1;
}

// GET MAX NOTES
int getMaxNotes( ) {
 return VOICES;
}

// CHECK IF ID IS AVAILABLE FOR NEW NOTE
int empty_id( int id ) {
 return tab[ id ] == NULL;
}
