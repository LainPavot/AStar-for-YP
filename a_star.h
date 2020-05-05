
#ifndef _A_STAR_H
#define _A_STAR_H


#include <math.h>
#include <Python.h>


#include "chained_list.h"


typedef struct s_position t_position ;
typedef t_position* p_position ;

typedef struct s_hitbox t_hitbox ;
typedef t_hitbox* p_hitbox ;

typedef struct s_context t_context ;
typedef t_context* p_context ;


struct s_position {
  p_context context ;
  p_position neighboors[8] ;
  p_position previous ;
  int x ;
  int y ;
  int cost ;
  float heuristic ;
  float end_proximity ;
  char direction ;
  char step_counter ;
} ;

struct s_hitbox {
  Py_ssize_t size ;
  t_position *vertex ;
} ;

struct s_context {
  p_chained_list opened_nodes ;
  p_chained_list closed_nodes ;
  p_position real_start ;
  PyObject *result ;
  t_hitbox *hitbox_list ;
  t_position start ;
  t_position end ;
  Py_ssize_t hitbox_nb ;
  unsigned char step_x ;
  unsigned char step_y ;
} ;


#define IGNORE_UNUSED_VARIABLE(x) if (x) {}


/**
 * Only for tests purpose.
 * This code must be removed or commented for release
 */
void shortcut_static_find (p_context context) ;
void shortcut_free_hitbox_list (p_context context) ;


PyMODINIT_FUNC inita_star (void) ;

#endif  /* _A_STAR_H */
