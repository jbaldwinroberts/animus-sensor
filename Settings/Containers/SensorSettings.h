#ifndef __SENSOR_SETTINGS_H_
#define __SENSOR_SETTINGS_H_

#include "ParameterSettings.h"
#include "RegistrationSettings.h"
#include "ServerSettings.h"

struct SensorSettings
{
    uint16_t version;
    ParameterSettings parameterSettings;
    RegistrationSettings registrationSettings;
    ServerSettings serverSettings;
    char accessKey[9];
    uint32_t dataIndex;
    unsigned long transmitInterval;
    bool sdMessageLogging;
    bool sdFrameLogging;
    bool savedHistoryAvailable;
    bool debugPrint;
};

#endif
