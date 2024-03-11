#ifndef _ACTIVE_H
#define _ACTIVE_H

#include <netinet/in.h>
#include <stdbool.h>
#include <string.h>

#define ACTIVEMAX 1000

struct ac {
  struct sockaddr addr;
  int fd;
};

extern struct ac active[ACTIVEMAX];
extern int active_count;

int active_add(struct sockaddr addr,int fd);
int active_remove(struct sockaddr addr);
struct ac *active_find(struct sockaddr addr);
#endif
