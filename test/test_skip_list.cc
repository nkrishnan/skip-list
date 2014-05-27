
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include "SKIP_LIST.h"

int compare_int( const void * n1, const void * n2 ) {
  int v1 = *(int *) n1;
  int v2 = *(int *) n2;
  if( v1 == v2 ) return 0;
  return (v1 < v2) ? -1 : 1;
}

int main( int argc, char * argv[] ) {

  SKIP_LIST::HANDLE * sl = SKIP_LIST::init(compare_int);
  printf("Inserting values in interval [%d, %d)\n", 0, 1000000);
  for( int i=0; i<1000000; ++i )
    SKIP_LIST::insert( sl, &i, sizeof(int) );

  int val = argc > 1 ? atoi(argv[1]) : 50;
  int * val_found = (int *) SKIP_LIST::find( sl, &val );
  printf( "requested: %d found: %d\n", val, (val_found) ? *val_found : INT_MAX );

  struct SKIP_LIST::NODE * n = SKIP_LIST::first( sl );
  int ctr = 20;
  while(n && ctr) {
    printf( "node: %d\n", *(int *) (n->data) );
    n = SKIP_LIST::next(n);
    --ctr;
  }

  printf( "size: %lu\n", SKIP_LIST::size(sl) );
  printf("Removing values in interval [%d, %d)\n", 0, 100);
  for( int i=0; i<100; ++i )
    SKIP_LIST::remove( sl, &i );

  printf( "size: %lu\n", SKIP_LIST::size(sl) );
}
