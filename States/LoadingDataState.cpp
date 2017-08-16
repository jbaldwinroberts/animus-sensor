#ifndef __LOADING_DATA_STATE_CPP_
#define __LOADING_DATA_STATE_CPP_

#include "LoadingDataState.h"

LoadingDataState::LoadingDataState() {}

AbstractState* LoadingDataState::enter()
{
    _logger.logMessage(INFO_LOG, "Entering LOADING DATA STATE");

    return NULL;
}

AbstractState* LoadingDataState::update()
{
    //_logger.logMessage(INFO_LOG, "Updating LOADING DATA STATE");

    if(Particle.connected())
    {
        _externalEeprom.readData();

        if(_externalEeprom.checkInvalidData())
        {
            _externalEeprom.readInvalidData(_storage._invalidCountBuffer);
            if(!_storage._invalidCountBuffer->empty()) return new SendingInvalidDataState();
        }
        else if(!_storage._gateCountBuffer->empty()) return new SendingDataState();
        else return new ReadyState();
    }
    return new NotConnectedState();
}

AbstractState* LoadingDataState::exit()
{
    _logger.logMessage(INFO_LOG, "Exiting LOADING DATA STATE");

    return NULL;
}

#endif
