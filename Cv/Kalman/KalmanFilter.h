#ifndef __KALMANFILTER_H_
#define __KALMANFILTER_H_

#include "application.h"
#include "QSMatrix.h"
#include "Point.h"

class KalmanFilter
{
  public:
    KalmanFilter(const Point& initialPosition);
    virtual ~KalmanFilter();

    Point update(const Point& actualPosition, const Point& actualVelocity = Point(0, 0));
    Point getNextStateEstimate();

  private:
    //Variables
    QSMatrix<float> _stateEstimate;
    QSMatrix<float> _nextStateEstimate;
    QSMatrix<float> _probabilityEstimate;

    //Constants
    QSMatrix<float> _stateTransition;
    QSMatrix<float> _observation;
    QSMatrix<float> _identity;
    QSMatrix<float> _processError;
    QSMatrix<float> _measurementError;
};

#endif
