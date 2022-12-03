#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"
int count = 0;
int compare(char *a,char *b)
{
int num = 0;

for (int i=0;i<strlen(b);i++)
{
if(a[i]==b[i])
{
num++;
}
}
if(num==strlen(b))
{
return 1;
}
return 0;
}
char*
fmtname(char *path)
{
  static char buf[DIRSIZ+1];
  char *p;

  // Find first character after last slash.
  for(p=path+strlen(path); p >= path && *p != '/'; p--)
    ;
  p++;

  // Return blank-padded name.
  if(strlen(p) >= DIRSIZ)
    return p;
  memmove(buf, p, strlen(p));
  memset(buf+strlen(p), ' ', DIRSIZ-strlen(p));
  return buf;
}

void
nfiles(char *path)
{
  char buf[512], *p;
  int fd;
  struct dirent de;
  struct stat st;

  if((fd = open(path, 0)) < 0){
    fprintf(2, "ls: cannot open %s\n", path);
    return;
  }

  if(fstat(fd, &st) < 0){
    fprintf(2, "ls: cannot stat %s\n", path);
    close(fd);
    return;
  }

  switch(st.type){
  case T_DEVICE:
  case T_FILE:
  case T_DIR:
    if(strlen(path) + 1 + DIRSIZ + 1 > sizeof buf){
      printf("ls: path too long\n");
      break;
    }
    strcpy(buf, path);
    p = buf+strlen(buf);
    *p++ = '/';
    while(read(fd, &de, sizeof(de)) == sizeof(de)){
      if(de.inum == 0)
        continue;
      memmove(p, de.name, DIRSIZ);
      p[DIRSIZ] = 0;
      if(stat(buf, &st) < 0){
        printf("nfiles: cannot stat %s\n", buf);
        continue;
      }

      if((compare(fmtname(buf),".") || compare(fmtname(buf),"..")) != 1)
      {
       count = count + 1;
      }

      if(compare(fmtname(buf),".")==1)
      {
        continue;
      }

     else if(compare(fmtname(buf),"..")==1)
      {
        continue;
      }
      else if((compare(fmtname(buf),".")==0 || compare(fmtname(buf),"..")==0) && st.type==1)
      {
        //Read the content inside directories through recursion
        nfiles(buf);
      }



    }

    break;
    
  }
  close(fd);
}

int
main(int argc, char *argv[])
{
  if(argc>1)
  {
    printf("No arguments should be passed\n);
    exit(0);
  }
  nfiles(".");
  printf("Total number of files in hard disk are : %d\n",count);
  exit(0);
}
