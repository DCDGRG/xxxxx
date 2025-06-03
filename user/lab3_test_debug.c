#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "user/thread.h"

struct lock_t lock;
int n_threads, n_passes, cur_turn, cur_pass;

void* thread_fn(void *arg)
{
  int thread_id = (uint64)arg;
  int done = 0;
  
  printf("Thread %d started\n", thread_id);  // Debug print
  
  while (!done) {
    lock_acquire(&lock);
    if (cur_pass >= n_passes) {
      done = 1;
    }
    else if (cur_turn == thread_id) {
      cur_turn = (cur_turn + 1) % n_threads;
      printf("Round %d: thread %d is passing the token to thread %d\n",
             ++cur_pass, thread_id, cur_turn);
    }
    lock_release(&lock);
    sleep(0);
  }
  
  printf("Thread %d exiting\n", thread_id);  // Debug print
  return 0;
}

int main(int argc, char *argv[])
{
  if (argc < 3) {
    printf("Usage: %s [N_PASSES] [N_THREADS]\n", argv[0]);
    exit(-1);
  }

  n_passes = atoi(argv[1]);
  n_threads = atoi(argv[2]);
  
  printf("Starting with %d passes and %d threads\n", n_passes, n_threads);  // Debug

  cur_turn = 0;
  cur_pass = 0;

  lock_init(&lock);
  
  printf("Lock initialized, creating threads...\n");  // Debug

  for (int i = 0; i < n_threads; i++) {
    int ret = thread_create(thread_fn, (void*)(uint64)i);
    printf("Created thread %d, ret = %d\n", i, ret);  // Debug
  }

  printf("All threads created, waiting...\n");  // Debug

  for (int i = 0; i < n_threads; i++) {
    int status;
    int pid = wait(&status);
    printf("Thread exited: pid=%d, status=%d\n", pid, status);  // Debug
  }

  printf("Frisbee simulation has finished, %d rounds played in total\n", n_passes);
  exit(0);
}