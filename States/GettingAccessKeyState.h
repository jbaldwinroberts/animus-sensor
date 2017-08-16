#ifndef __GETTING_ACCESS_KEY_STATE_H_
#define __GETTING_ACCESS_KEY_STATE_H_

#include "AbstractState.h"
#include "States.h"

class GettingAccessKeyState : public AbstractState
{
  public:
    GettingAccessKeyState();
    virtual AbstractState* enter();
    virtual AbstractState* update();
    virtual AbstractState* exit();

  private:
    unsigned long _timeout = 0;
};

#endif
