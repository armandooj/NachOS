#include "syscall.h"

int main() {
  ForkExec("putstring");
  //ForkExec("userpages1");
  return 0;
}
