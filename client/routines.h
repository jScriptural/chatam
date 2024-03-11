#ifndef _ROUTINES_H
#define _ROUTINES_H

#include <stdlib.h>
#include <sys/socket.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <time.h>
#include <netinet/in.h>
#include "unsdb.h"
#include "address.h"

#define MSGSZ 1024
#define MSGBUFSZ 4096

struct msgarg {
  int fildes;
  struct uns user;
};

void messengr_rtn(struct msgarg marg);
void receptr_rtn(int fd);

#endif
