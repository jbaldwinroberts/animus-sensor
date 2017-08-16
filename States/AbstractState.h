#ifndef __ABSTRACT_STATE_H_
#define __ABSTRACT_STATE_H_

#include "Global.h"

class AbstractState
{
  public:
    virtual ~AbstractState() {}
    virtual AbstractState* enter() = 0;
    virtual AbstractState* update() = 0;
    virtual AbstractState* exit() = 0;
};

#endif
