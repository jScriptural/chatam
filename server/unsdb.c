#include "unsdb.h"

struct uns *uns_head = NULL;
int uns_count = 0;

static bool uns_isempty(void)
{
  return uns_head == NULL;
}

static bool uns_contains(unsigned int uid)
{
  if(uns_isempty())
    return false;
  struct uns *up;
  for(up = uns_head; up != NULL; up=up->next){
    if(up->uid == uid)
      return true;
  }
  return false;
}

void uns_add(unsigned int uid,char *username)
{
  struct uns *p = malloc(sizeof(struct uns));
  
  if(uns_contains(uid))
    return;
  if(uns_isempty()){
    p->uid = uid;
    memset(p->username,0,USERNAMEMAX);
    strncpy(p->username,username,USERNAMEMAX);
    p->next = NULL;
    uns_head = p;
  } else {
    struct uns *tmp = uns_head;
    p->uid = uid;
    memset(p->username,0,USERNAMEMAX);
    strncpy(p->username,username,USERNAMEMAX);
    p->next = tmp;
    uns_head = p;
  }
  ++uns_count;
}


void uns_remove(unsigned int uid)
{
  struct uns *curr,*prev;
  if(uns_isempty())
    return;
  for(prev = NULL,curr=uns_head; curr != NULL; prev = curr,curr=curr->next){
    if(curr->uid == uid){
      prev->next = curr->next;
      free(curr);
      return;
    }
  }
}

int uns_update(void)
{
  FILE *fp;
  struct uns *curr;
  int fd;
  struct flock lck = {
    .l_type = F_WRLCK,
    .l_whence = SEEK_SET,
    .l_start = 0,
    .l_len = 0,
    };

  if((fp = fopen(DB,"wb")) == NULL)
    return -1;
  fd = fileno(fp);
  fcntl(fd,F_SETLKW,&lck);
  setvbuf(fp,NULL,_IONBF,0);
  for(curr=uns_head; curr != NULL; curr=curr->next)
    fwrite(curr,sizeof(struct uns),1,fp);
  lck.l_type = F_UNLCK;
  fcntl(fd,F_SETLKW,&lck);
  fclose(fp);
  return 0;
}

void uns_init(void)
{
  FILE *fp;
  int fd;
  if((fp = fopen(DB,"rb")) == NULL)
    return;
  fd = fileno(fp);
  struct flock lck = {
    .l_type = F_RDLCK,
    .l_whence = SEEK_SET,
    .l_start = 0,
    .l_len = 0,
  };
  fcntl(fd,F_SETLKW,&lck);
  do {
    struct uns *tmp = malloc(sizeof(struct uns));
    if(fread(tmp,sizeof(struct uns),1,fp)){
      tmp->next = uns_head;
      uns_head  = tmp;
      ++uns_count;
    }
  } while (!feof(fp) && !ferror(fp));

  lck.l_type = F_UNLCK;
  fcntl(fd,F_SETLKW,&lck);
  fclose(fp);
}

struct uns *uns_find(unsigned int uid)
{
  if(uns_isempty())
    return NULL;
  struct uns *ptr;
  for(ptr=uns_head; ptr != NULL; ptr=ptr->next){
    if(ptr->uid == uid)
      return ptr;
  }
  return NULL;
}

