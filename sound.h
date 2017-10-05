#ifndef SOUND_H
#define SOUND_H

int newNote( float freq );
void dropNote( int id );

int empty_id( int id );
void setEnergy( int id, float energy );
float getFreq( int id );
float getEnergy( int id );
int getMaxNotes( );

float mod12( float s );
float semitoneToFreq( float s );
float freqToSemitone( float f );

#endif
