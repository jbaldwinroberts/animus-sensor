#ifndef __DYNAMIC_QUEUE_CPP_
#define __DYNAMIC_QUEUE_CPP_

#include "DynamicQueue.h"

DynamicQueue::DynamicQueue()
{
    _queue = new std::queue<const char*>;
}

DynamicQueue::~DynamicQueue()
{
    clear();
    delete _queue;
}

bool DynamicQueue::empty() const
{
    return _queue->empty();
}

size_t DynamicQueue::size() const
{
    return _queue->size();
}

const char* DynamicQueue::front() const
{
    return _queue->front();
}

const char* DynamicQueue::back() const
{
    return _queue->back();
}

void DynamicQueue::push(const char* item)
{
    uint16_t len = strlen(item);
    char* itemCopy = new char[len + 1];
    strlcpy(itemCopy,  item, len + 1);
    _queue->push(itemCopy);
}

void DynamicQueue::pop()
{
    delete front();
    _queue->pop();
}

void DynamicQueue::clear()
{
    while(!empty()) pop();
}

#endif
