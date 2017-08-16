#ifndef __SENDING_HEARTBEAT_STATE_CPP_
#define __SENDING_HEARTBEAT_STATE_CPP_

#include "SendingHeartbeatState.h"

SendingHeartbeatState::SendingHeartbeatState() {}

AbstractState* SendingHeartbeatState::enter()
{
    _logger.logMessage(INFO_LOG, "Entering SENDING HEARTBEAT STATE");

    if(Particle.connected()) _animusCloud.sendHeartbeat();

    _timeout = millis();

    return NULL;
}

AbstractState* SendingHeartbeatState::update()
{
    //_logger.logMessage(INFO_LOG, "Updating SENDING HEARTBEAT STATE");

    if(!Particle.connected()) return new NotConnectedState();
    if(abs(millis() - _timeout) > WEBHOOK_TIMEOUT) return new NotConnectedState();

    Reply reply = _animusCloud.getReply();
    if(reply != NA)
    {
        switch(reply)
        {
            case S_DATA1:
                return new ReadyState();
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
                return new StartingState();
                break;
            case E_REG4:
                return new StartingState();
                break;
            case E_REG6:
                return new ReadyState();
                break;
            case E_REG7:
                return new ReadyState();
                break;
            case E_REG8:
                return new StartingState();
                break;
            case E_REG9:
                return new RegisteringState();
                break;
            case E_AUTH2:
                _settings.setAccessKey("");
                return new StartingState();
                break;
            case E_DATA1:
                return new StartingState();
                break;
            case E_DATA2:
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

AbstractState* SendingHeartbeatState::exit()
{
    _logger.logMessage(INFO_LOG, "Exiting SENDING HEARTBEAT STATE");

    return NULL;
}

#endif
