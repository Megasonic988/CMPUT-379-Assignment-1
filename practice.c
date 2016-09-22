#include <stdio.h>
#include <setjmp.h>
#include <stdlib.h>

int main() {
  printf("%d\n", 0xffffffff);
  jmp_buf env;
  int i;
  i = setjmp(env);
  printf("i = %d\n", i);
  if (i != 0) exit(0);
  longjmp(env, 2);
  printf("%s\n", "here");
}
