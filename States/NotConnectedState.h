#ifndef __NOT_CONNECTED_STATE_H_
#define __NOT_CONNECTED_STATE_H_

#include "AbstractState.h"
#include "States.h"

class NotConnectedState : public AbstractState
{
  public:
    NotConnectedState();
    virtual AbstractState* enter();
    virtual AbstractState* update();
    virtual AbstractState* exit();
};

#endif
