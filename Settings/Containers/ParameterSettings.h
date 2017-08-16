#ifndef __PARAMETER_SETTINGS_H_
#define __PARAMETER_SETTINGS_H_

struct ParameterSettings
{
    uint16_t binaryThreshold;
    uint8_t trackDeadTime;
    uint8_t trackAliveTime;
    uint8_t doorPosition;
    uint8_t distanceThreshold;
    uint8_t minDoor;
    uint8_t maxDoor;
    uint8_t pirTriggers;
    uint16_t historyLength;
    uint8_t minSize;
    uint8_t minAvgSize;
    uint8_t minLength;
    uint8_t minAvgLength;
    uint8_t minContours;
    uint8_t contourDeadTime;
    uint8_t likelihoodThreshold;
};

#endif
