/*
Brais Varela Sieiro         brais.sieiro@udc.es
Alejandro García Bendaña    alejandro.garcia12@udc.es
*/

// anxo.pvila@udc.es

#include "lab1.c"


#define TAMANO 1000

/** LAB ASSIGMENT 2 IMPLEMENTATION **/

/*******************************************************************/
/*******************************************************************/


void Recursiva(int n)
{
   char automatico[TAMANO];
   static char estatico[TAMANO];

   printf("parametro:%3d(%p) array %p, arr estatico %p\n", n, &n, automatico, estatico);

   if (n > 0)
      Recursiva(n - 1);
}

void LlenarMemoria(void *p, size_t cont, unsigned char byte)
{
   unsigned char *arr = (unsigned char *)p;
   size_t i;

   for (i = 0; i < cont; i++)
      arr[i] = byte;
}

void *ObtenerMemoriaShmget(key_t clave, size_t tam, tListM *L, char *timeAllocation, char *typeAllocation, char *nameFile, char *fileDescriptor)
{
   void *p;
   int aux, id, flags = 0777;
   struct shmid_ds s;

   if (tam) /*tam distito de 0 indica crear */
      flags = flags | IPC_CREAT | IPC_EXCL;
   if (clave == IPC_PRIVATE) /*no nos vale*/
   {
      errno = EINVAL;
      return NULL;
   }
   if ((id = shmget(clave, tam, flags)) == -1)
      return (NULL);
   if ((p = shmat(id, NULL, 0)) == (void *)-1)
   {
      aux = errno;
      if (tam)
         shmctl(id, IPC_RMID, NULL);
      errno = aux;
      return (NULL);
   }
   shmctl(id, IPC_STAT, &s);

   char str[MAX];
   sprintf(str, "%d", clave);

   /* Guardar en la lista   InsertarNodoShared (&L, p, s.shm_segsz, clave); */
   insertM(L, p, (unsigned long)s.shm_segsz, timeAllocation, typeAllocation, str, nameFile, fileDescriptor);
   return (p);
}
void do_AllocateCreateshared(char *tr[], tListM *L, char *timeAllocation, char *typeAllocation, char *nameFile, char *fileDescriptor)
{
   key_t cl;
   size_t tam;
   void *p;

   if (tr[0] == NULL || tr[1] == NULL)
   {
      printShared(*L);
      return;
   }

   cl = (key_t)strtoul(tr[0], NULL, 10);
   tam = (size_t)strtoul(tr[1], NULL, 10);
   if (tam == 0)
   {
      printf("No se asignan bloques de 0 bytes\n");
      return;
   }
   if ((p = ObtenerMemoriaShmget(cl, tam, L, timeAllocation, typeAllocation, nameFile, fileDescriptor)) != NULL)
      printf("Asignados %lu bytes en %p\n", (unsigned long)tam, p);
   else
      printf("Imposible asignar memoria compartida clave %lu:%s\n", (unsigned long)cl, strerror(errno));
}

void *MapearFichero(tListM *L, char *timeAllocation, char *typeAllocation, char *key, char *fichero, int protection)
{
   int df, map = MAP_PRIVATE, modo = O_RDONLY;
   struct stat s;
   void *p;

   if (protection & PROT_WRITE)
      modo = O_RDWR;
   if (stat(fichero, &s) == -1 || (df = open(fichero, modo)) == -1)
      return NULL;
   if ((p = mmap(NULL, s.st_size, protection, map, df, 0)) == MAP_FAILED)
      return NULL;

   char file_Descriptor[MAX];
   sprintf(file_Descriptor, "%d", df);

   /* Guardar en la lista    InsertarNodoMmap (&L,p, s.st_size,df,fichero); */
   insertM(L, p, (unsigned long)s.st_size, timeAllocation, typeAllocation, key, fichero, file_Descriptor);

   return p;
}

void do_AllocateMmap(char *arg[], tListM *L, char *timeAllocation, char *typeAllocation, char *key) // allocate -mmap
{
   char *perm;
   void *p;
   int protection = 0;

   if (arg[0] == NULL)
   {
      printMmap(*L);
   }
   if ((perm = arg[1]) != NULL && strlen(perm) < 4)
   {
      if (strchr(perm, 'r') != NULL)
         protection |= PROT_READ;
      if (strchr(perm, 'w') != NULL)
         protection |= PROT_WRITE;
      if (strchr(perm, 'x') != NULL)
         protection |= PROT_EXEC;
   }
   if (arg[1] != NULL && (p = MapearFichero(L, timeAllocation, typeAllocation, key, arg[0], protection)) == NULL)
      perror("Imposible mapear fichero");
   else
      printf("fichero %s mapeado en %p\n", arg[0], p);
}

void do_DeallocateDelkey(char *args[])
{
   key_t clave;
   int id;
   char *key = args[0];

   if (key == NULL || (clave = (key_t)strtoul(key, NULL, 10)) == IPC_PRIVATE)
   {
      printf("      delkey necesita clave_valida\n");
      return;
   }
   if ((id = shmget(clave, 0, 0666)) == -1)
   {
      perror("shmget: imposible obtener memoria compartida");
      return;
   }
   if (shmctl(id, IPC_RMID, NULL) == -1)
      perror("shmctl: imposible eliminar memoria compartida\n");
}

ssize_t LeerFichero(char *f, void *p, size_t cont)
{
   struct stat s;
   ssize_t n;
   int df, aux;

   if (stat(f, &s) == -1 || (df = open(f, O_RDONLY)) == -1)
      return -1;
   if (cont == -1) /* si pasamos -1 como bytes a leer lo leemos entero*/
      cont = s.st_size;
   if ((n = read(df, p, cont)) == -1)
   {
      aux = errno;
      close(df);
      errno = aux;
      return -1;
   }
   close(df);
   return n;
}

void do_I_O_read(char *ar[])
{
   void *p;
   size_t cont = -1;
   ssize_t n;
   if (ar[0] == NULL || ar[1] == NULL)
   {
      printf("faltan parametros\n");
      return;
   }
   // p = cadtop(ar[1]); /*convertimos de cadena a puntero*/
   sscanf(ar[1], "%p", &p);

   if (ar[2] != NULL)
      cont = (size_t)atoll(ar[2]);

   if ((n = LeerFichero(ar[0], p, cont)) == -1)
      perror("Imposible leer fichero");
   else
      printf("leidos %lld bytes de %s en %p\n", (long long)n, ar[0], p);
}

ssize_t EscribirFichero(char *f, void *p, size_t cont, int overwrite)
{
   ssize_t n;
   int df, aux, flags = O_CREAT | O_EXCL | O_WRONLY;

   if (overwrite)
      flags = O_CREAT | O_WRONLY | O_TRUNC;

   if ((df = open(f, flags, 0777)) == -1)
      return -1;

   if ((n = write(df, p, cont)) == -1)
   {
      aux = errno;
      close(df);
      errno = aux;
      return -1;
   }
   close(df);
   return n;
}

void Do_pmap(void) /*sin argumentos*/
{
   pid_t pid; /*hace el pmap (o equivalente) del proceso actual*/
   char elpid[32];
   char *argv[4] = {"pmap", elpid, NULL};

   sprintf(elpid, "%d", (int)getpid());
   if ((pid = fork()) == -1)
   {
      perror("Imposible crear proceso");
      return;
   }
   if (pid == 0)
   {
      if (execvp(argv[0], argv) == -1)
         perror("cannot execute pmap (linux, solaris)");

      argv[0] = "procstat";
      argv[1] = "vm";
      argv[2] = elpid;
      argv[3] = NULL;
      if (execvp(argv[0], argv) == -1) /*No hay pmap, probamos procstat FreeBSD */
         perror("cannot execute procstat (FreeBSD)");

      argv[0] = "procmap", argv[1] = elpid;
      argv[2] = NULL;
      if (execvp(argv[0], argv) == -1) /*probamos procmap OpenBSD*/
         perror("cannot execute procmap (OpenBSD)");

      argv[0] = "vmmap";
      argv[1] = "-interleave";
      argv[2] = elpid;
      argv[3] = NULL;
      if (execvp(argv[0], argv) == -1) /*probamos vmmap Mac-OS*/
         perror("cannot execute vmmap (Mac-OS)");
      exit(1);
   }
   waitpid(pid, NULL, 0);
}

void *allocate(char *option, char *atribute1, char *atribute2, tListM *L);
void deallocate(char *option, char *atribute1, char *atribute2, tListM *L);
void i_o(char *param, char *fiche, char *addr, char *cont, char *o2);
void memdump(char *addr, char *cont);
void memfill(char *addr, char *cont, char *byte);
void memory(char *option, tListM L);
void Recursiva(int n);

void *allocate(char *option, char *atribute1, char *atribute2, tListM *L)
{

   unsigned long size = 0;
   key_t key;
   time_t t = time(NULL);
   struct tm lt = *localtime(&t);
   char tiempo[MAX];
   strftime(tiempo, sizeof(tiempo), "%c", &lt);
   void *p;

   if (option == NULL)
   {
      // mostrar todos los bloques
      printAllList(*L);
   }
   else
   {
      if (strcmp(option, "-malloc") == 0)
      {
         if (atribute1 == NULL)
         {
            // Mostrar bloques malloc
            printMalloc(*L);
         }
         else
         {
            size = (unsigned long)atoi(atribute1);
            p = malloc(size);
            insertM(L, p, size, tiempo, "malloc", " ", " ", " ");
            printf("Asignados %s bytes a la direccion %p\n", atribute1, p);
         }
      }
      else if (strcmp(option, "-createshared") == 0)
      {
         char *atributes[] = {atribute1, atribute2};
         do_AllocateCreateshared(atributes, L, tiempo, "shared", " ", " ");
      }
      else if (strcmp(option, "-mmap") == 0)
      {

         char *atributes[] = {atribute1, atribute2};
         do_AllocateMmap(atributes, L, tiempo, "mmap", " ");
      }
      else if (strcmp(option, "-shared") == 0)
      {
         if (atribute1 == NULL)
         {
            // Mostrar bloques shared
            printShared(*L);
         }
         else
         {
            key = (key_t)strtoul(atribute1, NULL, 10);

            void *p = ObtenerMemoriaShmget(key, size, L, tiempo, "shared", " ", " ");
            if (p == NULL)
            {
               perror("Imposible asignar memoria compartida");
            }
            else
            {
               printf("Memoria compartida de clave %s en %p\n", atribute1, p);
            }
         }
      }
   }
   return p;
}

void deallocate(char *option, char *atribute1, char *atribute2, tListM *L)
{
   unsigned long size = 0;
   posM p;

   if (option == NULL)
   {
      printAllList(*L);
   }
   else
   {
      if (strcmp(option, "-malloc") == 0)
      {
         if (atribute1 == NULL)
         {
            printMalloc(*L);
         }
         else
         {
            size = (unsigned long)atoi(atribute1);
            p = findMalloc(size, *L);
            deletePosM(L, p);
         }
      }
      else if (strcmp(option, "-shared") == 0)
      {
         if (atribute1 == NULL)
         {
            printShared(*L);
         }
         else
         {
            p = findShared(atribute1, *L);
            deletePosM(L, p);
         }
         // Elimina de la lista
      }
      else if (strcmp(option, "-delkey") == 0)
      {
         char *delkey[] = {atribute1};
         do_DeallocateDelkey(delkey);
      }
      else if (strcmp(option, "-mmap") == 0)
      {
         if (atribute1 == NULL)
         {
            printMmap(*L);
         }
         else
         {
            p = findFile(atribute1, *L);
            deletePosM(L, p);
         }
         // Elimina de la lista
      }
      else
      {
         p = findAddr(option, *L);
         deletePosM(L, p);
      }
   }
}

void i_o(char *param, char *fiche, char *addr, char *cont, char *o2)
{
   size_t size = 0;
   ssize_t n;
   void *p;

   if (!strcmp(param, "read"))
   {
      char *cmd[] = {fiche, addr, cont};
      do_I_O_read(cmd);
   }
   else if (!strcmp(param, "write"))
   {
      if (!strcmp(fiche, "-o"))
      {
         sscanf(cont, "%p", &p);
         size = (size_t)strtol(o2, NULL, 10);
         EscribirFichero(addr, p, size, 1);
      }
      else
      {
         sscanf(addr, "%p", &p);
         size = (size_t)strtol(cont, NULL, 10);
         if ((n = EscribirFichero(fiche, p, size, 0)) == -1)
            perror("Error");
         else
            printf("Escritos %ld bytes en %s desde %p\n", (long)size, fiche, p);
      }
   }
}

int a1, b1, c1;

void memory(char *option, tListM L)
{

   posM p;
   static int a, b, c;

   if (option == NULL || strcmp(option, "-all") == 0)
   {
      printf("Variables locales:\t%p,    %p,    %p\n", &option, &L, &p);
      printf("Variables globales:\t%p,    %p,    %p\n", &a1, &b1, &c1);
      printf("Variables estaticas:\t%p,    %p,    %p\n", &a, &b, &c);
      printf("Funciones programa:\t%p,    %p,    %p\n", &allocate, &ConvierteModo2, &TrocearCadena);
      printf("Funciones libreria: \t%p,    %p,    %p\n", &printf, &scanf, &sscanf);
      printAllList(L);
   }
   else if (strcmp(option, "-blocks") == 0)
   {
      printAllList(L);
   }
   else if (strcmp(option, "-funcs") == 0)
   {
      printf("Funciones programa:\t%p,    %p,    %p\n", &allocate, &ConvierteModo2, &TrocearCadena);
      printf("Funciones libreria: \t%p,    %p,    %p\n", &printf, &scanf, &sscanf);
   }
   else if (strcmp(option, "-vars") == 0)
   {
      printf("Variables locales:\t%p,    %p,    %p\n", &option, &L, &p);
      printf("Variables globales:\t%p,    %p,    %p\n", &a1, &b1, &c1);
      printf("Variables estaticas:\t%p,    %p,    %p\n", &a, &b, &c);
   }
   else if (strcmp(option, "-pmap") == 0)
   {
      Do_pmap();
   }
}

void memdump(char *addr, char *cont)
{
   int i;
   void *p;

   if (addr != NULL)
   {
      p = (void *)strtoul(addr, NULL, 16);
      if (cont == NULL)
      {
         for (i = 0; i < 25; i++)
            printf("%c  ", *(char *)(p + i));
         printf("\n");
         for (i = 0; i < 25; i++)
            printf("%02hhx ", *(char *)(p + i));
         printf("\n");
      }
      else
      {
         for (i = 0; i < atoi(cont); i++)
            printf("%c  ", *(char *)(p + i));
         printf("\n");
         for (i = 0; i < atoi(cont); i++)
            printf("%02hhx ", *(char *)(p + i));
         printf("\n");
      }
   }
   else
   {
     printf("Error, direccion nula\n");
   }
}

void memfill(char *addr, char *cont, char *byte)
{

   size_t j;
   void *address;
   unsigned char c;

   sscanf(cont, "%zu", &j);
   sscanf(addr, "%p", &address);

   if (byte[0] == '\'' && byte[2] == '\'')
   {
      c = byte[1];
      printf("LLenando %ld bytes de memoria con el byte (%02hhx) a partir de la direccion %s\n", j, byte[1], addr);
      LlenarMemoria(address, j, c);
   }
   else
   {
      c = '0';
      printf("LLenando %ld bytes de memoria con el byte (00) a partir de la direccion %s\n", j, addr);
      LlenarMemoria(address, j, c);
   }
}

