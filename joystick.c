#include <linux/joystick.h>

#include "init.h"
#include "joystick.h"
#include "sound.h"

#define JOY_DEV "/dev/input/js0"
#define BUTTONS 6

int joystickServer( )
{
 // JOYSTICK INIT
 int joy_fd = open( JOY_DEV, O_RDONLY );
 if ( joy_fd == -1 ) return EXIT_FAILURE;
 int num_of_axis, num_of_buttons;
 ioctl( joy_fd, JSIOCGAXES, &num_of_axis );
 ioctl( joy_fd, JSIOCGBUTTONS, &num_of_buttons );
 int * axis = ( int * ) calloc( num_of_axis, sizeof( int ) );
 char * button = ( char * ) calloc( num_of_buttons, sizeof( char ) );
 char * b_edge = ( char * ) calloc( num_of_buttons, sizeof( char ) );
 struct js_event js;
 int note[ BUTTONS ] = {
	48 + 4, 	// A
	48 + 3, 	// B
	48 + 9, 	// X
	48 + 2, 	// Y
	48 + 8, 	// L1
	48 + 5 };	// R1

 // STORE NOTES PLAYED
 int joyTab[ 16 ];
 for ( int i = 0; i < 16; i++ ) {
  joyTab[ i ] = -1;
 }

 // NON-BLOCK
 fcntl( joy_fd, F_SETFL, O_NONBLOCK );

 // LOOP
 while( !params.exit ) {

  // STABILIZE
  usleep( 10000 );

  // READ
  read( joy_fd, &js, sizeof( struct js_event ) );

  // TYPE SWITCH
  switch ( js.type & ~JS_EVENT_INIT ) {

   // AXIS
   case JS_EVENT_AXIS:
    axis[ js.number ] = js.value;
    break;

   // BUTTON
   case JS_EVENT_BUTTON:
    button[ js.number ] = js.value;
    break;
  }

  // MUTEX LOCK
  pthread_mutex_lock( &mutex );

  // FOR EACH BUTTON
  for ( int i = 0; i < BUTTONS; i++ ) {

   // NOTE ON
   if( button[ i ] & ~b_edge[ i ] ) {
    int id = newNote( semitoneToFreq( note[ i ] ) );
    if ( id != -1 ) {
     joyTab[ i ] = id;
     setEnergy( id, 1.0 );
     b_edge[ i ] = 1;
    }
   }

   // NOTE OFF
   else if( ~button[ i ] & b_edge[ i ] ) {
    if ( joyTab[ i ] != -1 ) {
     dropNote( joyTab[ i ] );
     joyTab[ i ] = -1;
     b_edge[ i ] = 0;
    }
   }
  }

  // MUTEX UNLOCK
  pthread_mutex_unlock( &mutex );
 }

 // FREE ARRAYS
 free( button );
 free( axis );
 free( b_edge );

 // JOYSTICK EXIT
 close( joy_fd );
 return EXIT_SUCCESS;
}
