#ifndef __NOT_CONNECTED_STATE_CPP_
#define __NOT_CONNECTED_STATE_CPP_

#include "NotConnectedState.h"

NotConnectedState::NotConnectedState() {}

AbstractState* NotConnectedState::enter()
{
    _logger.logMessage(INFO_LOG, "Entering NOT CONNECTED STATE");

    return NULL;
}

AbstractState* NotConnectedState::update()
{
    //_logger.logMessage(INFO_LOG, "Updating NOT CONNECTED STATE");

    switch(_settings.getRegistration())
    {
        case NOT_REGISTERED:
            return new StartingState();
            break;
        case REGISTERED:
            return new GettingAccessKeyState();
            break;
        case SETUP:
            _storage.saveTempCounts();
            return new ReadyState();
            break;
        default:
            return new StartingState();
            break;
    }
}

AbstractState* NotConnectedState::exit()
{
    _logger.logMessage(INFO_LOG, "Exiting NOT CONNECTED STATE");

    return NULL;
}

#endif
