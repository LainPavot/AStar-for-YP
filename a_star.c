

#include "a_star.h"

#define CONSECUTIVE_STEPS 5

#define ANY         -1
#define UP_LEFT     0
#define UP          1
#define UP_RIGHT    2
#define LEFT        3
#define RIGHT       4
#define DOWN_LEFT   5
#define DOWN        6
#define DOWN_RIGHT  7

/**
 * The extendes api slows down the algorihm:
 *  every time a position is inserted into
 *  opened nodes or in closed nodes, the
 *  corresponding variables are set. These
 *  variables are accessible via the extended
 *  API's functions get_last_closed and
 *  get_last_opened. The has_finished method
 *  allow to know if the search has finished.
 *  The extended API should only be activated
 *  for debug, to be used in Python code.
 */
#define EXTENDED_API 0


/**
 * Show the remaining number of position instances and
 * the amount of memory not freed, used for hitbox.
 * Both should be 0.
 */
#define DEBUG_COUNT_MALLOC 0


#if EXTENDED_API
static PyObject *has_finished (PyObject *self, PyObject *args) ;
static PyObject *get_last_closed (PyObject *self, PyObject *args) ;
static PyObject *get_last_opened (PyObject *self, PyObject *args) ;
#endif


#if DEBUG_COUNT_MALLOC
static int _position_count ;
static int _hitbox_malloc_total_size ;
#endif


static PyObject *abort_a_star (PyObject *self, PyObject *args) ;
static float position_distance (p_position pos1, p_position pos2) ;
static p_position new_position (int x, int y, int cost, p_context context, p_position previous) ;
static char guess_direction (int dx, int dy) ;
static char cmp_position (p_position pos1, p_position pos2) ;
static void free_position (p_position position) ;
static PyObject *a_star (PyObject *self, PyObject *args) ;
static char a_star_extract_hitbox_list (PyObject *hitbox_list, p_context context) ;
static char a_star_extract_hitbox (PyObject *python_hitbox, p_hitbox c_hitbox) ;
static void initialize_context_data (p_context context) ;
static void a_star_find (p_context context) ;
static char position_in_hitbox (p_context context, p_position position) ;
static char point_in_ellipse (int x, int y, int xp, int yp, int d, int D) ;
static char point_inside_polygon (p_hitbox hitbox, p_position position) ;
static void build_all_neighboors (p_position position) ;
static void build_up_left_position (p_position position) ;
static void build_up_position (p_position position) ;
static void build_up_right_position (p_position position) ;
static void build_left_position (p_position position) ;
static void build_right_position (p_position position) ;
static void build_down_left_position (p_position position) ;
static void build_down_position (p_position position) ;
static void build_down_right_position (p_position position) ;
static void build_position_neighboorhood (p_position position) ;
static void add_to_opened_nodes (p_chained_list opened_nodes, p_position position);
static void add_to_closed_nodes (p_chained_list closed_nodes, p_position position) ;
static char in_opened_nodes  (p_chained_list opened_nodes, p_position position) ;
static char in_closed_nodes (p_chained_list closed_nodes, p_position position) ;
static float coef_dir (p_position p1, p_position p2) ;
static void set_good_result (p_context context) ;
static void set_bad_result (p_context context) ;
static void free_hitbox_list (p_context context) ;
static void free_hitbox (p_hitbox hitbox) ;
static void free_context (p_context context) ;

static PyObject *AStarError ;
static char _abort_a_star ;

#if EXTENDED_API
static char state ;
static int last_opened[2] ;
static int last_closed[2] ;
#endif

static PyMethodDef AStarMethods[] = {
  {
    "a_star", 
    a_star, 
    METH_VARARGS, 
    "Search for the path giving begin and end point."
  }, {
    "abort", 
    abort_a_star, 
    METH_VARARGS, 
    "Stop the A* search algorithm."
  }, {
#if EXTENDED_API
    "has_finished", 
    has_finished, 
    METH_VARARGS, 
    "Returns the state of the algorithm: \n"
    "\t-1 not begun, 0 not finished, 1 finished."
  }, {
    "get_last_closed", 
    get_last_closed, 
    METH_VARARGS, 
    "Returns the last closed position."
  }, {
    "get_last_opened", 
    get_last_opened, 
    METH_VARARGS, 
    "Returns the last opened position."
  }, {
#endif
    NULL,
    NULL,
    0,
    NULL
  }
} ;

/**
 * Only for tests purpose.
 * This code must be removed or commented for release
 */
void shortcut_static_find (p_context context) {
  a_star_find (context) ;
  free_context (context) ;
}

void shortcut_free_hitbox_list (p_context context) {
  free_hitbox_list (context) ;
}

PyMODINIT_FUNC
inita_star (void) {
  PyObject *module ;

  module = Py_InitModule ("a_star", AStarMethods) ;
  if (module == NULL)
    return ;

  AStarError = PyErr_NewException ("a_star.error", NULL, NULL) ;
  Py_INCREF (AStarError) ;
  _abort_a_star = 0 ;

#if EXTENDED_API
  last_opened[0] = 0 ;
  last_opened[1] = 0 ;
  last_closed[0] = 0 ;
  last_closed[1] = 0 ;
  state = -1 ;
#endif

  PyModule_AddObject (module, "error", AStarError) ;
}


#if EXTENDED_API

static PyObject *has_finished (PyObject *self, PyObject *args) {
  return PyInt_FromLong (state) ;

  IGNORE_UNUSED_VARIABLE (self) ;
  IGNORE_UNUSED_VARIABLE (args) ;
}

static PyObject *get_last_closed (PyObject *self, PyObject *args) {
  return Py_BuildValue ("ii", last_closed[0], last_closed[1]);

  IGNORE_UNUSED_VARIABLE (self) ;
  IGNORE_UNUSED_VARIABLE (args) ;
}

static PyObject *get_last_opened (PyObject *self, PyObject *args) {
  return Py_BuildValue ("ii", last_opened[0], last_opened[1]);

  IGNORE_UNUSED_VARIABLE (self) ;
  IGNORE_UNUSED_VARIABLE (args) ;
}

#endif


static PyObject *abort_a_star (PyObject *self, PyObject *args) {
  if (_abort_a_star == -1)
    _abort_a_star = 1 ;
  Py_RETURN_NONE ;

  IGNORE_UNUSED_VARIABLE (self) ;
  IGNORE_UNUSED_VARIABLE (args) ;

}

static float position_distance (p_position pos1, p_position pos2) {
  if (pos1 == NULL || pos2 == NULL)
    return -1 ;
  return (float) sqrt (
    pow ((pos2->x - pos1->x), 2) +
    pow ((pos2->y - pos1->y), 2)
  ) ;
}

static p_position new_position (int x, int y, int cost, p_context context, p_position previous) {
  p_position position ;
  float distance ;

  position = calloc (1, sizeof (t_position)) ;
  if (position == NULL)
    return NULL ;

#if DEBUG_COUNT_MALLOC
  _position_count += 1 ;
#endif

  position->x = x ;
  position->y = y ;
  position->cost = cost ;
  distance = position_distance (&context->end, position) ;
  position->heuristic = position->cost + distance ;
  position->context = context ;
  if (previous == NULL || previous->step_counter == 0 ||
      distance < context->step_x+context->step_y ||
      fabs (position->x - context->end.x) < context->step_x ||
      fabs (position->y - context->end.y) < context->step_y
      ) {
    position->direction = ANY ;
    position->step_counter = CONSECUTIVE_STEPS ;
  } else {
    position->step_counter = previous->step_counter - 1 ;
    if (previous->direction == ANY) {
      position->direction = guess_direction (
        position->x - previous->x, 
        position->y - previous->y
      ) ;
    } else {
      position->direction = previous->direction ;
    }
  }
  return position ;
}

static char guess_direction (int dx, int dy) {
  if (dx < 0) {
    return (dy < 0 ? UP_LEFT : dy > 0 ? DOWN_LEFT : LEFT) ;
  }
  if (dx > 0) {
    return (dy < 0 ? UP_RIGHT : dy > 0 ? DOWN_RIGHT : RIGHT) ;
  }
  return (dy < 0 ? UP : DOWN) ;
}

static char cmp_position (p_position pos1, p_position pos2) {
  if (pos1 == NULL || pos2 == NULL)
    return pos1 != pos2 ;
  return fabs (pos1->x - pos2->x) > pos1->context->step_x \
      || fabs (pos1->y - pos2->y) > pos1->context->step_y ;
}

static void free_position (p_position position) {
  int index ;

  if (position != NULL) {
    for (index = 0 ; index < 8 ; index += 1) {
      free_position (position->neighboors[index]) ;
    }

    free (position) ;

#if DEBUG_COUNT_MALLOC
    _position_count -= 1 ;
#endif

  }
}

static PyObject *a_star (PyObject *self, PyObject *args) {
  PyObject *hitbox_list ;
  t_context context ;

#if DEBUG_COUNT_MALLOC
  _position_count = 0 ;
  _hitbox_malloc_total_size = 0 ;
  fprintf (stderr, "[DEBUG] Position number initialized to 0.\n") ;
  fprintf (stderr, "[DEBUG] Hitbox allocated size initialized to 0.\n") ;
#endif


  _abort_a_star = -1 ; // algo started !

#if EXTENDED_API
  state = -1 ;
#endif

  IGNORE_UNUSED_VARIABLE (self)

  bzero (&context, sizeof (t_context)) ;
  if (!PyArg_ParseTuple (
      args, "iiiibbO!",
      &context.start.x, &context.start.y,
      &context.end.x, &context.end.y,
      &context.step_x, &context.step_y,
      &PyList_Type, &hitbox_list))
    return NULL ;

  if (a_star_extract_hitbox_list (hitbox_list, &context) == -1) {
    return NULL ;
  }
  a_star_find (&context) ;
  free_hitbox_list (&context) ;
  free_context (&context) ;
  _abort_a_star = 0 ; // ended

#if DEBUG_COUNT_MALLOC
  fprintf (stderr, "[DEBUG] Remaining position number: %d\n", _position_count) ;
  fprintf (stderr, "[DEBUG] Remaining hitbox memory: %d\n", _hitbox_malloc_total_size) ;
#endif

  Py_DECREF (hitbox_list) ;

  return context.result ;
}

static char a_star_extract_hitbox_list (PyObject *hitbox_list, p_context context) {
  PyObject *hitbox_iterator ;
  PyObject *hitbox ;
  Py_ssize_t index ;

  context->hitbox_nb = PyObject_Length (hitbox_list) ;
  if (context->hitbox_nb == -1) {
    PyErr_SetString (AStarError, "Bad length for hitbox list.") ;
    return -1 ;
  }

  context->hitbox_list = calloc (context->hitbox_nb, sizeof (t_hitbox)) ;
  if (context->hitbox_list == NULL) {
    context->hitbox_nb = -1 ;
    PyErr_SetString (AStarError, "Could not allocate memory for hitbox list.") ;
    return -1 ;
  }

#if DEBUG_COUNT_MALLOC
  _hitbox_malloc_total_size += context->hitbox_nb * sizeof (t_hitbox) ;
#endif

  hitbox_iterator = PyObject_GetIter (hitbox_list) ;
  if (hitbox_iterator == NULL) {
    PyErr_SetString (AStarError, "Could not iterate over hitbox list.") ;
    return -1 ;
  }

  index = 0 ;
  while (1) {
    hitbox = PyIter_Next (hitbox_iterator) ;
    if (hitbox == NULL)
      break ;
    if (a_star_extract_hitbox (hitbox, &context->hitbox_list[index]) == -1) {
      Py_DECREF (hitbox) ;
      Py_DECREF (hitbox_iterator) ;
      return -1 ;
    }
    index += 1 ;
    Py_DECREF (hitbox) ;
  }
  Py_DECREF (hitbox_iterator) ;
  return 0 ;
}

static char a_star_extract_hitbox (PyObject *python_hitbox, p_hitbox c_hitbox) {
  PyObject *position_iterator ;
  PyObject *position ;
  Py_ssize_t index ;

  c_hitbox->size = PyObject_Length (python_hitbox) ;
  if (c_hitbox->size < 2) {
    PyErr_SetString (AStarError, "Bad length for position list.") ;
    return -1 ;
  }

  c_hitbox->vertex = calloc (c_hitbox->size, sizeof (t_position)) ;
  if (c_hitbox->vertex == NULL) {
    c_hitbox->size = -1 ;
    PyErr_SetString (AStarError, "Could not allocate memory for hitbox positions.") ;
    return -1 ;
  }

#if DEBUG_COUNT_MALLOC
  _hitbox_malloc_total_size += c_hitbox->size * sizeof (t_position) ;
#endif

  position_iterator = PyObject_GetIter (python_hitbox) ;
  if (position_iterator == NULL) {
    PyErr_SetString (AStarError, "Could not iterate over position list.") ;
    return -1 ;
  }

  index = 0 ;
  while (1) {
    position = PyIter_Next (position_iterator) ;
    if (position == NULL)
      break ;
    if (!PyArg_ParseTuple (
        position, "ii",
        &c_hitbox->vertex[index].x, &c_hitbox->vertex[index].y)) {
      Py_DECREF (position_iterator) ;
      Py_DECREF (position) ;
      return -1 ;
    }
    index += 1 ;
    Py_DECREF (position) ;
  }
  Py_DECREF (position_iterator) ;
  return 0 ;
}

static void initialize_context_data (p_context context) {
  context->opened_nodes = new_chained_list () ;
  context->closed_nodes = new_chained_list () ;
  context->result = PyList_New (0) ;
  context->real_start = new_position (context->start.x, context->start.y, 0, context, NULL) ;
  push_chained_list (context->opened_nodes, &context->real_start, sizeof (p_position)) ;
}

static void a_star_find (p_context context) {
  p_position curent ;
  int index ;
  PyThreadState *_save ;

  initialize_context_data (context) ;
  _save = PyEval_SaveThread () ;
  //Py_BEGIN_ALLOW_THREADS

#if EXTENDED_API
  state = 0 ;
  usleep(1000);
#endif

  while (
      length_chained_list (context->opened_nodes) !=  0 &&
      _abort_a_star != 1 // aborted
    ) {
    unshift_chained_list (context->opened_nodes, &curent) ;
    if (cmp_position (curent, &context->end) == 0) {
      context->end.previous = curent ;
      PyEval_RestoreThread (_save) ;
      //Py_END_ALLOW_THREADS
      set_good_result (context) ;
      return ;
    }
    if (position_in_hitbox (context, curent) == 0) {
      build_position_neighboorhood (curent) ;
      for (index = 0 ; index < 8 ; index += 1) {
        if (curent->neighboors[index] == NULL ||
            in_opened_nodes (context->opened_nodes, curent->neighboors[index]) ||
            in_closed_nodes (context->closed_nodes, curent->neighboors[index])) {
          continue ;
        } else {
          curent->neighboors[index]->previous = curent ;
          add_to_opened_nodes (
            context->opened_nodes,
            curent->neighboors[index]
          ) ;
        }
      }
    }
    add_to_closed_nodes (context->closed_nodes, curent) ;

#if EXTENDED_API
    usleep (1000) ;
#endif

  }
  PyEval_RestoreThread (_save) ;
  //Py_END_ALLOW_THREADS
  set_bad_result (context) ;
}

static char position_in_hitbox (p_context context, p_position position) {
  Py_ssize_t index ;
  int center_x ;
  int center_y ;
  int dx ;
  int dy ;

  for (index=0 ; index < context->hitbox_nb ; index += 1) {
    if (context->hitbox_list[index].size == 2) {
      center_x = (context->hitbox_list[index].vertex[0].x + context->hitbox_list[index].vertex[1].x) / 2 ;
      center_y = (context->hitbox_list[index].vertex[0].y + context->hitbox_list[index].vertex[1].y) / 2 ;
      dx = fabs (context->hitbox_list[index].vertex[0].x - context->hitbox_list[index].vertex[1].x) ;
      dy = fabs (context->hitbox_list[index].vertex[0].y - context->hitbox_list[index].vertex[1].y) ;
      if (point_in_ellipse (center_x, center_y, position->x,
            position->y, dx, dy)) {
        return 1 ;
      }
    } else if (point_inside_polygon (&context->hitbox_list[index], position)) {
      return 1 ;
    }
  }
  return 0 ;
}

static char point_in_ellipse (int x, int y, int xp, int yp, int d, int D) {
  return (
    (pow (xp - x, 2) / pow (d / 2, 2)) + 
    (pow (yp - y, 2) / pow (D / 2, 2))
  ) <= 1 ;
}

static char point_inside_polygon (p_hitbox hitbox, p_position position) {
  char inside = 0 ;
  int p1x = hitbox->vertex[0].x ;
  int p1y = hitbox->vertex[0].y ;
  int p2x ;
  int p2y ;
  int xinters = 0 ;
  Py_ssize_t index ;

#define MIN(x,y) (x < y ? x : y)
#define MAX(x,y) (x > y ? x : y)

  for (index = 0 ; index < hitbox->size+1 ; index += 1) {
    p2x = hitbox->vertex[index%hitbox->size].x ;
    p2y = hitbox->vertex[index%hitbox->size].y ;
    if (position->y > MIN (p1y, p2y)) {
      if (position->y <= MAX (p1y, p2y)) {
        if (position->x <= MAX (p1x, p2x)) {
          if (p1y != p2y) {
            xinters = (position->y-p1y) * (p2x-p1x) / (p2y-p1y) + p1x ;
          }
          if (p1x == p2x || position->x <= xinters) {
            inside = !inside ;
          }
        }
      }
    }
    p1x = p2x ;
    p1y = p2y ;
  }
  return inside ;

#undef MIN
#undef MAX

}

static void build_all_neighboors (p_position position) {
  build_up_left_position (position) ;
  build_up_position (position) ;
  build_up_right_position (position) ;
  build_left_position (position) ;
  build_right_position (position) ;
  build_down_left_position (position) ;
  build_down_position (position) ;
  build_down_right_position (position) ;
}

static void build_up_left_position (p_position position) {
  if (position->neighboors[UP_LEFT] != NULL)
    return ;
  position->neighboors[UP_LEFT] = new_position (
    position->x - position->context->step_x,
    position->y - position->context->step_y,
    position->cost + 1, position->context,
    position
  ) ;
}

static void build_up_position (p_position position) {
  if (position->neighboors[UP] != NULL)
    return ;
  position->neighboors[UP] = new_position (
    position->x,
    position->y - position->context->step_y,
    position->cost + 1, position->context,
    position
  ) ;
}

static void build_up_right_position (p_position position) {
  if (position->neighboors[UP_RIGHT] != NULL)
    return ;
  position->neighboors[UP_RIGHT] = new_position (
    position->x + position->context->step_x,
    position->y - position->context->step_y,
    position->cost + 1, position->context,
    position
  ) ;
}

static void build_left_position (p_position position) {
  if (position->neighboors[LEFT] != NULL)
    return ;
  position->neighboors[LEFT] = new_position (
    position->x - position->context->step_x,
    position->y,
    position->cost + 1, position->context,
    position
  ) ;
}

static void build_right_position (p_position position) {
  if (position->neighboors[RIGHT] != NULL)
    return ;
  position->neighboors[RIGHT] = new_position (
    position->x + position->context->step_x,
    position->y,
    position->cost + 1, position->context,
    position
  ) ;
}

static void build_down_left_position (p_position position) {
  if (position->neighboors[DOWN_LEFT] != NULL)
    return ;
  position->neighboors[DOWN_LEFT] = new_position (
    position->x - position->context->step_x,
    position->y + position->context->step_y,
    position->cost + 1, position->context,
    position
  ) ;
}

static void build_down_position (p_position position) {
  if (position->neighboors[DOWN] != NULL)
    return ;
  position->neighboors[DOWN] = new_position (
    position->x,
    position->y + position->context->step_y,
    position->cost + 1, position->context,
    position
  ) ;
}

static void build_down_right_position (p_position position) {
  if (position->neighboors[DOWN_RIGHT] != NULL)
    return ;
  position->neighboors[DOWN_RIGHT] = new_position (
    position->x + position->context->step_x,
    position->y + position->context->step_y,
    position->cost + 1, position->context,
    position
  ) ;
}

static void build_position_neighboorhood (p_position position) {
  switch (position->direction) {
    case ANY:
      build_all_neighboors (position) ;
      break ;
    case UP_LEFT:
      build_up_left_position (position) ;
      break ;
    case UP:
      build_up_position (position) ;
      break ;
    case UP_RIGHT:
      build_up_right_position (position) ;
      break ;
    case LEFT:
      build_left_position (position) ;
      break ;
    case RIGHT:
      build_right_position (position) ;
      break ;
    case DOWN_LEFT:
      build_down_left_position (position) ;
      break ;
    case DOWN:
      build_down_position (position) ;
      break ;
    case DOWN_RIGHT:
      build_down_right_position (position) ;
      break ;
    default:
      break;
  }
}

static void add_to_opened_nodes (p_chained_list opened_nodes, p_position position) {
  p_chain chain ;

#if EXTENDED_API
  last_opened[0] = position->x ;
  last_opened[1] = position->y ;
  usleep(1000);
#endif

  if (opened_nodes->size == 0) {
    push_chained_list (opened_nodes, &position, sizeof (p_position)) ;
  } else if (((*((p_position*)opened_nodes->last->value)
      )->heuristic <= position->heuristic)) {
    push_chained_list (opened_nodes, &position, sizeof (p_position)) ;
  } else if (((*((p_position*)opened_nodes->first->value)
      )->heuristic >= position->heuristic)) {
    shift_chained_list (opened_nodes, &position, sizeof (p_position)) ;
  } else {
    chain = opened_nodes->first ;
    while ((*((p_position*)chain->value))->heuristic < position->heuristic) {
      chain = chain->next ;
    }
    chain = chain->previous ;
    chain->next->previous = new_chain_init (&position, sizeof (p_position)) ;
    chain->next->previous->previous = chain ;
    chain->next->previous->next = chain->next ;
    chain->next = chain->next->previous ;
    opened_nodes->size_change = 1 ;
    opened_nodes->size += 1 ;
  }
}

static void add_to_closed_nodes (p_chained_list closed_nodes, p_position position) {

#if EXTENDED_API
  last_closed[0] = position->x ;
  last_closed[1] = position->y ;
  usleep(1000);
#endif

  shift_chained_list (closed_nodes, &position, sizeof (p_position)) ;
}

static char in_opened_nodes  (p_chained_list opened_nodes, p_position position) {
  p_chain chain ;

  if (opened_nodes->size == 0 || (*(
      (p_position*)opened_nodes->last->value)
      )->heuristic - position->heuristic < -4 ||(*(
      (p_position*)opened_nodes->first->value)
      )->heuristic - position->heuristic > 4
    )
    return 0 ;
  chain = opened_nodes->first ;
  while (chain != NULL && (
            (*((p_position*)chain->value))->x != position->x
        ||  (*((p_position*)chain->value))->y != position->y
      ))
    chain = chain->next ;
  return chain != NULL ;
}

static char in_closed_nodes (p_chained_list closed_nodes, p_position position) {
  p_chain chain ;

  if (closed_nodes->size == 0)
    return 0 ;
  chain = closed_nodes->first ;
  while (chain != NULL) {
    if ((*((p_position*)chain->value))->x == position->x &&
        (*((p_position*)chain->value))->y == position->y) {
      return 1 ;
    }
    chain = chain->next ;
  }
  return 0 ;
}

static float coef_dir (p_position p1, p_position p2) {
  int dx ;
  int dy ;

  dx = p2->x - p1->x ;
  dy = p2->y - p1->y ;
  return dx ? ((float)dy) / dx : 0 ;
}

static void set_good_result (p_context context) {
  p_position curent = &context->end ;
  p_position previous = curent->previous ;
  p_position tmp ;
  float dir1 ;
  float dir2 ;

  PyList_Append (context->result, Py_BuildValue ("ii", curent->x, curent->y)) ;
  while (previous != NULL && previous->previous != NULL) {
    dir1 = coef_dir (previous, curent) ;
    dir2 = coef_dir (previous->previous, previous) ;
    if (fabs (dir1) > 1.5) { // the last change did not happen as expected...
      tmp = curent ;
      while (tmp->previous != NULL && tmp->previous != previous ) {
        tmp = tmp->previous ;
      }
      PyList_Append (
        context->result,
        Py_BuildValue ("ii", tmp->x, tmp->y)
      ) ;
      PyList_Append (
        context->result,
        Py_BuildValue ("ii", previous->x, previous->y)
      ) ;
      curent = tmp ;
      previous = curent->previous ;
    } else if (fabs (dir1 - dir2) >= 0.01) {
      PyList_Append (
        context->result,
        Py_BuildValue ("ii", previous->x, previous->y)
      ) ;
      curent = previous ;
      previous = curent->previous ;
    }
    previous = previous->previous ;
  }
  if (previous != NULL) { 
    PyList_Append (
      context->result,
      Py_BuildValue ("ii", previous->x, previous->y)
    );
  } else if (curent != NULL) {
    PyList_Append (
      context->result,
      Py_BuildValue ("ii", curent->x, curent->y)
    );
  }
  PyList_Reverse (context->result) ;

#if EXTENDED_API
  state = 1 ;
  usleep(1000);
  state = -1 ;
#endif

}

static void set_bad_result (p_context context) {
  IGNORE_UNUSED_VARIABLE (context) ;

#if EXTENDED_API
  state = 1 ;
  usleep(1000);
  state = -1 ;
#endif

}

static void free_hitbox_list (p_context context) {
  int i ;

  if (context != NULL) {
    if (context->hitbox_list != NULL) {
      i = context->hitbox_nb ;
      while (i > 0) {
        i -= 1 ;
        free_hitbox (&context->hitbox_list[i]) ;
      }

#if DEBUG_COUNT_MALLOC
      _hitbox_malloc_total_size -= context->hitbox_nb * sizeof (t_hitbox) ;
#endif

      free (context->hitbox_list) ;

    }
  }
}

static void free_hitbox (p_hitbox hitbox) {
  if (hitbox != NULL) {
    if (hitbox->vertex != NULL) {

#if DEBUG_COUNT_MALLOC
            _hitbox_malloc_total_size -= hitbox->size * sizeof (t_position) ;
#endif

      free (hitbox->vertex) ;
    }
  }
}

static void free_context (p_context context) {
  if (context == NULL) 
    return ;
  free_chained_list (context->opened_nodes) ;
  free_chained_list (context->closed_nodes) ;
  free_position (context->real_start) ;
}