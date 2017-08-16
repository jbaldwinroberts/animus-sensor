#ifndef __STORAGE_CPP_
#define __STORAGE_CPP_

#include "Storage.h"

Storage::Storage()
{
    _gateCountBuffer = new DynamicQueue;
    _transmitGateCountBuffer = new DynamicQueue;
    _invalidCountBuffer = new DynamicQueue;
    _transmitInvalidCountBuffer = new DynamicQueue;
}

Storage::~Storage()
{
    delete _gateCountBuffer;
    delete _transmitGateCountBuffer;
    delete _invalidCountBuffer;
    delete _transmitInvalidCountBuffer;
}

void Storage::begin()
{
    _logger.logMessage(INFO_LOG, "Starting Storage initialisation");

    _logger.logMessage(INFO_LOG, "Finished Storage initialisation");
}

void Storage::addGateCount(int8_t count)
{
    if(count == 0) return;

    _logger.logMessage(INFO_LOG, "Adding: " + String(count));
    _totalGateCount += count;
}

void Storage::saveGateCount(bool force)
{
    static int currentMinute = Time.format(Time.now(), "%M").toInt();
    int now = Time.format(Time.now(), "%M").toInt();

    if(now != currentMinute || force == true)
    {
        char buffer[32];
        char itoaBuffer[4];
        if(strlcpy(buffer,  "{", sizeof(buffer)) >= sizeof(buffer)) _logger.logMessage(ERROR_LOG, "Gate count truncated");
        itoa(Time.now(), itoaBuffer, 10);
        if(strlcat(buffer, itoaBuffer, sizeof(buffer)) >= sizeof(buffer)) _logger.logMessage(ERROR_LOG, "Gate count truncated");
        if(strlcat(buffer, ",c=", sizeof(buffer)) >= sizeof(buffer)) _logger.logMessage(ERROR_LOG, "Gate count truncated");
        itoa(_totalGateCount, itoaBuffer, 10);
        if(strlcat(buffer, itoaBuffer, sizeof(buffer)) >= sizeof(buffer)) _logger.logMessage(ERROR_LOG, "Gate count truncated");
        if(strlcat(buffer, ",t=", sizeof(buffer)) >= sizeof(buffer)) _logger.logMessage(ERROR_LOG, "Gate count truncated");
        if(_pirTriggers >= _settings.getPirTriggers())
        {
            if(strlcat(buffer, "1", sizeof(buffer)) >= sizeof(buffer)) _logger.logMessage(ERROR_LOG, "Gate count truncated");
        }
        else
        {
            if(strlcat(buffer, "0", sizeof(buffer)) >= sizeof(buffer)) _logger.logMessage(ERROR_LOG, "Gate count truncated");
        }
        if(strlcat(buffer, "}", sizeof(buffer)) >= sizeof(buffer)) _logger.logMessage(ERROR_LOG, "Gate count truncated");

        _logger.logMessage(INFO_LOG, "Adding count: " + String(buffer));

        _externalEeprom.writeData(buffer, strlen(buffer) + 1);

        currentMinute = now;
        _totalGateCount = 0;
        _pirTriggers = 0;
    }
}

void Storage::pirTriggered() volatile
{
    ++_pirTriggers;
}

void Storage::saveCounts()
{
    if(_gateCountBuffer->empty()) _externalEeprom.setLastSaveTime();
    else _externalEeprom.writeData();
}

void Storage::saveTempCounts()
{
    _logger.logMessage(INFO_LOG, "Saving transmit buffer");
    while(!_transmitGateCountBuffer->empty())
    {
        _gateCountBuffer->push(_transmitGateCountBuffer->front());
        _transmitGateCountBuffer->pop();
    }

    while(!_transmitInvalidCountBuffer->empty())
    {
        _invalidCountBuffer->push(_transmitInvalidCountBuffer->front());
        _transmitInvalidCountBuffer->pop();
    }

    saveCounts();
}

void Storage::clear()
{
    clearCounts();
    clearTempCounts();
    clearInvalidData();
    clearInvalidTempData();
    _settings.setDataIndex(0);
}

void Storage::clearCounts()
{
    _logger.logMessage(INFO_LOG, "Clearing counts");
    _gateCountBuffer->clear();
}

void Storage::clearTempCounts()
{
    _logger.logMessage(INFO_LOG, "Clearing temp counts");
    _transmitGateCountBuffer->clear();
}

void Storage::clearInvalidData()
{
    _logger.logMessage(INFO_LOG, "Clearing invalid data");
    _invalidCountBuffer->clear();
}

void Storage::clearInvalidTempData()
{
    _logger.logMessage(INFO_LOG, "Clearing invalid temp data");
    _transmitInvalidCountBuffer->clear();
}

void Storage::readSavedHistory()
{
    _logger.logMessage(INFO_LOG, "Reading saved history");
    _average = _externalEeprom.readAverageFrame();
    _variance = _externalEeprom.readVarianceFrame();
    _standardDeviation = _externalEeprom.readSdFrame();
}

void Storage::writeSavedHistory()
{
    _logger.logMessage(INFO_LOG, "Writing saved history");
    _externalEeprom.writeAverageFrame(_average);
    _externalEeprom.writeVarianceFrame(_variance);
    _externalEeprom.writeSdFrame(_standardDeviation);
    _settings.setSavedHistoryAvailable(true);
}

void Storage::initialiseHistory(uint8_t frameSize, uint8_t scaleFactor)
{
    _logger.logMessage(INFO_LOG, "Initialising saved history");
    _average = QSMatrix<float> (frameSize * scaleFactor, frameSize * scaleFactor, 0.0);
    _variance = QSMatrix<float> (frameSize * scaleFactor, frameSize * scaleFactor, 0.5);
    _standardDeviation = QSMatrix<float> (frameSize * scaleFactor, frameSize * scaleFactor, 0.0);
}

Storage _storage;

#endif
