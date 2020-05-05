

#define _INTERNAL_CHAINED_LIST
#include "chained_list.h"


/**
 * The nomenclarure of the functions is the following:
 *  action_name underscore the_element_we_act_on [underscore second_action]
 */

/**
 * Allocate memory for a new chained list and return the pointer to the new
 *  chained list.
 * 
 * Params:
 *  None.
 * Return:
 *  p_chained_list = The newly created the chained list.
 * Error:
 *  None.
 */
p_chained_list new_chained_list (void) {
  p_chained_list chained_list ;

  if ((chained_list = calloc (1, sizeof (t_chained_list))) == NULL)
    return NULL ;

  chained_list->comparator = chain_contains_value ;
  chained_list->free_chain_value = (void (*)(void*))free_chain_value_no_method ;

  return chained_list ;
}


/**
 * Allocate memory for a new chained list, initialize the list with the given
 *  element and return the pointer to the new chained list.
 * 
 * Params:
 *  void *value : The value of the first element of the chained list.
 *  size_t size : The size of the element.
 * Return:
 *  p_chained_list = The newly created the chained list.
 * Error:
 *  None.
 */
p_chained_list new_chained_list_init (void *value,
                                      size_t size) {
  return init_chained_list (new_chained_list (), value, size) ;
}


/**
 * Initialize the given list with the element and return the pointer to the new
 *  chained list. Creates a new chained list is null is given.
 * 
 * Params:
 *  void *value : The value of the first element of the chained list.
 *  size_t size : The size of the element.
 * Return:
 *  p_chained_list = The newly created the chained list.
 * Error:
 *  None.
 */
p_chained_list init_chained_list (p_chained_list chained_list,
                                  void *value,
                                  size_t size) {
  if (chained_list == NULL)
    /* Return a new chained list, initialized with the given element. */
    return new_chained_list_init (value, size) ;

  /* the first and last element is the only element of the list */
  chained_list->first = (chained_list->last = new_chain_init (value, size)) ;

  /* now, there is exactly one element in the chain */
  chained_list->size = 1 ;
  return chained_list ;
}


/**
 * Add the given value to the end of the chained list.
 * If the given chained list is null, create a new chained list.
 * 
 * Params:
 *  p_chained_list chained_list : The list to push the element into.
 *  void *value : The value to push into the list.
 *  size_t size : The size of the element.
 * Return:
 *  p_chained_list : The (eventually new) chained list.
 * Error:
 *  None.
 */
p_chained_list push_chained_list (p_chained_list chained_list,
                                  void *value,
                                  size_t size) {
  p_chain last_chain ;

  if (chained_list == NULL)
    /* Return a new chained list, initialized with the given element. */
    return new_chained_list_init (value, size) ;
  if (chained_list_is_reversed (chained_list))
    return reverse_chained_list (shift_chained_list (
      reverse_chained_list (chained_list), value, size)) ;
  if (chained_list->size == 0)
    /* Return the initialized chained list with the given element. */
    return init_chained_list (chained_list, value, size) ;
  last_chain = chained_list->last ;
  last_chain->next = new_chain_init (value, size) ;
  last_chain->next->previous = last_chain ;
  chained_list->last = last_chain->next ;
  chained_list->size += 1 ;
  chained_list->size_change = 1 ;
  return chained_list ;
}


/**
 * Place the value of the last chain into the pointer if not null, remove the
 *  last chain from the end of the chained list.
 * 
 * Params:
 *  p_chained_list chained_list : The list to pop the element from.
 *  void *value : The pointer where to put the last chain's value.
 * Return:
 *  p_chained_list : The (eventually newly created) chained list.
 * Error:
 *  EPERM : chained_list is NULL or the size of the chained list is 0.
 *  (should errno be ENOSPC ?)
 */
p_chained_list pop_chained_list (p_chained_list chained_list,
                                  void *value) {
  errno = -1 ;
  if (chained_list == NULL || chained_list->size == 0) {
    errno = EPERM ;
    return NULL ;
  }
  if (chained_list_is_reversed (chained_list)) {
    return reverse_chained_list (unshift_chained_list (
      reverse_chained_list (chained_list), value)) ;
  }
  if (value != NULL) {
    if (chained_list->last->value == NULL)
      bzero (value, sizeof (void *)) ;
    else
      memcpy (value, chained_list->last->value, chained_list->last->size) ;
  }
  if (chained_list->size == 1) {
    free_chain (chained_list->last, chained_list->free_chain_value) ;
    chained_list->first = (chained_list->last = NULL) ;
  } else {
    chained_list->last = chained_list->last->previous ;
    free_chain (chained_list->last->next, chained_list->free_chain_value) ;
    chained_list->last->next = NULL ;
  }
  chained_list->size -= 1 ;
  chained_list->size_change = 1 ;
  return chained_list ;
}


/**
 * Add the given value to the begining of the chained list.
 * If the given chained list is null, create a new chained list.
 * 
 * Params:
 *  p_chained_list chained_list : The list to shift the element into.
 *  void *value : The value to shift into the list.
 *  size_t size : The size of the element.
 * Return:
 *  p_chained_list : The (eventually new) chained list.
 * Error:
 *  None.
 */
p_chained_list shift_chained_list (p_chained_list chained_list,
                                    void *value,
                                    size_t size) {
  p_chain first_chain ;

  if (chained_list == NULL)
    /* Return a new chained list, initialized with the given element. */
    return new_chained_list_init (value, size) ;
  if (chained_list_is_reversed (chained_list))
    return reverse_chained_list (push_chained_list (
      reverse_chained_list (chained_list), value, size)) ;
  if (chained_list->size == 0)
    /* Return the initialized chained list with the given element. */
    return init_chained_list (chained_list, value, size) ;
  first_chain = chained_list->first ;
  first_chain->previous = new_chain_init (value, size) ;
  first_chain->previous->next = first_chain ;
  chained_list->first = first_chain->previous ;
  chained_list->size += 1 ;
  chained_list->size_change = 1 ;
  return chained_list ;
}


/**
 * Place the value of the first chain into the pointer if not null, remove the
 *  first chain from the begining of the chained list.
 * 
 * Params:
 *  p_chained_list chained_list : The list to unshift the element from.
 *  void *value : The pointer where to put the first chain's value.
 * Return:
 *  p_chained_list : The (eventually newly created) chained list.
 * Error:
 *  EPERM : chained_list is NULL or the size of the chained list is 0.
 */
p_chained_list unshift_chained_list (p_chained_list chained_list,
                                      void *value) {
  errno = -1 ;
  if (chained_list == NULL || chained_list->size == 0) {
    errno = EPERM ;
    return NULL ;
  }
  if (chained_list_is_reversed (chained_list))
    return reverse_chained_list (pop_chained_list (
      reverse_chained_list (chained_list), value)) ;
  if (value != NULL) {
    if (chained_list->first->value == NULL)
      bzero (value, sizeof (void *)) ;
    else
      memcpy (value, chained_list->first->value, chained_list->first->size) ;
  }
  if (chained_list->size == 1) {
    free_chain (chained_list->first, chained_list->free_chain_value) ;
    chained_list->first = (chained_list->last = NULL) ;
  } else {
    chained_list->first = chained_list->first->next ;
    free_chain (chained_list->first->previous, chained_list->free_chain_value) ;
    chained_list->first->previous = NULL ;
  }
  chained_list->size -= 1 ;
  chained_list->size_change = 1 ;
  return chained_list ;
}

/**
 * Concatenate the second chained list into the first one, element by element,
 *  by copy and without removing them.
 * 
 * Params:
 *  p_chained_list left : The left chained list.
 *  p_chained_list right : The right chained list.
 * Return:
 *  p_chained_list = The left chained list.
 * Error:
 *  EPERM : chained_list is NULL or the size of the chained list is 0.
 */
p_chained_list concatenate_chained_lists (p_chained_list left,
                                          p_chained_list right) {
  for_chained_list_chain (right)
    left = push_chained_list (left, chain->value, chain->size) ;
  return left ;
}

/**
 * Remove the element with the given value, having the given size. Copy the
 * value in the pointer_value variable if not NULL. 
 * 
 * Params:
 *  p_chained_list chained_list : The chained list we want the value to be
 *    removed from.
 *  void* value : The value the element we want to remove has.
 *  size_t size : The size of the element to remove.
 * Return:
 *  p_chained_list = The chained list.
 * Error:
 *  EPERM : chained_list is NULL or the size of the chained list is 0.
 */
p_chained_list remove_chained_list (p_chained_list chained_list,
                                    void *value,
                                    size_t size) {
  p_chain current_chain ;

  errno = -1 ;
  if (chained_list == NULL || chained_list->size == 0) {
    errno = EPERM ;
    return NULL ;
  }
  if (chained_list->comparator (chained_list->first, value, size))
    return (chained_list->reversed ? pop_chained_list : unshift_chained_list)
      (chained_list, NULL) ;
  if (chained_list->comparator (chained_list->last, value, size))
    return (chained_list->reversed ? unshift_chained_list : pop_chained_list)
      (chained_list, NULL) ;
  current_chain = chained_list->first->next ;
  while (current_chain != NULL) {
    if (chained_list->comparator (current_chain, value, size)) {
      current_chain->previous->next = current_chain->next ;
      current_chain->next->previous = current_chain->previous ;
      chained_list->size_change = 1 ;
      chained_list->size -= 1 ;
      free_chain (current_chain, chained_list->free_chain_value) ;
      break ;
    }
    current_chain = current_chain->next ;
  }
  return chained_list ;
}


/**
 * Internal function.
 * Does not verify if index is in range or not.
 * Delete the element at the given index in the chained list. Accept negative 
 *  index.
 * 
 * Params:
 *  p_chained_list chained_list : The chained list we want the chain be removed
 *    from.
 *  int index : The index of the chain to remove. Negative index allowed.
 * Return:
 *  p_chained_list : The chained list
 * Errors:
 *  None.
 */
static p_chained_list _delete_chained_list (p_chained_list chained_list,
                                          int index) {
  p_chain current_chain ;

  if (index == 0)
    return (chained_list->reversed ? pop_chained_list : unshift_chained_list)
      (chained_list, NULL) ;
  if ((unsigned)index == chained_list->size - 1)
    return (chained_list->reversed ? unshift_chained_list : pop_chained_list)
      (chained_list, NULL) ;
  current_chain = chained_list->first ;
  while (index--)
    current_chain = current_chain->next ;
  current_chain->previous->next = current_chain->next ;
  current_chain->next->previous = current_chain->previous ;
  chained_list->size_change = 1 ;
  chained_list->size -= 1 ;
  free_chain (current_chain, chained_list->free_chain_value) ;
  return chained_list ;
}


/**
 * Delete the element at the given index in the chained list. Accept negative 
 *  index.
 * 
 * Params:
 *  p_chained_list chained_list : The chained list we want the chain be removed
 *    from.
 *  int index : The index of the chain to remove. Negative index allowed.
 * Return:
 *  p_chained_list : The chained list
 * Errors:
 *  EPERM : chained_list is NULL or the size of the chained list is 0.
 *  EINVAL : The index is greater than the size.
 */
p_chained_list delete_chained_list (p_chained_list chained_list,
                                    int index) {
  errno = -1 ;
  if (chained_list == NULL || chained_list->size == 0) {
    errno = EPERM ;
    return NULL ;
  }
  if (index < 0)
    index += chained_list->size ;
  if (index < 0 || (unsigned)index >= chained_list->size) {
    errno = EINVAL ;
    return NULL ;
  }
  /**
   * Now that we have verified the index range, we can call the real
   * deletion function.
   */
  return _delete_chained_list (chained_list, index) ;
}


/**
 * Internal function.
 * Does not verify if index is in range or not.
 * Insert the element at the given index in the chained list. Accept negative 
 *  index.
 * 
 * Params:
 *  p_chained_list chained_list : The chained list to insert the element into.
 *  void *value : The value to insert at the given index.
 *  size_t size : The size of the element to insert.
 *  int index : The index at which we want to insert the element.
 * Return:
 *  p_chained_list : The chained list.
 * Error:
 *  None.
 */
static p_chained_list _insert_chained_list (p_chained_list chained_list,
                                            void *value,
                                            size_t size,
                                            int index) {
  p_chain current_chain ;

  if (index == 0)
    return (chained_list->reversed ? push_chained_list : shift_chained_list)
      (chained_list, NULL, 0) ;
  if ((unsigned)index == chained_list->size - 1)
    return (chained_list->reversed ? shift_chained_list : push_chained_list)
      (chained_list, NULL, 0) ;
  current_chain = chained_list->first ;
  while (--index)
    current_chain = current_chain->next ;
  current_chain->next->previous = new_chain_init (value, size) ;
  current_chain->next->previous->previous = current_chain ;
  current_chain->next->previous->next = current_chain->next ;
  current_chain->next = current_chain->next->previous ;
  chained_list->size_change = 1 ;
  chained_list->size += 1 ;
  return chained_list ;
}

/**
 * Insert the given value at the given index.
 * Create the chained list if null is provided.
 * 
 * Params:
 *  p_chained_list chained_list : The chained list to insert the element into.
 *  void *value : The value to insert at the given index.
 *  size_t size : The size of the element to insert.
 *  int index : The index at which we want to insert the element.
 * Return:
 *  p_chained_list : The chained list.
 * Error:
 *  EPERM : chained_list is NULL or the size of the chained list is 0.
 *  EINVAL : The index is greater than the size.
 */
p_chained_list insert_chained_list (p_chained_list chained_list,
                          void *value,
                          size_t size,
                          int index) {
  errno = -1 ;
  if (chained_list == NULL || chained_list->size == 0) {
    errno = EPERM ;
    return NULL ;
  }
  if (index < 0)
    index += chained_list->size ;
  if (index < 0 || (unsigned)index >= chained_list->size) {
    errno = EINVAL ;
    return NULL ;
  }
  /**
   * Now that we have verified the index range, we can call the real
   * deletion function.
   */
  return _insert_chained_list (chained_list, value, size, index) ;
}


/**
 * Search for the element with the given index. If found, set the *value with
 *  its value.
 * 
 * Params:
 *  p_chained_list chained_list : The chained from from which get the value
 *  void *value : The pointer to copy the value in.
 *  int index : The index of the element.
 * Return:
 *  p_chained_list : The chained list.
 * Error:
 *  EPERM : chained_list is NULL or the size of the chained list is 0.
 *  EINVAL : The index is greater than the size.
 *  EFAULT : The value pointer is NULL.
 */
p_chained_list get_at_chained_list (p_chained_list chained_list,
                                void *value,
                                int index) {
  p_chain current_chain ;

  errno = -1 ;
  if (chained_list == NULL || chained_list->size == 0) {
    errno = EPERM ;
    return NULL ;
  }
  if (index < 0)
    index += chained_list->size ;
  if (index < 0 || (unsigned)index >= chained_list->size) {
    errno = EINVAL ;
    return NULL ;
  }
  if (value == NULL) {
    errno = EFAULT ;
    return NULL ;
  }
  current_chain = chained_list->first ;
  while (index--)
    current_chain = current_chain->next ;
  memcpy (value, current_chain->value, current_chain->size) ;
  return chained_list ;
}


/**
 * Search for the element with the given index. If found, set the chain's value
 *  with the given value.
 * 
 * Params:
 *  p_chained_list chained_list : The chained from from which get the value
 *  void *value : The pointer to copy the value in.
 *  size_t size: the size of the new value.
 *  int index : The index of the element.
 * Return:
 *  p_chained_list : The chained list.
 * Error:
 *  EPERM : chained_list is NULL or the size of the chained list is 0.
 *  EINVAL : The index is greater than the size.
 *  EFAULT : The value pointer is NULL.
 */
p_chained_list set_at_chained_list (p_chained_list chained_list,
                                void *value,
                                size_t size,
                                int index) {
  p_chain current_chain ;

  errno = -1 ;
  if (chained_list == NULL || chained_list->size == 0) {
    errno = EPERM ;
    return NULL ;
  }
  if (index < 0)
    index += chained_list->size ;
  if (index < 0 || (unsigned)index >= chained_list->size) {
    errno = EINVAL ;
    return NULL ;
  }
  if (value == NULL) {
    errno = EFAULT ;
    return NULL ;
  }
  current_chain = chained_list->first ;
  while (index--)
    current_chain = current_chain->next ;
  if (current_chain->value != NULL)
    free (current_chain->value) ;
  current_chain->value = calloc (1, size) ;
  current_chain->size = size ;
  memcpy (current_chain->value, value, size) ;
  return chained_list ;
}


/**
 * Search for the element with the given index. If found, return the chain.
 * 
 * Params:
 *  p_chained_list chained_list : The chained from from which get the value
 *  int index : The index of the element.
 * Return:
 *  p_chained_list : The chained list.
 * Error:
 *  EPERM : chained_list is NULL or the size of the chained list is 0.
 *  EINVAL : The index is greater than the size.
 *  EFAULT : The value pointer is NULL.
 */
p_chain chain_at_chained_list (p_chained_list chained_list,
                                int index) {
  p_chain current_chain ;

  errno = -1 ;
  if (chained_list == NULL || chained_list->size == 0) {
    errno = EPERM ;
    return NULL ;
  }
  if (index < 0)
    index += chained_list->size ;
  if (index < 0 || (unsigned)index >= chained_list->size) {
    errno = EINVAL ;
    return NULL ;
  }
  current_chain = chained_list->first ;
  while (index--)
    current_chain = current_chain->next ;
  return current_chain ;
}


/**
 * Search for the element with the given index. If found, return the chain's
 *  value.
 * 
 * Params:
 *  p_chained_list chained_list : The chained from from which get the value
 *  int index : The index of the element.
 * Return:
 *  p_chained_list : The chained list.
 * Error:
 *  EPERM : chained_list is NULL or the size of the chained list is 0.
 *  EINVAL : The index is greater than the size.
 *  EFAULT : The value pointer is NULL.
 */
void *value_at_chained_list (p_chained_list chained_list,
                                int index) {
  p_chain current_chain ;

  errno = -1 ;
  if (chained_list == NULL || chained_list->size == 0) {
    errno = EPERM ;
    return NULL ;
  }
  if (index < 0)
    index += chained_list->size ;
  if (index < 0 || (unsigned)index >= chained_list->size) {
    errno = EINVAL ;
    return NULL ;
  }
  current_chain = chained_list->first ;
  while (index--)
    current_chain = current_chain->next ;
  return current_chain->value ;
}


/**
 * Give the first chained list element's index that has got the same value as
 *  the one given in parameter.
 * 
 * Params:
 *  chained_list : The chained list to search into.
 *  value : The searched value's.
 *  size : The value's size.
 * Return:
 *  int : The index of the element, or -1 if not found.
 * Error:
 *  None.
 */
int index_of_chained_list (p_chained_list chained_list,
                          void *value,
                          size_t size) {
  p_chain current_chain ;
  int index ;

  if (chained_list == NULL || chained_list->size == 0)
    return -1 ;
  current_chain = chained_list->first ;
  index = 0 ;
  while (current_chain != NULL) {
    if (chained_list->comparator (current_chain, value, size))
      return index ;
    current_chain = current_chain->next ;
    index += 1 ;
  }
  return -1 ;
}


/**
 * Give the first chained list element that has got the same value as the one
 *  given in parameter.
 * 
 * Params:
 *  chained_list : The chained list to search into.
 *  value : The searched value's.
 *  size : The value's size.
 * Return:
 *  p_chain : The element, or NULL if not found.
 * Error:
 *  None.
 */
p_chain search_chained_list (p_chained_list chained_list,
                             void *value,
                             size_t size) {
  p_chain current_chain ;

  if (chained_list == NULL || chained_list->size == 0)
    return NULL ;
  current_chain = chained_list->first ;
  while (current_chain != NULL) {
    if (chained_list->comparator (current_chain, value, size))
      return current_chain ;
    current_chain = current_chain->next ;
  }
  return NULL ;
}


/**
 * Give the length of the given chained list.
 * 
 * Params:
 *  chained_list : The chained list we want to reverse.
 * Return:
 *  p_chained_list : The chained list.
 * Error:
 *  EFAULT : The value pointer is NULL.
 */
p_chained_list reverse_chained_list (p_chained_list chained_list) {
  if (chained_list == NULL) {
    errno = EFAULT ;
    return NULL ;
  }
  chained_list->reversed ^= 1 ;
  return chained_list ;
}


/**
 * Give the length of the given chained list.
 * 
 * Params:
 *  p_chained_list chained_list : The chained list we want the length of.
 * Return:
 *  size_t : the length of the chained list.
 * Error:
 *  None.
 */
size_t length_chained_list (p_chained_list chained_list) {
  if (chained_list == NULL)
    return 0 ;
  return chained_list->size ;
}


/**
 * Tell if the chained list is reversed or not.
 * 
 * Params:
 *  p_chained_list chained_list : The chained list we want the length of.
 * Return:
 *  char : 1 if reversed, 0 otherwise.
 * Error:
 *  None.
 */
char chained_list_is_reversed (p_chained_list chained_list) {
  return chained_list->reversed ;
}


/**
 * Return the first element according to the reverseness of the list.
 * 
 * Params:
 *  p_chained_list chained_list : The chained list we want the length of.
 * Return:
 *  p_chain : The first element of the list.
 * Error:
 *  None.
 */
p_chain chained_list_first (p_chained_list chained_list) {
  return chained_list->reversed ? chained_list->last : chained_list->first ;
}


/**
 * Return the last element according to the reverseness of the list.
 * 
 * Params:
 *  p_chained_list chained_list : The chained list we want the length of.
 * Return:
 *  p_chain : The last element of the list.
 * Error:
 *  None.
 */
p_chain chained_list_last (p_chained_list chained_list) {
  return chained_list->reversed ? chained_list->first : chained_list->last ;
}


/**
 * Empty the given chained list.
 * 
 * Params:
 *  p_chained_list chained_list : The chained list to empty.
 * Return:
 *  p_chained_list : The given chained list.
 * Error:
 *  EPERM : The chained list is NULL or empty.
 */
p_chained_list set_chained_list_empty (p_chained_list chained_list) {
  errno = -1 ;
  if (chained_list == NULL || chained_list->size == 0) {
    errno = EPERM ;
    return NULL ;
  }
  free_chain_and_neighbour (chained_list->first,
    chained_list->free_chain_value) ;
  chained_list->first = (chained_list->last = NULL) ;
  return chained_list ;
}


/**
 * Initialize the list to do some itterations on it.
 * 
 * Params:
 *  p_chained_list chained_list : The chained list we want to itterate on.
 * Return:
 *  p_chained_list : The chained list.
 * Error:
 *  EFAULT : The value pointer is NULL.
 */
p_chained_list begin_chained_list_itteration (p_chained_list chained_list) {
  if (chained_list == NULL) {
    errno = EFAULT ;
    return NULL ;
  }
  chained_list->size_change = 0 ;
  chained_list->current_itteration = chained_list->reversed ? \
    chained_list->last : chained_list->first ;
  return chained_list ;
}


/**
 * Give the current itterations of the list and pass to the next one.
 * 
 * Params:
 *  p_chained_list chained_list : The chained list we want to itterate on.
 * Return:
 *  p_chained_list : The chained list.
 * Error:
 *  EFAULT : The value pointer is NULL.
 *  EPERM : The size of the list changed during itteration.
 */
p_chain next_chained_list_itteration (p_chained_list chained_list) {
  p_chain chain ;

  if (chained_list == NULL) {
    errno = EFAULT ;
    return NULL ;
  }
  if (chained_list->size_change) {
    errno = EPERM ;
    return NULL ;
  }
  chain = chained_list->current_itteration ;
  if (chain != NULL)
    chained_list->current_itteration = chained_list->reversed ? \
      chained_list->current_itteration->previous : \
      chained_list->current_itteration->next ;
  return chain ;
}


/**
 * Just give the current itterations of the list.
 * 
 * Params:
 *  p_chained_list chained_list : The chained list we want to itterate on.
 * Return:
 *  p_chain : The current chain.
 * Error:
 *  None
 */
p_chain current_chained_list_itteration (p_chained_list chained_list) {
  if (chained_list != NULL)
    return chained_list->current_itteration ;
  return NULL ;
}


/**
 * Set the comparator used to compare the chain with a value.
 * 
 * Params:
 *  p_chained_list chained_list : The chained list we want to itterate on.
 *  char (*comparator) (p_chain, void *, size_t) : The comparator function
 *    to assign to the chained list.
 * Return:
 *  p_chained_list : The chained list.
 * Error:
 *  None
 */
p_chained_list set_chained_list_comparator (p_chained_list chained_list,
                                            char (*comparator) (p_chain,
                                                                void *,
                                                                size_t)) {
  if (comparator != NULL)
    chained_list->comparator = comparator ;
  else {
    errno = EPERM ;
    return NULL ;
  }
  return chained_list ;
}


/**
 * Set the chain's value free mehod, used to free the value of a chain.
 * 
 * Params:
 *  p_chained_list chained_list : The chained list we want to itterate on.
 *  char (*comparator) (p_chain, void *, size_t) : The comparator function
 *    to assign to the chained list.
 * Return:
 *  p_chained_list : The chained list.
 * Error:
 *  None
 */
p_chained_list set_chained_list_free_chain_value (p_chained_list chained_list,
                                                  void (*free_value)(void *)) {
  if (free_value != NULL)
    chained_list->free_chain_value = free_value ;
  else {
    errno = EPERM ;
    return NULL ;
  }
  return chained_list ;
}


/**
 * Free all elements of the chained list from anywhere (you can provide the
 * middle of the list, all the list will be freed). Also free the values
 * pointed by to the fields value of each element of the chained list.
 * 
 * Params:
 *  chained_list : The chained list to free.
 * Return:
 *  None.
 * Error:
 *  None.
 */
void free_chained_list (p_chained_list chained_list) {
  if (chained_list != NULL) {
    free_chain_and_neighbour (chained_list->first,
      chained_list->free_chain_value) ;
    free (chained_list) ;
  }
}


/**
 * Allocate memory for a new chain and return the pointer to the new chain.
 * 
 * Params:
 *  None.
 * Return:
 *  p_chain = The newly created the chain.
 * Error:
 *  None.
 */
p_chain new_chain (void) {
  p_chain chain ;

  if ((chain = calloc (1, sizeof (t_chain))) == NULL)
    return NULL ;

  /* eventual initialization instructions */
  /* every element has already been initialized by calloc */

  return chain ;
}


/**
 * Allocate memory for a new chain, initialize the list with the given
 *  element and return the pointer to the new chain.
 * 
 * Params:
 *  void *value : The value of the chain.
 *  size_t size : The size of the element.
 * Return:
 *  p_chain = The newly created the chain.
 * Error:
 *  None.
 */
p_chain new_chain_init (void *value,
                        size_t size) {
  return init_chain (new_chain (), value, size) ;
}


/**
 * Initialize the given list with the element and return the pointer to the new
 *  chain. Creates a new chain is null is given.
 * 
 * Params:
 *  void *value : The value of the chain.
 *  size_t size : The size of the element.
 * Return:
 *  p_chain = The newly created the chain.
 * Error:
 *  None.
 */
p_chain init_chain (p_chain chain,
                    void *value,
                    size_t size) {
  if (chain == NULL)
    /* Return a new chain, initialized with the given element. */
    return new_chain_init (value, size) ;
  if (value != NULL && size != 0) {
    chain->value = memcpy (calloc (1, size), value, size) ;
    chain->size = size ;
  } else {
    chain->value = NULL ;
    chain->size = 0 ;
  }
  return chain ;
}


/**
 * Tell if a chain contains the given value.
 *
 * Params:
 *  p_chain chain : The chain to test.
 *  void *value : The pointer containing the value to test.
 *  size_t size : The size of the value
 * Return
 *  char : 1 if the chain contains the value, 0 otherwise.
 * Error:
 *  None.
 */
char chain_contains_value (p_chain chain, void *value, size_t size) {
  if (chain == NULL)
    return 0 ;
  if (value == chain->value)
    return 1 ;
  if (value == NULL || chain->value == NULL || chain->size != size)
    return 0 ;
  return memcmp (chain->value, value, size) == 0 ;
}



/**
 * Free a value using "free" method.
 *
 * Params:
 *  p_chain chain : The value to free.
 * Return
 *  None.
 * Error:
 *  None.
 */
void free_chain_value_no_method (void *value) {
  if (value != NULL)
    free (value) ;
}

/**
 * Free a value using the given method.
 *
 * Params:
 *  p_chain chain : The chain to free.
 * Return
 *  None.
 * Error:
 *  None.
 */
void free_chain_value (void *value, void (*free_chain_value_method)(void *)) {
  if (free_chain_value_method != NULL)
    free_chain_value_method (value) ;
  else
    free_chain_value_no_method (value) ;
}


/**
 * Free one chain and its value.
 *
 * Params:
 *  p_chain chain : The chain to free.
 * Return
 *  None.
 * Error:
 *  None.
 */
void free_chain (p_chain chain, void (*free_chain_value_method)(void *)) {
  if (chain != NULL) {
    free_chain_value (chain->value, free_chain_value_method) ;
    free (chain) ;
  }
}

/**
 * Free one chain, its value and neighbours.
 *
 * Params:
 *  p_chain chain : The chain to free.
 * Return
 *  None.
 * Error:
 *  None.
 */
void free_chain_and_neighbour (p_chain chain, void (*free_chain_value_method)(void *)) {
  if (chain != NULL) {
    if (chain->next != NULL) {
      chain->next->previous = NULL ;
      free_chain_and_neighbour (chain->next, free_chain_value_method) ;
    }
    if (chain->previous != NULL) {
      chain->previous->next = NULL ;
      free_chain_and_neighbour (chain->previous, free_chain_value_method) ;
    }
    free_chain (chain, free_chain_value_method) ;
  }
}

void display_chained_list (p_chained_list chained_list, char *format) {
  size_t length ;

  printf ("[") ;
  length = length_chained_list (chained_list) ;
  for_chained_list_value (chained_list) {
    printf (format, value) ;
    if (--length) 
      printf (", ") ;
  }
  printf ("]\n") ;
}