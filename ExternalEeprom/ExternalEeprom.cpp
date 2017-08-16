#ifndef __EXTERNAL_EEPROM_CPP_
#define __EXTERNAL_EEPROM_CPP_

#include "ExternalEeprom.h"

ExternalEeprom::ExternalEeprom()
{
    _logger.logMessage(INFO_LOG, "Started EEPROM initialisation");

    _invalidItems = new DynamicQueue;
    _lastLoadTime = millis();
    _lastSaveTime = millis();

    _logger.logMessage(INFO_LOG, "Finished EEPROM initialisation");
}

ExternalEeprom::~ExternalEeprom()
{
    delete _spieep;
    delete _invalidItems;
}

void ExternalEeprom::begin()
{
    _logger.logMessage(INFO_LOG, "Starting External EEPROM initialisation");

    _spieep = new SPIEEP(24, 256, 131072);
    _spieep->begin_spi(A2);
    if(!_spieep->test_chip()) _logger.logMessage(ERROR_LOG, "Eeprom NOT FOUND!!");

    _logger.logMessage(INFO_LOG, "Finished External EEPROM initialisation");
}

void ExternalEeprom::readSettings(byte* settings, uint16_t size)
{
    byte checkbuffer[size];

    do
    {
        _spieep->readn(SETTINGS_ADDR, settings, size);
        _spieep->readn(SETTINGS_ADDR, checkbuffer, size);
    }
    while(memcmp(settings, checkbuffer, size) != 0);
}

void ExternalEeprom::writeSettings(byte* settings, uint16_t size)
{
    byte checkbuffer[size];

    do
    {
        _spieep->writen(SETTINGS_ADDR, settings, size);
        _spieep->readn(SETTINGS_ADDR, checkbuffer, size);
    }
    while(memcmp(settings, checkbuffer, size) != 0);
}

void ExternalEeprom::readData()
{
    _logger.logMessage(INFO_LOG, "Loading data from: " + String(_settings.getDataIndex()));

    //Read data from EEPROM
    char* buffer;
    int index = 0;
    if(_settings.getDataIndex() >= BLOCK_SIZE)
    {
        buffer = readPartialData();
        _logger.logMessage(INFO_LOG, "Loaded items: " + String(buffer));
        index = indexOf(buffer, "{");
        _settings.setDataIndex(_settings.getDataIndex() - BLOCK_SIZE + index);
    }
    else
    {
        buffer = readFullData();
        _logger.logMessage(INFO_LOG, "Loaded items: " + String(buffer));
        _settings.setDataIndex(index);
    }

    //Extract items from data
    char* item = NULL;
    item = strtok(buffer + index, ".");
    while(NULL != item)
    {
        //Validate item
        if(validateItem(item))
        {
            _logger.logMessage(INFO_LOG, "Loading item: " + String(item));
            _storage._gateCountBuffer->push(item);
        }
        else
        {
            _logger.logMessage(WARN_LOG, "Invalid item: " + String(item));
            _invalidItems->push(item);
        }

        item = strtok(NULL, ".");
    }

    delete[] buffer;

    _lastLoadTime = millis();

    _logger.logMessage(INFO_LOG, "Address set to: " + String(_settings.getDataIndex()));
}

char* ExternalEeprom::readFullData()
{
    _logger.logMessage(INFO_LOG, "Reading full data");

    char* buffer = new char[_settings.getDataIndex()];
    _spieep->readn(WALKS_ADDR, buffer, _settings.getDataIndex() - 1);
    std::replace(buffer, buffer + _settings.getDataIndex(), '\0', '#'); //Remove null chars so that strtok does not break before buffer has been processed
    buffer[_settings.getDataIndex() - 1] = '\0';
    return buffer;
}

char* ExternalEeprom::readPartialData()
{
    _logger.logMessage(INFO_LOG, "Reading partial data");

    char* buffer = new char[BLOCK_SIZE + 1];
    _spieep->readn(WALKS_ADDR + _settings.getDataIndex() - BLOCK_SIZE, buffer, BLOCK_SIZE);
    std::replace(buffer, buffer + BLOCK_SIZE + 1, '\0', '#'); //Remove null chars so that strtok does not break before buffer has been processed
    buffer[BLOCK_SIZE] = '\0';
    return buffer;
}

void ExternalEeprom::writeData()
{
    while(!_storage._gateCountBuffer->empty())
    {
        uint16_t length = strlen(_storage._gateCountBuffer->front()) + 1;
        char buffer[length];
        if(strlcpy(buffer, _storage._gateCountBuffer->front(), sizeof(buffer)) >= sizeof(buffer)) _logger.logMessage(ERROR_LOG, "EEPROM write truncated");
        _storage._gateCountBuffer->pop();

        writeData(buffer, length);
    }
}

void ExternalEeprom::writeData(char* buffer, uint16_t length)
{
    _logger.logMessage(INFO_LOG, "Saving data at: " + String(_settings.getDataIndex()));
    _logger.logMessage(INFO_LOG, "Saving item: " + String(buffer));

    //Create check buffer
    char checkBuffer[length];
    checkBuffer[0] = '\0';

    //Keep writing item until it matches the item read out
    while(strcmp(checkBuffer, buffer) != 0)
    {
        //Save item to EEPROM
        buffer[length - 1] = '.';
        _spieep->writen(WALKS_ADDR + _settings.getDataIndex(), buffer, length);
        buffer[length - 1] = '\0';
        //Serial.printf("Write buffer: %s\r\n", buffer);

        //Read back item from EEPROM
        _spieep->readn(WALKS_ADDR + _settings.getDataIndex(), checkBuffer, length);
        checkBuffer[length - 1] = '\0';
        //Serial.printf("Read buffer: %s\r\n", checkBuffer);
    }

    //Update address
    _settings.setDataIndex(_settings.getDataIndex() + length);

    _lastSaveTime = millis();

    _logger.logMessage(INFO_LOG, "Address set to: " + String(_settings.getDataIndex()));

}

void ExternalEeprom::readInvalidData(DynamicQueue* invalidItems)
{
    while(!_invalidItems->empty())
    {
        invalidItems->push(_invalidItems->front());
        _invalidItems->pop();
    }
}

void ExternalEeprom::writeAverageFrame(const QSMatrix<float>& frame)
{
    _logger.logMessage(INFO_LOG, "Saving average frame");
    writeFrame(frame, AVERAGE_ADDR);
}

void ExternalEeprom::writeVarianceFrame(const QSMatrix<float>& frame)
{
    _logger.logMessage(INFO_LOG, "Saving variance frame");
    writeFrame(frame, VARIANCE_ADDR);
}

void ExternalEeprom::writeSdFrame(const QSMatrix<float>& frame)
{
    _logger.logMessage(INFO_LOG, "Saving standard deviation frame");
    writeFrame(frame, SD_ADDR);
}

void ExternalEeprom::writeFrame(const QSMatrix<float>& frame, int index)
{
    char* buffer = new char[FRAME_SIZE + 1];

    int localIndex = 0;
    for(uint8_t row = 0, rows = frame.get_rows(); row < rows; ++row)
	{
        for(uint8_t col = 0, cols = frame.get_cols(); col < cols; ++col)
        {
            _u.f = frame(row, col);
            //Serial.printf("input at %d: %f\r\n", localIndex, _u.f);
            buffer[localIndex + 0] = _u.c[0];
            buffer[localIndex + 1] = _u.c[1];
            buffer[localIndex + 2] = _u.c[2];
            buffer[localIndex + 3] = _u.c[3];
            localIndex += 4;
        }
    }
    buffer[FRAME_SIZE] = '\0';

    //Create check buffer
    char* checkBuffer = new char[FRAME_SIZE + 1];
    checkBuffer[0] = '\0';

    //Keep writing item until it matches the item read out
    while(strcmp(checkBuffer, buffer) != 0)
    {
        //Save item to EEPROM
        _spieep->writen(index, buffer, FRAME_SIZE);
        //Serial.printf("Write buffer: %s\r\n", buffer);

        //Read back item from EEPROM
        _spieep->readn(index, checkBuffer, FRAME_SIZE);
        checkBuffer[FRAME_SIZE] = '\0';
        //Serial.printf("Read buffer: %s\r\n", checkBuffer);
    }

    delete[] buffer;
    delete[] checkBuffer;
}

QSMatrix<float> ExternalEeprom::readAverageFrame()
{
    _logger.logMessage(INFO_LOG, "Loading average frame");
    return readFrame(AVERAGE_ADDR);
}

QSMatrix<float> ExternalEeprom::readVarianceFrame()
{
    _logger.logMessage(INFO_LOG, "Loading variance frame");
    return readFrame(VARIANCE_ADDR);
}

QSMatrix<float> ExternalEeprom::readSdFrame()
{
    _logger.logMessage(INFO_LOG, "Loading standard deviation frame");
    return readFrame(SD_ADDR);
}

QSMatrix<float> ExternalEeprom::readFrame(int index)
{
    char* buffer = new char[FRAME_SIZE + 1];
    char* checkBuffer = new char[FRAME_SIZE + 1];

    do
    {
        _spieep->readn(index, buffer, FRAME_SIZE);
        buffer[FRAME_SIZE] = '\0';

        _spieep->readn(index, checkBuffer, FRAME_SIZE);
        checkBuffer[FRAME_SIZE] = '\0';
    }
    while(strcmp(checkBuffer, buffer) != 0);

    delete[] checkBuffer;

    //Extract items from data
    QSMatrix<float> frame(32, 32, 0.0);
    int localIndex = 0;
    for(uint8_t row = 0, rows = frame.get_rows(); row < rows; ++row)
	{
        for(uint8_t col = 0, cols = frame.get_cols(); col < cols; ++col)
        {
            _u.c[0] = buffer[localIndex + 0];
            _u.c[1] = buffer[localIndex + 1];
            _u.c[2] = buffer[localIndex + 2];
            _u.c[3] = buffer[localIndex + 3];

            //Basic validation
            float value = _u.f;
            if(value < 0.0) value = 0.0;
            else if(value >= 50.0) value = 0.0;

            frame(row, col) = value;
            //Serial.printf("output at %d: %f\r\n", localIndex, _u.f);
            localIndex += 4;
        }
    }

    delete[] buffer;

    return frame;
}

void ExternalEeprom::setLastSaveTime()
{
    _lastSaveTime = millis();
}

unsigned long ExternalEeprom::getLastSaveTime() const
{
    return _lastSaveTime;
}

void ExternalEeprom::setLastLoadTime()
{
    _lastLoadTime = millis();
}

unsigned long ExternalEeprom::getLastLoadTime() const
{
    return _lastLoadTime;
}

bool ExternalEeprom::checkInvalidData() const
{
    return !_invalidItems->empty();
}

bool ExternalEeprom::validateItem(const char* item)
{
    if(!startsWith(item, "{")) return false;
    if(!endsWith(item, "}")) return false;

    uint8_t firstComma = indexOf(item, ",");
    uint8_t secondComma = indexOf(item, ",", firstComma + 1);

    char buffer[64];

    //Validate time stamp
    if(firstComma - 1 >= sizeof(buffer))
    {
        _logger.logMessage(ERROR_LOG, "Item validation truncated");
        return false;
    }
    strncpy(buffer, item + 1, firstComma - 1);
    buffer[firstComma - 1] = '\0';
    if(strlen(buffer) != 10) return false;
    if(!startsWith(buffer, "1")) return false;
    for(std::size_t i = 0; i < strlen(buffer); ++i) if(!std::isdigit(buffer[i])) return false;

    //Validate counts
    if(secondComma - (firstComma + 1) >= sizeof(buffer))
    {
        _logger.logMessage(ERROR_LOG, "Item validation truncated");
        return false;
    }
    strncpy(buffer, item + firstComma + 1, secondComma - (firstComma + 1));
    buffer[secondComma - (firstComma + 1)] = '\0';
    if(strlen(buffer) < 3) return false;
    if(buffer[0] != 'c') return false;
    if(buffer[1] != '=') return false;
    for(std::size_t i = 2; i < strlen(buffer); ++i)
    {
        if(buffer[i] == '-' && i == 2) continue;
        if(!std::isdigit(buffer[i])) return false;
    }

    //Validate PIR
    if(strlen(item) - (secondComma + 2) >= sizeof(buffer))
    {
        _logger.logMessage(ERROR_LOG, "Item validation truncated");
        return false;
    }
    strncpy(buffer, item + secondComma + 1, strlen(item) - (secondComma + 2));
    buffer[strlen(item) - (secondComma + 2)] = '\0';
    if(strlen(buffer) < 3) return false;
    if(buffer[0] != 't') return false;
    if(buffer[1] != '=') return false;
    for(std::size_t i = 2; i < strlen(buffer); ++i)
    {
        if(buffer[i] == '-' && i == 2) continue;
        if(!std::isdigit(buffer[i])) return false;
    }

    return true;
}

bool ExternalEeprom::startsWith(const char *a, const char *b)
{
   return (strstr(a, b) - a) == 0;
}

bool ExternalEeprom::endsWith(const char *a, const char *b)
{
    uint16_t alen = strlen(a);
    uint16_t blen = strlen(b);
    return (alen >= blen) && (0 == strcmp(a + alen - blen, b));
}

int16_t ExternalEeprom::indexOf(const char* a, const char* b, int16_t startIndex)
{
    int16_t result = -1;
    uint16_t alen = strlen(a);
    if(strlen(b) > alen || startIndex > alen) return result;
    if(startIndex < 0) startIndex = 0;
    char* pos = strstr(a + startIndex, b);
    if(pos != NULL) result = pos - a;
    return result;
}

int16_t ExternalEeprom::indexOf(const char* a, const char* b)
{
    return indexOf(a, b, 0);
}

ExternalEeprom _externalEeprom;

#endif
