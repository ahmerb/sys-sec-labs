#include <stdbool.h>
#include  <stdint.h>
#include   <stdio.h>
#include  <stdlib.h>

#include  <string.h>
#include  <signal.h>
#include  <unistd.h>
#include   <fcntl.h>

#define BUFFER_SIZE ( 80 )

pid_t pid        = 0;    // process ID (of either parent or child) from fork

int   target_raw[ 2 ];   // unbuffered communication: attacker -> attack target
int   attack_raw[ 2 ];   // unbuffered communication: attack target -> attacker

FILE* target_out = NULL; // buffered attack target input  stream
FILE* target_in  = NULL; // buffered attack target output stream

void interact( ) {
  // Receive line `The variable secret's address is 0x________ (on stack)`
  if( 1 != fscanf( target_out, "%*s" ) ) {
    abort();
  }
  printf("hello, world\n");
  // Receive line `The variable secret's value is 0x________ (on heap)`
  if( 1 != fscanf( target_out, "%*s" ) ) {
    abort();
  }
  // Receive line `secret[0]'s address is 0x________ (on heap)`
  if( 1 != fscanf( target_out, "%*s" ) ) {
    abort();
  }

  unsigned int secret1addr;
  // Receive line `secret[1]'s address is 0x________ (on heap)` and extract the address
  if( 1 != fscanf( target_out, "%*s%x%*s", &secret1addr ) ) {
    abort();
  }

  // Receive line `Please enter a string.`
  if( 1 != fscanf( target_out, "%*s" ) ) {
    abort();
  }

  // Send secret1addr to   attack target.
  fprintf( target_in, "%x\n", secret1addr );  fflush( target_in );
}

void attack() {
  // interact with the attack target.

  interact( );

  // Print all of the inputs and outputs.

  // printf( "G = %s\n", G );
  // printf( "t = %d\n", t );
  // printf( "r = %d\n", r );
}

void cleanup( int s ){
  // Close the   buffered communication handles.

  fclose( target_in  );
  fclose( target_out );

  // Close the unbuffered communication handles.

  close( target_raw[ 0 ] ); 
  close( target_raw[ 1 ] ); 
  close( attack_raw[ 0 ] ); 
  close( attack_raw[ 1 ] ); 

  // Forcibly terminate the attack target process.

  if( pid > 0 ) {
    kill( pid, SIGKILL );
  }

  // Forcibly terminate the attacker      process.

  exit( 1 ); 
}
 
int main( int argc, char* argv[] ) {
  // Ensure we clean-up correctly if Control-C (or similar) is signalled.

  signal( SIGINT, &cleanup );

  // Create pipes to/from attack target

  if( pipe( target_raw ) == -1 ) {
    abort();
  }
  if( pipe( attack_raw ) == -1 ) {
    abort();
  }

  pid = fork();

  if     ( pid >  0 ) { // parent
    // Construct handles to attack target standard input and output.

    if( ( target_out = fdopen( attack_raw[ 0 ], "r" ) ) == NULL ) {
      abort();
    }
    if( ( target_in  = fdopen( target_raw[ 1 ], "w" ) ) == NULL ) {
      abort();
    }

    // Execute a function representing the attacker.

    attack();
  }
  else if( pid == 0 ) { // child 
    // (Re)connect standard input and output to pipes.

    close( STDOUT_FILENO );
    if( dup2( attack_raw[ 1 ], STDOUT_FILENO ) == -1 ) {
      abort();
    }
    close(  STDIN_FILENO );
    if( dup2( target_raw[ 0 ],  STDIN_FILENO ) == -1 ) {
      abort();
    }

    // Produce a sub-process representing the attack target.

    execl( argv[ 1 ], argv[ 0 ], NULL );
  }
  else if( pid <  0 ) { // error
    // The fork failed; reason is stored in errno, but we'll just abort.

    abort();
  }

  // Clean up any resources we've hung on to.

  cleanup( SIGINT );

  return 0;
}
