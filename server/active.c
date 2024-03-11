#include  "active.h"

int active_count = 0;
struct ac active[ACTIVEMAX]={0};

static bool active_isempty(void)
{
  return active_count == 0;
}

static bool active_isfull(void)
{
  return active_count == ACTIVEMAX;
}

static bool active_isdup(struct sockaddr addr)
{
  if(active_isempty())
    return false;

  for(int i =0; i < active_count; ++i){
    if(!strcmp(active[i].addr.sa_data,addr.sa_data))
      return true;
  }
  return false;
}

int active_add(struct sockaddr addr,int fd)
{
  if(active_isfull())
    return -1;

  if(active_isdup(addr))
    return -1;

  struct ac tmp = {
    .addr = addr,
    .fd = fd,
  };

  active[active_count++] = tmp;
  return 0;
}

int active_remove(struct sockaddr addr)
{
  if(active_isempty())
    return 0;
  for(int j=0,i=0; i< active_count; ++i){
    if(!strcmp(active[i].addr.sa_data,addr.sa_data)){
      while( i < active_count){
	j= i+1;
	active[i] = active[j];
      }
      --active_count;
      return 0;
    }
  }
  return -1;
}

struct ac *active_find(struct sockaddr addr)
{
  if(active_isempty())
    return NULL;
  for(int i=0; i < active_count; ++i){
    if(!strcmp(active[i].addr.sa_data,addr.sa_data))
      return &active[i];
  }
  return NULL;
}
