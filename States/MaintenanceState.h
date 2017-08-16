#ifndef __MAINTENANCE_STATE_H_
#define __MAINTENANCE_STATE_H_

#include "AbstractState.h"
#include "States.h"

class MaintenanceState : public AbstractState
{
  public:
    MaintenanceState();
    virtual AbstractState* enter();
    virtual AbstractState* update();
    virtual AbstractState* exit();

  private:
    unsigned long _flashingStartTime = 0;
};

#endif
