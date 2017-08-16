#ifndef __CONTOUR_H_
#define __CONTOUR_H_

#include "Point.h"

struct Contour
{
	uint8_t label; //Contour label
	uint8_t minRow; //The minimum contour pixel row
	uint8_t minCol; //The minium contour pixel column
	uint8_t maxRow; //The maximum contour pixel row
	uint8_t maxCol; //The maximum contour pixel column
	uint16_t size; //Number of pixels in contour
    Point centroid; //Centre of contour
	bool updated; //True if the contour as been updated in the current frame
	uint8_t deadTime;
};

#endif
