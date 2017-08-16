#ifndef __GETTING_ACCESS_KEY_STATE_CPP_
#define __GETTING_ACCESS_KEY_STATE_CPP_

#include "GettingAccessKeyState.h"

GettingAccessKeyState::GettingAccessKeyState() {}

AbstractState* GettingAccessKeyState::enter()
{
    _logger.logMessage(INFO_LOG, "Entering GETTING ACCESS KEY STATE");

    _animusCloud.checkRestart();

    if(Particle.connected()) _animusCloud.sendGetAccessKey();

    _timeout = millis();

    return NULL;
}

AbstractState* GettingAccessKeyState::update()
{
    //_logger.logMessage(INFO_LOG, "Updating GETTING ACCESS KEY STATE");

    if(!Particle.connected()) return new NotConnectedState();
    if(abs(millis() - _timeout) > WEBHOOK_TIMEOUT) return new NotConnectedState();

    _animusCloud.checkRestart();

    Reply reply = _animusCloud.getReply();
    if(reply != NA)
    {
        switch(reply)
        {
            case S_REG2:
                _settings.setRegistration(SETUP);
                //Delay to slow webhooks down to send 10 times per second
                while((millis() - _timeout) < WEBHOOK_INTERVAL) delay(100);
                return new ReadyState();
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
            case E_REG1:
                //Delay to slow webhooks down to send 10 times per second
                while((millis() - _timeout) < WEBHOOK_INTERVAL) delay(100);
                return new RegisteringState();
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
                return new GettingAccessKeyState();
                break;
            case E_REG6:
                return new StartingState();
                break;
            case E_REG9:
                //Delay to slow webhooks down to send 10 times per second
                while((millis() - _timeout) < WEBHOOK_INTERVAL) delay(100);
                return new RegisteringState();
                break;
            case E_AUTH1:
                return new StartingState();
                break;
            case E_AUTH3:
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

AbstractState* GettingAccessKeyState::exit()
{
    _logger.logMessage(INFO_LOG, "Exiting GETTING ACCESS KEY STATE");

    return NULL;
}

#endif
