#include "routines.h"

static size_t  get_msg(int fd,char *msgbuf, size_t msgbufsz,struct sockaddr *addr,socklen_t *addrlen)
{
  ssize_t nbytes;
  if((nbytes = recvfrom(fd,msgbuf,msgbufsz,0,addr,addrlen)) <=  0)
    return -1;

  return nbytes;
}

static int parse_msg(char *buf,struct message *msg)
{
  /*incoming message format
   * "msgsz:sz\nto:addr\nfrom:uid\ntime:\nmsg:\n"
   */
  char *p;
  if(strlen(buf) == 0)
    return -1;
  char *ptr = strtok(buf,"\n");
  p=strchr(ptr,':');
  ++p;
  msg->msgsz = atoi(p);
  ptr = strtok(NULL,"\n");
  p=strchr(ptr,':');
  ++p;
  msg->to = strdup(p);
  ptr = strtok(NULL,"\n");
  p=strchr(ptr,':');
  ++p;
  msg->from = strdup(p);
  struct uns *tmp;
  if((tmp = uns_find(atoi(msg->from))) == NULL)
    return -1;
  free(msg->from);
  msg->from = tmp->username;
  ptr = strtok(NULL,"\n");
  p=strchr(ptr,':');
  ++p;
  msg->time = (time_t) atoi(p);
  ptr = strtok(NULL,"\n");
  memset(msg->msg,0,MSGSZ);
  p=strchr(ptr,':');
  ++p;
  memcpy(msg->msg,p,MSGSZ);
  return 0;
}

static ssize_t send_msg(struct message *msg)
{
  struct ac *acp;
  ssize_t nbytes;
  struct sockaddr inet;
  char tm[17]={0},msgbuf[2*msg->msgsz];
  memset(&inet,0,sizeof(inet));
  inet.sa_family = AF_INET;
  strncpy(inet.sa_data,msg->to,sizeof(inet.sa_data));
  if((acp= active_find(inet)) == NULL)
    return -1;
  
  strcpy(msgbuf,"\n\t");
  char *from = strtok(msg->from,"\n");
  strcat(msgbuf,from);
  strftime(tm,17," [%r]\n",localtime(&msg->time));
  strcat(msgbuf,tm);
  strcat(msgbuf,":>> ");
  strncat(msgbuf,msg->msg,msg->msgsz);
  strcat(msgbuf,"\r\n\r\n");

  if((nbytes = send(acp->fd,msgbuf,strlen(msgbuf),0)) < strlen(msgbuf))
   return -1;

  return 0;
}



void *start_rtn(void *arg)
{
  int clientfd = *(int *)arg;
  struct message msg;
  ssize_t nbytes;
  struct sockaddr clientaddr;
  socklen_t addrlen = sizeof(clientaddr);
wait:;
  char msgbuf[MSGBUFSZ];
  memset(msgbuf,0,MSGBUFSZ);
  memset(&msg,0,sizeof(msg));
  if((nbytes = get_msg(clientfd,msgbuf,MSGBUFSZ,&clientaddr,&addrlen)) < 0){
    close(clientfd);
    active_remove(clientaddr);
    pthread_exit((void*)1);
  }

  if(parse_msg(msgbuf,&msg) < 0){
    close(clientfd);
    active_remove(clientaddr);
    pthread_exit((void*)1);
  }

  if(send_msg(&msg) < 0)
    perror("sending err");
  goto wait;
}
