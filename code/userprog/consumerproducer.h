#ifdef CHANGED

#include "synch.h"

class ConsumerProducer {

  public:
    ConsumerProducer();
    ~ConsumerProducer();
    void start();

  private:
    void put();
    char get();

    void consumer();
    void producer();

    Lock *lock;
    Condition *conditionFull, *conditionEmpty;    
};

#endif
