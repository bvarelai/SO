/*
Brais Varela Sieiro         brais.sieiro@udc.es
Alejandro García Bendaña    alejandro.garcia12@udc.es
*/

#include "histList.c"
#include "helpCommands.c"
#include <fcntl.h>
#include <sys/stat.h>
#include <dirent.h>
#include <pwd.h>

/** LAB ASSIGMENT 1 IMPLEMENTATION **/

/*******************************************************************/
/*******************************************************************/

void create(char *f, char *name);
void Stat(char *long1, char *link, char *acc, char *names[]);
void list(char *reca, char *recb, char *hid, char *long1, char *link, char *acc, char *names[]);
void delete (char *names[]);
void remove_recursive();
void deltree(char *names[]);

void create(char *f, char *name)
{

   char cwd[256];
   errno = 0;

   if (f == NULL)
   {
      // Show path
      printf("%s\n", getcwd(cwd, sizeof(cwd)));
   }
   else if (!strcmp(f, "-f"))
   {
      // file creation
      if (name == NULL)
      {
         printf("%s\n", getcwd(cwd, sizeof(cwd)));
      }
      else
      {
         if (open(name, O_CREAT + O_EXCL, 0775) == -1)
         {
            perror("Create() error");
         }
      }
   }
   else
   {
      // directory creation
      if (mkdir(f, S_IRWXU) == -1)
      {
         perror("Create() error");
      }
   }
}

void long_listing(char *names[], int pos, bool isAcc, bool show_Links)
{

   /*makes a long list of files and directories. */

   char link[30] = "";
   FILE *f;
   int d;
   struct tm lt;
   struct stat stats;
   struct passwd *owner, *group;
   time_t t;

   for (d = pos; names[d] != NULL; d++)
   {
      if ((f = fopen(names[d], "r")) != NULL)
      {
         if (lstat(names[d], &stats) == -1)
         {
            perror("stat() Error");
         }
         else
         {
            // time
            if (isAcc)
               t = stats.st_atime;

            else
               t = stats.st_mtime;

            char tiempo[80];
            localtime_r(&t, &lt);
            strftime(tiempo, sizeof(tiempo), "%Y/%m/%d-%H:%M", &lt);

            // propietario y group
            owner = getpwuid(stats.st_uid);
            group = getpwuid(stats.st_gid);

            // modo
            char *permisos;
            permisos = ConvierteModo2(stats.st_mode);

            if (show_Links == true)
            {
               if (LetraTF(stats.st_mode) == 'l')
               {
                  if (readlink(names[d], link, sizeof(link)) < 0)
                  {
                     perror("readlink() error");
                  }
                  else
                  {
                     printf("%s  %ld (%ld)  %s  %s  %s  %7ld   %s -> %s\n", tiempo, stats.st_nlink, stats.st_ino,
                            owner->pw_name,
                            group->pw_name, permisos, stats.st_size, names[d], link);
                  }
               }
               else
                  printf("%s  %ld (%ld)  %s  %s  %s  %7ld  %s\n", tiempo, stats.st_nlink, stats.st_ino, owner->pw_name,
                         group->pw_name, permisos, stats.st_size, names[d]);
            }
            else
            {
               printf("%s  %ld (%ld)  %s  %s  %s  %7ld  %s\n", tiempo, stats.st_nlink, stats.st_ino, owner->pw_name,
                      group->pw_name, permisos, stats.st_size, names[d]);
            }
         }
         fclose(f);
      }
      else
         perror("stat() Error");
   }
}

void listing(char *names[], int pos, bool isAcc, bool show_Links)
{

   /*makes a list of files and directories(only the size and name).*/

   int cnt;
   FILE *file;
   struct stat Stats;

   for (cnt = pos; names[cnt] != NULL; cnt++)
   {
      if ((file = fopen(names[cnt], "r")))
      {
         if (lstat(names[cnt], &Stats) == -1)
         {
            perror("stat() Error");
         }
         else
         {
            printf("%ld\t%s\n", Stats.st_size, names[cnt]);
         }
      }
      else if (file == NULL)
         perror("stat() Error");
   }
}

void check_options(bool *isreca, bool *isrecb, bool *ishid, bool *islong, bool *islink, bool *isacc, int n, char *names[])
{

   /* Check the different options of the stat and list commands.*/

   int cnt;
   for (cnt = 1; cnt < n; cnt++)
   {
      if (names[cnt] == NULL)
         break;
      else if (strcmp(names[cnt], "-reca") == 0)
         *isreca = true;
      else if (strcmp(names[cnt], "-recb") == 0)
         *isrecb = true;
      else if (strcmp(names[cnt], "-hid") == 0)
         *ishid = true;
      else if (strcmp(names[cnt], "-long") == 0)
         *islong = true;
      else if (strcmp(names[cnt], "-acc") == 0)
         *isacc = true;
      else if (strcmp(names[cnt], "-link") == 0)
         *islink = true;
   }
}

void Stat(char *long1, char *link, char *acc, char *names[])
{
   char cwd[256];
   bool isreca = false, isrecb = false, ishid = false, islong = false, isacc = false, islink = false;
   int cnt = 0, pos = 0;

   int size = (int)sizeof(*names);

   check_options(&isreca, &isrecb, &ishid, &islong, &islink, &isacc, size, names);

   cnt = 1;
   while (names[cnt] != NULL) // comprobe the position where there are a file or directory.
   {
      if (strcmp(names[cnt], "-long") != 0 && strcmp(names[cnt], "-link") != 0 && strcmp(names[cnt], "-acc") != 0)
      {
         pos = cnt;
         break;
      }
      cnt++;
   }

   if (islong == true && islink == true && isacc == true)
   {
      if (pos == 0)
      {
         printf("%s\n", getcwd(cwd, sizeof(cwd)));
      }
      else
      {
         long_listing(names, pos, isacc, islink);
      }
   }
   else if (islong == true && islink == true)
   {
      if (pos == 0)
      {
         printf("%s\n", getcwd(cwd, sizeof(cwd)));
      }
      else
      {
         long_listing(names, pos, isacc, islink);
      }
   }
   else if (islong == true && isacc == true)
   {
      if (pos == 0)
      {
         printf("%s\n", getcwd(cwd, sizeof(cwd)));
      }
      else
      {
         long_listing(names, pos, isacc, islink);
      }
   }
   else if (isacc == true && islink == true)
   {
      if (pos == 0)
      {
         printf("%s\n", getcwd(cwd, sizeof(cwd)));
      }
      else
      {
         listing(names, pos, isacc, islink);
      }
   }
   else if (islong == true)
   {
      if (pos == 0)
      {
         printf("%s\n", getcwd(cwd, sizeof(cwd)));
      }
      else
      {
         long_listing(names, pos, isacc, islink);
      }
   }
   else
   {
      if (pos == 0)
      {
         printf("%s\n", getcwd(cwd, sizeof(cwd)));
      }
      else
      {
         listing(names, pos, isacc, islink);
      }
   }
}

void recursive_listing(void (*listing_fuction)(char *names[], int pos, bool isAcess, bool show_links), char *path, bool isAcess, bool show_links, bool ishid)
{

   /*recursive listing of all subdirectories.*/

   DIR *dir;
   struct stat Stats;
   struct dirent *direc;

   char *d_name[1024];
   int cnt = 0;

   dir = opendir(".");
   if (dir == NULL)
   {
      perror("opendir Error()");
   }
   else
   {
      direc = readdir(dir);
      while (direc != NULL)
      {
         // incializar path
         if (lstat(direc->d_name, &Stats) != -1)
         {

            if (ishid == true) // Here its allow to show the hidden files and directories information
            {
               if (LetraTF(Stats.st_mode) == 'd')
               {
                  if (strcmp(direc->d_name, ".") != 0 && strcmp(direc->d_name, "..") != 0) // To prevent entry in . and .. directories
                  {
                     d_name[cnt] = malloc(1024);
                     strcpy(d_name[cnt], direc->d_name);
                     listing_fuction(&d_name[cnt], 0, isAcess, show_links);
                     chdir(direc->d_name);
                     printf("************%s/%s\n", path, direc->d_name);
                     free(d_name[cnt]);
                     recursive_listing(listing_fuction, path, isAcess, show_links, ishid);
                     chdir("..");
                  }
                  else
                  {
                     d_name[cnt] = malloc(1024);
                     strcpy(d_name[cnt], direc->d_name);
                     listing_fuction(&d_name[cnt], 0, isAcess, show_links);
                     free(d_name[cnt]);
                  }
               }
               else if (LetraTF(Stats.st_mode) == 'd') // Is a file
               {
                  d_name[cnt] = malloc(1024);
                  strcpy(d_name[cnt], direc->d_name);
                  listing_fuction(&d_name[cnt], 0, isAcess, show_links);
                  free(d_name[cnt]);
               }
               else
               {
                  d_name[cnt] = malloc(1024);
                  strcpy(d_name[cnt], direc->d_name);
                  listing_fuction(&d_name[cnt], 0, isAcess, show_links);
                  free(d_name[cnt]);
               }
            }
            else if (ishid == false)
            {

               if (LetraTF(Stats.st_mode) == 'd' && strcmp(direc->d_name, ".") != 0 && strcmp(direc->d_name, "..") != 0 && *direc->d_name != '.') // To prevent entry in hidden directories
               {
                  d_name[cnt] = malloc(1024);
                  strcpy(d_name[cnt], direc->d_name);
                  listing_fuction(&d_name[cnt], 0, isAcess, show_links);
                  chdir(direc->d_name);
                  printf("************%s/%s\n", path, direc->d_name);
                  free(d_name[cnt]);
                  recursive_listing(listing_fuction, path, isAcess, show_links, ishid);
                  chdir("..");
               }
               else if (strcmp(direc->d_name, ".") != 0 && strcmp(direc->d_name, "..") != 0 && *direc->d_name != '.') // To prevent show hidden files and directories
               {
                  d_name[cnt] = malloc(1024);
                  strcpy(d_name[cnt], direc->d_name);
                  listing_fuction(&d_name[cnt], 0, isAcess, show_links);
                  free(d_name[cnt]);
               }
            }
         }
         else
         {
            perror("lstat Error()");
         }
         direc = readdir(dir);
      }
      closedir(dir);
   }
}

void listing_list(void (*listing_fuction)(char *names[], int pos, bool isAcess, bool show_links), int pos, char *names[], bool isAcess, bool show_links, bool ishid)
{

   DIR *dir;
   struct stat Stats;
   struct dirent *direc;
   int cnt;
   dir = opendir(".");
   if (dir == NULL)
   {
      perror("opendir Error()");
   }
   else
   {
      for (cnt = pos; names[cnt] != NULL; cnt++)
      {
         rewinddir(dir);
         direc = readdir(dir);
         while (direc != NULL && strcmp(direc->d_name, names[cnt]) != 0) // ensure directory reading
         {
            direc = readdir(dir);
         }
         if (direc == NULL)
            perror("readdir Error()");

         else
         {
            if (strcmp(direc->d_name, names[cnt]) == 0)
            {
               if (lstat(names[cnt], &Stats) != -1)
               {

                  if (LetraTF(Stats.st_mode) == 'd')
                  {
                     chdir(direc->d_name);
                     printf("************%s\n", direc->d_name);
                     recursive_listing(listing_fuction, direc->d_name, isAcess, show_links, ishid);
                     chdir("..");
                     // listado
                  }
                  else if (LetraTF(Stats.st_mode) == '-')
                  {
                     listing_fuction(&names[cnt], 0, isAcess, show_links);
                  }
                  else
                     listing_fuction(&names[cnt], 0, isAcess, show_links);
               }
            }
            else
               perror("lstat Error()");
         }
      }
   }
   closedir(dir);
}

void list(char *reca, char *recb, char *hid, char *long1, char *link, char *acc, char *names[])
{

   char cwd[120];
   int cnt = 0, pos = 0;
   bool isreca = false, isrecb = 0, ishid = false, islong = false, isacc = false, islink = false;

   int size = (int)sizeof(*names);

   check_options(&isreca, &isrecb, &ishid, &islong, &islink, &isacc, size, names);

   cnt = 1;
   while (names[cnt] != NULL) // comprobe the position where there are a file or directory.
   {
      if (strcmp(names[cnt], "-reca") != 0 && strcmp(names[cnt], "-recb") != 0 && strcmp(names[cnt], "-hid") != 0 && strcmp(names[cnt], "-long") != 0 && strcmp(names[cnt], "-link") != 0 && strcmp(names[cnt], "-acc") != 0)
      {
         pos = cnt;
         break;
      }
      cnt++;
   }

   if (isreca == true && islong == true && islink == true && isacc == true)
   {
      if (pos == 0)
      {
         printf("%s\n", getcwd(cwd, sizeof(cwd)));
      }
      else
      {
         listing_list(long_listing, pos, names, isacc, islink, ishid);
      }
   }
   else if (isrecb == true && islong == true && islink == true && isacc == true)
   {
      if (pos == 0)
      {
         printf("%s\n", getcwd(cwd, sizeof(cwd)));
      }
      else
      {
         listing_list(long_listing, pos, names, isacc, islink, ishid);
      }
   }
   else if (isreca == true && islong == true && islink == true)
   {
      if (pos == 0)
      {
         printf("%s\n", getcwd(cwd, sizeof(cwd)));
      }
      else
      {
         listing_list(long_listing, pos, names, isacc, islink, ishid);
      }
   }
   else if (isrecb == true && islong == true && islink == true)
   {
      if (pos == 0)
      {
         printf("%s\n", getcwd(cwd, sizeof(cwd)));
      }
      else
      {
         listing_list(long_listing, pos, names, isacc, islink, ishid);
      }
   }
   else if (isreca == true && islong == true && isacc == true)
   {
      if (pos == 0)
      {
         printf("%s\n", getcwd(cwd, sizeof(cwd)));
      }
      else
      {
         listing_list(long_listing, pos, names, isacc, islink, ishid);
      }
   }
   else if (isrecb == true && islong == true && isacc == true)
   {
      if (pos == 0)
      {
         printf("%s\n", getcwd(cwd, sizeof(cwd)));
      }
      else
      {
         listing_list(long_listing, pos, names, isacc, islink, ishid);
      }
   }
   else if (isreca == true && islink == true && isacc == true)
   {
      if (pos == 0)
      {
         printf("%s\n", getcwd(cwd, sizeof(cwd)));
      }
      else
      {
         listing_list(listing, pos, names, isacc, islink, ishid);
      }
   }
   else if (isrecb == true && islink == true && isacc == true)
   {
      if (pos == 0)
      {
         printf("%s\n", getcwd(cwd, sizeof(cwd)));
      }
      else
      {
         listing_list(listing, pos, names, isacc, islink, ishid);
      }
   }

   else if (isreca == true && islong == true) // list -reca -reca directorio
   {
      if (pos == 0)
      {
         printf("%s\n", getcwd(cwd, sizeof(cwd)));
      }
      else
      {
         listing_list(long_listing, pos, names, isacc, islink, ishid);
      }
   }
   else if (isrecb == true && islong == true)
   {

      if (pos == 0)
      {
         printf("%s\n", getcwd(cwd, sizeof(cwd)));
      }
      else
      {
         listing_list(long_listing, pos, names, isacc, islink, ishid);
      }
   }
   else if (islong == true)
   {

      if (pos == 0)
      {
         printf("%s\n", getcwd(cwd, sizeof(cwd)));
      }
      else
      {
         listing_list(long_listing, pos, names, isacc, islink, ishid);
      }
   }
   else
   {
      if (pos == 0)
      {
         printf("%s\n", getcwd(cwd, sizeof(cwd)));
      }
      else
      {
         listing_list(listing, pos, names, isacc, islink, ishid);
      }
   }
}
void delete (char *names[])
{
   char cwd[100];
   int i;
   struct stat Stat;

   if (names[1] == NULL)
   {
      printf("%s\n", getcwd(cwd, sizeof(cwd)));
   }
   else
   {
      for (i = 1; names[i] != NULL; i++)
      {
         if (lstat(names[i], &Stat) == 0)
         {
            if (LetraTF(Stat.st_mode) == 'd') // Directory
            {
               if (rmdir(names[i]) == -1)
               {
                  perror("Delete() Error");
               }
            }
            if (LetraTF(Stat.st_mode) == '-') // File
            {
               if (unlink(names[i]) == -1)
               {
                  perror("Delete() Error");
               }
            }
         }
         else
            perror("Delete() Error");
      }
   }
}

void remove_recursive()
{

   DIR *pDir;
   struct dirent *pDirent;
   struct stat Stat;

   pDir = opendir(".");
   if (pDir == NULL)
      printf("Cannot open directory\n");
   else
   {
      pDirent = readdir(pDir);
      while (pDirent != NULL)
      {
         if (lstat(pDirent->d_name, &Stat) != -1)
         {
            if (LetraTF(Stat.st_mode) == 'd' && strcmp(pDirent->d_name, ".") != 0 && strcmp(pDirent->d_name, "..") != 0)
            {
               if (rmdir(pDirent->d_name) == -1)
               {
                  chdir(pDirent->d_name);
                  remove_recursive();
                  chdir("..");
                  rmdir(pDirent->d_name);
               }
            }
            else if (LetraTF(Stat.st_mode) == '-')
            {
               if (remove(pDirent->d_name) == -1)
                  perror("Cannot Delete File");
            }
            else
               unlink(pDirent->d_name);
         }
         pDirent = readdir(pDir);
      }
      closedir(pDir);
   }
}

void deltree(char *names[])
{
   int i;
   struct stat statbuf;
   DIR *pDir;
   struct dirent *pDirent;
   char cwd[100];

   if (names[1] == NULL)
   {
      printf("%s\n", getcwd(cwd, sizeof(cwd)));
   }
   else
   {
      pDir = opendir(".");
      if (pDir == NULL)
         printf("Cannot open directory\n");
      else
      {
         for (i = 1; names[i] != NULL; i++)
         {
            rewinddir(pDir);
            pDirent = readdir(pDir);
            while (pDirent != NULL && strcmp(pDirent->d_name, names[i]) != 0) // ensure directory reading
            {
               pDirent = readdir(pDir);
            }
            if (pDirent == NULL)
               printf("Delete error : %s\n", strerror(2));
            else
            {
               if (strcmp(pDirent->d_name, names[i]) == 0)
               {
                  if (lstat(names[i], &statbuf) != -1)
                  {
                     if (LetraTF(statbuf.st_mode) == 'd' && strcmp(names[i], ".") != 0 &&
                         strcmp(names[i], "..") != 0) // not remove directories "." and ".."
                     {
                        if (rmdir(pDirent->d_name) == -1)
                        {
                           chdir(pDirent->d_name);
                           remove_recursive();
                           chdir("..");
                           rmdir(pDirent->d_name);
                        }
                     }
                     else if (LetraTF(statbuf.st_mode) == '-')
                     {
                        if (remove(names[i]) == -1)
                           perror("Cannot Delete File");
                     }
                     else
                        unlink(names[i]);
                  }
               }
            }
         }
         closedir(pDir);
      }
   }
}

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
void comando(char *string, char *number, bool end, tList L);
void ayuda(char *cmd);
void imprimirPromp();
void leerEntrada(char *string, char *chunks[], tList *L);
void ProcesarCadena(char *string, char *chunks[], bool *end, tList *L);

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

void comando(char *string, char *number, bool end, tList L)
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
         ProcesarCadena(string, cut_up, &end, &L);
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
   else
   {
      printf("%s: Comando no encontrado, para más información teclee 'ayuda ayuda'\n", cmd);
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

void ProcesarCadena(char *string, char *chunks[], bool *end, tList *L)
{

   // char *names[250];

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
      comando(string, chunks[1], end, *L);
   }
   else if (!strcmp(chunks[0], "salir") || !strcmp(chunks[0], "fin") || !strcmp(chunks[0], "bye"))
   {
      *end = true;
      DeleteList(L);
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
   else
   {
      printf("%s: no es un comando del shell\n", chunks[0]);
   }
}

int main(int argv, char *args[])
{

   bool end = false;
   tList *L;
   tList M;
   L = &M;
   createEmptyList(L);

   char *string = (char *)malloc(250);

   while (end != true)
   {
      imprimirPromp();
      leerEntrada(string, args, L);
      ProcesarCadena(string, args, &end, L);
   }
   free(string);

   return 0;
}
