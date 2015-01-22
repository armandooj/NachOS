#include "syscall.h"

int main() {
  ForkExec("userpages0");
  ForkExec("userpages1");
  PutChar('e');
  //ForkExec("putstring");
  return 0;
}
