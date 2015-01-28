#include "syscall.h"

int main() {
  char buffer[20] = {};
  Create("test");
  int fd = Open("test");
  if (fd != -1) {
     Read(buffer,15,fd);
     PutString(buffer);
     Write(buffer,10,fd);
     Close(fd);
  }
  else {
     Close(fd);
     PutString("fail\n");
  }
  return 0;
}
