#ifndef __STARTING_STATE_CPP_
#define __STARTING_STATE_CPP_

#include "StartingState.h"

StartingState::StartingState() {}

AbstractState* StartingState::enter()
{
    _logger.logMessage(INFO_LOG, "Entering STARTING STATE");

    _animusCloud.checkRestart();

    return NULL;
}

AbstractState* StartingState::update()
{
    //_logger.logMessage(INFO_LOG, "Updating STARTING STATE");

    return new RegisteringState();
}

AbstractState* StartingState::exit()
{
    _logger.logMessage(INFO_LOG, "Exiting STARTING STATE");

    return NULL;
}

#endif
