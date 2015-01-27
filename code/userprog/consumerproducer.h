#ifdef CHANGED

#include "synch.h"
#define SIZE 10 

class ConsumerProducer {

  public:
    ConsumerProducer();
    ~ConsumerProducer();
    void start();

  private:
    void put(char c);
    char get();

    void consumer();
    void producer();

   char buffer[SIZE];
    int count , head, tail ;

    Lock *lock;
    Lock *isEmpty;
    Lock *isFull;
    Condition *conditionFull, *conditionEmpty;    
};

#endif
