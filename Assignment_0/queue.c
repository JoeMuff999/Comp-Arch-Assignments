/* 
 * Code for basic C skills diagnostic.
 * Developed for courses 15-213/18-213/15-513 by R. E. Bryant, 2017
 * Modified to store strings, 2018
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
  queue_t *q = malloc(sizeof(queue_t));
  if (q != NULL)
  {
    q->head = NULL;
    q->tail = NULL;
    q->size = 0;
  }

  return q;
}

/* Free all storage used by queue */
void q_free(queue_t *q)
{

  //free(q->tail);
  //free(q->head);

  /* How about freeing the list elements and the strings? */
  /* Free queue structure */
  if(q == NULL) //already freed
    return;
  while (q->head != NULL)
  {
    list_ele_t *dummy = q->head;
    char *dummyChar = q->head->value;
    q->head = q->head->next;
    free(dummy);
    free(dummyChar);
  }
  // free(q->tail->value);
  // free(q->tail);
  // free(q->tail);
  //free(q->head->value);
  free(q);
}

/*
  Attempt to insert element at head of queue.
  Return true if successful. -> handled
  Return false if q is NULL or could not allocate space. -> handled
  Argument s points to the string to be stored. -> handled
  The function must explicitly allocate space and copy the string into it. -> handled
 */
bool q_insert_head(queue_t *q, char *s)
{

  /* What should you do if the q is NULL? */
  if (q == NULL)
    return false;

  list_ele_t *newh;
  newh = malloc(sizeof(list_ele_t));

  /* Don't forget to allocate space for the string and copy it */
  if (newh == NULL)
    return false;
  char *sCopy = malloc(sizeof(char) * (strlen(s) + 1));
  if (sCopy == NULL)
  {
    free(newh);
    return false;
  }
  /* What if either call to malloc returns NULL? */
  if (s == NULL)
  {
    newh->value = NULL;
  }
  else
  {
    strcpy(sCopy, s);
    newh->value = sCopy;
  }

  newh->next = NULL;
  if (q->head == NULL)
  {
    q->head = newh;
    q->tail = newh;
  }
  else
  {
    newh->next = q->head;
    q->head = newh;
  }

  q->size++;

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
  /* You need to write the complete code for this function */
  /* Remember: It should operate in O(1) time */
  if (q == NULL)
    return false;

  list_ele_t *newt;
  newt = malloc(sizeof(list_ele_t));
  
  if (newt == NULL)
    return false;

  char *sCopy = malloc(sizeof(char) * (strlen(s) + 1));
  if (sCopy == NULL)
  {
    free(newt);
    return false;
  }
    

  /* Don't forget to allocate space for the string and copy it */
  if (s == NULL)
  {
    newt->value = NULL;
  }
  else
  {
    strcpy(sCopy, s);
    newt->value = sCopy;
  }
  newt->next = NULL;

  if (q->head == NULL)
  {
    q->tail = newt;
    q->head = q->tail;
  }
  else
  {
    q->tail->next = newt;
    q->tail = newt;
  }
  q->size++;
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
  /* You need to fix up this code. */
  if (q == NULL || q->head == NULL || q->head->value == NULL)
    return false;

  if (sp != NULL)
  {
    memset(sp, '\0', bufsize); // NULL NULL ... NULL (bufsize times)
    strncpy(sp, q->head->value, bufsize-1);
  }

  if (q->size == 1)
  {
    free(q->head->value);
    free(q->head);
    q->head = NULL;
  }
  else
  {
    list_ele_t *dummy = q->head;
    q->head = q->head->next;
    free(dummy->value);
    free(dummy);
  }

  q->size--;
  return true;
}

/*
  Return number of elements in queue.
  Return 0 if q is NULL or empty
 */
int q_size(queue_t *q)
{
  /* You need to write the code for this function */
  /* Remember: It should operate in O(1) time */
  if(q == NULL)
    return 0;
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
  /* You need to write the code for this function */
  if (q == NULL || q->head == NULL)
    return;

  list_ele_t *curr = q->head->next;
  list_ele_t *prev = q->head;
  while (curr != NULL)
  {
    list_ele_t *foreNext = curr->next;
    curr->next = prev;
    prev = curr;
    curr = foreNext;
  }
  q->tail = q->head;
  q->tail->next = NULL;
  q->head = prev;

  //printf("the head :: %s, and this is the tail :: %s \n", q->head->value, q->tail->value);
}
