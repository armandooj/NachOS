#include "syscall.h"

int main() {
  ForkExec("putchar");
  //ForkExec("putstring");
  return 0;
}
