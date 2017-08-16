#ifndef __EXTERNAL_EEPROM_H_
#define __EXTERNAL_EEPROM_H_

#include "Global.h"
#include "SPIEEP.h"
//#include "AnimusUtil.h"
#include "DynamicQueue.h"
#include "QSMatrix.h"
#include <algorithm>

//using namespace util;

class ExternalEeprom
{
  public:
    ExternalEeprom();
    virtual ~ExternalEeprom();
    void begin();

    void readSettings(byte* settings, uint16_t size);
    void writeSettings(byte* settings, uint16_t size);
    void readData();
    void writeData();
    void writeData(char* buffer, uint16_t length);
    void readInvalidData(DynamicQueue* items);

    void writeAverageFrame(const QSMatrix<float>& frame);
    void writeVarianceFrame(const QSMatrix<float>& frame);
    void writeSdFrame(const QSMatrix<float>& frame);
    void writeFrame(const QSMatrix<float>& frame, int index);
    QSMatrix<float> readAverageFrame();
    QSMatrix<float> readVarianceFrame();
    QSMatrix<float> readSdFrame();
    QSMatrix<float> readFrame(int index);

    void setLastSaveTime();
    unsigned long getLastSaveTime() const;
    void setLastLoadTime();
    unsigned long getLastLoadTime() const;
    bool checkInvalidData() const;

  private:
    SPIEEP* _spieep;
    DynamicQueue* _invalidItems;

    char* readFullData();
    char* readPartialData();
    bool validateItem(const char* item);
    bool startsWith(const char *a, const char *b);
    bool endsWith(const char *a, const char *b);
    int16_t indexOf(const char* a, const char* b, int16_t startIndex);
    int16_t indexOf(const char* a, const char* b);

    unsigned long _lastSaveTime = 0;
    unsigned long _lastLoadTime = 0;

    union
    {
        float f;
        char c[4];
    } _u;
};

extern ExternalEeprom _externalEeprom;

#endif
