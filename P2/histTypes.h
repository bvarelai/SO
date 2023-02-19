/*
Brais Varela Sieiro         brais.sieiro@udc.es
Alejandro García Bendaña    alejandro.garcia12@udc.es
*/

#include <unistd.h>
#include <sys/utsname.h>
#include <errno.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <math.h>
#include <ctype.h>

#include <fcntl.h>
#include <sys/stat.h>
#include <sys/shm.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <dirent.h>
#include <pwd.h>

#define LMAX 4096

typedef int pos;

struct List {
char *data[LMAX];
pos size;
};

typedef struct List tList;


void createEmptyList(tList *L);
bool isEmptyList(tList L);
pos First(tList L);
pos Last(tList L);
pos Next(tList L,pos p);
char* getItem (pos p,tList L);
void DeleteList(tList *L); 
bool insert(tList *L, char *cadena);





