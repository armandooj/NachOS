#ifdef CHANGED

#include "consumerproducer.h"
#include "syscall.h"

ConsumerProducer::ConsumerProducer() { 
   lock = new Lock("Lock");    // Semaphore *lock; int owner;  Semaphore *internalLock; 
   isEmpty = new Lock ("isEmpty");
   isFull = new Lock ("isFull");

   conditionFull = new Condition("Full"); //const char *name; Semaphore *internalLock; Semaphore *CV_sleep;int num_sleepers;
   conditionEmpty = new Condition("Empty");
   count = 0;
   head = 0;
   tail = 0;

  // Look at the monitor implementation:
  // http://en.wikipedia.org/wiki/Producer%E2%80%93consumer_problem

// struct lock l;
// struct condition notEmpty;
// struct condition notFull;

}

void ConsumerProducer::consumer() {
  char c='w';
   if(c=='b')
    printf("Just for test");

  while (1) {
    // Consume while it's not empty, otherwise wait on condition empty
    //void Wait (Lock * conditionLock); // these are the 3 operations on 


       lock->Acquire();
     while (count == 0) {
      conditionEmpty->Wait(isEmpty);
         // condition_wait(&notEmpty, &l); 
      }
       count--;
       c = buffer[tail];
       tail++;
       if (tail == SIZE) {
           tail = 0;
       }
//     condition_signal(&notFull, &l);
       conditionFull->Signal(isFull);
       lock->Release();
       //     return c;

  }
}

void ConsumerProducer::producer() {
  char c = 'a';
  if(c=='b')
    printf("Just for test");
  while (1) {
    // Produce while buffer is not full, otherwise wait on condition full

       lock->Acquire();
     while (count == SIZE) {
      conditionFull->Wait(isFull);
       //  condition_wait(&notFull, &l);
     }
     count++;
     buffer[head] = c;
     head++;
     if (head == SIZE) {
         head = 0;
     }
     conditionEmpty->Signal(isEmpty);
   //  condition_signal(&notEmpty, &l);
      lock->Release();

  }
}

void ConsumerProducer::start() {
  // and here loop both funtions
  while(TRUE)
  {
    producer();
    consumer();

  }

}

void put(char c) {
  // lock->Acquire();
  // TODO
  // lock->Wait();
}

char get() {
  // lock->Acquire();
  // TODO
  // lock->Wait();
  return '0';
}
ConsumerProducer::~ConsumerProducer() {
  // Here you are suppposed to delete what you created as this is the distructor 
  // lock->Acquire();
  // TODO
  // lock->Wait();

  delete lock;
  delete conditionFull;
  delete conditionEmpty;
}

#endif


// It should be something like this, I haven't tested it


// char buffer[SIZE];
// int count = 0, head = 0, tail = 0;
// struct lock l;
// struct condition notEmpty;
// struct condition notFull;

// lock_init(&l);
// condition_init(&notEmpty);
// condition_init(&notFull);

// void put(char c) {
//     lock_acquire(&l);
//     while (count == SIZE) {
//         condition_wait(&notFull, &l);
//     }
//     count++;
//     buffer[head] = c;
//     head++;
//     if (head == SIZE) {
//         head = 0;
//     }
//     condition_signal(&notEmpty, &l);
//     lock_release(&l);
// }

// char get() {
//     char c;
//     lock_acquire(&l);
//     while (count == 0) {
//         condition_wait(&notEmpty, &l);
//     }
//     count--;
//     c = buffer[tail];
//     tail++;
//     if (tail == SIZE) {
//         tail = 0;
//     }
//     condition_signal(&notFull, &l);
//     lock_release(&l);
//     return c;
// }