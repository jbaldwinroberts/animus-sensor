#ifndef __REGISTERING_STATE_CPP_
#define __REGISTERING_STATE_CPP_

#include "RegisteringState.h"

RegisteringState::RegisteringState() {}

AbstractState* RegisteringState::enter()
{
    _logger.logMessage(INFO_LOG, "Entering REGISTERING STATE");

    _animusCloud.checkRestart();

    if(Particle.connected()) _animusCloud.sendRegister();

    _timeout = millis();

    return NULL;
}

AbstractState* RegisteringState::update()
{
    //_logger.logMessage(INFO_LOG, "Updating REGISTERING STATE");

    if(!Particle.connected()) return new NotConnectedState();
    if(abs(millis() - _timeout) > WEBHOOK_TIMEOUT) return new NotConnectedState();

    _animusCloud.checkRestart();

    Reply reply = _animusCloud.getReply();
    if(reply != NA)
    {
        switch(reply)
        {
            case S_REG1:
                //Delay to slow webhooks down to send 10 times per second
                while((millis() - _timeout) < WEBHOOK_INTERVAL) delay(100);
                _settings.setRegistration(REGISTERED);
                return new GettingAccessKeyState();
                break;
            case E_NF1:
                return new StartingState();
                break;
            case E_NF2:
                return new StartingState();
                break;
            case E_NF3:
                return new StartingState();
                break;
            case E_NF4:
                return new StartingState();
                break;
            case E_NF5:
                return new StartingState();
                break;
            case E_NF9:
                return new StartingState();
                break;
            case E_REG2:
                return new StartingState();
                break;
            case E_REG3:
                return new StartingState();
                break;
            case E_REG4:
                //Delay to slow webhooks down to send 10 times per second
                while((millis() - _timeout) < WEBHOOK_INTERVAL) delay(100);
                _settings.setRegistration(REGISTERED);
                return new GettingAccessKeyState();
                break;
            case E_REG5:
                if(strcmp(_settings.getAccessKey(), ""))
                {
                    _logger.logMessage(INFO_LOG, "Found access key");
                    _settings.setRegistration(SETUP);
                    //Delay to slow webhooks down to send 10 times per second
                    while((millis() - _timeout) < WEBHOOK_INTERVAL) delay(100);
                    return new SendingHeartbeatState();
                }
                else
                {
                    _logger.logMessage(ERROR_LOG, "No access key found");
                    _settings.setRegistration(REGISTERED);
                    //Delay to slow webhooks down to send 10 times per second
                    while((millis() - _timeout) < WEBHOOK_INTERVAL) delay(100);
                    return new GettingAccessKeyState();
                }
                break;
            case E_REG9:
                //Delay to slow webhooks down to send 10 times per second
                while((millis() - _timeout) < WEBHOOK_INTERVAL) delay(100);
                return new GettingAccessKeyState();
                break;
            case E_AUTH1:
                return new StartingState();
                break;
            case E_DATA1:
                return new StartingState();
                break;
            case E_DATA2:
                return new StartingState();
                break;
            default:
                return new NotConnectedState();
                break;
            case INV:
                return new StartingState();
                break;
        }
    }

    return NULL;
}

AbstractState* RegisteringState::exit()
{
    _logger.logMessage(INFO_LOG, "Exiting REGISTERING STATE");

    return NULL;
}

#endif
