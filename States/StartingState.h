#ifndef __STARTING_STATE_H_
#define __STARTING_STATE_H_

#include "AbstractState.h"
#include "States.h"

class StartingState : public AbstractState
{
  public:
    StartingState();
    virtual AbstractState* enter();
    virtual AbstractState* update();
    virtual AbstractState* exit();
};

#endif
