#ifndef __GRIDEYE_CPP_
#define __GRIDEYE_CPP_

#include "GridEye.h"

GridEye::GridEye()
{
    _logger.logMessage(INFO_LOG, "Started Grid Eye initialisation");

    Wire.setSpeed(CLOCK_SPEED_400KHZ);
    Wire.begin();
    initialReset();
    setFPS(true);
    setAverage(true);

    _logger.logMessage(INFO_LOG, "Finished Grid Eye initialisation");
}

GridEye::~GridEye() {}

void GridEye::readFrame(QSMatrix<float>& frame)
{
    std::vector<byte> data;
    readPacket(data, 0x80, 128);

	for(int16_t i = 0; i < 64; ++i)
	{
	    int16_t row = i / 8;
        int16_t column = i % 8;
        float value = (((data[(2*i)+1] << 8) | data[2*i]) * 1) * 0.25;
        frame(row, column) = value;
	}
}

int16_t GridEye::readThermistor()
{
    std::vector<byte> data;
	readPacket(data, 0x0E, 2);
    float value = (((data[1] << 8) | data[0]) * 0.0625);
	return value * 100;
}

bool GridEye::setAverage(bool on)
{
	int16_t data = (on << 5);
	return writePacket(0x07, data);
}

bool GridEye::setFPS(bool fps_set_10)
{
	int16_t data = !fps_set_10;
	return writePacket(0x02, data);
}

bool GridEye::initialReset()
{
	int16_t data = 0x3F;
	return writePacket(0x01, data);
}

bool GridEye::writePacket(int16_t addr, int16_t data)
{
	Wire.beginTransmission(ADDRESS);
	Wire.write(addr);
	Wire.write(data);
	return Wire.endTransmission() == 0;
}

void GridEye::readPacket(std::vector<byte>& data, int16_t addr, int16_t datalength)
{
    if(datalength <= 32)
    {
        Wire.beginTransmission(ADDRESS);
        Wire.write(addr);
        Wire.endTransmission();

        Wire.requestFrom(ADDRESS, datalength);
        while(Wire.available()) data.push_back(Wire.read());
    }
    else
    {
        int16_t dataRead = 0;

        while((datalength - dataRead) > 32)
        {
            Wire.beginTransmission(ADDRESS);
            Wire.write(addr + dataRead);
            Wire.endTransmission();

            Wire.requestFrom(ADDRESS, 32);
            while(Wire.available()) data.push_back(Wire.read());

            dataRead += 32;
        }

        Wire.beginTransmission(ADDRESS);
        Wire.write(addr + dataRead);
        Wire.endTransmission();

        Wire.requestFrom(ADDRESS, (datalength - dataRead));
        while(Wire.available()) data.push_back(Wire.read());
    }
}

#endif
