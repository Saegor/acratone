#ifndef INIT_H
#define INIT_H

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <fcntl.h>
#include <math.h>

pthread_mutex_t mutex;

typedef struct {
 char exit :1;
 char debug : 1;
} params_t;

params_t params;

int soundServer( );
int screenServer( );
int midiServer( );
int joystickServer( );

#endif
