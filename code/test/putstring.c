#include "syscall.h"
void print (char* c)
{
    PutString(c);
}

int main() {
  print("Hello World. Here Come The Robots. This was a pleasure to work with you. \n");
  print("2nd line string \n");
  return 0;
}
