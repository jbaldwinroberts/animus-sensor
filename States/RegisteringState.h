#ifndef __REGISTERING_STATE_H_
#define __REGISTERING_STATE_H_

#include "AbstractState.h"
#include "States.h"

class RegisteringState : public AbstractState
{
  public:
    RegisteringState();
    virtual AbstractState* enter();
    virtual AbstractState* update();
    virtual AbstractState* exit();

  private:
    unsigned long _timeout = 0;
};

#endif
