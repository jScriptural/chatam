#ifndef _UNSDB_H
#define _UNSDB_H

#include <stdbool.h>
#include <stdlib.h>
#include <fcntl.h>

#define USERNAMEMAX 50
#define DB "/var/log/tellam-unsdb"


struct uns {
  unsigned int uid;
  char username[USERNAMEMAX];
 // char *passwd;
  struct uns *next;
};

extern struct uns *uns_head;
extern int uns_count;

void uns_add(unsigned int uid, char *username);
void uns_remove(unsigned int uid);
int uns_update(void);
struct uns *uns_find(unsigned int uid);
void  uns_init(void);

#endif
