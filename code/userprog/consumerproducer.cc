#ifdef CHANGED

#include "consumerproducer.h"
#include "syscall.h"

ConsumerProducer::ConsumerProducer() {
  // lock = new Lock("Lock");
  // conditionFull = new Condition("Full");
  // conditionEmpty = new Condition("Empty");

  // Look at the monitor implementation:
  // http://en.wikipedia.org/wiki/Producer%E2%80%93consumer_problem
}

ConsumerProducer::~ConsumerProducer() {
  // lock->Acquire();
  // TODO
  // lock->Wait();
}

void ConsumerProducer::consumer() {
  while (1) {
    // Consume while it's not empty, otherwise wait on condition empty
  }
}

void ConsumerProducer::producer() {
  while (1) {
    // Produce while buffer is not full, otherwise wait on condition full
  }
}

void ConsumerProducer::start() {
  // and here loop both funtions
}

void put() {
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