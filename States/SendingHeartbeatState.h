#ifndef __SENDING_HEARTBEAT_STATE_H_
#define __SENDING_HEARTBEAT_STATE_H_

#include "AbstractState.h"
#include "States.h"

class SendingHeartbeatState : public AbstractState
{
  public:
    SendingHeartbeatState();
    virtual AbstractState* enter();
    virtual AbstractState* update();
    virtual AbstractState* exit();

  private:
    unsigned long _timeout = 0;
};

#endif
