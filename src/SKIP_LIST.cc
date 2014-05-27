
#include <stdlib.h>
#include <string.h>

#include "SKIP_LIST.h"

namespace SKIP_LIST {

  struct HANDLE {
    int (*compare) ( void const * node1, void const * node2 );
    int max_level;
    int current_level;
    double prob;
    unsigned short rand_state[3];
    size_t num_items;
    struct NODE * header;
  };

  struct HANDLE * init( int (*compare) ( void const * node1, void const * node2 ), int max_level, double prob ) {
    struct HANDLE *h = (struct HANDLE *) calloc( 1, sizeof(struct HANDLE) );
    h->compare = compare;
    if(max_level < 1) max_level = 32;
    if(prob < 0 || prob >= 1) prob = 0.25;
    h->max_level = max_level;
    h->current_level = 0;
    h->prob = prob;
    h->header = (struct NODE *) calloc( 1, sizeof(struct NODE) + sizeof(struct NODE*) * h->max_level );
    h->header->data = NULL;
    h->header->num_forward = h->max_level;
    h->num_items = 0;
    return h;
  }

  void destroy( struct HANDLE *& h ) {
    free(h);
    h = NULL;
  }

  int generate_level( HANDLE * h ) {
    int level = 0;
    double r;
    do {
      r = erand48(h->rand_state);
      ++level;
    } while(r < h->prob && level < h->max_level);
    return level;
  }

  void insert( struct HANDLE * h, void * item, size_t item_len ) {
    struct NODE ** update = (struct NODE **) calloc( 1, sizeof(struct NODE*) * h->max_level );
    struct NODE * n = h->header;
    struct NODE ** forward = (struct NODE **) (n+1);
    for( int i=h->current_level; i>0; --i ) {
      while( n->num_forward >= i && forward[i-1] && h->compare((forward[i-1])->data, item) < 0 ) {
	n = forward[i-1];
	forward = (struct NODE **) (n+1);
      }
      update[i-1] = n;
    }
    n = *(struct NODE **) (n+1);
    if( n && h->compare(n->data, item) == 0 )
      return;
    else {
      int new_level = generate_level( h );
      if( new_level > h->current_level ) {
	for( int i=h->current_level; i<new_level; ++i ) {
	  update[i] = h->header;
	}
	h->current_level = new_level;
      }
      NODE * new_node = (struct NODE *) calloc( 1, sizeof(struct NODE) + item_len + sizeof(struct NODE*) * new_level );
      new_node->data = ((char *) (new_node+1)) + sizeof(struct NODE *) * new_level;
      memcpy( new_node->data, item, item_len );
      new_node->data_len = item_len;
      new_node->num_forward = new_level;
      struct NODE ** nforward = (struct NODE **) (new_node+1);
      for( int i=0; i<new_level; ++i ) {
	struct NODE ** forward = (struct NODE **) (update[i]+1);
	nforward[i] = forward[i];
	forward[i] = new_node;
      }
      h->num_items += 1;
    }
    free( update );
  }

  void remove( struct HANDLE * h, void * item ) {
    struct NODE ** update = (struct NODE **) calloc( 1, sizeof(struct NODE*) * h->max_level );
    struct NODE * n = h->header;
    struct NODE ** forward = (struct NODE **) (n+1);
    for( int i=h->current_level; i>0; --i ) {
      while( n->num_forward >= i && forward[i-1] && h->compare((forward[i-1])->data, item) < 0 ) {
	n = forward[i-1];
	forward = (struct NODE **) (n+1);
      }
      update[i-1] = n;
    }
    n = *(struct NODE **) (n+1);
    struct NODE ** nforward = (struct NODE **) (n+1);
    if( h->compare(n->data, item) == 0 ) {
      for( int i=0; i<h->current_level; ++i ) {
	struct NODE ** forward = (struct NODE **) (update[i]+1);
	if( forward[i] != n )
	  break;
	forward[i] = nforward[i];
      }
      free( n );
      struct NODE ** hforward = (struct NODE **) (h->header+1);
      while( h->current_level > 1 && hforward[h->current_level] == NULL )
	h->current_level--;
      h->num_items -= 1;
    }
    free( update );
  }

  void * find( struct HANDLE * h, void const * item ) {
    struct NODE * n = h->header;
    struct NODE ** forward = (struct NODE **) (n+1);
    for( int i=h->current_level; i>0; --i ) {
      while( n->num_forward >= i && forward[i-1] && h->compare((forward[i-1])->data, item) < 0 ) {
	n = forward[i-1];
	forward = (struct NODE **) (n+1);
      }
    }
    n = *(struct NODE **) (n+1);
    return (n && h->compare(n->data, item) == 0) ? n->data : NULL;
  }

  struct NODE * first( struct HANDLE * h ) {
    struct NODE * n = h->header;
    struct NODE ** forward = (struct NODE **) (n+1);
    if( forward[0] ) {
      n = forward[0];
      return n;
    }
    return NULL;
  }

  struct NODE * next ( struct NODE * n ) {
    if( !n ) return NULL;
    struct NODE ** forward = (struct NODE **) (n+1);
    if( forward[0] ) {
      n = forward[0];
      return n;
    }
    return NULL;
  }

  size_t size( struct HANDLE * h ) {
    return h->num_items;
  }
};
