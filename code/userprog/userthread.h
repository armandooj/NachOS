#ifdef CHANGED

extern int do_UserThreadCreate(int f, int arg);
extern void do_UserThreadExit();

typedef struct {  
  int function;
  int arg;
} ParamFunction;

#endif
