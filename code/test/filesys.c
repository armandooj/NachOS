#include "syscall.h"

int main() {
  char buffer[20] = {};
  //GetString(buffer,10);
  //Create("test");

  int fd = Open("test");
  PutInt(fd);
  if (fd != -1) {
     Read(buffer,15,fd);
     PutString(buffer);
     Write(buffer,10,fd);
     //GetString(buffer,5);
     //Write(buffer,5,fd);
     Close(fd);
     //PutInt(res);
     PutString("\nok\n");
  }
  else {
     Close(fd);
     PutString("fail\n");
  }
  return 0;
}
