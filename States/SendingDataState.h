#ifndef __SENDING_DATA_STATE_H_
#define __SENDING_DATA_STATE_H_

#include "AbstractState.h"
#include "States.h"

class SendingDataState : public AbstractState
{
  public:
    SendingDataState();
    virtual AbstractState* enter();
    virtual AbstractState* update();
    virtual AbstractState* exit();

  private:
    unsigned long _timeout = 0;
};

#endif
