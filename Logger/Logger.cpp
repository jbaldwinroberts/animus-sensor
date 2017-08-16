#ifndef __LOGGER_CPP_
#define __LOGGER_CPP_

#include "Logger.h"

Logger::Logger()
{
    _logBuffer = new DynamicQueue;
}

Logger::~Logger()
{
    delete _logBuffer;
    delete _sdFat;
}

void Logger::begin()
{
    _sdFat = new SdFat(1);

    logMessage("\r\n\r\n");
    logMessage(INFO_LOG, "Started Logger initialisation");

    logMessage(INFO_LOG, "Finished Logger initialisation");
}

void Logger::checkSd()
{
    if(_sdFat->begin(D5, SPI_HALF_SPEED)) _sdPresent = true;
    if(_sdPresent) logMessage(INFO_LOG, "Logging SD card present");
    else logMessage(WARN_LOG, "Logging SD card not present");
}

void Logger::logMessage(const LogType logType, const char* message) //TODO variac function - printf
{
    uint16_t length = 64 + strlen(message) + 1;
    char* buffer = new char[length];

    if(INFO_LOG == logType)
    {
        if(!ENABLE_INFO_LOG) return;
        if(strlcpy(buffer, "INFO LOG - ", length) >= length) _logger.logMessage(ERROR_LOG, "Log truncated");
    }
    else if(WARN_LOG == logType)
    {
        if(!ENABLE_WARN_LOG) return;
        if(strlcpy(buffer, "WARN LOG - ", length) >= length) _logger.logMessage(ERROR_LOG, "Log truncated");
    }
    else if(ERROR_LOG == logType)
    {
        if(!ENABLE_ERROR_LOG) return;
        if(strlcpy(buffer, "ERROR LOG - ", length) >= length) _logger.logMessage(ERROR_LOG, "Log truncated");
    }
    else if(WALKS_LOG == logType)
    {
        if(!ENABLE_WALKS_LOG) return;
        if(strlcpy(buffer, "WALKS LOG - ", length) >= length) _logger.logMessage(ERROR_LOG, "Log truncated");
    }
    else if(CV_LOG == logType)
    {
        if(!ENABLE_CV_LOG) return;
        if(strlcpy(buffer, "CV LOG - ", length) >= length) _logger.logMessage(ERROR_LOG, "Log truncated");
    }
    else if(strlcpy(buffer, "DEFAULT LOG - ", length) >= length) _logger.logMessage(ERROR_LOG, "Log truncated");

    if(strlcat(buffer, Time.timeStr(), length) >= length) _logger.logMessage(ERROR_LOG, "Log truncated");
    if(strlcat(buffer, " - ", length) >= length) _logger.logMessage(ERROR_LOG, "Log truncated");
    if(strlcat(buffer, message, length) >= length) _logger.logMessage(ERROR_LOG, "Log truncated");

    logMessage(buffer);

    delete buffer;
}

void Logger::logFrame(const QSMatrix<float>& frame)
{
    if(_settings.getSdFrameLogging() && _sdPresent)
    {
        _sdBusy = true;

        File logFile;
        char buffer[24];
        if(strlcpy(buffer,  Time.format(Time.now(), "%d%m%y_Frame.txt"), sizeof(buffer)) >= sizeof(buffer)) _logger.logMessage(ERROR_LOG, "Frame log name truncated");

        if (logFile.open(buffer, O_RDWR | O_CREAT | O_AT_END))
        {
            logFile.printf("%d\r\n", Time.now());

            int16_t rows = frame.get_rows();
            int16_t columns = frame.get_cols();

            for(int16_t row = 0; row < rows; ++row)
            {
                for(int16_t column = 0; column < columns; ++column)
                {
                    logFile.printf("%2.2f ", frame(row, column));
                }
                logFile.printf("\r\n");
            }

            logFile.close();
        }
        else logMessage(ERROR_LOG, "Failed to log to SD card");

        _sdBusy = false;
    }
}

void Logger::logThermistor(const int16_t thermistor)
{
    if(_settings.getSdFrameLogging() && _sdPresent)
    {
        _sdBusy = true;

        File logFile;
        char buffer[24];
        if(strlcpy(buffer,  Time.format(Time.now(), "%d%m%y_Frame.txt"), sizeof(buffer)) >= sizeof(buffer)) _logger.logMessage(ERROR_LOG, "Frame log name truncated");

        if (logFile.open(buffer, O_RDWR | O_CREAT | O_AT_END))
        {
            logFile.printf("Thermistor: %3d\r\n\r\n", thermistor);
            logFile.close();
        }
        else logMessage(ERROR_LOG, "Failed to log to SD card");

        _sdBusy = false;
    }
}

void Logger::logMessage(const char* message)
{
    if(_settings.getDebugPrint()) Serial.printf("%s\r\n", message);

    if(_settings.getSdMessageLogging() && _sdPresent)
    {
        if(!_sdBusy)
        {
            _sdBusy = true;

            File logFile;
            char buffer[24];
            if(strlcpy(buffer, Time.format(Time.now(), "%d%m%y_Message.txt"), sizeof(buffer)) >= sizeof(buffer)) _logger.logMessage(ERROR_LOG, "Message log name truncated");


            if (logFile.open(buffer, O_CREAT | O_AT_END | O_RDWR))
            {
                while(!_logBuffer->empty())
                {
                    logFile.printf("%s\r\n", _logBuffer->front());
                    _logBuffer->pop();
                }

                logFile.printf("%s\r\n", message);
                logFile.close();
            }
            else logMessage(ERROR_LOG, "Failed to log to SD card");

            _sdBusy = false;
        }
        else _logBuffer->push(message);
    }
}

void Logger::clearSd()
{
    if(_sdPresent)
    {
        logMessage(INFO_LOG, "Clearing SD card");
        _sdFat->wipe();
        checkSd();
    }
}

int Logger::sdPercentFull()
{
    if(_sdPresent)
    {
        uint32_t cardSize = _sdFat->card()->cardSize();
        double cs = 0.000512 * cardSize;

        uint32_t volFree = _sdFat->vol()->freeClusterCount();
        double fs = 0.000512 * volFree * _sdFat->vol()->blocksPerCluster();

        int percent = 100 - round((cs / fs) * 100); //TODO confirm this really works
        logMessage(INFO_LOG, "SD percent full: " + percent);

        return percent;
    }
    else return 100;
}

Logger _logger;

#endif
