#ifndef __SETTINGS_H_
#define __SETTINGS_H_

#include "Global.h"
#include "SensorSettings.h"

class Settings
{
  public:
    Settings();
    virtual ~Settings();
    void begin();

    void print() const;
    void setDefault();

    bool setVersion(const uint16_t version);
    uint16_t getVersion() const;
    bool setBinaryThreshold(const uint16_t binaryThreshold);
    uint16_t getBinaryThreshold() const;
    bool setTrackDeadTime(const uint8_t trackDeadTime);
    uint8_t getTrackDeadTime() const;
    bool setTrackAliveTime(const uint8_t trackAliveTime);
    uint8_t getTrackAliveTime() const;
    bool setDoorPosition(const uint8_t doorPosition);
    uint8_t getDoorPosition() const;
    bool setDistanceThreshold(const uint8_t distanceThreshold);
    uint8_t getDistanceThreshold() const;
    bool setMinSize(const uint8_t minSize);
    uint8_t getMinSize() const;
    bool setMinAvgSize(const uint8_t minAvgSize);
    uint8_t getMinAvgSize() const;
    bool setMinLength(const uint8_t minLength);
    uint8_t getMinLength() const;
    bool setMinAvgLength(const uint8_t minAvgLength);
    uint8_t getMinAvgLength() const;
    bool setMinContours(const uint8_t minContours);
    uint8_t getMinContours() const;
    bool setContourDeadTime(const uint8_t trackContourTime);
    uint8_t getContourDeadTime() const;
    bool setLikelihoodThreshold(const uint8_t likelihoodThreshold);
    uint8_t getLikelihoodThreshold() const;
    bool setMinDoor(const uint8_t minDoor);
    uint8_t getMinDoor() const;
    bool setMaxDoor(const uint8_t maxDoor);
    uint8_t getMaxDoor() const;
    bool setPirTriggers(const uint8_t pirTriggers);
    uint8_t getPirTriggers() const;
    bool setHistoryLength(const uint16_t historyLength);
    uint16_t getHistoryLength() const;
    bool setRegistration(const RegistrationSettings registration);
    RegistrationSettings getRegistration() const;
    bool setServer(const ServerSettings server);
    ServerSettings getServer() const;
    bool setAccessKey(const char* accessKey);
    const char* getAccessKey() const;
    bool setDataIndex(const uint32_t dataIndex);
    uint32_t getDataIndex() const;
    bool setTransmitInterval(const uint32_t transmitInterval);
    uint32_t getTransmitInterval() const;
    bool setSdMessageLogging(const bool sdMessageLogging);
    bool getSdMessageLogging() const;
    bool setSdFrameLogging(const bool sdFrameLogging);
    bool getSdFrameLogging() const;
    bool setSavedHistoryAvailable(const bool savedHistoryAvailable);
    bool getSavedHistoryAvailable() const;
    bool setDebugPrint(const bool debugPrint);
    bool getDebugPrint() const;

  private:
    SensorSettings _settings;
    byte _buffer[SETTINGS_SIZE];
    void load();
    void save();
};

extern Settings _settings;

#endif
