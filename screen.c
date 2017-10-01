#include <ncurses.h>

#include "init.h"
#include "screen.h"
#include "sound.h"

#define GETCH_TIMEOUT 80

void setColor( int pair, int bold ) {
 attrset( COLOR_PAIR( pair ) | bold * A_BOLD );
}

// PICK A NOTE
void pick( int st ) {
 int id = newNote( semitoneToFreq( st ) );
 if ( id != -1 ) {
  setEnergy( id, 1.0 );
  dropNote( id );
 }
}

// PRINT TITLE
void printTitle( ) {
 setColor( 5, 1 );
 mvaddstr( 2, 4, "Acratone " );
 setColor( 5, 0 );
 addstr( "(0.17.10)" );
 setColor( 3, 0 );
 mvaddstr( 3, 4, "by " );
 setColor( 3, 1 );
 addstr( "Saegor" );
}

// PRINT LIST
void printList( ) {
 float f;
 int st;
 int max = getMaxNotes( );

 for ( int id = 0; id < max; id++ ) {
  f = getFreq( id );
  if ( f > 0 ) {
   setColor( 2, 0 );
   if ( getEnergy( id ) > 0.5) setColor( 2, 1 );
  }
  else setColor( 8, 0 );
  move( LINES/2 + id - max/2, 4 );
  st = (int) mod12( freqToSemitone( f ) + 0.5 );
  printw( "#%X: %X", id, st );
 }
}

// PRINT MATRIX
void printMatrix( ) {
 int f = 0;
 int st, st2;

 // FOR EACH CELL
 for ( int y = LINES/2 - 3; y < LINES/2 + 3; y++ ) {
  for ( int x = COLS/3 - 12; x < COLS/3 + 12; x = x + 2 ) {

   // X = FIFTHS, Y = THIRDS
   st = (y * 8 + x * 7/2) % 12;

   // DEFAULT GRID
   setColor( 8, 0 );
   for ( int i = 0; i < 7; i++ ) {
    if ( ( ( 5 + 7 * i ) % 12 ) == st ) setColor( 8, 1 );
   }

   // COLOR PLAYED NOTES
   for ( int id = 0; id < getMaxNotes( ); id++ ) {
    f = getFreq( id );
    if ( f > 0 ) {
     st2 = mod12( freqToSemitone( f ) + 0.5 );
     if ( st == st2 ) {
      setColor( 7, 0 );
      if ( getEnergy( id ) > 0.5) setColor( 7, 1 );
     }
    }
   }

   // MOVE & PRINT
   move( y, x );
   printw( "%X", st );
  }
 }
}

// STORE CIRCLE COORDS
int circ[ 2 ][ 12 ] = {
{ -2, -2, -1,  0,  1,  2,  2,  2,  1,  0, -1, -2 },
{  0,  2,  3,  4,  3,  2,  0, -2, -3, -4, -3, -2 } };

// PRINT CIRCLE
void printCircle( ) {
 float f;
 int st;
 for ( int i = 0; i < 12; i++ ) {
  move( LINES / 2 + circ[0][i], 2 * COLS / 3 + circ[1][i] );
  setColor( 8, 1 );
  printw( "%X", i );
 }
 for ( int id = 0; id < getMaxNotes( ); id++ ) {
  f = getFreq( id );
  if ( f > 0 ) {
   st = mod12( freqToSemitone( f ) + 0.5 );
   setColor( 6, 0 );
   if ( getEnergy( id ) > 0.5 ) setColor( 6, 1 );
   move( LINES / 2 + circ[ 0 ][ st ], 2 * COLS / 3 + circ[ 1 ][ st ] );
   printw( "%X", st );
  }
 }
}

//MAIN SCREENSERVER
int screenServer( ) {

 // NCURSES INIT
 initscr( );
 noecho( );
 cbreak( );
 curs_set( 0 );
 keypad( stdscr, TRUE );
 start_color( );
 for ( short i = 0; i < 8; i++ ) {
  init_pair( (i ? i : 8), i, 0 );
 }
 timeout( GETCH_TIMEOUT );

 // LOOP
 while( !params.exit ) {

  // MUTEX LOCK
  pthread_mutex_lock( &mutex );

  // PRINT ALL THE SHIT
  if ( params.debug ) {
   printList( );
  }
  printTitle( );
  printCircle( );
  printMatrix( );

  // UPDATE SCREEN BUFFER
  refresh( );

  // MUTEX UNLOCK
  pthread_mutex_unlock( &mutex );

  // WAIT FOR KEYBOARD
  char key = getch( );
  erase( );

  // MUTEX LOCK
  pthread_mutex_lock( &mutex );

  switch( key ) {

   // QUIT
   case 'q': params.exit = TRUE; break;
   case '&': params.debug ^= 1; break;

   // TRY A NOTE
   case 'w': pick( 60 ); break;
   case 's': pick( 61 ); break;
   case 'x': pick( 62 ); break;
   case 'd': pick( 63 ); break;
   case 'c': pick( 64 ); break;
   case 'v': pick( 65 ); break;
   case 'g': pick( 66 ); break;
   case 'b': pick( 67 ); break;
   case 'h': pick( 68 ); break;
   case 'n': pick( 69 ); break;
   case 'j': pick( 70 ); break;
   case ',': pick( 71 ); break;
   case ';': pick( 72 ); break;
   case 'l': pick( 73 ); break;
   case ':': pick( 74 ); break;
   case 'm': pick( 75 ); break;
   case '!': pick( 76 ); break;

   case 'a': pick( 72 ); break;
   case 'z': pick( 74 ); break;
   case 'e': pick( 76 ); break;
   case 'r': pick( 77 ); break;
   case 't': pick( 79 ); break;
   case '-': pick( 80 ); break;
   case 'y': pick( 81 ); break;
   case 'u': pick( 83 ); break;
   case 'i': pick( 84 ); break;
   case 'o': pick( 86 ); break;
   case 'p': pick( 88 ); break;
  }

  // MUTEX UNLOCK
  pthread_mutex_unlock( &mutex );

 }
 // NCURSES EXIT
 endwin( );
 return EXIT_SUCCESS;
}
