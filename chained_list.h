/*
 * chained_list.h
 * 
 * Copyright 2016 balto <lain@lain>
 * 
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 * MA 02110-1301, USA.
 * 
 * 
 */



#ifndef _CHAINED_LIST_H

#define _CHAINED_LIST_H



#ifndef _STDIO_H
#include <stdio.h>
#endif  /* end of ifndef _STDIO_H_ */


#ifndef _STRINGS_H
#include <strings.h>
#endif  /* end of ifndef _STRINGS_H */


#ifndef _STRING_H
#include <string.h>
#endif  /* end of ifndef _STRING_H */


#ifndef _STDLIB_H
#include <stdlib.h>
#endif  /* end of ifndef _STDLIB_H */


#ifndef _ERRNO_H
#include <errno.h>
#endif  /* end of ifndef _ERRNO_H */


/**
 * Defines the type t_chained_list not to have to write
 *  'struct s_chained_list'
 * every time.
 */
typedef struct s_chained_list t_chained_list ;

/* Defines a pointer to a chained list. */
typedef t_chained_list* p_chained_list ;

/* Defines the type t_chain not to have to write struct s_chain. */
typedef struct s_chain t_chain ;

/* Defines a pointer to a struct s_chain. */
typedef t_chain* p_chain ;

/* Defines the type t_wrapper not to have to write struct s_wrapper. */
typedef struct s_wrapper t_wrapper ;

/* Defines a pointer to a struct s_wrapper. */
typedef t_wrapper* p_wrapper ;

typedef void *(*t_function) (void *) ;

typedef char(*p_chain_comparator) (p_chain, void *, size_t) ;
typedef void (*p_free_chain_value_method) (void *) ;


/* Defines a chained list object. */
struct s_chained_list {

  /* The first element of the list. */ 
  p_chain first ;

  /* The last element of the list. */
  p_chain last ;

  /* The element number of the list. */
  size_t size ;

  /* The current chain of the itteration. */
  p_chain current_itteration ;

  /**
   * Set to 1 if size has been changed. Usefull to check if size changed during
   *  itteration and raise an error.
   */
  char size_change ;

  /**
   * Tell if the chained list has been reversed:
   *  1: reversed
   *  0: not reversed
   */
  char reversed ;

  /**
   * This pointer points to on a method to compar a chain and a value.
   * It must return 1 if the chain do contains the value, 0 otherwise.
   * Used only if set. If not set, use the memcmp method.
   */
  char (*comparator) (p_chain chain, void *value, size_t value_size) ;

  /**
   * This pointer points to on a method to free the value of a chain.
   * Used only if set. If not set, use the free method.
   */
  void (*free_chain_value) (void *chain) ;

  p_chained_list (*itter) (p_chained_list) ;
  p_chained_list (*reverse) (p_chained_list) ;
  p_chained_list (*pop) (p_chained_list, void *) ;
  p_chained_list (*push) (p_chained_list, void *, size_t) ;
  p_chained_list (*shift) (p_chained_list, void *, size_t) ;
  p_chained_list (*unshift) (p_chained_list, void *) ;
  p_chained_list (*insert) (p_chained_list, void *, size_t, size_t) ;
  p_chained_list (*remove) (p_chained_list, void *, size_t) ;
  p_chained_list (*delete) (p_chained_list, int) ;
  p_chained_list (*at) (p_chained_list, size_t) ;
  p_chained_list (*index_of) (p_chained_list) ;

} ; /* end of struct s_chained_list */


/* Defines an chained list element. */
struct s_chain {

  /* The next chain. */
  p_chain next ;

  /* The previous chain. */
  p_chain previous ;

  /* A pointer to the stored value in the chained list. */
  void *value ;

  /* The size of the stored value. */ 
  size_t size ;

} ; /* end of struct s_chain */


/* Defines a wrapper to be allowed to store function pointers */
struct s_wrapper {

  /* Defines the function pointer */
  t_function function ;

} ; /* end of struct s_wrapper */


#define for_chained_list_value(chained_list)                                  \
  for (void *value=length_chained_list (                                      \
      begin_chained_list_itteration (chained_list)) == 0 ? NULL :             \
        current_chained_list_itteration (chained_list)->value ;               \
    value != NULL ;                                                           \
    next_chained_list_itteration (chained_list),                              \
    value = current_chained_list_itteration (chained_list) == NULL ? NULL :   \
      current_chained_list_itteration (chained_list)->value)

#define for_chained_list_value_of_type(chained_list,type)                     \
  for (type value=length_chained_list (                                       \
      begin_chained_list_itteration (chained_list)) == 0 ? 0 :                \
        *(type*)current_chained_list_itteration (chained_list)->value ;       \
    current_chained_list_itteration (chained_list) ;                          \
    next_chained_list_itteration (chained_list),                              \
    value = current_chained_list_itteration (chained_list) == NULL ? 0 :      \
      *(type*)current_chained_list_itteration (chained_list)->value)

#define for_chained_list_value_of_simple_type(chained_list,type)              \
  for (type value=length_chained_list (                                       \
      begin_chained_list_itteration (chained_list)) == 0 ? 0 :                \
        *(type*)current_chained_list_itteration (chained_list)->value ;       \
    current_chained_list_itteration (chained_list) ;                          \
    next_chained_list_itteration (chained_list),                              \
    value = current_chained_list_itteration (chained_list) == NULL ? 0 :      \
      *(type*)current_chained_list_itteration (chained_list)->value)

#define for_chained_list_value_of_complex_type(chained_list, type)            \
  for (type *value=length_chained_list (                                      \
      begin_chained_list_itteration (chained_list)) == 0 ? NULL :             \
        current_chained_list_itteration (chained_list)->value ;               \
    value != NULL ;                                                           \
    next_chained_list_itteration (chained_list),                              \
    value = current_chained_list_itteration (chained_list) == NULL ? NULL :   \
      current_chained_list_itteration (chained_list)->value)

#define for_chained_list_chain(chained_list)                                  \
  for (p_chain chain=current_chained_list_itteration (                        \
      begin_chained_list_itteration (chained_list)) ;                         \
    current_chained_list_itteration (chained_list) != NULL ;                  \
    next_chained_list_itteration (chained_list),                              \
    chain=current_chained_list_itteration(chained_list))


p_chained_list
  new_chained_list (void) ;
p_chained_list
  new_chained_list_init (
    void *          value,
    size_t          size
  ) ;
p_chained_list
  init_chained_list (
    p_chained_list  chained_list,
    void *          value,
    size_t          size
  ) ;
p_chained_list
  push_chained_list (
    p_chained_list  chained_list,
    void *          value,
    size_t          size
  ) ;
p_chained_list
  pop_chained_list (
    p_chained_list  chained_list,
    void *          value
  ) ;
p_chained_list
  shift_chained_list (
    p_chained_list  chained_list,
    void *          value,
    size_t          size
  ) ;
p_chained_list
  unshift_chained_list (
    p_chained_list  chained_list,
    void *          value
  ) ;
p_chained_list
  concatenate_chained_lists (
    p_chained_list  left,
    p_chained_list  right
  ) ;
p_chained_list
  remove_chained_list (
    p_chained_list  chained_list,
    void *          value,
    size_t          size
  ) ;
p_chained_list
  delete_chained_list (
    p_chained_list  chained_list,
    int             index
  ) ;
p_chained_list
  insert_chained_list (
    p_chained_list  chained_list,
    void *          value,
    size_t          size,
    int             index
  ) ;
p_chained_list
  get_at_chained_list (
    p_chained_list  chained_list,
    void *          value,
    int             index
  ) ;
p_chained_list
  set_at_chained_list (
    p_chained_list  chained_list,
    void *          value,
    size_t          size,
    int             index
  ) ;
p_chain
  chain_at_chained_list (
    p_chained_list  chained_list,
    int             index
  ) ;
void *
  value_at_chained_list (
    p_chained_list  chained_list,
    int             index
  ) ;
int
  index_of_chained_list (
    p_chained_list  chained_list,
    void *          value,
    size_t          size
  ) ;
p_chain
  search_chained_list (
    p_chained_list  chained_list,
    void *          value,
    size_t          size
  ) ;
p_chained_list
  reverse_chained_list (
    p_chained_list  chained_list
  ) ;
size_t
  length_chained_list (
    p_chained_list  chained_list
  ) ;
char
  chained_list_is_reversed (
    p_chained_list  chained_list
  ) ;
p_chain
  chained_list_first (
    p_chained_list  chained_list
  ) ;
p_chain
  chained_list_last (
    p_chained_list  chained_list
  ) ;
p_chained_list
  set_chained_list_empty (
    p_chained_list  chained_list
  ) ;
p_chained_list
  begin_chained_list_itteration (
    p_chained_list  chained_list
  ) ;
p_chain
  next_chained_list_itteration (
    p_chained_list  chained_list
  ) ;
p_chain
  current_chained_list_itteration (
    p_chained_list  chained_list
  ) ;
p_chained_list
  set_chained_list_comparator (
    p_chained_list      chained_list,
    p_chain_comparator  comparator
  ) ;
p_chained_list
  set_chained_list_free_chain_value (
    p_chained_list            chained_list,
    p_free_chain_value_method free_value
  );
void
  free_chained_list (
    p_chained_list  chained_list
  ) ;
p_chain
  new_chain (void) ;
p_chain
  new_chain_init (
    void *          value,
    size_t          size
  ) ;
p_chain
  init_chain (
    p_chain         chain,
    void *          value,
    size_t          size
  ) ;
char
  chain_contains_value (
    p_chain         chain,
    void *          value,
    size_t          size
  ) ;
void
  free_chain_value_no_method (
    void *          value
  ) ;
void
  free_chain_value (
    void *          value,
    p_free_chain_value_method free_chain_value_method
  ) ;
void
  free_chain (
    p_chain         chain,
    p_free_chain_value_method free_chain_value_method
  ) ;
void
  free_chain_and_neighbour (
    p_chain         chain,
    p_free_chain_value_method free_chain_value_method
  ) ;
void
  display_chained_list (
    p_chained_list  chained_list,
    char *          format
  ) ;


#endif  /* end ifndef _CHAINED_LIST_H */
