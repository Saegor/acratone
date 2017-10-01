#include "init.h"

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

params_t params = {
.exit = 0,
.debug = 0 };

int main( ) {

 pthread_t sound_thread_id;
 pthread_t screen_thread_id;
 pthread_t midi_thread_id;
 pthread_t joystick_thread_id;

 pthread_create( &sound_thread_id, NULL, (void *) soundServer, NULL );
 pthread_create( &screen_thread_id, NULL, (void *) screenServer, NULL );
 pthread_create( &midi_thread_id, NULL, (void *) midiServer, NULL );
 pthread_create( &joystick_thread_id, NULL, (void *) joystickServer, NULL );

 pthread_join( screen_thread_id, NULL );
 pthread_join( sound_thread_id, NULL );
 pthread_join( midi_thread_id, NULL );
 pthread_join( joystick_thread_id, NULL );

 return EXIT_SUCCESS;
}
