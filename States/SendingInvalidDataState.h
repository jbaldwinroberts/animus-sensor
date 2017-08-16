#ifndef __SENDING_INVALID_DATA_STATE_H_
#define __SENDING_INVALID_DATA_STATE_H_

#include "AbstractState.h"
#include "States.h"

class SendingInvalidDataState : public AbstractState
{
  public:
    SendingInvalidDataState();
    virtual AbstractState* enter();
    virtual AbstractState* update();
    virtual AbstractState* exit();

  private:
    unsigned long _timeout = 0;
};

#endif
