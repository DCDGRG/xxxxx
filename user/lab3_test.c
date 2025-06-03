#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "user/thread.h"

struct lock_t lock; // Renamed from lock_t to struct lock_t due to .h definition
int n_threads, n_passes, cur_turn, cur_pass;

void* thread_fn(void *arg)
{
  int thread_id = (uint64)arg;
  int done = 0;

  while (!done) {
    lock_acquire(&lock);
    if (cur_pass >= n_passes) {
        done = 1;
    }
    else if (cur_turn == thread_id) {
      cur_turn = (cur_turn + 1) % n_threads;
      printf("Round %d: thread %d is passing the token to thread %d\n",
             ++cur_pass, thread_id, cur_turn); // [cite: 86]
    }
    lock_release(&lock);
    sleep(0); // Yield to potentially allow other threads to run
  }
  return 0;
}

int main(int argc, char *argv[])
{
  if (argc < 3) {
    printf("Usage: %s [N_PASSES] [N_THREADS]\n", argv[0]);
    exit(-1);
  }

  n_passes = atoi(argv[1]);
  n_threads = atoi(argv[2]); // [cite: 88]

  cur_turn = 0;
  cur_pass = 0;

  lock_init(&lock); // [cite: 88]

  for (int i = 0; i < n_threads; i++) {
    thread_create(thread_fn, (void*)(uint64)i); // [cite: 88]
  }

  for (int i = 0; i < n_threads; i++) {
    wait(0); // [cite: 89]
  }

  printf("Frisbee simulation has finished, %d rounds played in total\n", n_passes); // [cite: 89]
  exit(0);
}