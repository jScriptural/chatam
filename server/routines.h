#ifndef _ROUTINES_H
#define _ROUTINES_H

#include "active.h"
#include <pthread.h>
#include <sys/socket.h>
#include <unistd.h>
#include <sys/stat.h>
#include <netinet/in.h>
#include <string.h>
#include "unsdb.h"
#include "active.h"

#define MSGSZ 1024
#define MSGBUFSZ 4096

struct message {
  size_t msgsz;
  char *from;
  char *to;
  char msg[MSGSZ];
  time_t time;
};

void *start_rtn(void *arg);

#endif
