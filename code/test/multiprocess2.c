#include "syscall.h"
int main() {
 int t1 = -1,t2 = -1,t3 = -1,t4 = -1,t5 = -1,t6 = -1,t7 = -1,t8 = -1,t9 = -1;
 //int t1 = 0,t2 = 0,t3 = 0,t4 = 0,t5 = 0,t6 = 0;
 Create("test10");
 Create("test11");
 Create("test12");
 Create("test13");
 Create("test14");
 Create("test15");
 Create("test16");
 Create("test17");
 Create("test18");
 if ((t1 = Open("test10")) != -1) 
    PutString("open test10\n");
 //if(Close(t1) == -1)
  //  PutString("fail to close t1\n");
 if ((t2 = Open("test11")) != -1)
    PutString("open test11\n");
 //if(Close(t2) == -1)
  //  PutString("fail to close t2\n");
 if ((t3 = Open("test12")) != -1)
    PutString("open test12\n");
 if ((t4 = Open("test13")) != -1)
    PutString("open test13\n");
 if ((t5 = Open("test14")) != -1)
    PutString("open test14\n");
/*
 if(Close(t1) == -1)
    PutString("fail to close t1\n");
 if(Close(t2) == -1)
    PutString("fail to close t2\n");
 if(Close(t3) == -1)
    PutString("fail to close t3\n");
 if(Close(t4) == -1)
    PutString("fail to close t4\n");
 if(Close(t5) == -1)
    PutString("fail to close t5\n");
*/
 if ((t6 = Open("test15")) != -1)
    PutString("open test15\n");
 if ((t7 = Open("test16")) != -1)
    PutString("open test16\n");
 if ((t8 = Open("test17")) != -1)
    PutString("open test17\n");
 if ((t9 = Open("test18")) != -1)
    PutString("open test18\n");
 if(Close(t6) == -1)
    PutString("fail to close t6\n");
 if(Close(t7) == -1)
    PutString("fail to close t7\n");
 if(Close(t8) == -1)
    PutString("fail to close t8\n");
 if(Close(t9) == -1)
    PutString("fail to close t9\n");
 return 0;
}
