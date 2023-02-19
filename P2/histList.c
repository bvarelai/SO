/*
Brais Varela Sieiro         brais.sieiro@udc.es
Alejandro García Bendaña    alejandro.garcia12@udc.es
*/

#include "histTypes.h"
#define LNULL -1

void createEmptyList(tList *L){
  L->size=LNULL;
}

bool isEmptyList(tList L){
  return (L.size==LNULL);
}

pos First(tList L){
  return 0;
}

pos Last(tList L){
   return L.size;
}

pos Next(tList L,pos p){
   return p++; 
}

char* getItem (pos p,tList L)
{
   return L.data[p];
}

bool insert(tList *L, char *string){

  if(L->size==LMAX-1){
    return false;
  }
  else {
    L->size++;
    L->data[L->size]=malloc(250); // array's pointer initialization 
    strcpy((L->data[L->size]),string); 
    return true;
  }
}

void DeleteList(tList *L){
  
  pos cnt=0;
  while(cnt<=L->size){  //free array pointers
    free(L->data[cnt]);
    cnt++; 
  }  
  L->size=LNULL;
} 

