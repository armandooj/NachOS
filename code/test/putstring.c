#include "syscall.h"
void print (char* c)
{
    PutString(c);
}

int main() {
  print("Hello World. Here Come The Robots. This was a pleasure to work with you. \n");
  return 0; 
}
