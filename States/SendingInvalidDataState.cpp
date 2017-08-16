#ifndef __SENDING_INVALID_DATA_STATE_CPP_
#define __SENDING_INVALID_DATA_STATE_CPP_

#include "SendingInvalidDataState.h"

SendingInvalidDataState::SendingInvalidDataState() {}

AbstractState* SendingInvalidDataState::enter()
{
    _logger.logMessage(INFO_LOG, "Entering SENDING INVALID DATA STATE");

    if(Particle.connected()) _animusCloud.sendInvalidData();

    _timeout = millis();

    return NULL;
}

AbstractState* SendingInvalidDataState::update()
{
    //_logger.logMessage(INFO_LOG, "Updating SENDING INVALID DATA STATE");

    if(!Particle.connected()) return new NotConnectedState();
    if(abs(millis() - _timeout) > WEBHOOK_TIMEOUT) return new NotConnectedState();

    Reply reply = _animusCloud.getReply();
    if(reply != NA)
    {
        switch(reply)
        {
            case S_DATA2:
                _storage.clearInvalidTempData();
                //Delay to slow webhooks down to send 10 times per second
                while((millis() - _timeout) < WEBHOOK_INTERVAL) delay(100);
                if(!_storage._invalidCountBuffer->empty()) return new SendingInvalidDataState();
                else if(!_storage._gateCountBuffer->empty()) return new SendingDataState();
                else if(_settings.getDataIndex()) return new LoadingDataState();
                else return new ReadyState();
                break;
            case E_NF1:
                return new StartingState();
                break;
            case E_NF2:
                return new StartingState();
                break;
            case E_NF6:
                return new StartingState();
                break;
            case E_NF7:
                return new StartingState();
                break;
            case E_NF8:
                return new StartingState();
                break;
            case E_NF9:
                return new StartingState();
                break;
            case E_REG1:
                _storage.clearInvalidTempData();
                //Delay to slow webhooks down to send 10 times per second
                while((millis() - _timeout) < WEBHOOK_INTERVAL) delay(100);
                return new RegisteringState();
                break;
            case E_REG4:
                _storage.clearInvalidTempData();
                //Delay to slow webhooks down to send 10 times per second
                while((millis() - _timeout) < WEBHOOK_INTERVAL) delay(100);
                return new GettingAccessKeyState();
                break;
            case E_REG6:
                return new ReadyState();
                break;
            case E_REG7:
                return new ReadyState();
                break;
            case E_REG8:
                _storage.clearInvalidTempData();
                return new StartingState();
                break;
            case E_REG9:
                _storage.clearInvalidTempData();
                //Delay to slow webhooks down to send 10 times per second
                while((millis() - _timeout) < WEBHOOK_INTERVAL) delay(100);
                return new RegisteringState();
                break;
            case E_AUTH2:
                _storage.clearInvalidTempData();
                _settings.setAccessKey("");
                return new StartingState();
                break;
            case E_DATA1:
                _storage.clearInvalidTempData();
                //Delay to slow webhooks down to send 10 times per second
                while((millis() - _timeout) < WEBHOOK_INTERVAL) delay(100);
                if(!_storage._invalidCountBuffer->empty()) return new SendingInvalidDataState();
                else if(!_storage._gateCountBuffer->empty()) return new SendingDataState();
                else if(_settings.getDataIndex()) return new LoadingDataState();
                else return new ReadyState();
                break;
            case E_DATA2:
                _storage.clearInvalidTempData();
                return new StartingState();
                break;
            case INV:
                return new StartingState();
                break;
            default:
                return new NotConnectedState();
                break;
        }
    }

    return NULL;
}

AbstractState* SendingInvalidDataState::exit()
{
    _logger.logMessage(INFO_LOG, "Exiting SENDING INVALID DATA STATE");

    return NULL;
}

#endif
