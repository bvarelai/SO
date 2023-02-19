/*
Brais Varela Sieiro         brais.sieiro@udc.es
Alejandro García Bendaña    alejandro.garcia12@udc.es
*/

#include <sys/resource.h>
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

struct SEN
{
    char *nombre;
    int senal;
};

/*las siguientes funciones nos permiten obtener el nombre de una senal a partir
del número y viceversa */
static struct SEN sigstrnum[] = {
    {"HUP", SIGHUP},
    {"INT", SIGINT},
    {"QUIT", SIGQUIT},
    {"ILL", SIGILL},
    {"TRAP", SIGTRAP},
    {"ABRT", SIGABRT},
    {"IOT", SIGIOT},
    {"BUS", SIGBUS},
    {"FPE", SIGFPE},
    {"KILL", SIGKILL},
    {"USR1", SIGUSR1},
    {"SEGV", SIGSEGV},
    {"USR2", SIGUSR2},
    {"PIPE", SIGPIPE},
    {"ALRM", SIGALRM},
    {"TERM", SIGTERM},
    {"CHLD", SIGCHLD},
    {"CONT", SIGCONT},
    {"STOP", SIGSTOP},
    {"TSTP", SIGTSTP},
    {"TTIN", SIGTTIN},
    {"TTOU", SIGTTOU},
    {"URG", SIGURG},
    {"XCPU", SIGXCPU},
    {"XFSZ", SIGXFSZ},
    {"VTALRM", SIGVTALRM},
    {"PROF", SIGPROF},
    {"WINCH", SIGWINCH},
    {"IO", SIGIO},
    {"SYS", SIGSYS},
/*senales que no hay en todas partes*/
#ifdef SIGPOLL
    {"POLL", SIGPOLL},
#endif
#ifdef SIGPWR
    {"PWR", SIGPWR},
#endif
#ifdef SIGEMT
    {"EMT", SIGEMT},
#endif
#ifdef SIGINFO
    {"INFO", SIGINFO},
#endif
#ifdef SIGSTKFLT
    {"STKFLT", SIGSTKFLT},
#endif
#ifdef SIGCLD
    {"CLD", SIGCLD},
#endif
#ifdef SIGLOST
    {"LOST", SIGLOST},
#endif
#ifdef SIGCANCEL
    {"CANCEL", SIGCANCEL},
#endif
#ifdef SIGTHAW
    {"THAW", SIGTHAW},
#endif
#ifdef SIGFREEZE
    {"FREEZE", SIGFREEZE},
#endif
#ifdef SIGLWP
    {"LWP", SIGLWP},
#endif
#ifdef SIGWAITING
    {"WAITING", SIGWAITING},
#endif
    {NULL, -1}}; /*fin array sigstrnum */

int ValorSenal(char *sen) /*devuelve el numero de senial a partir del nombre*/
{
    int i;
    for (i = 0; sigstrnum[i].nombre != NULL; i++)
        if (!strcmp(sen, sigstrnum[i].nombre))
            return sigstrnum[i].senal;
    return -1;
}

char *NombreSenal(int sen) /*devuelve el nombre senal a partir de la senal*/
{                          /* para sitios donde no hay sig2str*/
    int i;
    for (i = 0; sigstrnum[i].nombre != NULL; i++)
        if (sen == sigstrnum[i].senal)
            return sigstrnum[i].nombre;
    return ("SIGUNKNOWN");
}

typedef int posP;

typedef struct itemP
{
    int pid;
    char user[MAX];
    int priority;
    char datetimeLaunching[MAX];
    char status[MAX];
    int retSignal; // return value of status
    char *commandLine;

} itemP;

struct ListP
{
    itemP *data[LMAX];
    posP size;
};

typedef struct ListP tListP;

void createEmptyListP(tListP *L);
bool isEmptyListP(tListP L);
posP FirstP(tListP L);
posP LastP(tListP L);
posP NextP(tListP L, posP p);
itemP *getItemP(posP p, tListP L);
void DeleteListP(tListP *L);
bool insertP(tListP *L, int pid, char *user, int priority, char *datetimeLaunching, char *commandLine);
