
#include "lab2.c"
#define MAXNAME 300

/** LAB ASSIGMENT 3 IMPLEMENTATION **/

/*******************************************************************/
/*******************************************************************/

/// changevar -p crear variables
//// al crearla asi aparece environ y puten()

// Waitpid() WUOHAND

extern char **environ;

void priority(char *pid, char *valor);
void showvar(char *var, char *env[]);
void changevar(char *atribute, char *var, char *valor, char *env[]);
void showenv(char *param, char *env[]);
void execute(char *chunks[]);
void process(char *chunks[], tListP *L3);
void listjobs(tListP L);
void deljobs(char *param, tListP *L);
void job(char *fg, char *pid, tListP L);

void Cmd_fork(char *tr[], tListP *L)
{
   pid_t pid;

   if ((pid = fork()) == 0)
   {
      /*		VaciarListaProcesos(&LP); Depende de la implementación de cada uno*/ /*Se hace para que no se clone la lista en el hijo*/
      DeleteListP(L);
      printf("ejecutando proceso %d\n", getpid());
   }
   else if (pid != -1)
      waitpid(pid, NULL, 0);
}

int BuscarVariable(char *var, char *e[]) /*busca una variable en el entorno que se le pasa como parámetro*/
{
   int pos = 0;
   char aux[MAX];

   strcpy(aux, var);
   strcat(aux, "=");

   printf("%s\n", aux);

   while (e[pos] != NULL)
      if (!strncmp(e[pos], aux, strlen(aux)))
      {
         // printf("entering");
         return (pos);
      }
      else
         pos++;
   errno = ENOENT; /*no hay tal variable*/
   return (-1);
}

int CambiarVariable(char *var, char *valor, char *e[]) /*cambia una variable en el entorno que se le pasa como parámetro*/
{                                                      /*lo hace directamente, no usa putenv*/
   int pos;
   char *aux;

   if ((pos = BuscarVariable(var, e)) == -1)
      return (-1);

   if ((aux = (char *)malloc(strlen(var) + strlen(valor) + 2)) == NULL)
      return -1;
   strcpy(aux, var);
   strcat(aux, "=");
   strcat(aux, valor);
   e[pos] = aux;
   return (pos);
}

/*para sistemas donde no hay (o no queremos usuar) execvpe*/
char *Ejecutable(char *s)
{
   char path[MAX];
   static char aux2[MAX];
   struct stat st;
   char *p;
   if (s == NULL || (p = getenv("PATH")) == NULL)
      return s;
   if (s[0] == '/' || !strncmp(s, "./", 2) || !strncmp(s, "../", 3))
      return s; /*is an absolute pathname*/
   strncpy(path, p, MAX);
   for (p = strtok(path, ":"); p != NULL; p = strtok(NULL, ":"))
   {
      sprintf(aux2, "%s/%s", p, s);
      if (lstat(aux2, &st) != -1)
         return aux2;
   }
   return s;
}

int OurExecvpe(const char *file, char *const argv[], char *const envp[])
{
   return (execve(Ejecutable((char *)file), argv, envp));
}

void priority(char *pid, char *valor)
{

   id_t priority;
   int witch, new_priority;
   int ret;

   witch = PRIO_PROCESS;

   if (pid == NULL)
   {
      priority = getpid();
      ret = getpriority(witch, priority);
      if (ret == -1)
         perror("No se pudo mostrar la prioridad del proceso");
      else
         printf("La prioridad del proceso %d es %d\n", priority, ret);
   }
   else
   {
      if (valor == NULL)
      {
         priority = (id_t)strtoul(pid, NULL, 10);
         ret = getpriority(witch, priority);
         if (ret == -1)
            perror("No se pudo mostrar la prioridad del proceso");
         else
            printf("La prioridad del proceso %d es %d\n", priority, ret);
      }
      else
      {
         priority = (id_t)strtoul(pid, NULL, 10);
         new_priority = strtoul(valor, NULL, 10);
         ret = setpriority(witch, priority, new_priority);
         if (ret == -1)
            perror("Error");
      }
   }
}

void MuestraEntorno(char **entorno, char *nombre_entorno)
{
   int i = 0;

   while (entorno[i] != NULL)
   {
      printf("%p->%s[%d]=(%p) %s\n", &entorno[i],
             nombre_entorno, i, entorno[i], entorno[i]);
      i++;
   }
}

void showvar(char *var, char *env[])
{

   int ret = 0;

   if (var == NULL)
   {
      MuestraEntorno(env, "main arg3");
   }

   else if ((ret = BuscarVariable(var, environ)) != -1)
   {
      printf("Con arg3 main %s (%p) @%p\n", env[ret], env[ret], &env[ret]);
      printf("con environ %s (%p) @%p\n", environ[ret], environ[ret], &environ[ret]);
      printf("con getenv %s (%p)\n", getenv(var), getenv(var));
   }
}

void changevar(char *atribute, char *var, char *valor, char *env[])
{
   char *aux = malloc(100);

   if (atribute == NULL || var == NULL)
      printf("Uso: changevar [-a|-e|-p] var valor\n");

   else if (!strcmp(atribute, "-a"))
   {
      CambiarVariable(var, valor, env);
   }
   else if (!strcmp(atribute, "-e"))
   {
      CambiarVariable(var, valor, environ);
   }
   else if (!strcmp(atribute, "-p"))
   {
      strcpy(aux, var);
      strcat(aux, "=");
      strcat(aux, valor);
      putenv(aux);
   }
   free(aux);
}

void showenv(char *param, char *env[])
{

   if (param == NULL)
   {
      MuestraEntorno(env, "main arg3");
   }
   else if (!strcmp(param, "-environ"))
   {
      MuestraEntorno(environ, "main arg3");
   }
   else if (!strcmp(param, "-addr"))
   {
      printf("environ: %p (almacenado en %p)\n", environ, &environ);
      printf("main arg3: %p (almacenado en %p)\n", env, &env);
   }
}

void execute(char *chunks[])
{
   char *string;
   string = malloc(120);

   if (chunks[1] != NULL)
   {
      strcpy(string, "/usr/bin/");
      strcat(string, chunks[1]);

      if (OurExecvpe(string, &chunks[1], __environ) == -1)
      {
         free(string);
         perror("Imposible ejecutar");
      }
      else
      {
         free(string);
         exit(0);
      }
   }
   else printf("Error: Comando no valido\n");
}

void copyString(char *argv[], char *chunks[], int pos, int n)  
{
   int i = pos;

   while (i < n)
   {
      argv[i] = malloc(100);
      strcpy(argv[i], chunks[i]);
      i++;
   }
}

int copyVars(char *argv[], char *chunks[], int n)
{
   int i = 0;

   while (i < n)
   {
      if (strcmp(chunks[i], "HOME") == 0)
      {
         argv[i] = malloc(100);
         strcpy(argv[i], chunks[i]);
         i++;
      }
      else
         break;
   }
   return i;
}

void freeArray(char *argv[])
{
   int i = 0;
   while (argv[i] != NULL)
   {
      free(argv[i]);
      i++;
   }
}

void check_option(bool *bg, bool *priority, char *chunks[])
{
   /* Check the different options of the process command.*/

   int cnt;
   for (cnt = 0; chunks[cnt] != NULL; cnt++)
   {
      if (chunks[cnt] == NULL)
         break;
      else if (!strncmp(chunks[cnt], "@", 1))
         *priority = true;
      else if (!strcmp(chunks[cnt], "&"))
         *bg = true;
   }
}

void process(char *chunks[], tListP *L)
{

   bool bg = 0, priority = 0;
   int pos = 0, cnt = 0, pos_var = 0;
   char *cmd[MAX];
   char *vars[MAX];
   char *string;
   pid_t pid;
   time_t t = time(NULL);
   struct tm tiempoLocal = *localtime(&t);
   char fechaHora[70];

   check_option(&bg, &priority, chunks);

   while (chunks[cnt] != NULL) // comprobe the position where there are an & or @.
   {
      if (!strncmp(chunks[cnt], "@", 1) || !strcmp(chunks[cnt], "&"))
      {
         pos = cnt;
         break;
      }
      cnt++;
   }

   if (priority == true && bg == true) // case with @ and &
   {

      pos_var = copyVars(vars, chunks, pos); // pos_var is the position where end environ vars
      copyString(cmd, chunks, pos_var, pos); // here begin the program

      vars[pos_var] = NULL;
      cmd[cnt] = NULL;

      string = malloc(100);

      if ((pid = fork()) == 0)
      {
         strcpy(string, "/usr/bin/");
         if (*vars == NULL)
            strcat(string, cmd[0]);
         else
            strcat(string, cmd[1]);

         if (setpriority(PRIO_PROCESS, getpid(), atoi(chunks[pos - 1])) == -1)
         {
            perror("Error");
         }

         if (OurExecvpe(string, *vars == NULL ? &cmd[0] : &cmd[1], *vars == NULL ? __environ : vars) == -1)
         {
            perror("Imposible ejecutar");
            free(string);
         }
         else
         {
            free(string);
         }
      }

      strftime(fechaHora, sizeof(fechaHora), "%Y/%m/%d %H:%M:%S", &tiempoLocal);

      if (*vars == NULL)
         insertP(L, pid, getlogin(), priority, fechaHora, *cmd);
      else
         insertP(L, pid, getlogin(), priority, fechaHora, cmd[1]);
   }
   else if (bg == true) // case with &
   {
      pos_var = copyVars(vars, chunks, pos);
      copyString(cmd, chunks, pos_var, pos);

      if (pos_var != 0)
         vars[pos_var] = NULL;

      cmd[cnt] = NULL;

      string = malloc(100);

      if ((pid = fork()) == 0)
      {
         strcpy(string, "/usr/bin/");
         strcat(string, cmd[0]);

         if (OurExecvpe(string, cmd, __environ) == -1)
         {
            perror("Imposible ejecutar");
            free(string);
         }
         else
         {
            free(string);
         }
      }

      strftime(fechaHora, sizeof(fechaHora), "%Y/%m/%d %H:%M:%S", &tiempoLocal);

      insertP(L, pid, getlogin(), priority, fechaHora, *cmd);
   }
   else if (priority == true) // case with @
   {
      pos_var = copyVars(vars, chunks, pos);
      copyString(cmd, chunks, pos_var, pos);

      if (pos_var != 0)
         vars[pos_var] = NULL;

      cmd[cnt] = NULL;

      string = malloc(100);

      if ((pid = fork()) == 0)
      {
         strcpy(string, "/usr/bin/");
         strcat(string, cmd[0]);

         if (setpriority(PRIO_PROCESS, getpid(), atoi(chunks[pos - 1])) == -1)
         {
            perror("Error");
         }

         if (OurExecvpe(string, cmd, NULL) == -1)
         {
            perror("Imposible ejecutar");
            free(string);
         }
         else
         {
            free(string);
         }
      }
      else
         waitpid(pid, NULL, 0);
   }
   else // Case without & y @
   {
      pos_var = copyVars(vars, chunks, cnt);
      copyString(cmd, chunks, pos_var, cnt);

      if (pos_var != 0)
         vars[pos_var] = NULL;

      cmd[cnt] = NULL;

      string = malloc(100);

      if ((pid = fork()) == 0)
      {
         strcpy(string, "/usr/bin/");
         strcat(string, cmd[0]);

         if (OurExecvpe(string, cmd, __environ) == -1)
         {
            perror("Imposible ejecutar");
            free(string);
         }
         else
         {
            free(string);
         }
      }
      else
         waitpid(pid, NULL, 0);
   }
}

void listjobs(tListP L)
{
   printAllListP(L);
}

void deljobs(char *param, tListP *L)
{
   posP p, p2;

   if (param == NULL)
   {
      printAllListP(*L);
   }
   else if (!strcmp(param, "-term"))
   {
      p = 0;
      while (!isEmptyListP(*L))
      {
         p2 = findEnd(*L);
         deletePosP(L, p2);
         p++;
      }
   }
   else if (!strcmp(param, "-sig"))
   {
      p = 0;
      while (!isEmptyListP(*L))
      {
         p2 = findEndForSignal(*L);
         deletePosP(L, p2);
         p++;
      }
   }
}

void job(char *fg, char *pid, tListP L)
{
   int pids;
   posP p;
   int state = 0;

   if (fg == NULL)
   {
      // Muestra la lista de procesos
      printAllListP(L);
   }
   else if (!strcmp(fg, "-fg"))
   {
      if (pid == NULL)
         printAllListP(L);

      else
      {
         // Esperar a que ese proceso acabe, tras eso se elimina de la lista.
         //   Y los pasas a primer plano
         pids = (int)strtol(pid, NULL, 10);
         waitpid(pids, NULL, 0);
         if (WIFEXITED(state) != 0)
         {
            printf("El proceso termino con el estado %d\n", WEXITSTATUS(state));
         }
         else if (WIFSIGNALED(state) != 0)
         {
            printf("El proceso recibio la señal %s\n", NombreSenal(WTERMSIG(state)));
         }
         else if (WIFSTOPPED(state) != 0)
         {
            printf("El proceso recibio la señal %s", NombreSenal(WSTOPSIG(state)));
         }
         // Eliminar de la lista
      }
   }
   else
   {
      pids = (int)strtol(fg, NULL, 10);
      p = findPid(L, pids);
      if (p != -1)
         printf("%d\t%s p=%d %s %s (%d) %s\n", getItemP(p, L)->pid, getItemP(p, L)->user, getItemP(p, L)->priority, getItemP(p, L)->datetimeLaunching, getItemP(p, L)->status, getItemP(p, L)->retSignal, getItemP(p, L)->commandLine);
      else
         printf("El pid no esta en la lista\n");
      // Es un pid, lo busco en la lista y lo muestro
   }
}

void imprimirPromp()
{
   printf(">");
}

void leerEntrada(char *string, char *chunks[], tList *L)
{

   fgets(string, 250, stdin);
   while (!strcmp(string, "\n")) // Caso de cadena vacia
   {
      imprimirPromp();
      fgets(string, 250, stdin);
   }
   if (!insert(L, string))
   {
      printf("Error\n");
   }
}

void ProcesarCadena(char *string, char *chunks[], char *env[], bool *end, tList *L, tListM *L2, tListP *L3)
{

   TrocearCadena(string, chunks);

   if (!strcmp(chunks[0], "autores"))
   {
      autores(chunks[1]);
   }
   else if (!strcmp(chunks[0], "pid"))
   {
      pid(chunks[1]);
   }
   else if (!strcmp(chunks[0], "infosis"))
   {
      infosis();
   }
   else if (!strcmp(chunks[0], "carpeta"))
   {
      carpeta(chunks[1]);
   }
   else if (!strcmp(chunks[0], "fecha"))
   {
      fecha(chunks[1]);
   }
   else if (!strcmp(chunks[0], "hist"))
   {
      hist(chunks[1], L);
   }
   else if (!strcmp(chunks[0], "ayuda"))
   {
      ayuda(chunks[1]);
   }
   else if (!strcmp(chunks[0], "comando"))
   {
      comando(env, string, chunks[1], end, *L, *L2, *L3);
   }
   else if (!strcmp(chunks[0], "salir") || !strcmp(chunks[0], "fin") || !strcmp(chunks[0], "bye"))
   {
      *end = true;
      DeleteList(L);
      DeleteListM(L2);
      DeleteListP(L3);
   }
   else if (!strcmp(chunks[0], "create"))
   {
      create(chunks[1], chunks[2]);
   }
   else if (!strcmp(chunks[0], "stat"))
   {
      Stat(chunks[1], chunks[2], chunks[3], chunks);
   }
   else if (!strcmp(chunks[0], "list"))
   {
      list(chunks[1], chunks[2], chunks[3], chunks[4], chunks[5], chunks[6], chunks);
   }
   else if (!strcmp(chunks[0], "delete"))
   {
      delete (chunks);
   }
   else if (!strcmp(chunks[0], "deltree"))
   {
      deltree(chunks);
   }
   else if (!strcmp(chunks[0], "allocate"))
   {
      allocate(chunks[1], chunks[2], chunks[3], L2);
   }
   else if (!strcmp(chunks[0], "deallocate"))
   {
      deallocate(chunks[1], chunks[2], chunks[3], L2);
   }
   else if (!strcmp(chunks[0], "memfill"))
   {
      memfill(chunks[1], chunks[2], chunks[3]);
   }
   else if (!strcmp(chunks[0], "memdump"))
   {
      memdump(chunks[1], chunks[2]);
   }
   else if (!strcmp(chunks[0], "memory"))
   {
      memory(chunks[1], *L2);
   }
   else if (!strcmp(chunks[0], "i-o"))
   {
      i_o(chunks[1], chunks[2], chunks[3], chunks[4], chunks[5]);
   }
   else if (!strcmp(chunks[0], "recurse"))
   {
      int n = atoi(chunks[1]);
      Recursiva(n);
   }
   else if (!strcmp(chunks[0], "priority"))
   {
      priority(chunks[1], chunks[2]);
   }
   else if (!strcmp(chunks[0], "showvar"))
   {
      showvar(chunks[1], env);
   }
   else if (!strcmp(chunks[0], "changevar"))
   {
      changevar(chunks[1], chunks[2], chunks[3], env);
   }
   else if (!strcmp(chunks[0], "showenv"))
   {
      showenv(chunks[1], env);
   }
   else if (!strcmp(chunks[0], "fork"))
   {
      Cmd_fork(NULL, L3);
   }
   else if (!strcmp(chunks[0], "execute"))
   {
      execute(chunks);
   }
   else if (!strcmp(chunks[0], "listjobs"))
   {
      listjobs(*L3);
   }
   else if (!strcmp(chunks[0], "deljobs"))
   {
      deljobs(chunks[1], L3);
   }
   else if (!strcmp(chunks[0], "job"))
   {
      job(chunks[1], chunks[2], *L3);
   }
   else
   {
      process(chunks, L3);
   }
}

int main(int argc, char *argv[], char *env[])
{

   bool end = false;
   tList *L;
   tList M;
   L = &M;

   tListM *L2;
   tListM M2;
   L2 = &M2;

   tListP *L3;
   tListP M3;
   L3 = &M3;

   char *chunks[MAX];

   createEmptyList(L);
   createEmptyListM(L2);
   createEmptyListP(L3);

   char *string = (char *)malloc(250);

   while (end != true)
   {
      imprimirPromp();
      leerEntrada(string, chunks, L);
      ProcesarCadena(string, chunks, env, &end, L, L2, L3);
   }
   free(string);

   return 0;
}