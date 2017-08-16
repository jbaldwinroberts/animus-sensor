#ifndef __ANIMUS_CLOUD_CPP_
#define __ANIMUS_CLOUD_CPP_

#include "AnimusCloud.h"

AnimusCloud::AnimusCloud() {}

AnimusCloud::~AnimusCloud() {}

void AnimusCloud::begin()
{
    _logger.logMessage(INFO_LOG, "Started Animus Cloud initialisation");

    Particle.function("control", &AnimusCloud::control, this);
    Particle.subscribe(System.deviceID(), &AnimusCloud::replyHandler, this, MY_DEVICES);
    _lastTransmitTime = millis();

    _logger.logMessage(INFO_LOG, "Finished Animus Cloud initialisation");
}

int AnimusCloud::control(String command)
{
    _logger.logMessage(INFO_LOG, "Control received");

    if(command == "restart")
    {
        _logger.logMessage(INFO_LOG, "Restart");
        _restart = true;
        return 1;
    }
    else if(command == "reset")
    {
        _logger.logMessage(INFO_LOG, "Reset");
        _settings.setDefault();
        _restart = true;
        return 1;
    }
    else if(command == "deregister")
    {
        _logger.logMessage(INFO_LOG, "Deregister");
        _settings.setRegistration(NOT_REGISTERED);
        _settings.setAccessKey("");
        return 1;
    }
    else if(command == "clearCounts")
    {
        _logger.logMessage(INFO_LOG, "Clear counts");
        _storage.clear();
        return 1;
    }
    else if(command == "enableSdMessageLogging")
    {
        _logger.logMessage(INFO_LOG, "Enabling SD message logging");
        _settings.setSdMessageLogging(true);
        return 1;
    }
    else if(command == "disableSdMessageLogging")
    {
        _logger.logMessage(INFO_LOG, "Disabling SD message logging");
        _settings.setSdMessageLogging(false);
        return 1;
    }
    else if(command == "enableSdFrameLogging")
    {
        _logger.logMessage(INFO_LOG, "Enabling SD frame logging");
        _settings.setSdFrameLogging(true);
        return 1;
    }
    else if(command == "disableSdFrameLogging")
    {
        _logger.logMessage(INFO_LOG, "Disabling SD frame logging");
        _settings.setSdFrameLogging(false);
        return 1;
    }
    else if(command == "enableSavedHistoryAvailable")
    {
        _logger.logMessage(INFO_LOG, "Enabling saved history available");
        _settings.setSavedHistoryAvailable(true);
        return 1;
    }
    else if(command == "disableSavedHistoryAvailable")
    {
        _logger.logMessage(INFO_LOG, "Disabling saved history available");
        _settings.setSavedHistoryAvailable(false);
        return 1;
    }
    else if(command == "enableDebugPrint")
    {
        _logger.logMessage(INFO_LOG, "Enabling debug print");
        _settings.setDebugPrint(true);
        return 1;
    }
    else if(command == "disableDebugPrint")
    {
        _logger.logMessage(INFO_LOG, "Disabling debug print");
        _settings.setDebugPrint(false);
        return 1;
    }
    else if(command == "clearSd")
    {
        _logger.logMessage(INFO_LOG, "Clear SD");
        _logger.clearSd(); //TODO make this return 1. same way as restart function
        return 1;
    }
    else if(command == "percentFullSd")
    {
        int percent = _logger.sdPercentFull();
        _logger.logMessage(INFO_LOG, "Percent full SD: " + String(percent));
        return percent;
    }
    else if(command == "setToStaging")
    {
        _logger.logMessage(INFO_LOG, "Setting to staging");
        _settings.setServer(STAGING);
        return 1;
    }
    else if(command == "setToProduction")
    {
        _logger.logMessage(INFO_LOG, "Setting to production");
        _settings.setServer(PRODUCTION);
        return 1;
    }
    else if(command.startsWith("interval"))
    {
        int16_t timeInterval = atoi(extractParameter(command).c_str());
        _logger.logMessage(INFO_LOG, "Setting time interval: " + String(timeInterval));
        return _settings.setTransmitInterval(timeInterval);
    }
    else if(command.startsWith("binaryThreshold"))
    {
        int16_t binaryThreshold = atoi(extractParameter(command).c_str());
        _logger.logMessage(INFO_LOG, "Setting binary threshold: " + String(binaryThreshold));
        return _settings.setBinaryThreshold(binaryThreshold);
    }
    else if(command.startsWith("doorPos"))
    {
        int16_t doorPosition = atoi(extractParameter(command).c_str());
        _logger.logMessage(INFO_LOG, "Setting door position: " + String(doorPosition));
        return _settings.setDoorPosition(doorPosition);
    }

    else if(command.startsWith("trackAliveTime"))
    {
        int16_t trackAliveTime = atoi(extractParameter(command).c_str());
        _logger.logMessage(INFO_LOG, "Setting track alive time: " + String(trackAliveTime));
        return _settings.setTrackAliveTime(trackAliveTime);
    }
    else if(command.startsWith("trackDeadTime"))
    {
        int16_t trackDeadTime = atoi(extractParameter(command).c_str());
        _logger.logMessage(INFO_LOG, "Setting track dead time: " + String(trackDeadTime));
        return _settings.setTrackDeadTime(trackDeadTime);
    }
    else if(command.startsWith("distanceThreshold"))
    {
        int16_t distanceThreshold = atoi(extractParameter(command).c_str());
        _logger.logMessage(INFO_LOG, "Setting distance threshold: " + String(distanceThreshold));
        return _settings.setDistanceThreshold(distanceThreshold);
    }
    else if(command.startsWith("minDoor"))
    {
        int16_t minDoor = atoi(extractParameter(command).c_str());
        _logger.logMessage(INFO_LOG, "Setting min door: " + String(minDoor));
        return _settings.setMinDoor(minDoor);
    }
    else if(command.startsWith("maxDoor"))
    {
        int16_t maxDoor = atoi(extractParameter(command).c_str());
        _logger.logMessage(INFO_LOG, "Setting max door: " + String(maxDoor));
        return _settings.setMaxDoor(maxDoor);
    }
    else if(command.startsWith("pirTriggers"))
    {
        int16_t pirTriggers = atoi(extractParameter(command).c_str());
        _logger.logMessage(INFO_LOG, "Setting PIR triggers: " + String(pirTriggers));
        return _settings.setPirTriggers(pirTriggers);
    }
    else if(command.startsWith("historyLength"))
    {
        int16_t historyLength = atoi(extractParameter(command).c_str());
        _logger.logMessage(INFO_LOG, "Setting history length: " + String(historyLength));
        return _settings.setHistoryLength(historyLength);
    }
    else if(command.startsWith("minSize"))
    {
        int16_t minSize = atoi(extractParameter(command).c_str());
        _logger.logMessage(INFO_LOG, "Setting minimum size: " + String(minSize));
        return _settings.setMinSize(minSize);
    }
    else if(command.startsWith("contourDeadTime"))
    {
        int16_t contourDeadTime = atoi(extractParameter(command).c_str());
        _logger.logMessage(INFO_LOG, "Setting contour dead time: " + String(contourDeadTime));
        return _settings.setContourDeadTime(contourDeadTime);
    }
    else return -1;
}

String AnimusCloud::extractParameter(const String& reply)
{
    int index = reply.indexOf('=');
    if(index == -1) return "";
    else return reply.substring(index + 1);
}

void AnimusCloud::sendHeartbeat()
{
    char* buffer = new char[255];
    char itoaBuffer[4];
    if(strlcpy(buffer,  "{\"msg_no\":", 255) >= 255) _logger.logMessage(ERROR_LOG, "Send heartbeat truncated");
    itoa(_msgNo, itoaBuffer, 10);
    if(strlcat(buffer, itoaBuffer, 255) >= 255) _logger.logMessage(ERROR_LOG, "Send heartbeat truncated");
    if(strlcat(buffer, ", \"access_key\":\"", 255) >= 255) _logger.logMessage(ERROR_LOG, "Send heartbeat truncated");
    if(strlcat(buffer, _settings.getAccessKey(), 255) >= 255) _logger.logMessage(ERROR_LOG, "Send heartbeat truncated");
    if(strlcat(buffer, "\", \"sensor_data\":{}}", 255) >= 255) _logger.logMessage(ERROR_LOG, "Send heartbeat truncated");
    _logger.logMessage(INFO_LOG, "Sending heartbeat: " + String(buffer));

    if(Particle.connected())
    {
        if(_settings.getServer() == PRODUCTION) Particle.publish("PROD_sendData", buffer, 60, PRIVATE);
        else Particle.publish("STAGING_sendData", buffer, 60, PRIVATE);
    }

    delete[] buffer;

    _lastTransmitTime = millis();
    if(_msgNo < 9999) _msgNo++;
    else _msgNo = 0;
}

void AnimusCloud::sendRegister()
{
    char* buffer = new char[255];
    char itoaBuffer[4];
    if(strlcpy(buffer,  "{\"msg_no\":", 255) >= 255) _logger.logMessage(ERROR_LOG, "Send register truncated");
    itoa(_msgNo, itoaBuffer, 10);
    if(strlcat(buffer, itoaBuffer, 255) >= 255) _logger.logMessage(ERROR_LOG, "Send register truncated");
    if(strlcat(buffer, ", \"sensor_model\":\"", 255) >= 255) _logger.logMessage(ERROR_LOG, "Send register truncated");
    if(strlcat(buffer, MODEL, 255) >= 255) _logger.logMessage(ERROR_LOG, "Send register truncated");
    if(strlcat(buffer, "\", \"sensor_revision\":\"", 255) >= 255) _logger.logMessage(ERROR_LOG, "Send register truncated");
    if(strlcat(buffer, HARDWARE_REVISION, 255) >= 255) _logger.logMessage(ERROR_LOG, "Send register truncated");
    if(strlcat(buffer, "\", \"registration_key\":\"", 255) >= 255) _logger.logMessage(ERROR_LOG, "Send register truncated");
    if(strlcat(buffer, REGISTRATION_KEY, 255) >= 255) _logger.logMessage(ERROR_LOG, "Send register truncated");
    if(strlcat(buffer, "\"}", 255) >= 255) _logger.logMessage(ERROR_LOG, "Send register truncated");
    _logger.logMessage(INFO_LOG, "Sending register: " + String(buffer));

    if(Particle.connected())
    {
        if(_settings.getServer() == PRODUCTION) Particle.publish("PROD_register", buffer, 60, PRIVATE);
        else Particle.publish("STAGING_register", buffer, 60, PRIVATE);
    }

    delete[] buffer;

    if(_msgNo < 9999) _msgNo++;
    else _msgNo = 0;
}

void AnimusCloud::sendGetAccessKey()
{
    char* buffer = new char[255];
    char itoaBuffer[4];
    if(strlcpy(buffer,  "{\"msg_no\":", 255) >= 255) _logger.logMessage(ERROR_LOG, "Send get access key truncated");
    itoa(_msgNo, itoaBuffer, 10);
    if(strlcat(buffer, itoaBuffer, 255) >= 255) _logger.logMessage(ERROR_LOG, "Send get access key truncated");
    if(strlcat(buffer, ", \"sensor_model\":\"", 255) >= 255) _logger.logMessage(ERROR_LOG, "Send get access key truncated");
    if(strlcat(buffer, MODEL, 255) >= 255) _logger.logMessage(ERROR_LOG, "Send get access key truncated");
    if(strlcat(buffer, "\", \"sensor_revision\":\"", 255) >= 255) _logger.logMessage(ERROR_LOG, "Send get access key truncated");
    if(strlcat(buffer, HARDWARE_REVISION, 255) >= 255) _logger.logMessage(ERROR_LOG, "Send get access key truncated");
    if(strlcat(buffer, "\", \"registration_key\":\"", 255) >= 255) _logger.logMessage(ERROR_LOG, "Send get access key truncated");
    if(strlcat(buffer, REGISTRATION_KEY, 255) >= 255) _logger.logMessage(ERROR_LOG, "Send get access key truncated");
    if(strlcat(buffer, "\"}", 255) >= 255) _logger.logMessage(ERROR_LOG, "Send get access key truncated");
    _logger.logMessage(INFO_LOG, "Sending getAccessKey: " + String(buffer));

    if(Particle.connected())
    {
        if(_settings.getServer() == PRODUCTION) Particle.publish("PROD_accessKey", buffer, 60, PRIVATE);
        else Particle.publish("STAGING_accessKey", buffer, 60, PRIVATE);
    }

    delete[] buffer;

    if(_msgNo < 9999) _msgNo++;
    else _msgNo = 0;
}

void AnimusCloud::sendData()
{
    char* buffer = new char[255];
    char itoaBuffer[4];
    if(strlcpy(buffer,  "{\"msg_no\":", 255) >= 255) _logger.logMessage(ERROR_LOG, "Send data truncated");
    itoa(_msgNo, itoaBuffer, 10);
    if(strlcat(buffer, itoaBuffer, 255) >= 255) _logger.logMessage(ERROR_LOG, "Send data truncated");
    if(strlcat(buffer, ", \"access_key\":\"", 255) >= 255) _logger.logMessage(ERROR_LOG, "Send data truncated");
    if(strlcat(buffer, _settings.getAccessKey(), 255) >= 255) _logger.logMessage(ERROR_LOG, "Send data truncated");
    if(strlcat(buffer, "\", \"sensor_data\":{\"counts\":\"", 255) >= 255) _logger.logMessage(ERROR_LOG, "Send data truncated");

    while((strlen(buffer) + strlen(_storage._gateCountBuffer->front())) < 250)
    {
        if(strlcat(buffer, _storage._gateCountBuffer->front(), 255) >= 255) _logger.logMessage(ERROR_LOG, "Send data truncated");
        _storage._transmitGateCountBuffer->push(_storage._gateCountBuffer->front());
        _storage._gateCountBuffer->pop();
        if(_storage._gateCountBuffer->empty()) break;
    }
    if(strlcat(buffer, "\"}}", 255) >= 255) _logger.logMessage(ERROR_LOG, "Send data truncated");

    _logger.logMessage(INFO_LOG, "Sending data: " + String(buffer));

    if(Particle.connected())
    {
        if(_settings.getServer() == PRODUCTION) Particle.publish("PROD_sendData", buffer, 60, PRIVATE);
        else Particle.publish("STAGING_sendData", buffer, 60, PRIVATE);
    }

    delete[] buffer;

    _lastTransmitTime = millis();
    if(_msgNo < 9999) _msgNo++;
    else _msgNo = 0;
}

void AnimusCloud::sendInvalidData()
{
    char* buffer = new char[255];
    char itoaBuffer[4];
    if(strlcpy(buffer,  "{\"msg_no\":", 255) >= 255) _logger.logMessage(ERROR_LOG, "Send invalid data truncated");
    itoa(_msgNo, itoaBuffer, 10);
    if(strlcat(buffer, itoaBuffer, 255) >= 255) _logger.logMessage(ERROR_LOG, "Send invalid data truncated");
    if(strlcat(buffer, ", \"access_key\":\"", 255) >= 255) _logger.logMessage(ERROR_LOG, "Send invalid data truncated");
    if(strlcat(buffer, _settings.getAccessKey(), 255) >= 255) _logger.logMessage(ERROR_LOG, "Send invalid data truncated");
    if(strlcat(buffer, "\", \"sensor_data\":{\"invalid_counts\":\"", 255) >= 255) _logger.logMessage(ERROR_LOG, "Send invalid data truncated");


    while((strlen(buffer) + strlen(_storage._invalidCountBuffer->front())) < 250)
    {
        if(strlcat(buffer, _storage._invalidCountBuffer->front(), 255) >= 255) _logger.logMessage(ERROR_LOG, "Send invalid data truncated");
        _storage._transmitInvalidCountBuffer->push(_storage._invalidCountBuffer->front());
        _storage._invalidCountBuffer->pop();
        if(_storage._invalidCountBuffer->empty()) break;
    }
    if(strlcat(buffer, "\"}}", 255) >= 255) _logger.logMessage(ERROR_LOG, "Send invalid data truncated");

    _logger.logMessage(INFO_LOG, "Sending invalid data: " + String(buffer));

    if(Particle.connected())
    {
        if(_settings.getServer() == PRODUCTION) Particle.publish("PROD_sendData", buffer, 60, PRIVATE);
        else Particle.publish("STAGING_sendData", buffer, 60, PRIVATE);
    }

    delete[] buffer;

    _lastTransmitTime = millis();
    if(_msgNo < 9999) _msgNo++;
    else _msgNo = 0;
}

Reply AnimusCloud::getReply()
{
    if(_reply == NA) return _reply;

    Reply reply = _reply;
    _reply = NA;
    return reply;
}

unsigned long AnimusCloud::getLastTransmitTime()
{
    return _lastTransmitTime;
}

void AnimusCloud::setLastTransmitTime()
{
    _lastTransmitTime = millis();
}

void AnimusCloud::checkRestart(bool force)
{
    if(_restart || force)
    {
        _logger.logMessage(INFO_LOG, "Restart flag set to true. RESTARTING");
        _storage.writeSavedHistory();
        delay(1000);
        System.reset();
    }
}

void AnimusCloud::replyHandler(const char* event, const char* data)
{
    String reply = String(data);
    _logger.logMessage(INFO_LOG, "Data received: " + reply);

    if(reply.equals("invalid_counts")) return;

    //Initial validation
    if(!reply.startsWith("{") || !reply.endsWith("}"))
    {
        _reply = INV;
        _logger.logMessage(ERROR_LOG, "Reply not valid JSON");
        if(Particle.connected()) Particle.publish("server_down", "", 60, PRIVATE);
        return;
    }

    //Get number of tokens in reply
    int tokens = 0;
    int index = reply.indexOf("\":", 0);
    while(index != -1)
    {
        tokens++;
        index = reply.indexOf(":", index + 1);
    }

    //Parse reply
    jsmn_parser p;
    jsmntok_t tok[tokens * 2];
    jsmn_init(&p);
    int r = jsmn_parse(&p, reply, tok, tokens * 2);
	if (r != JSMN_SUCCESS)
    {
        _reply = INV;
        _logger.logMessage(ERROR_LOG, "Reply not valid JSON");
        if(Particle.connected()) Particle.publish("server_down", "", 60, PRIVATE);
        return;
    }

    //Split reply in key value pairs
    std::unordered_map<std::string, std::string> dictionary;
    for(int i = 1; i < (tokens * 2); i += 2) dictionary.insert(std::make_pair<std::string, std::string>(reply.substring(tok[i].start, tok[i].end).c_str(), reply.substring(tok[i + 1].start, tok[i + 1].end).c_str()));

    //Check for e:data1 and e:data2
    if(dictionary.count("status") != 0)
    {
        if(String(dictionary["status"].c_str()).equals("e:data1"))
        {
            _reply = E_DATA1;
            return;
        }
        else if(String(dictionary["status"].c_str()).equals("e:data2"))
        {
            _reply = E_DATA2;
            return;
        }
    }

    //Check dictionary contains the needed keys
    if(dictionary.count("msg_no") == 0 || dictionary.count("status") == 0)
    {
        _reply = INV;
        _logger.logMessage(ERROR_LOG, "Needed keys not found");
        if(Particle.connected())  Particle.publish("server_down", "", 60, PRIVATE);
        return;
    }

    //Check message numbers match
    if(atoi(dictionary["msg_no"].c_str()) == (_msgNo - 1))
    {
        //Check for access key message as that needs an extra field in the JSON
        if(String(dictionary["status"].c_str()).equals("s:reg2"))
        {
            _logger.logMessage(INFO_LOG, "Recieved access key");
            if(dictionary.count("key") == 0)
            {
                _reply = INV;
                _logger.logMessage(ERROR_LOG, "Access key not found");
                if(Particle.connected()) Particle.publish("server_down", "", 60, PRIVATE);
                return;
            }

            _reply = S_REG2;
            _settings.setAccessKey(dictionary["key"].c_str());
        }
        else parseReply(String(dictionary["status"].c_str()));
    }
    else if(atoi(dictionary["msg_no"].c_str()) > (_msgNo - 1))
    {
        _logger.logMessage(ERROR_LOG, "Message numbers do not match");
        _reply = INV;
        if(Particle.connected()) Particle.publish("server_ahead_message", "", 60, PRIVATE);
    }
    else
    {
        _logger.logMessage(ERROR_LOG, "Message numbers do not match");
        _reply = NA;
        if(Particle.connected()) Particle.publish("server_late_message", "", 60, PRIVATE);
    }
}

void AnimusCloud::parseReply(String reply)
{
    if(reply.startsWith("s:")) handleSuccessMessage(reply);
    else if(reply.startsWith("e:nf")) handleNotFoundErrors(reply);
    else if(reply.startsWith("e:reg")) handleRegErrors(reply);
    else if(reply.startsWith("e:auth")) handleAuthErrors(reply);
    else if(reply.startsWith("e:data")) handleDataError(reply);
    else _reply = INV;
}

void AnimusCloud::handleSuccessMessage(String reply)
{
    if(reply.equals("s:reg1")) _reply = S_REG1;
    else if(reply.equals("s:data1")) _reply = S_DATA1;
    else if(reply.equals("s:data2")) _reply = S_DATA2;
    else _reply = INV;
}

void AnimusCloud::handleNotFoundErrors(String reply)
{
    if(reply.equals("e:nf1")) _reply = E_NF1;
    else if(reply.equals("e:nf2")) _reply = E_NF2;
    else if(reply.equals("e:nf3")) _reply = E_NF3;
    else if(reply.equals("e:nf4")) _reply = E_NF4;
    else if(reply.equals("e:nf5")) _reply = E_NF5;
    else if(reply.equals("e:nf6")) _reply = E_NF6;
    else if(reply.equals("e:nf7")) _reply = E_NF7;
    else if(reply.equals("e:nf8")) _reply = E_NF8;
    else if(reply.equals("e:nf9")) _reply = E_NF9;
    else _reply = INV;
}

void AnimusCloud::handleRegErrors(String reply)
{
    if(reply.equals("e:reg1")) _reply = E_REG1;
    else if(reply.equals("e:reg2")) _reply = E_REG2;
    else if(reply.equals("e:reg3")) _reply = E_REG3;
    else if(reply.equals("e:reg4")) _reply = E_REG4;
    else if(reply.equals("e:reg5")) _reply = E_REG5;
    else if(reply.equals("e:reg6")) _reply = E_REG6;
    else if(reply.equals("e:reg7")) _reply = E_REG7;
    else if(reply.equals("e:reg8")) _reply = E_REG8;
    else if(reply.equals("e:reg9")) _reply = E_REG9;
    else _reply = INV;
}

void AnimusCloud::handleAuthErrors(String reply)
{
    if(reply.equals("e:auth1")) _reply = E_AUTH1;
    else if(reply.equals("e:auth2")) _reply = E_AUTH2;
    else if(reply.equals("e:auth3")) _reply = E_AUTH3;
    else _reply = INV;
}

void AnimusCloud::handleDataError(String reply)
{
    if(reply.equals("e:data1")) _reply = E_DATA1;
    else if(reply.equals("e:data2")) _reply = E_DATA2;
    else _reply = INV;
}

AnimusCloud _animusCloud;

#endif
