#ifdef CHANGED

int do_UserThreadCreate(int f, int arg, int ret_function);
void do_UserThreadExit();

typedef struct {  
  int function;
  int arg;
  int ret_function;
} ParamFunction;

#endif
