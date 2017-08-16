#ifndef __SENDING_DATA_STATE_CPP_
#define __SENDING_DATA_STATE_CPP_

#include "SendingDataState.h"

SendingDataState::SendingDataState() {}

AbstractState* SendingDataState::enter()
{
    _logger.logMessage(INFO_LOG, "Entering SENDING DATA STATE");

    if(Particle.connected()) _animusCloud.sendData();

    _timeout = millis();

    return NULL;
}

AbstractState* SendingDataState::update()
{
    //_logger.logMessage(INFO_LOG, "Updating SENDING DATA STATE");

    if(!Particle.connected()) return new NotConnectedState();
    if(abs(millis() - _timeout) > WEBHOOK_TIMEOUT) return new NotConnectedState();

    Reply reply = _animusCloud.getReply();
    if(reply != NA)
    {
        switch(reply)
        {
            case S_DATA2:
                _storage.clearTempCounts();
                //Delay to slow webhooks down to send 10 times per second
                while((millis() - _timeout) < WEBHOOK_INTERVAL) delay(100);
                if(!_storage._invalidCountBuffer->empty()) return new SendingInvalidDataState();
                else if(!_storage._gateCountBuffer->empty()) return new SendingDataState();
                else if(_settings.getDataIndex()) return new LoadingDataState();
                else return new ReadyState();
                break;
            case E_NF1:
                _storage.saveTempCounts();
                return new StartingState();
                break;
            case E_NF2:
                _storage.saveTempCounts();
                return new StartingState();
                break;
            case E_NF6:
                _storage.saveTempCounts();
                return new StartingState();
                break;
            case E_NF7:
                _storage.saveTempCounts();
                return new StartingState();
                break;
            case E_NF8:
                _storage.saveTempCounts();
                return new StartingState();
                break;
            case E_NF9:
                _storage.saveTempCounts();
                return new StartingState();
                break;
            case E_REG1:
                _storage.clearTempCounts();
                //Delay to slow webhooks down to send 10 times per second
                while((millis() - _timeout) < WEBHOOK_INTERVAL) delay(100);
                return new RegisteringState();
                break;
            case E_REG4:
                _storage.clearTempCounts();
                //Delay to slow webhooks down to send 10 times per second
                while((millis() - _timeout) < WEBHOOK_INTERVAL) delay(100);
                return new GettingAccessKeyState();
                break;
            case E_REG6:
                return new ReadyState();
                break;
            case E_REG7:
                _storage.saveTempCounts();
                return new ReadyState();
                break;
            case E_REG8:
                _storage.clearTempCounts();
                return new StartingState();
                break;
            case E_REG9:
                _storage.clearTempCounts();
                //Delay to slow webhooks down to send 10 times per second
                while((millis() - _timeout) < WEBHOOK_INTERVAL) delay(100);
                return new RegisteringState();
                break;
            case E_AUTH2:
                _storage.clearTempCounts();
                _settings.setAccessKey("");
                return new StartingState();
                break;
            case E_DATA1:
                _storage.clearTempCounts();
                return new StartingState();
                break;
            case E_DATA2:
                _storage.clearTempCounts();
                return new StartingState();
                break;
            case INV:
                _storage.saveTempCounts();
                return new StartingState();
                break;
            default:
                return new NotConnectedState();
                break;
        }
    }

    return NULL;
}

AbstractState* SendingDataState::exit()
{
    _logger.logMessage(INFO_LOG, "Exiting SENDING DATA STATE");

    return NULL;
}

#endif
