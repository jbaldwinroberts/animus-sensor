#ifndef __READY_STATE_CPP_
#define __READY_STATE_CPP_

#include "ReadyState.h"

ReadyState::ReadyState() {}

AbstractState* ReadyState::enter()
{
    _logger.logMessage(INFO_LOG, "Entering READY STATE");

    _animusCloud.checkRestart();

    if(Particle.connected())
    {
        _logger.logMessage(INFO_LOG, "Particle connected - syncing time");
        Particle.syncTime();
    }
    _particleCloud.disconnect();

    return NULL;
}

AbstractState* ReadyState::update()
{
    //_logger.logMessage(INFO_LOG, "Updating READY STATE");

    _animusCloud.checkRestart();

    if(Time.hour() == _storage._sendingHour && !_storage._sent)
    {
        bool timedOut = _particleCloud.connect();

        if(timedOut == false)
        {
            _logger.logMessage(WARN_LOG, "Connection timed out");
            _animusCloud.setLastTransmitTime();
            _storage._sent = true;
            return new ReadyState();
        }

        if(Particle.connected())
        {
            _logger.logMessage(INFO_LOG, "Connected");
            _animusCloud.setLastTransmitTime();
            _storage._sent = true;
            _storage.writeSavedHistory();

            if(_settings.getDataIndex()) return new LoadingDataState();
            else if(!_storage._gateCountBuffer->empty()) return new SendingDataState();
            else return new SendingHeartbeatState();
        }
    }

    if(Time.hour() > _storage._sendingHour && _storage._sent) _animusCloud.checkRestart(true);

    return NULL;
}

AbstractState* ReadyState::exit()
{
    _logger.logMessage(INFO_LOG, "Exiting READY STATE");

    return NULL;
}

#endif
