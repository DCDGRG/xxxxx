// user/thread.c
#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "user/thread.h"

int thread_create(void *(start_routine)(void*), void *arg)
{
  // Allocate stack
  void *stack = malloc(4096);
  if(stack == 0)
    return -1;
    
  // Stack grows downward, so point to the top
  // Leave some space for safety
  stack = (char*)stack + 4096 - 64;
  
  printf("thread_create: before clone, stack=%p\n", stack);  // Debug
  
  int pid = clone(stack);
  
  printf("thread_create: after clone, pid=%d\n", pid);  // Debug
  
  if(pid < 0){
    // Free the stack on failure
    free((char*)stack - 4096 + 64);
    return -1;
  }
  
  if(pid == 0){
    // Child thread - execute the start routine
    printf("thread_create: I am child thread!\n");  // Debug
    start_routine(arg);
    exit(0);
  }
  
  // Parent returns the PID of the child thread
  printf("thread_create: in parent, returning %d\n", pid);  // Debug
  return 0;  // 根据规范，父进程应该返回0表示成功
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