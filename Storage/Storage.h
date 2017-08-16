#ifndef __STORAGE_H_
#define __STORAGE_H_

#include "Global.h"
#include "QSMatrix.h"
#include "DynamicQueue.h"

class Storage
{
  public:
    Storage();
    virtual ~Storage();
    void begin();

    void addGateCount(int8_t count);
    void saveGateCount(bool force = false);
    void pirTriggered() volatile;
    void saveCounts();
    void saveTempCounts();
    void clear();
    void clearCounts();
    void clearTempCounts();
    void clearInvalidData();
    void clearInvalidTempData();
    void readSavedHistory();
    void writeSavedHistory();
    void initialiseHistory(uint8_t frameSize, uint8_t scaleFactor);

    DynamicQueue* _gateCountBuffer;
    DynamicQueue* _transmitGateCountBuffer;
    DynamicQueue* _invalidCountBuffer;
    DynamicQueue* _transmitInvalidCountBuffer;

    QSMatrix<float> _average;
    QSMatrix<float> _variance;
    QSMatrix<float> _standardDeviation;

    const int _sendingHour = 3;
    bool _sent = false;

  private:
    volatile uint16_t _pirTriggers = 0;
    int16_t _totalGateCount = 0;
};

extern Storage _storage;

#endif
