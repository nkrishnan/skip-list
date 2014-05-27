
#ifndef _SKIP_LIST_H
#define _SKIP_LIST_H

namespace SKIP_LIST {

  struct NODE {
    void * data;
    int data_len;
    int num_forward;
  };

  struct HANDLE;

  struct HANDLE * init( int (*compare) ( void const * node1, void const * node2 ), int max_level = 32, double prob = 0.25 );

  void destroy( struct HANDLE *& h );

  void insert( struct HANDLE * h, void * item, size_t item_len );

  void remove( struct HANDLE * h, void * item );

  void * find( struct HANDLE * h, void const * item );

  struct NODE * first( struct HANDLE * h );

  struct NODE * next ( struct NODE * n );

  size_t size( struct HANDLE * h );
};

#endif // _SKIP_LIST_H
