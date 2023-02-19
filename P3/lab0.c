#include "histList.c"
#include "memoryList.c"
#include "processList.c"

/** LAB ASSIGMENT 0 IMPLEMENTATION **/

/*******************************************************************/
/*******************************************************************/

int TrocearCadena(char *string, char *chunks[]);
void autores(char *letter);
void pid(char *letter);
void carpeta(char *directory);
void infosis();
void fecha(char *letter);
void hist(char *atribute, tList *L);
void comando(char *env[],char *string, char *number, bool end, tList L, tListM L2,tListP L3);
void ayuda(char *cmd);
void ProcesarCadena(char *string, char *chunks[], char *env[], bool *end, tList *L, tListM *L2,tListP *L3);


int TrocearCadena(char *string, char *chunks[])
{

   int i = 1;

   if ((chunks[0] = strtok(string, " \n\t")) == NULL)
      return 0;
   while ((chunks[i] = strtok(NULL, " \n\t")) != NULL)
      i++;
   return i;
}

void autores(char *letter)
{

   if (letter == NULL)
   {
      printf("Alejandro García Bendaña: alejandro.garcia12\n");
      printf("Brais Varela: brais.sieiro\n");
   }
   else if (strcmp(letter, "-l") == 0)
   {
      printf("Alejandro García Bendaña\nBrais Valera Sieiro\n");
   }
   else if (strcmp(letter, "-n") == 0)
   {
      printf("alejandro.garcia12\nbrais.sieiro\n");
   }
}

void pid(char *letter)
{

   if (letter == NULL)
   {
      printf("Pid del shell: %d\n", getpid());
   }
   else if (strcmp(letter, "-p") == 0)
   {
      printf("Pid del padre del shell: %d\n", getppid());
   }
   else
      printf(" Pid del shell: %d\n", getpid());
}

void carpeta(char *directory)
{

   char cwd[100];
   char partes[100];

   if (directory == NULL)
   {
      if (getcwd(cwd, sizeof(cwd)) == NULL) // saber el directorio principal
         printf("Error\n");
      else
         printf("%s\n", cwd);
   }
   else
   {
      if (chdir(directory) < 0)
      {
         perror("Imposible cambiar de directorio");
      }
      strcpy(partes, "carpeta ");
      strcat(partes, directory);
   }
}

void infosis()
{

   struct utsname buffer;
   errno = 0;
   if (uname(&buffer) < 0)
   { // comprobar si podemos obtener la informacion de nuestra maquina
      perror("uname");
      exit(EXIT_FAILURE);
   }

   printf("%s (%s) %s %s %s\n", buffer.nodename, buffer.machine, buffer.sysname, buffer.release, buffer.version);
}

void fecha(char *letter)
{

   time_t t = time(NULL);
   struct tm tm = *localtime(&t); // almacenamos la hora actual de la maquina.

   if (letter == NULL)
   {
      printf("%d/%02d/%02d\n%02d:%02d:%02d\n", tm.tm_mday, tm.tm_mon + 1, tm.tm_year + 1900, tm.tm_hour, tm.tm_min, tm.tm_sec);
   }
   else if (strcmp(letter, "-d") == 0)
   {
      printf("%d/%02d/%02d\n", tm.tm_mday, tm.tm_mon + 1, tm.tm_year + 1900);
   }
   else if (strcmp(letter, "-h") == 0)
   {
      printf("%02d:%02d:%02d\n", tm.tm_hour, tm.tm_min, tm.tm_sec);
   }
}

void hist(char *attribute, tList *L)
{

   pos i;
   int n;
   char entero[LMAX], *pChar;

   if (attribute == NULL)
   {
      for (i = First(*L); i <= Last(*L); i++)
      {
         printf("%d)->%s\n", i, getItem(i, *L));
      }
   }
   else if (strcmp(attribute, "-c") == 0)
   {
      DeleteList(L);
      printf("Historico Borrado\n");
   }
   else if (strncmp(attribute, "-", 1) == 0) // vemos si existe un numero en la cadena introducida.
   {
      strcpy(entero, attribute);
      n = (int)strtol(&entero[1], &pChar, 10); // obtenemos ese numero y lo almacenamos como un entero.
      if (getItem(n, *L) == NULL)
         printf("Error: %s\n", strerror(22));
      else
      {
         int j = 0;
         for (i = First(*L); j < n; i = Next(*L, i))
         {
            printf("%d)->%s\n", j, getItem(j, *L));
            ++j;
         }
      }
   }
}

void comando(char *env[],char *string, char *number, bool end, tList L, tListM L2,tListP L3)
{

   int n;
   pos i;
   char *pointer;
   char *cut_up[LMAX];

   if (number == NULL)
   {
      for (i = First(L); i <= Last(L); i++)
      {
         printf("%d)->%s\n", i, getItem(i, L));
      }
   }
   else if (isdigit(*number))
   {
      n = (int)strtol(number, &pointer, 10);
      if (getItem(n, L) == NULL)
         printf("Error:%s\n", strerror(22));
      else
      {
         printf("Ejecutando hist (%d)->%s", n, getItem(n, L));
         TrocearCadena(getItem(n, L), cut_up);
         ProcesarCadena(string, cut_up,env, &end, &L, &L2,&L3);
      }
   }
   else
      printf("%s: comando no encontrado\n\r", number);
}

void ayuda(char *cmd)
{

   if (cmd == NULL)
   {
      printf("autores [-l|-n]\npid     [-p]\ncarpeta [direct]\nfecha   [-d|-h]\nayuda   [cmd]\ninfosis\nhist    [-c|-N]\ncomando [N]\nsalir\nfin\nbye\n");
   }
   else if (!strcmp(cmd, "autores"))
   {
      printf("autores [-l|-n]: Enseña por pantalla los nombres y usuarios de los creadores del programa.\n\tautores -l: Enseña por pantalla solo los usuarios.\n\tautores -n: Enseña por pantalla solo los nombres.\n");
   }
   else if (!strcmp(cmd, "pid"))
   {
      printf("pid [-p]: Enseña el pid del proceso ejecutado en el terminal.\n\tpid -p: Enseña el pid del proceso Padre del terminal.\n");
   }
   else if (!strcmp(cmd, "carpeta"))
   {
      printf("carpeta [direct]: Cambia el directorio actual de trabajo por el introducido en [direct].\n\tCuando se escibre sin argumentos enseña por pantalla el directorio actual.\n");
   }
   else if (!strcmp(cmd, "fecha"))
   {
      printf("fecha [-d|-h]: Cuando se escribe sin argumentos enseña por pantalla la hora actual y la fecha actual.\n\tfecha -d: Enseña por pantalla la fecha actual en formato DD/MM/YYYY.\n\tfecha -h: Enseña por pantalla la hora actual en formato hh:mm:ss.\n");
   }
   else if (!strcmp(cmd, "infosis"))
   {
      printf("infosis: Enseña por pantalla la máquina utilizada en el momentos actual\n");
   }
   else if (!strcmp(cmd, "ayuda"))
   {
      printf("ayuda [cmd]: Enseña por pantalla una lista de todos los comandos disponibles del shell.\n\tayuda cmd: Enseña una breve explcacion del uso del comando que se introduzca en [cmd]\n");
   }
   else if (!strcmp(cmd, "hist"))
   {
      printf("hist [-c|-N] : Muestra el Historico de los comandos utilizados en el shell.\n\thist -c: Borra el Historico\n\thist -N: Siendo N un número entero y que referencia el orden de los comandos ejecutados, muestra el Historico solo hasta ese número\n");
   }
   else if (!strcmp(cmd, "comando"))
   {
      printf("comando [N] : Muestra el Historico de los comandos utilizados en el shell.\n\tcomando N: Siendo N un número entero y que referencia el orden de los comandos ejecutados, ejecuta el comando de ese número\n");
   }
   else if (!strcmp(cmd, "salir"))
   {
      printf("salir : Termina la ejecucion del shell\n");
   }
   else if (!strcmp(cmd, "fin"))
   {
      printf("fin : Termina la ejecucion del shell\n");
   }
   else if (!strcmp(cmd, "bye"))
   {
      printf("bye : Termina la ejecucion del shell\n");
   }
   else if (!strcmp(cmd, "create"))
   {
      printf("create [f] [name] : Crea un directorio o un fichero (-f)\n");
   }
   else if (!strcmp(cmd, "delete"))
   {
      printf("delete [name1 name2 ..] : Borra ficheros o directorios vacios\n");
   }
   else if (!strcmp(cmd, "deltree"))
   {
      printf("deltree [name1 name2 ..] : Borra ficheros o directorios no vacios recursivamente\n");
   }
   else if (!strcmp(cmd, "stat"))
   {
      printf("stat [-long] [-link] [-acc] name name2 ... : \tListar ficheros\n\t\t-long: listado largo\n\t\t-acc: accestime\n\t\t-link: si es enlace simbolico, el path contenido\n");
   }
   else if (!strcmp(cmd, "list"))
   {
      printf("list [-reca] [-recb] [-hid] [-long] [-link] [-acc] n1 n2 ... : \tListar contenido de directorios\n\t\t-hid: incluye los ficheros ocultos\n\t\t-reca: recursivo(antes)\n\t\t-recb: recursivo(despues)\n\t\tresto de parametros del stat\n");
   }
   else if (!strcmp(cmd, "allocate"))
   {
      printf("allocate [-malloc|-shared|-createshared|-mmap]... Asigna un bloque de memoria\n\t-malloc tam: asigna un bloque malloc de tamano tam\n\t-createshared cl tam: asigna (creando) el bloque de memoria compartida de clave cl y tamano tam\n\t-shared cl: asigna el bloque de memoria compartida (ya existente) de clave cl\n\t-mmap fich perm: mapea el fichero fich, perm son los permisos\n");
   }
   else if (!strcmp(cmd, "deallocate"))
   {
      printf("deallocate [-malloc|-shared|-delkey|-mmap|addr]... Desasigna un bloque de memoria\n\t-malloc tam: desasigna el bloque malloc de tamano tam\n\t-shared cl: desasigna (desmapea) el bloque de memoria compartida de clave cl\n\t-delkey cl: elimina del sistema (sin desmapear) la clave de memoria cl\n\t-mmap fich: desmapea el fichero mapeado fich\n\taddr: desasigna el bloque de memoria en la direccion addr\n");
   }
   else if (!strcmp(cmd, "i-o"))
   {
      printf("i-o [read|write] [-o] fiche addr cont\n\tread fich addr cont: Lee cont bytes desde fich a addr\n\twrite [-o] fich addr cont: Escribe cont bytes desde addr a fich. -o para sobreescribir\n\t\taddr es una direccion de memoria\n");
   }
   else if (!strcmp(cmd, "memdump"))
   {
      printf("memdump addr cont\nVuelca en pantallas los contenidos (cont bytes) de la posicion de memoria addr\n");
   }
   else if (!strcmp(cmd, "memfill"))
   {
      printf("memfill addr cont byte\tLlena la memoria a partir de addr con byte\n");
   }
   else if (!strcmp(cmd, "memory"))
   {
      printf("memory [-blocks|-funcs|-vars|-all|-pmap] ..\tMuestra muestra detalles de la memoria del proceso\n\t-blocks: los bloques de memoria asignados\n\t-funcs: las direcciones de las funciones\n\t-vars: las direcciones de las variables\n\t-all: todo\n\t-pmap: muestra la salida del comando pmap(o similar)\n");
   }
   else if (!strcmp(cmd, "recurse") || !strcmp(cmd, "recursiva"))
   {
      printf("recurse|recursiva [n]\tInvoca a la funcion recursiva n veces\n");
   }
   else if (!strcmp(cmd, "priority"))
   {
      printf("priority [pid] [valor]\tMuestra o cambia la prioridad del proceso pid a valor\n");
   }
   else if (!strcmp(cmd, "showvar"))
   {
      printf("showvar var1\tMuestra el valor y las direcciones de la variable de entorno var\n");
   }
   else if (!strcmp(cmd, "changevar"))
   {
      printf("changevar [-a|-e|-p] var valor\tCambia el valor de una variable de entorno\n\t-a: accede por el tercer arg de main\n\t-e: accede mediante environ\n\t-p: accede mediante putenv\n");
   }
   else if (!strcmp(cmd, "showenv"))
   {
      printf("showenv [-environ|-addr]\tMuestra el entorno del proceso\n");
   }
   else if (!strcmp(cmd, "fork"))
   {
      printf("fork\tHace una llamada fork para crear un proceso\n");
   }
   else if (!strcmp(cmd, "execute"))
   {
      printf("execute prog args....\tEjecuta, sin crear proceso,prog con argumentos\n");
   }
   else if (!strcmp(cmd, "listjobs"))
   {
      printf("listjobs\tLista los procesos en segundo plano\n");
   }
   else if (!strcmp(cmd, "deljobs"))
   {
      printf("deljobs [-term][-sig]\tElimina los procesos de la lista procesos en sp\n\t-term: los terminados\n\t-sig: los terminados por senal\n");
   }
   else if (!strcmp(cmd, "job"))
   {
      printf("job [-fg] pid\tMuestra informacion del proceso pid.\n\t-fg: lo pasa a primer plano\n");
   }
   else
   {
      printf("%s: Comando no encontrado, para más información teclee 'ayuda ayuda'\n", cmd);
   }
}