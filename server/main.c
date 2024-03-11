#include <stdlib.h>
#include <sys/stat.h>
#include <stdbool.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>
#include <signal.h>
#include <stdio.h>
#include "err.h"
#include "address.h"
#include "routines.h"
#include "bindsock.h"
#include "active.h"
#include "unsdb.h"


#define QLEN 128


pthread_mutex_t mutex;
pthread_mutexattr_t mutexattr;
pthread_attr_t attr;



int main(void)
{
  pid_t pid;
  struct addrinfo *addrinfo;
  int err,servfd;
  sigset_t mask;

  sigemptyset(&mask);
  sigfillset(&mask);

  if(sigprocmask(SIG_BLOCK,&mask,NULL) < 0)
    err_sys();



  if((err = getaddr("127.0.0.1","1024",&addrinfo)) != 0)
    err_user((char *)gai_strerror(err));

  if((servfd = bindsock(addrinfo)) < 0)
    err_sys();

  if((err = pthread_attr_init(&attr)) != 0)
    err_user((char *)strerror(err));
  if((err = pthread_mutexattr_init(&mutexattr)) != 0)
    err_user((char *)strerror(err));
  if((err = pthread_mutexattr_settype(&mutexattr,PTHREAD_MUTEX_ERRORCHECK)) != 0)
    err_user((char *)strerror(err));
  if((err = pthread_attr_setdetachstate(&attr,PTHREAD_CREATE_DETACHED)) != 0)
    err_user((char *)strerror(err));
  if((err = pthread_mutex_init(&mutex,&mutexattr)) != 0)
    err_user((char *)strerror(err));

  if(listen(servfd,QLEN) < 0)
    err_sys();

 if((pid = fork()) < 0){
    err_user("fork");
  }else if(pid != 0){
    exit(0);
  }
  setsid();
 if(( pid = fork()) < 0){
   err_user("fork");
 }else if(pid != 0){
   exit(0);
 }
  umask(0);
  if(chdir("/") < 0)
    err_sys();

  while(true){
    struct sockaddr addr;
    socklen_t addrlen = sizeof(addr);
    int clientfd = accept(servfd,&addr,&addrlen);
    uns_init();
    
    pthread_mutex_lock(&mutex);
    active_add(addr,clientfd);
    pthread_mutex_unlock(&mutex);
    pthread_t tid;
    if((err = pthread_create(&tid,&attr,start_rtn,&clientfd)) < 0)
      err_log((char*)strerror(err));

  }


}
