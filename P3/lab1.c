

#include "lab0.c"

/** LAB ASSIGMENT 1 IMPLEMENTATION **/

/*******************************************************************/
/*******************************************************************/

void create(char *f, char *name);

void stat_listing(char *names[], int pos, bool islong, bool isAcc, bool show_Links);
void list_listing(char *names, bool islong, bool isAcc, bool show_Links);
void Stat(char *long1, char *link, char *acc, char *names[]);

void recursive_listing(char *path, bool reca, bool recb, bool islong, bool isAcess, bool show_links, bool ishid);
void list_fuction(int pos, char *names, bool isreca, bool isrecb, bool islong, bool isAcess, bool show_links, bool ishid);
void list(char *reca, char *recb, char *hid, char *long1, char *link, char *acc, char *names[]);

void delete (char *names[]);

void remove_recursive();
void deltree(char *names[]);

char LetraTF(mode_t m)
{
   switch (m & S_IFMT)
   { /*and bit a bit con los bits de formato,0170000 */
   case S_IFSOCK:
      return 's'; /*socket */
   case S_IFLNK:
      return 'l'; /*symbolic link*/
   case S_IFREG:
      return '-'; /* fichero normal*/
   case S_IFBLK:
      return 'b'; /*block device*/
   case S_IFDIR:
      return 'd'; /*directorio */
   case S_IFCHR:
      return 'c'; /*char device*/
   case S_IFIFO:
      return 'p'; /*pipe*/
   default:
      return '?'; /*desconocido, no deberia aparecer*/
   }
}
/*las siguientes funciones devuelven los permisos de un fichero en formato rwx----*/
/*a partir del campo st_mode de la estructura stat */
/*las tres son correctas pero usan distintas estrategias de asignación de memoria*/

char *ConvierteModo2(mode_t m)
{
   static char permisos[12];
   strcpy(permisos, "---------- ");

   permisos[0] = LetraTF(m);
   if (m & S_IRUSR)
      permisos[1] = 'r'; /*propietario*/
   if (m & S_IWUSR)
      permisos[2] = 'w';
   if (m & S_IXUSR)
      permisos[3] = 'x';
   if (m & S_IRGRP)
      permisos[4] = 'r'; /*grupo*/
   if (m & S_IWGRP)
      permisos[5] = 'w';
   if (m & S_IXGRP)
      permisos[6] = 'x';
   if (m & S_IROTH)
      permisos[7] = 'r'; /*resto*/
   if (m & S_IWOTH)
      permisos[8] = 'w';
   if (m & S_IXOTH)
      permisos[9] = 'x';
   if (m & S_ISUID)
      permisos[3] = 's'; /*setuid, setgid y stickybit*/
   if (m & S_ISGID)
      permisos[6] = 's';
   if (m & S_ISVTX)
      permisos[9] = 't';

   return permisos;
}

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

void stat_listing(char *names[], int pos, bool islong, bool isAcc, bool show_Links)
{

   /*this is the function used in stat implememtation */

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

            if (islong == true) // long listing
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
            else
            {
               printf("%ld\t%s\n", stats.st_size, names[d]);
            }
         }
         fclose(f);
      }
      else
         perror("stat() Error");
   }
}

void list_listing(char *names, bool islong, bool isAcc, bool show_Links)
{

   /*this is the function used in list implememtation. */

   char link[30] = "";
   FILE *f;
   struct tm lt;
   struct stat stats;
   struct passwd *owner, *group;
   time_t t;

   if ((f = fopen(names, "r")) != NULL)
   {
      if (lstat(names, &stats) == -1)
      {
         perror("stat() Error");
      }
      else
      {
         if (islong == true)
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
                  if (readlink(names, link, sizeof(link)) < 0)
                  {
                     perror("readlink() error");
                  }
                  else
                  {
                     printf("%s  %ld (%ld)  %s  %s  %s  %7ld   %s -> %s\n", tiempo, stats.st_nlink, stats.st_ino,
                            owner->pw_name,
                            group->pw_name, permisos, stats.st_size, names, link);
                  }
               }
               else
                  printf("%s  %ld (%ld)  %s  %s  %s  %7ld  %s\n", tiempo, stats.st_nlink, stats.st_ino, owner->pw_name,
                         group->pw_name, permisos, stats.st_size, names);
            }
            else
            {
               printf("%s  %ld (%ld)  %s  %s  %s  %7ld  %s\n", tiempo, stats.st_nlink, stats.st_ino, owner->pw_name,
                      group->pw_name, permisos, stats.st_size, names);
            }
         }
         else
         {
            printf("%ld\t%s\n", stats.st_size, names);
         }
      }
      fclose(f);
   }
   else
      perror("stat() Error");
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

   int size = (int)sizeof(*names); // number of chunks

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

   if (pos == 0) //No arguments
   {
      printf("%s\n", getcwd(cwd, sizeof(cwd)));
   }
   else
   {
      stat_listing(names, pos, islong, isacc, islink);
   }
}

void list_fuction(int pos, char *names, bool reca, bool recb, bool islong, bool isAcess, bool show_links, bool ishid)
{

   /*Makes the recursive listing*/

   DIR *dir;
   struct stat Stats;
   struct dirent *direc;
   char path[100];

   lstat(names, &Stats);

   if (LetraTF(Stats.st_mode) == 'd')
   {
      strcpy(path, names);
      dir = opendir(names);

      chdir(names);

      if (reca == true)
      {
         printf("***********%s\n", path);
         while ((direc = readdir(dir)) != NULL)
         {
            lstat(direc->d_name, &Stats);
            if (ishid == false) // show not hidden directories
            {
               if (strcmp(direc->d_name, ".") != 0 && strcmp(direc->d_name, "..") != 0 && *direc->d_name != '.')
                  list_listing(direc->d_name, islong, isAcess, show_links);
            }
            else if (ishid == true) // show hidden directories
               list_listing(direc->d_name, islong, isAcess, show_links);
         }
         closedir(dir);
         dir = opendir(".");

         while ((direc = readdir(dir)) != NULL)
         {
            lstat(direc->d_name, &Stats);

            if (!strcmp(direc->d_name, ".") || !strcmp(direc->d_name, ".."))
               continue;

            strcat(path, "/");
            strcat(path, direc->d_name);

            if (S_ISDIR(Stats.st_mode))
            {
               if (ishid == false) // not entry in hidden directories
               {
                  if (strcmp(direc->d_name, ".") != 0 && strcmp(direc->d_name, "..") != 0 && *direc->d_name != '.')
                     list_fuction(pos, direc->d_name, reca, recb, islong, isAcess, show_links, ishid);
               }
               else if (ishid == true && strcmp(direc->d_name, ".") != 0 && strcmp(direc->d_name, "..") != 0) // show hidden files
                  list_fuction(pos, direc->d_name, reca, recb, islong, isAcess, show_links, ishid);
            }
         }
      }
      else if (recb == true)
      {
         while ((direc = readdir(dir)) != NULL)
         {
            lstat(direc->d_name, &Stats);

            if (!strcmp(direc->d_name, ".") || !strcmp(direc->d_name, ".."))
               continue;

            if (S_ISDIR(Stats.st_mode))
            {
               strcat(path, "/");
               if (ishid == false)
               {
                  if (strcmp(direc->d_name, ".") != 0 && strcmp(direc->d_name, "..") != 0 && *direc->d_name != '.')
                  {
                     strcat(path, direc->d_name);
                     list_fuction(pos, direc->d_name, reca, recb, islong, isAcess, show_links, ishid);
                  }
               }
               else if (ishid == true && strcmp(direc->d_name, ".") != 0 && strcmp(direc->d_name, "..") != 0)
               {
                  strcat(path, direc->d_name);
                  list_fuction(pos, direc->d_name, reca, recb, islong, isAcess, show_links, ishid);
               }
            }
         }
         closedir(dir);
         dir = opendir(".");
         printf("***********%s\n", names);
         while ((direc = readdir(dir)) != NULL)
         {
            lstat(direc->d_name, &Stats);
            if (ishid == false)
            {
               if (strcmp(direc->d_name, ".") != 0 && strcmp(direc->d_name, "..") != 0 && *direc->d_name != '.')
                  list_listing(direc->d_name, islong, isAcess, show_links);
            }
            else if (ishid == true)
               list_listing(direc->d_name, islong, isAcess, show_links);
         }
      }
      else
      {
         printf("*************%s\n", names);
         while ((direc = readdir(dir)) != NULL)
         {
            if (ishid == false)
            {
               if (strcmp(direc->d_name, ".") != 0 && strcmp(direc->d_name, "..") != 0 && *direc->d_name != '.')
                  list_listing(direc->d_name, islong, isAcess, show_links);
            }
            else if (ishid == true)
               list_listing(direc->d_name, islong, isAcess, show_links);
         }
      }
      closedir(dir);
      chdir("..");
   }
   else
      list_listing(names, islong, isAcess, show_links);
}

void list(char *reca, char *recb, char *hid, char *long1, char *link, char *acc, char *names[])
{

   char cwd[120];
   int cnt = 0, pos = 0;
   bool isreca = false, isrecb = false, ishid = false, islong = false, isacc = false, islink = false;

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

   if (pos == 0) // No arguments
   {
      printf("%s\n", getcwd(cwd, sizeof(cwd)));
   }
   else
   {
      for (cnt = pos; names[cnt] != NULL; cnt++)
      {
         list_fuction(pos, names[cnt], isreca, isrecb, islong, isacc, islink, ishid);
         puts("\n");
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