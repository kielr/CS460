
PROC *get_proc(PROC **list)
{
  return dequeue(list);
}
int   enqueue(PROC **queue, PROC *p)
{
  PROC *temp = 0, *prev = 0;

  if(*queue == 0)
  {
    *queue = p;
    p->next = 0;
  }
  else
  {
    if(p->priority > (*queue)->priority)
    {
      p->next = (*queue);
      *queue = p;
    }
    else
    {
      temp = prev = *queue;
      while((temp) && temp->priority >= p->priority)
      {
        prev = temp;
        temp = temp->next;
      }

      prev->next = p;
      p->next = temp;
    }
  }

  return 1;
} //: enter p into queue by priority

PROC *dequeue(PROC **queue)
{
  PROC *p = 0;

  if(*queue == 0)
  {
    p = 0;
  }
  else
  {
    p = *queue;
    *queue = (*queue)->next;
  }
} //: return first element removed from queue

printList(char *name, PROC *list)
{
  PROC *p = list, *root = list;

  while (p != 0)
  {
    printf("< %d > => ", p->pid);
    p = p->next;
  }
  printf("NULL\r\n\n");

  return 1;
}    //: print name=list content
