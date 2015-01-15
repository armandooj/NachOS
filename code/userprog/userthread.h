#ifdef CHANGED

int do_UserThreadCreate(int f, int arg);
void do_UserThreadExit();

typedef struct {  
  int function;
  int arg;
} ParamFunction;

#endif
