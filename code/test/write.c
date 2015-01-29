#include "syscall.h"

int main() {
 char *buffer = "1234567890";
 int fd = 0;
 Create("wtest");
 fd = Open("wtest");
 int num = Write(buffer,10,fd);
 PutInt(num);
 Close(fd);
 return 0;
}
