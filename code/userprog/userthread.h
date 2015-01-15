#ifdef CHANGED

extern int do_UserThreadCreate(int f, int arg);

typedef struct {  
  int function;
  int arg;
  int stack_position;
} ParamFunction;

#endif
