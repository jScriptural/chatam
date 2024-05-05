#include <sys/socket.h>
#include <sys/select.h>
#include <signal.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <unistd.h>
#include <arpa/inet.h>
#include "unsdb.h"
#include "address.h"
#include "routines.h"
#include "err.h"

//GLOBAL VARIABLES
struct uns *user;
struct sockaddr_in *addr;
int clientfd;

static void auth(void);
void init_scr(void);

int main(int argc, char *argv[])
{
  setvbuf(stderr,NULL,_IONBF,0);
  setvbuf(stdout,NULL,_IONBF,0);
  setvbuf(stdin,NULL,_IONBF,0);
  struct addrinfo *ainfo,*aip,*cai,*caip;
  struct msgarg marg;
  int retval, err;
  sigset_t sigmask;
  sigemptyset(&sigmask);
  sigfillset(&sigmask);
  sigdelset(&sigmask,SIGINT);
  auth();

  //SERVER ADDRESS
  if((err = getaddr("127.0.0.1","1024",&ainfo)) < 0)
    err_user((char *) gai_strerror(err));

    //CLIENT ADDRESS
    if((err = getaddr("127.0.0.1",argv[1],&cai)) < 0)
      err_user((char *) gai_strerror(err));

    for(caip = cai; caip != NULL; caip=caip->ai_next){
      if((clientfd = socket(caip->ai_family,caip->ai_socktype,0)) < 0)
	err_sys();
      if(bind(clientfd,caip->ai_addr,caip->ai_addrlen) < 0){
	close(clientfd);
	continue;
      }

      break;
    }
   for(aip = ainfo; aip != NULL; aip = aip->ai_next){
    if(connect(clientfd,aip->ai_addr,aip->ai_addrlen) < 0)
      continue;
    printf("clientfd: %d\n",clientfd);
    break;
  }

  addr = malloc(sizeof(addr));
  socklen_t addrlen = sizeof(struct sockaddr_in);
  if(getsockname(clientfd,(struct sockaddr *)addr,&addrlen) < 0)
    err_sys();

  char dom[INET_ADDRSTRLEN];
  printf("ADDRESS: %u\n",addr->sin_addr.s_addr);
  printf("PORT: %u\n",ntohs(addr->sin_port));
  inet_ntop(AF_INET,&addr->sin_addr,dom,INET_ADDRSTRLEN);
  printf("DOMAIN: %s\r\n\r\n",dom);

  marg.fildes = clientfd;
  marg.user = *user;
  sigprocmask(SIG_BLOCK,&sigmask,NULL);
  while(true){
    fd_set rdset;
    FD_ZERO(&rdset);
    FD_SET(STDIN_FILENO,&rdset);
    FD_SET(clientfd,&rdset);
    retval = pselect(clientfd+1,&rdset,NULL,NULL,NULL,&sigmask);
    if(retval != -1 && retval != 0){
      if(FD_ISSET(STDIN_FILENO,&rdset) && FD_ISSET(clientfd,&rdset)){
	receptr_rtn(clientfd);
	messengr_rtn(marg);
      }else if(FD_ISSET(clientfd,&rdset)){
	receptr_rtn(clientfd);
      }else {
	messengr_rtn(marg);
      }
    }
  }
}






static void auth(void)
{
  char *userid,*endptr;
  char idbuf[100];
  unsigned int id;

  uns_init();
  do {
    printf("Enter userid: ");
    userid = fgets(idbuf,100,stdin);
    id = (unsigned int) strtol(userid,&endptr,10);
  } while((id == 0 && endptr != NULL) || !strcmp(userid,endptr));

  if((user = uns_find(id)) == NULL){
    char username[USERNAMEMAX];
    printf("Enter username: ");
    fgets(username,USERNAMEMAX,stdin);
    while(strchr(username,' ') != NULL || !strcmp(username,"\n")){
      printf("Enter username: ");
      fgets(username,USERNAMEMAX,stdin);
    }
    uns_add(id,username);
    user = uns_find(id);
    uns_update();
  }

  init_scr();
  printf("\nUserID: %u\nName: %s\n",user->uid,user->username);

}



void init_scr(void)
{
  char buf[BUFSIZ];
  memset(buf,0,BUFSIZ);
  strcpy(buf,"\033#3\033[H\033[2J");
  strcat(buf,"\033[0;41m\033[1;32m TELLAM CHAT\033[K\033[0m\n\033[1;32m\n");
  write(STDOUT_FILENO,buf,strlen(buf));
}
