#ifndef __GLOBAL_H_
#define __GLOBAL_H_

#include "application.h"

const uint16_t P_ID = 302;
const uint16_t P_VERSION = 31;
const char PARTICLE_FIRMWARE_VERSION [] = "0.4.9";

const char MODEL [] = "Kite";
const char HARDWARE_REVISION [] = "0.1";

const uint16_t SETTINGS_VERSION = 7;
const uint8_t SETTINGS_SIZE = 128;
const uint16_t BLOCK_SIZE = 200;
const uint16_t FRAME_SIZE = 4096;

const uint16_t SETTINGS_ADDR = 0;
const uint16_t AVERAGE_ADDR = SETTINGS_ADDR + SETTINGS_SIZE;
const uint16_t VARIANCE_ADDR = AVERAGE_ADDR + FRAME_SIZE;
const uint16_t SD_ADDR = VARIANCE_ADDR + FRAME_SIZE;
const uint16_t WALKS_ADDR = SD_ADDR + FRAME_SIZE;

const bool ENABLE_INFO_LOG = true;
const bool ENABLE_WARN_LOG = true;
const bool ENABLE_ERROR_LOG = true;
const bool ENABLE_WALKS_LOG = true;
const bool ENABLE_CV_LOG = true;

const unsigned long CLOUD_CONNECTION_TIMEOUT = 60000;
const unsigned long WEBHOOK_TIMEOUT = 60000;
const unsigned long WEBHOOK_INTERVAL = 6000;
const char REGISTRATION_KEY [] = "h6-d3$llf5g4sdpcfiDh49Vn6bsK3c-df.T59nNw22n-8ipyCwrgjHjL-4rQ";

//Singletons
#include "Logger.h"
#include "ParticleCloud.h"
#include "AnimusCloud.h"
#include "ExternalEeprom.h"
#include "Settings.h"
#include "Storage.h"

#endif
