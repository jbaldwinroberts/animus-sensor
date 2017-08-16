#ifndef __MAINTENANCE_STATE_CPP_
#define __MAINTENANCE_STATE_CPP_

#include "MaintenanceState.h"

MaintenanceState::MaintenanceState() {}

AbstractState* MaintenanceState::enter()
{
    _logger.logMessage(INFO_LOG, "Entering MAINTENANCE STATE");

    _animusCloud.checkRestart();

    return NULL;
}

AbstractState* MaintenanceState::update()
{
    //_logger.logMessage(INFO_LOG, "Updating MAINTENANCE STATE");

    _animusCloud.checkRestart();

    bool timedOut = _particleCloud.connect();

    if(timedOut == false)
    {
        _logger.logMessage(WARN_LOG, "Connection timed out");
        return new ReadyState();
    }

    if(Particle.connected())
    {
        if(_flashingStartTime == 0)
        {
            _flashingStartTime = millis();
            _logger.logMessage(INFO_LOG, "Starting internet time: " + String(_flashingStartTime));
        }
        else if(abs(millis() - _flashingStartTime) >= (10 * 60 * 1000))
        {
            _logger.logMessage(INFO_LOG, "Finished internet time: " + String(millis()));
            return new ReadyState();
        }
    }

    return NULL;
}

AbstractState* MaintenanceState::exit()
{
    _logger.logMessage(INFO_LOG, "Exiting MAINTENANCE STATE");

    return NULL;
}

#endif
