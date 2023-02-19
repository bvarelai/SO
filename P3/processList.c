/*
Brais Varela Sieiro         brais.sieiro@udc.es
Alejandro García Bendaña    alejandro.garcia12@udc.es
*/

#include "processTypes.h"
#define LNULL -1

void createEmptyListP(tListP *L)
{
  L->size = LNULL;
}

bool isEmptyListP(tListP L)
{
  return (L.size == LNULL);
}

posP FirstP(tListP L)
{
  return 0;
}

posP LastP(tListP L)
{
  return L.size;
}

posP NextP(tListP L, posP p)
{
  return p++;
}

itemP *getItemP(posP p, tListP L)
{
  return L.data[p];
}

posP findEnd(tListP L)
{
  posP p;

  if (isEmptyListP(L))
  {
    return -1;
  }
  else
  {
    for (p = 0; p < L.size && (strcmp(L.data[p]->status, "TERMINADO") != 0); p++)
      ;
    if (!strcmp(L.data[p]->status, "TERMINADO"))
      return p;
    else
      return -1;
  }
}

posP findEndForSignal(tListP L)
{
  posP p;
  int state = 0;

  if (isEmptyListP(L))
  {
    return -1;
  }
  else
  {
    for (p = 0; p < L.size && L.data[p]->retSignal != WTERMSIG(state); p++)
      ;
    if (L.data[p]->retSignal == WTERMSIG(state))
      return p;
    else
      return -1;
  }
}

posP findPid(tListP L, int pid)
{
  posP p;

  if (isEmptyListP(L))
  {
    return -1;
  }
  else
  {
    for (p = 0; p < L.size && L.data[p]->pid != pid; p++)
      ;
    if (L.data[p]->pid == pid)
      return p;
    else
      return -1;
  }
}

void Signal(posP p, tListP *L)
{
  
  if (waitpid(L->data[p]->pid, &L->data[p]->retSignal, WNOHANG | WUNTRACED | WCONTINUED) != -1)
  {
    if (WIFEXITED(L->data[p]->retSignal))
    {
      strcpy(L->data[p]->status, "TERMINADO");
      L->data[p]->retSignal = WEXITSTATUS(L->data[p]->retSignal);
      L->data[p]->priority = -1;
    }
    else if (WIFCONTINUED(L->data[p]->retSignal))
    {
      strcpy(L->data[p]->status, "ACTIVO");
      L->data[p]->retSignal = 0;
    }
    else if (WIFSTOPPED(L->data[p]->retSignal))
    {
      strcpy(L->data[p]->status, "PARADO");
      L->data[p]->retSignal = (WIFSTOPPED(L->data[p]->retSignal));
    }
    else if (WTERMSIG(L->data[p]->retSignal))
    {
      strcpy(L->data[p]->status, "TERMINADO POR SEÑAL");
      L->data[p]->retSignal = (WTERMSIG(L->data[p]->retSignal));
    }
  }
}

bool insertP(tListP *L, int pid, char *user, int priority, char *datetimeLaunching, char *commandLine)
{
  if (L->size == LMAX - 1)
  {
    return false;
  }
  else
  {
    L->size++;
    L->data[L->size] = malloc(LMAX); // array's pointer initialization
    L->data[L->size]->pid = pid;
    strcpy(L->data[L->size]->user, user);
    L->data[L->size]->priority = priority;
    strcpy(L->data[L->size]->datetimeLaunching, datetimeLaunching);
    L->data[L->size]->commandLine = malloc(100);
    strcpy(L->data[L->size]->commandLine, commandLine);
    return true;
  }
}

void deletePosP(tListP *L, posP cnt)
{
  int i;

  if (cnt == -1)
    printf("Error the block is not in the list\n");
  else
  {
    L->size--;
    free(L->data[cnt]);

    i = cnt;
    while (i <= L->size)
    {
      L->data[i] = L->data[i + 1];
      i++;
    }
  }
}

void DeleteListP(tListP *L)
{
  posP cnt = 0;
  while (cnt <= L->size)
  {
    free(L->data[cnt]);
    cnt++;
  }
  L->size = LNULL;
}

void printAllListP(tListP L)
{

  posP p;
  for (p = FirstP(L); p <= LastP(L); p++)
  {
    Signal(p, &L);
    printf("%d\t%s p=%d %s %s (%d) %s\n", getItemP(p, L)->pid, getItemP(p, L)->user, getItemP(p, L)->priority, getItemP(p, L)->datetimeLaunching, getItemP(p, L)->status, getItemP(p, L)->retSignal, getItemP(p, L)->commandLine);
  }
}
