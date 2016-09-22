#include <setjmp.h>
#include <stdio.h>
#include <stdlib.h>

#define MEM_RW 0
#define MEM_RO 1
#define MEM_NO 2

unsigned int PAGE_SIZE = 4096;

unsigned char current_mode;
unsigned int current_address = 0;
sigjmp_buf env;

struct memregion {
  void *from;
  void *to;
  unsigned char mode; /* MEM_RW, or MEM_RO, or MEM_NO */
};

int get_mem_layout(struct memregion *regions, unsigned int size);
int get_mem_diff(struct memregion *regions, unsigned int howmany,
                 struct memregion *thediff, unsigned int diffsize);

void segfault_handler(int signo) {
  printf("segmentation fault at address %d, mode: %c\n", current_address, current_mode);
  if (current_mode == '0') {
    exit(0);
  }
  current_address += PAGE_SIZE;

  // reset signal handler to remove signal mask
  struct sigaction segfault_act;
	segfault_act.sa_handler = segfault_handler;
	sigemptyset(&segfault_act.sa_mask);
	sigaction(SIGSEGV, &segfault_act, NULL);

  siglongjmp(env, 0);
}

int main() {
  // create signal handler
  struct sigaction segfault_act;
	segfault_act.sa_handler = segfault_handler;
	sigemptyset(&segfault_act.sa_mask);
	sigaction(SIGSEGV, &segfault_act, NULL);

  struct memregion region;
  get_mem_layout(&region, 1);
}

int get_mem_layout(struct memregion *regions, unsigned int size) {
  while (current_address < 0xffffffff) {
    sigsetjmp(env, 1);

    char *ptr = current_address;
    current_mode = '2';

    char temp = *ptr; // try reading
    current_mode = '1'; // if it hasn't segfaulted, we can safely read

    *ptr = 'a'; // try writing
    current_mode = '0'; // if it hasn't segfaulted, we can safely write
    current_address += PAGE_SIZE;
  }
}
