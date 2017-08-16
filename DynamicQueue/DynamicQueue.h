#ifndef __DYNAMIC_QUEUE_H_
#define __DYNAMIC_QUEUE_H_

#include "application.h"
#include <queue>

class DynamicQueue
{
  public:
    DynamicQueue();
    virtual ~DynamicQueue();

    bool empty() const;
    size_t size() const;
    const char* front() const;
    const char* back() const;
    void push(const char* item);
    void pop();
    void clear();

  private:
    std::queue<const char*>* _queue;
};

#endif
