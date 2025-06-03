#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "user/thread.h"

int thread_create(void *(start_routine)(void*), void *arg)
{
  // Use 4096 instead of PGSIZE in user space.
  void *stack = malloc(4096);
  if(stack == 0)
    return -1;
    
  // Stack grows downward, so point to the top
  stack = (char*)stack + 4096;
  
  int pid = clone(stack);
  if(pid < 0){
    free((char*)stack - 4096);
    return -1;
  }
  
  if(pid == 0){
    // Child thread
    start_routine(arg);
    exit(0);
  }
  
  // Parent
  return 0;
}

void lock_init(struct lock_t* lock)
{
  lock->locked = 0;
}

void lock_acquire(struct lock_t* lock)
{
  while(__sync_lock_test_and_set(&lock->locked, 1) != 0)
    ;
  __sync_synchronize();
}

void lock_release(struct lock_t* lock)
{
  __sync_synchronize();
  lock->locked = 0;
}