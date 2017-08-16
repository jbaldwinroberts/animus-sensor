#ifndef __SETTINGS_CPP_
#define __SETTINGS_CPP_

#include "Settings.h"

Settings::Settings() {}

Settings::~Settings() {}

void Settings::begin()
{
    _logger.logMessage(INFO_LOG, "Started Settings initialisation");
    load();
    _logger.logMessage(INFO_LOG, "Finished Settings initialisation");
}

void Settings::load()
{
    _logger.logMessage(INFO_LOG, "Started loading settings from external eeprom");

    //Load data from external eeprom
    _externalEeprom.readSettings(_buffer, SETTINGS_SIZE);

    //Check settings version number
    uint16_t settingsVersion;
    memcpy(&settingsVersion, &_buffer, sizeof(settingsVersion));
    if(settingsVersion != SETTINGS_VERSION)
    {
        _logger.logMessage(ERROR_LOG, "Settings version does not match!");
        setDefault();
    }
    else memcpy(&_settings, &_buffer, sizeof(_settings)); //Convert byte array to settings

    print();

    _logger.logMessage(INFO_LOG, "Finished loading settings from external eeprom");
}

void Settings::save()
{
    //_logger.logMessage(INFO_LOG, "Started saving settings to external eeprom");

    //print();

    //Convert settings to byte array
    memcpy(&_buffer, &_settings, sizeof(_settings));

    //Save data to external eeprom
    _externalEeprom.writeSettings(_buffer, SETTINGS_SIZE);

    //_logger.logMessage(INFO_LOG, "Finished saving settings to external eeprom");
}

void Settings::print() const
{
    //Print out settings
    _logger.logMessage(INFO_LOG, "Printing settings");
    _logger.logMessage(INFO_LOG, "Version: " + String(_settings.version));
    _logger.logMessage(INFO_LOG, "Binary threshold hreshold: " + String(_settings.parameterSettings.binaryThreshold));
    _logger.logMessage(INFO_LOG, "Track dead time: " + String(_settings.parameterSettings.trackDeadTime));
    _logger.logMessage(INFO_LOG, "Track alive time: " + String(_settings.parameterSettings.trackAliveTime));
    _logger.logMessage(INFO_LOG, "Door position: " + String(_settings.parameterSettings.doorPosition));
    _logger.logMessage(INFO_LOG, "Distance Threshold: " + String(_settings.parameterSettings.distanceThreshold));
    _logger.logMessage(INFO_LOG, "Minimum size: " + String(_settings.parameterSettings.minSize));
    _logger.logMessage(INFO_LOG, "Minimum average size: " + String(_settings.parameterSettings.minAvgSize));
    _logger.logMessage(INFO_LOG, "Minimum length: " + String(_settings.parameterSettings.minLength));
    _logger.logMessage(INFO_LOG, "Minimum average length: " + String(_settings.parameterSettings.minAvgLength));
    _logger.logMessage(INFO_LOG, "Minimum contours: " + String(_settings.parameterSettings.minContours));
    _logger.logMessage(INFO_LOG, "Contour dead time: " + String(_settings.parameterSettings.contourDeadTime));
    _logger.logMessage(INFO_LOG, "Likelihood threshold: " + String(_settings.parameterSettings.likelihoodThreshold));
    _logger.logMessage(INFO_LOG, "Min door: " + String(_settings.parameterSettings.minDoor));
    _logger.logMessage(INFO_LOG, "Max door: " + String(_settings.parameterSettings.maxDoor));
    _logger.logMessage(INFO_LOG, "PIR triggers: " + String(_settings.parameterSettings.pirTriggers));
    _logger.logMessage(INFO_LOG, "History length: " + String(_settings.parameterSettings.historyLength));
    _logger.logMessage(INFO_LOG, "Registration: " + String(_settings.registrationSettings));
    _logger.logMessage(INFO_LOG, "Server: " + String(_settings.serverSettings));
    _logger.logMessage(INFO_LOG, "Access key: " + String(_settings.accessKey));
    _logger.logMessage(INFO_LOG, "Data index: " + String(_settings.dataIndex));
    _logger.logMessage(INFO_LOG, "Transmit Interval: " + String(_settings.transmitInterval));
    _logger.logMessage(INFO_LOG, "Sd message logging enabled: " + String(_settings.sdMessageLogging));
    _logger.logMessage(INFO_LOG, "Sd frame logging enabled: " + String(_settings.sdFrameLogging));
    _logger.logMessage(INFO_LOG, "Saved history available: " + String(_settings.savedHistoryAvailable));
    _logger.logMessage(INFO_LOG, "Debug print enabled: " + String(_settings.debugPrint));
}

void Settings::setDefault()
{
    //Set default settings
    _logger.logMessage(INFO_LOG, "Setting default settings");

    _settings.version = SETTINGS_VERSION;
    _settings.parameterSettings.binaryThreshold = 225;
    _settings.parameterSettings.trackDeadTime = 3;
    _settings.parameterSettings.trackAliveTime = 3;
    _settings.parameterSettings.doorPosition = 16;
    _settings.parameterSettings.distanceThreshold = 8;
    _settings.parameterSettings.minSize = 8;
    _settings.parameterSettings.minAvgSize = 8;
    _settings.parameterSettings.minLength = 16;
    _settings.parameterSettings.minAvgLength = 2;
    _settings.parameterSettings.minContours = 4;
    _settings.parameterSettings.contourDeadTime = 3;
    _settings.parameterSettings.likelihoodThreshold = 50;
    _settings.parameterSettings.minDoor = 0;
    _settings.parameterSettings.maxDoor = 31;
    _settings.parameterSettings.pirTriggers = 3;
    _settings.parameterSettings.historyLength = 1200;
    _settings.registrationSettings = UNKNOWN;
    _settings.serverSettings = STAGING;
    if(strlcpy(_settings.accessKey, "", sizeof(_settings.accessKey)) >= sizeof(_settings.accessKey)) _logger.logMessage(ERROR_LOG, "Access key truncated");
    _settings.dataIndex = 0;
    _settings.transmitInterval = 60;
    _settings.sdMessageLogging = true;
    _settings.sdFrameLogging = false;
    _settings.savedHistoryAvailable = false;
    _settings.debugPrint = true;

    save();
}

bool Settings::setVersion(const uint16_t version)
{
    if(version <= 0 || version > 65535) return false;

    _settings.version = version;
    _logger.logMessage(INFO_LOG, "Set version to: " + String(_settings.version));
    save();
    return true;
}

uint16_t Settings::getVersion() const
{
    return _settings.version;
}

bool Settings::setBinaryThreshold(const uint16_t binaryThreshold)
{
    if(binaryThreshold <= 0 || binaryThreshold > 65535) return false;

    _settings.parameterSettings.binaryThreshold = binaryThreshold;
    _logger.logMessage(INFO_LOG, "Set binary threshold to: " + String(_settings.parameterSettings.binaryThreshold));
    save();
    return true;
}

uint16_t Settings::getBinaryThreshold() const
{
    return _settings.parameterSettings.binaryThreshold;
}

bool Settings::setTrackDeadTime(const uint8_t trackDeadTime)
{
    if(trackDeadTime <= 0 || trackDeadTime > 255) return false;

    _settings.parameterSettings.trackDeadTime = trackDeadTime;
    _logger.logMessage(INFO_LOG, "Set track dead time to: " + String(_settings.parameterSettings.trackDeadTime));
    save();
    return true;
}

uint8_t Settings::getTrackDeadTime() const
{
    return _settings.parameterSettings.trackDeadTime;
}

bool Settings::setTrackAliveTime(const uint8_t trackAliveTime)
{
    if(trackAliveTime <= 0 || trackAliveTime > 255) return false;

    _settings.parameterSettings.trackAliveTime = trackAliveTime;
    _logger.logMessage(INFO_LOG, "Set track alive time to: " + String(_settings.parameterSettings.trackAliveTime));
    save();
    return true;
}

uint8_t Settings::getTrackAliveTime() const
{
    return _settings.parameterSettings.trackAliveTime;
}

bool Settings::setDoorPosition(const uint8_t doorPosition)
{
    if(doorPosition <= 0 || doorPosition > 255) return false;

    _settings.parameterSettings.doorPosition = doorPosition;
    _logger.logMessage(INFO_LOG, "Set door position to: " + String(_settings.parameterSettings.doorPosition));
    save();
    return true;
}

uint8_t Settings::getDoorPosition() const
{
    return _settings.parameterSettings.doorPosition;
}

//TODO min and max distance threshold

bool Settings::setDistanceThreshold(const uint8_t distanceThreshold)
{
    if(distanceThreshold <= 0 || distanceThreshold > 255) return false;

    _settings.parameterSettings.distanceThreshold = distanceThreshold;
    _logger.logMessage(INFO_LOG, "Set distance threshold to: " + String(_settings.parameterSettings.distanceThreshold));
    save();
    return true;
}

uint8_t Settings::getDistanceThreshold() const
{
    return _settings.parameterSettings.distanceThreshold;
}

bool Settings::setMinSize(const uint8_t minSize)
{
    if(minSize <= 0 || minSize > 255) return false;

    _settings.parameterSettings.minSize = minSize;
    _logger.logMessage(INFO_LOG, "Set minimum size to: " + String(_settings.parameterSettings.minSize));
    save();
    return true;
}

uint8_t Settings::getMinSize() const
{
    return _settings.parameterSettings.minSize;
}

bool Settings::setMinAvgSize(const uint8_t minAvgSize)
{
    if(minAvgSize <= 0 || minAvgSize > 255) return false;

    _settings.parameterSettings.minAvgSize = minAvgSize;
    _logger.logMessage(INFO_LOG, "Set minimum average size to: " + String(_settings.parameterSettings.minAvgSize));
    save();
    return true;
}

uint8_t Settings::getMinAvgSize() const
{
    return _settings.parameterSettings.minAvgSize;
}

bool Settings::setMinLength(const uint8_t minLength)
{
    if(minLength <= 0 || minLength > 255) return false;

    _settings.parameterSettings.minLength = minLength;
    _logger.logMessage(INFO_LOG, "Set minimum length to: " + String(_settings.parameterSettings.minLength));
    save();
    return true;
}

uint8_t Settings::getMinLength() const
{
    return _settings.parameterSettings.minLength;
}

bool Settings::setMinAvgLength(const uint8_t minAvgLength)
{
    if(minAvgLength <= 0 || minAvgLength > 255) return false;

    _settings.parameterSettings.minAvgLength = minAvgLength;
    _logger.logMessage(INFO_LOG, "Set minimum average length to: " + String(_settings.parameterSettings.minAvgLength));
    save();
    return true;
}

uint8_t Settings::getMinAvgLength() const
{
    return _settings.parameterSettings.minAvgLength;
}

bool Settings::setMinContours(const uint8_t minContours)
{
    if(minContours <= 0 || minContours > 255) return false;

    _settings.parameterSettings.minContours = minContours;
    _logger.logMessage(INFO_LOG, "Set minimum contours to: " + String(_settings.parameterSettings.minContours));
    save();
    return true;
}

uint8_t Settings::getMinContours() const
{
    return _settings.parameterSettings.minContours;
}

bool Settings::setContourDeadTime(const uint8_t contourDeadTime)
{
    if(contourDeadTime <= 0 || contourDeadTime > 255) return false;

    _settings.parameterSettings.contourDeadTime = contourDeadTime;
    _logger.logMessage(INFO_LOG, "Set contour dead time to: " + String(_settings.parameterSettings.contourDeadTime));
    save();
    return true;
}

uint8_t Settings::getContourDeadTime() const
{
    return _settings.parameterSettings.contourDeadTime;
}

bool Settings::setLikelihoodThreshold(const uint8_t likelihoodThreshold)
{
    if(likelihoodThreshold <= 0 || likelihoodThreshold > 255) return false;

    _settings.parameterSettings.likelihoodThreshold = likelihoodThreshold;
    _logger.logMessage(INFO_LOG, "Set likeliood threshold to: " + String(_settings.parameterSettings.likelihoodThreshold));
    save();
    return true;
}

uint8_t Settings::getLikelihoodThreshold() const
{
    return _settings.parameterSettings.likelihoodThreshold;
}

bool Settings::setMinDoor(const uint8_t minDoor)
{
    if(minDoor <= 0 || minDoor > 255) return false;

    _settings.parameterSettings.minDoor = minDoor;
    _logger.logMessage(INFO_LOG, "Set min door to: " + String(_settings.parameterSettings.minDoor));
    save();
    return true;
}

uint8_t Settings::getMinDoor() const
{
    return _settings.parameterSettings.minDoor;
}

bool Settings::setMaxDoor(const uint8_t maxDoor)
{
    if(maxDoor <= 0 || maxDoor > 255) return false;

    _settings.parameterSettings.maxDoor = maxDoor;
    _logger.logMessage(INFO_LOG, "Set max door to: " + String(_settings.parameterSettings.maxDoor));
    save();
    return true;
}

uint8_t Settings::getMaxDoor() const
{
    return _settings.parameterSettings.maxDoor;
}

bool Settings::setPirTriggers(const uint8_t pirTriggers)
{
    if(pirTriggers <= 0 || pirTriggers > 10) return false;

    _settings.parameterSettings.pirTriggers = pirTriggers;
    _logger.logMessage(INFO_LOG, "Set PIR triggers to: " + String(_settings.parameterSettings.pirTriggers));
    save();
    return true;
}

uint8_t Settings::getPirTriggers() const
{
    return _settings.parameterSettings.pirTriggers;
}

bool Settings::setHistoryLength(const uint16_t historyLength)
{
    if(historyLength <= 0 || historyLength > 65535) return false;

    _settings.parameterSettings.historyLength = historyLength;
    _logger.logMessage(INFO_LOG, "Set history length to: " + String(_settings.parameterSettings.historyLength));
    save();
    return true;
}
uint16_t Settings::getHistoryLength() const
{
    return _settings.parameterSettings.historyLength;
}

bool Settings::setRegistration(const RegistrationSettings registration)
{
    _settings.registrationSettings = registration;
    _logger.logMessage(INFO_LOG, "Set registration to: " + String(_settings.registrationSettings));
    save();
    return true;
}

RegistrationSettings Settings::getRegistration() const
{
    return _settings.registrationSettings;
}

bool Settings::setServer(const ServerSettings server)
{
    _settings.serverSettings = server;
    _logger.logMessage(INFO_LOG, "Set server to: " + String(_settings.serverSettings));
    save();
    return true;
}

ServerSettings Settings::getServer() const
{
    return _settings.serverSettings;
}

bool Settings::setAccessKey(const char* accessKey)
{
    if(strlcpy(_settings.accessKey, accessKey, sizeof(_settings.accessKey)) >= sizeof(_settings.accessKey)) _logger.logMessage(ERROR_LOG, "Access key truncated");
    _logger.logMessage(INFO_LOG, "Set access key to: " + String(_settings.accessKey));
    save();
    return true;
}

const char* Settings::getAccessKey() const
{
    return _settings.accessKey;
}

bool Settings::setDataIndex(const uint32_t dataIndex)
{
    if(dataIndex < 0 || dataIndex > 4294967295) return false;

    _settings.dataIndex = dataIndex;
    //_logger.logMessage(INFO_LOG, "Set data index to: " + String(_settings.dataIndex));
    save();
    return true;
}

uint32_t Settings::getDataIndex() const
{
    return _settings.dataIndex;
}

bool Settings::setTransmitInterval(const uint32_t transmitInterval)
{
    if(transmitInterval <= 0 || transmitInterval > 4294967295) return false;

    _settings.transmitInterval = transmitInterval;
    _logger.logMessage(INFO_LOG, "Set transmit interval to: " + String(_settings.transmitInterval));
    save();
    return true;
}

uint32_t Settings::getTransmitInterval() const
{
    return _settings.transmitInterval;
}

bool Settings::setSdMessageLogging(const bool sdMessageLogging)
{
    _settings.sdMessageLogging = sdMessageLogging;
    _logger.logMessage(INFO_LOG, "Set SD message logging to: " + String(_settings.sdMessageLogging));
    save();
    return true;
}

bool Settings::getSdMessageLogging() const
{
    return _settings.sdMessageLogging;
}

bool Settings::setSdFrameLogging(const bool sdFrameLogging)
{
    _settings.sdFrameLogging = sdFrameLogging;
    _logger.logMessage(INFO_LOG, "Set SD frame logging to: " + String(_settings.sdFrameLogging));
    save();
    return true;
}

bool Settings::getSdFrameLogging() const
{
    return _settings.sdFrameLogging;
}

bool Settings::setSavedHistoryAvailable(const bool savedHistoryAvailable)
{
    _settings.savedHistoryAvailable = savedHistoryAvailable;
    _logger.logMessage(INFO_LOG, "Set saved history available to: " + String(_settings.savedHistoryAvailable));
    save();
    return true;
}

bool Settings::getSavedHistoryAvailable() const
{
    return _settings.savedHistoryAvailable;
}

bool Settings::setDebugPrint(const bool debugPrint)
{
    _settings.debugPrint = debugPrint;
    _logger.logMessage(INFO_LOG, "Set debug print to: " + String(_settings.debugPrint));
    save();
    return true;
}

bool Settings::getDebugPrint() const
{
    return _settings.debugPrint;
}

Settings _settings;

#endif
