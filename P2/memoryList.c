
/*
Brais Varela Sieiro         brais.sieiro@udc.es
Alejandro García Bendaña    alejandro.garcia12@udc.es
*/

#include "memoryTypes.h"
#define LNULL -1

void createEmptyListM(tListM *L)
{
  L->size = LNULL;
}

bool isEmptyListM(tListM L)
{
  return (L.size == LNULL);
}

posM FirstM(tListM L)
{
  return 0;
}

posM LastM(tListM L)
{
  return L.size;
}

posM NextM(tListM L, posM p)
{
  return p++;
}

itemM *getItemM(posM p, tListM L)
{
  return L.data[p];
}

posM findMalloc(unsigned long size, tListM L)
{
  posM p;

  if (isEmptyListM(L))
  {
    return -1;
  }
  else
  {
    for (p = 0; p < L.size && (L.data[p]->tam != size || strcmp(L.data[p]->typeAllocation, "malloc") != 0); p++)
      ;
    if (L.data[p]->tam == size && !strcmp(L.data[p]->typeAllocation, "malloc"))
      return p;
    else
      return -1;
  }
}

posM findShared(char *key, tListM L)
{
  posM p;

  if (isEmptyListM(L))
  {
    return -1;
  }
  else
  {
    for (p = 0; p < L.size && strcmp(L.data[p]->key, key) != 0; p++)
      ;
    if (!strcmp(L.data[p]->key, key))
      return p;
    else
      return -1;
  }
}

posM findFile(char *namefile, tListM L)
{
  posM p;

  if (isEmptyListM(L))
  {
    return -1;
  }
  else
  {
    for (p = 0; p < L.size && strcmp(L.data[p]->nameFile, namefile) != 0; p++)
      ;
    if (!strcmp(L.data[p]->nameFile, namefile))
      return p;
    else
      return -1;
  }
}

posM findAddr(char *addr, tListM L)
{
  posM p;
  void *address;

  if (isEmptyListM(L))
  {
    return -1;
  }
  else
  {
    sscanf(addr, "%p", &address);

    for (p = 0; p < L.size && L.data[p]->addres != address; p++)
      ;
    if (L.data[p]->addres == address)
      return p;
    else
      return -1;
  }
}

bool insertM(tListM *L, void *addres, unsigned long tam, char timeAllocation[MAX], char *typeAllocation, char *key, char *namefile, char *fileDescriptor)
{

  if (L->size == LMAX - 1)
  {
    return false;
  }
  else
  {
    L->size++;
    L->data[L->size] = malloc(LMAX); // array's pointer initialization
    L->data[L->size]->addres = addres;
    strcpy(L->data[L->size]->timeAllocation, timeAllocation);
    L->data[L->size]->tam = tam;
    strcpy(L->data[L->size]->key, key);
    strcpy(L->data[L->size]->typeAllocation, typeAllocation);
    strcpy(L->data[L->size]->nameFile, namefile);
    strcpy(L->data[L->size]->fileDescriptor, fileDescriptor);
    return true;
  }
}

void deletePosM(tListM *L, posM cnt)
{
  int i;

  if (cnt == -1)
    printf("Error the block is not in the list\n");
  else
  {
    L->size--;
    if (!strcmp(L->data[cnt]->typeAllocation, "malloc"))
      free(L->data[cnt]->addres);

    free(L->data[cnt]);

    i = cnt;
    while (i <= L->size)
    {
      L->data[i] = L->data[i + 1];
      i++;
    }
  }
}

void DeleteListM(tListM *L)
{

  posM cnt = 0;
  while (cnt <= L->size)
  {
    if (!strcmp(L->data[cnt]->typeAllocation, "malloc"))
    { // in malloc we have to free the address.

      free(L->data[cnt]->addres);
      free(L->data[cnt]);
    }
    else
      free(L->data[cnt]);

    cnt++;
  }
  L->size = LNULL;
}

void printMalloc(tListM L)
{
  posM p;
  printf("**********Lista de bloques asignados malloc para el proceso %d\n", getpid());
  for (p = FirstM(L); p <= LastM(L); p++)
  {

    if (!strcmp(getItemM(p, L)->typeAllocation, "malloc"))
    {
      printf("\t%p\t\t%ld  %s  %s\n", getItemM(p, L)->addres, getItemM(p, L)->tam, getItemM(p, L)->timeAllocation, getItemM(p, L)->typeAllocation);
    }
  }
}

void printShared(tListM L)
{
  posM p;
  printf("**********Lista de bloques asignados shared para el proceso %d\n", getpid());
  for (p = FirstM(L); p <= LastM(L); p++)
  {

    if (!strcmp(getItemM(p, L)->typeAllocation, "shared"))
    {
      printf("\t%p\t\t%ld  %s  %s  (key %s)\n", getItemM(p, L)->addres, getItemM(p, L)->tam, getItemM(p, L)->timeAllocation, getItemM(p, L)->typeAllocation, getItemM(p, L)->key);
    }
  }
}

void printMmap(tListM L)
{
  posM p;
  printf("**********Lista de bloques asignados mmap para el proceso %d\n", getpid());
  for (p = FirstM(L); p <= LastM(L); p++)
  {

    if (!strcmp(getItemM(p, L)->typeAllocation, "mmap"))
    {
      printf("\t%p\t\t%ld  %s  %s  (descriptor %s)\n", getItemM(p, L)->addres, getItemM(p, L)->tam, getItemM(p, L)->timeAllocation, getItemM(p, L)->nameFile, getItemM(p, L)->fileDescriptor);
    }
  }
}

void printAllList(tListM L)
{
  posM p;
  printf("**********Lista de bloques asignados para el proceso %d\n", getpid());
  for (p = FirstM(L); p <= LastM(L); p++)
  {
    if (!strcmp(getItemM(p, L)->typeAllocation, "malloc"))
    {
      printf("\t%p\t\t%ld  %s  %s\n", getItemM(p, L)->addres, getItemM(p, L)->tam, getItemM(p, L)->timeAllocation, getItemM(p, L)->typeAllocation);
    }
    else if (!strcmp(getItemM(p, L)->typeAllocation, "shared"))
    {
      printf("\t%p\t\t%ld  %s  %s  (key %s)\n", getItemM(p, L)->addres, getItemM(p, L)->tam, getItemM(p, L)->timeAllocation, getItemM(p, L)->typeAllocation, getItemM(p, L)->key);
    }
    else if (!strcmp(getItemM(p, L)->typeAllocation, "mmap"))
    {
      printf("\t%p\t\t%ld  %s  %s  (descriptor %s)\n", getItemM(p, L)->addres, getItemM(p, L)->tam, getItemM(p, L)->timeAllocation, getItemM(p, L)->nameFile, getItemM(p, L)->fileDescriptor);
    }
  }
}
