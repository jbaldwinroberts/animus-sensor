#ifndef __READY_STATE_H_
#define __READY_STATE_H_

#include "AbstractState.h"
#include "States.h"

class ReadyState : public AbstractState
{
  public:
    ReadyState();
    virtual AbstractState* enter();
    virtual AbstractState* update();
    virtual AbstractState* exit();
};

#endif
