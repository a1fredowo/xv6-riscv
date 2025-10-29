#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

#define NPROC 10

void
work(int id, int tickets)
{
  int i, j;
  
  // Set tickets for this process
  settickets(tickets);
  
  printf("Process %d: started with %d tickets\n", id, tickets);
  
  // Do some CPU-intensive work
  for(i = 0; i < 100000; i++) {
    for(j = 0; j < 100; j++) {
      asm("nop");  // Prevent optimization
    }
  }
  
  printf("Process %d: finished (tickets=%d)\n", id, tickets);
  exit(0);
}

int
main(int argc, char *argv[])
{
  int i, pid;
  
  printf("Starting lottery scheduling demo with %d processes\n", NPROC);
  printf("Processes have different ticket counts:\n");
  
  for(i = 0; i < NPROC; i++) {
    int tickets = 50 * (i + 1);  // 50, 100, 150, ..., 500
    
    pid = fork();
    if(pid < 0) {
      printf("fork failed\n");
      exit(1);
    }
    
    if(pid == 0) {
      // Child process
      work(i, tickets);
    }
    
    printf("Created process %d with %d tickets (pid=%d)\n", i, tickets, pid);
  }
  
  // Parent waits for all children
  for(i = 0; i < NPROC; i++) {
    wait(0);
  }
  
  printf("\nAll processes completed!\n");
  printf("Processes with more tickets should have finished faster.\n");
  
  exit(0);
}