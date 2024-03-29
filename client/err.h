#ifndef _ERR_H
#define _ERR_H


#include <errno.h>
#include <stdio.h>
#include <stdlib.h>

typedef char* string; 

void err_quit(const string msg,int line)
{
  char *fmt="\033[1;31merror:\x1b[0m%s:%d:%s\n";
  fprintf(stderr,fmt,getprogname(),line,msg);
  exit(1);	
}

void err_sys(int line)
{
  char *fmt = "\033[1;31merror:\x1b[0m%s:%d:%s\n";
  fprintf(stderr,fmt,getprogname(),line,strerror(errno));
  exit(1);
}

void err_log(const string msg, int line)
{
  char *fmt="\033[1;31merror:\x1b[0m%s:%d:%s\n";
  fprintf(stderr,fmt,getprogname(),line,msg);
}


#define err_sys() err_sys(__LINE__)
#define err_user(msg) err_quit(msg,__LINE__)
#define err_log(msg) err_log(msg,__LINE__);

#endif
