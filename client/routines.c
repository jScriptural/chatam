#include "routines.h"

void receptr_rtn(int fd)
{
  ssize_t nbytes;
  char msgbuf[MSGBUFSZ] = {0};
  fcntl(fd,F_SETFL,O_NONBLOCK);
  while((nbytes = recv(fd,msgbuf,MSGBUFSZ,0)) > 0){
    char m[nbytes+12];
    strcpy(m,"\033[1;33m");
    strcat(m,msgbuf);
    strcat(m,"\033[0m");
    if(write(STDOUT_FILENO,m,strlen(m)) != strlen(m))
      perror("write");
    memset(msgbuf,0,MSGBUFSZ);
  }
  if(nbytes == 0){
    printf("Server  closed\n");
    exit(0);
  }
}

void messengr_rtn(struct msgarg marg)
{
  /*
   * "msgsz:sz\nto:addr\nfrom:uid\ntime:\nmsg:\n"
   */
     char msgbuf[MSGBUFSZ] = {0};
     struct addrinfo *info;
     char msg[MSGSZ] = {0};
     char port[50]={0};
     ssize_t nbytes;
     time_t tim;
     char numstr[100]={0};
     if((nbytes = read(STDIN_FILENO,msg,MSGSZ)) < 0)
       perror("read");
     strcpy(msgbuf,"msgsz:");
     snprintf(numstr,100,"%ld",nbytes);
     strcat(msgbuf,numstr);
     strcat(msgbuf,"\nto:");

     int err;
     printf("RECIPIENT: ");
     fgets(port,50,stdin);
     char *p = strtok(port,"\n");
     if((err=getaddr("127.0.0.1",p,&info)) != 0){
       puts((char*) gai_strerror(err));
       return;
     }

     strcat(msgbuf,info->ai_addr->sa_data);
     strcat(msgbuf,"\nfrom:");
     memset(numstr,0,sizeof(numstr));
     snprintf(numstr,100,"%u",marg.user.uid);
     strcat(msgbuf,numstr);
     strcat(msgbuf,"\ntime:");
     time(&tim);
     snprintf(numstr,100,"%lu",tim);
     strcat(msgbuf,numstr);
     strcat(msgbuf,"\nmsg:");
     strcat(msgbuf,msg);
     if(send(marg.fildes,msgbuf,strlen(msgbuf),0) <0){
       perror("send");
       return;
     }
     printf("sent\n");
} 
