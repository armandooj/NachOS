#include "syscall.h"

int main(int argc, char **argv) {

  if (argc == 0)
    PutChar('a');
  else 
    PutChar('b');

  return 0;
}
