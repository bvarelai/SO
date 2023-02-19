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
#define MAX 60

typedef int posM;

typedef struct itemM
{
    void *addres;
    unsigned long tam;
    char timeAllocation[MAX];
    char typeAllocation[MAX];
    char key[MAX];
    char nameFile[MAX];
    char fileDescriptor[MAX];
} itemM;

struct ListM
{
    itemM *data[LMAX];
    posM size;
};

typedef struct ListM tListM;

void createEmptyListM(tListM *L);
bool isEmptyListM(tListM L);
posM FirstM(tListM L);
posM LastM(tListM L);
posM NextM(tListM L, posM p);
itemM *getItemM(posM p, tListM L);
posM findItemM(unsigned long size, tListM L);
void DeleteListM(tListM *L);
bool insertM(tListM *L, void *addres, unsigned long tam, char timeAllocation[MAX], char *typeAllocation, char *key, char *namefile, char *fileDescriptor);
