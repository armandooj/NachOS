#include "syscall.h"

int main() {
  ForkExec("../test/userpages0");
  ForkExec("../test/userpages1");
  return 0;
}
