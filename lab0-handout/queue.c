/*
 * Starter file for CS 208 Lab 0: Welcome to C
 * Adapted from lab developed at CMU by R. E. Bryant, 2017-2018
 */

/*
 * This program implements a queue supporting both FIFO and LIFO
 * operations.
 *
 * It uses a singly-linked list to represent the set of queue elements
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "harness.h"
#include "queue.h"

/*
  Create empty queue.
  Return NULL if could not allocate space.
*/

queue_t *q_new()
{
    queue_t *q =  malloc(sizeof(queue_t));
    if (q == NULL){
      return NULL;
    }
    q->head = NULL;
    return q;
}


/* Free all storage used by queue */
void q_free(queue_t *q)
{
  if (q != NULL){
    list_ele_t *next = q->head;
    while (next != NULL){
      list_ele_t *freed = next;
      next = freed->next;
      free(freed->value);
      free(freed);
    }
    free(q);
    
  }
}


/*
  Attempt to insert element at head of queue.
  Return true if successful.
  Return false if q is NULL or could not allocate space.
  Argument s points to the string to be stored.
  The function must explicitly allocate space and copy the string into it.
 */

bool q_insert_head(queue_t *q, char *s)
{
  list_ele_t *newh;

  if(q == NULL){
    return false;
  }

  newh = malloc(sizeof(list_ele_t)); 
  if (newh == NULL){
      return false;
  }
    
  newh->value = (char*) malloc(strlen(s) + 1);
  if(newh->value == NULL){
    free(newh);
    return false;
  }
  strncpy(newh->value, s, strlen(s) + 1);
  
  newh->next = q->head;
  if (q->head == NULL){
      q->tail = newh;
      q->size = 0;
  }
  q->head = newh;
  q->size += 1;
   
  return true;
}


/*
  Attempt to insert element at tail of queue.
  Return true if successful.
  Return false if q is NULL or could not allocate space.
  Argument s points to the string to be stored.
  The function must explicitly allocate space and copy the string into it.
 */

bool q_insert_tail(queue_t *q, char *s)
{
  list_ele_t *newh;

  if(q == NULL){
    return false;
  }

  newh = malloc(sizeof(list_ele_t)); 
  if (newh == NULL){
      return false;
  }
    
  newh->value = (char*) malloc(strlen(s) + 1);
  newh->next = NULL;
  
  if(newh->value == NULL){
    free(newh);
    return false;
  }
  strncpy(newh->value, s, strlen(s) + 1);
  

  if (q->head == NULL){
      q->head = newh;
      q->tail = newh;
      q->size = 1;
      return true;
  }
  q->tail->next = newh;
  q->tail = newh;
  q->size += 1;
   
  return true;
}


/*
  Attempt to remove element from head of queue.
  Return true if successful.
  Return false if queue is NULL or empty.
  If sp is non-NULL and an element is removed, copy the removed string to *sp
  (up to a maximum of bufsize-1 characters, plus a null terminator.)
  The space used by the list element and the string should be freed.
*/
bool q_remove_head(queue_t *q, char *sp, size_t bufsize)
{
    if(q == NULL || q->head == NULL){
      return false;
    }

    if(sp != NULL){
      strncpy(sp, q->head->value, bufsize-1);
      char null = '\0';
      sp[bufsize-1] = null;
    }

    list_ele_t* holder = q->head;
    q->head = holder->next;
    free(holder->value);
    free(holder);
    q->size -= 1;

    return true;
}

/*
  Return number of elements in queue.
  Return 0 if q is NULL or empty
 */

int q_size(queue_t *q)
{
  if (q == NULL || q->head == NULL){
    return 0;
  }
  return q->size;
}

/*
  Reverse elements in queue
  No effect if q is NULL or empty
  This function should not allocate or free any list elements
  (e.g., by calling q_insert_head, q_insert_tail, or q_remove_head).
  It should rearrange the existing ones.
 */
 

void q_reverse(queue_t *q)
{
 if(q != NULL && q->head != NULL){
  
  list_ele_t *head = q->tail;
  list_ele_t *next = q->head;
  list_ele_t *previous = NULL;
  while(next != NULL){
    list_ele_t *inWhile = next->next; 
    next->next = previous;
    previous = next;
    next = inWhile;
  }
  q->tail = q->head;
  q->head = head;
 }
  
}
