#ifndef __LOADING_DATA_STATE_H_
#define __LOADING_DATA_STATE_H_

#include "AbstractState.h"
#include "States.h"

class LoadingDataState : public AbstractState
{
  public:
    LoadingDataState();
    virtual AbstractState* enter();
    virtual AbstractState* update();
    virtual AbstractState* exit();
};

#endif
