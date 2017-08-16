#ifndef __TRACK_H_
#define __TRACK_H_

#include "Point.h"
#include "KalmanFilter.h"

struct Track
{
	Track(const Point& initialPosition) : kalmanFilter(initialPosition) {} //Initialise kalmanFilter
	virtual ~Track() {}
    Point start; //Start position of track
    Point current; //Current position of track
    Point predicted; //Predicted posion of track
    Point end; //End position of track
	KalmanFilter kalmanFilter; //Kalman filter for this track
    int16_t aliveTime; //Number of frames since the track began
    int16_t deadTime; //Number of frames since the track was last seen
    bool updated; //True if the track as been updated in the current frame
	bool valid;
};

#endif
