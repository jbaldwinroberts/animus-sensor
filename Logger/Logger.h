#ifndef __LOGGER_H_
#define __LOGGER_H_

#include "Global.h"
#include "SdFat.h"
#include "QSMatrix.h"
#include "DynamicQueue.h"

enum LogType
{
    INFO_LOG,
    WARN_LOG,
    ERROR_LOG,
    WALKS_LOG,
    CV_LOG
};

class Logger
{
  public:
    Logger();
    virtual ~Logger();
    void begin();
    void checkSd();

    void logMessage(const LogType logType, const char* message);
    void logFrame(const QSMatrix<float>& frame);
    void logThermistor(const int16_t thermistor);
    void clearSd();
    int sdPercentFull();

  private:
    SdFat* _sdFat;

    void logMessage(const char* message);
    bool _sdPresent = false;
    bool _sdBusy = false;
    DynamicQueue* _logBuffer;
};

extern Logger _logger;

#endif
