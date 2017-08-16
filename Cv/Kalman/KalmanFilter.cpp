#ifndef __KALMANFILTER_CPP_
#define __KALMANFILTER_CPP_

#include "KalmanFilter.h"

KalmanFilter::KalmanFilter(const Point& initialPosition) :
    _stateEstimate(4, 1, 0.0), _nextStateEstimate(4, 1, 0.0), _probabilityEstimate(4, 4, 0.0), _stateTransition(4, 4, 0),
    _observation(4, 4, 0), _identity(4, 4, 0), _processError(4, 4, 0.0), _measurementError(4, 4, 0.0)
{
    //Initalise state estimate
    _stateEstimate(0, 0) = initialPosition.row;
    _stateEstimate(1, 0) = initialPosition.col;

    //Initialise probability estimate
    _probabilityEstimate(0, 0) = 1.0;
    _probabilityEstimate(1, 1) = 1.0;
    _probabilityEstimate(2, 2) = 1.0;
    _probabilityEstimate(3, 3) = 1.0;

    //Initialise state transition
    _stateTransition(0, 0) = 1.0;
    _stateTransition(0, 2) = 1.0;
    _stateTransition(1, 1) = 1.0;
    _stateTransition(1, 3) = 1.0;
    _stateTransition(2, 2) = 1.0;
    _stateTransition(3, 3) = 1.0;

    //Initialise observation
    _observation(0, 0) = 1.0;
    _observation(1, 1) = 1.0;
    _observation(2, 2) = 1.0;
    _observation(3, 3) = 1.0;

    //Initialise identity
    _identity(0, 0) = 1.0;
    _identity(1, 1) = 1.0;
    _identity(2, 2) = 1.0;
    _identity(3, 3) = 1.0;

    //Initialise process error - model
    _processError(0, 0) = 0.2;
    _processError(1, 1) = 0.2;
    _processError(2, 2) = 0.2;
    _processError(3, 3) = 0.2;

    //Initialise measurement error - sensor
    _measurementError(0, 0) = 0.0;
    _measurementError(1, 1) = 0.0;
    _measurementError(2, 2) = 0.0;
    _measurementError(3, 3) = 0.0;
}

KalmanFilter::~KalmanFilter() {}

Point KalmanFilter::update(const Point& actualPosition, const Point& actualVelocity)
{
    //Convert position and velocity to state
    QSMatrix<float> actualState(4, 1, 0.0);
    actualState(0, 0) = actualPosition.row;
    actualState(1, 0) = actualPosition.col;
    actualState(2, 0) = actualVelocity.row;
    actualState(3, 0) = actualVelocity.col;

    //Prediction
    QSMatrix<float> predictedStateEstimate = _stateTransition * _stateEstimate;
    QSMatrix<float> predictedProbabilityEstimate = (_stateTransition * _probabilityEstimate) * _stateTransition.transpose() + _processError;

    //Observation
    QSMatrix<float> innovation = actualState - _observation * predictedStateEstimate;
    QSMatrix<float> innovation_covariance = _observation * predictedProbabilityEstimate * _observation.transpose() + _measurementError;

    //Update
    QSMatrix<float> gain = predictedProbabilityEstimate * _observation.transpose()* innovation_covariance.inverse();
    _stateEstimate = predictedStateEstimate + gain * innovation;
    _probabilityEstimate = (_identity - gain * _observation) * predictedProbabilityEstimate;

    //Predict next step
    _nextStateEstimate = _stateTransition * _stateEstimate;

    //Convert state estimate to point
    Point point(_stateEstimate(0, 0), _stateEstimate(1, 0));

    return point;
}

Point KalmanFilter::getNextStateEstimate()
{
    //Convert next state estimate to point
    Point point(_nextStateEstimate(0, 0), _nextStateEstimate(1, 0));

    return point;
}

#endif
