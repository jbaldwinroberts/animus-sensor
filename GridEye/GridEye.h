#ifndef __GRIDEYE_H_
#define __GRIDEYE_H_

#include "Global.h"
#include "QSMatrix.h"

class GridEye
{
  public:
    GridEye();
    virtual ~GridEye();

    void readFrame(QSMatrix<float>& frame);
    int16_t readThermistor();

  private:
    bool setFPS(bool fps_set_10);
    bool setAverage(bool on);
    bool initialReset();
    bool writePacket(int16_t addr, int16_t data);
    void readPacket(std::vector<byte>& data, int16_t addr, int16_t datalength);

    const int ADDRESS = 0x68;
};

#endif
