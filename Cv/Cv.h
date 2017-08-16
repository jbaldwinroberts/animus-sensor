#ifndef __CV_H_
#define __CV_H_

#include "Global.h"
#include "QSMatrix.h"
#include "math.h"
#include "Munkres.h"
#include "Point.h"
#include "Track.h"
#include "Contour.h"

#include <vector>
#include <map>
#include <set>
#include <algorithm>

class Cv
{
  public:
    Cv(uint8_t frameSize, uint8_t scaleFactor);
    virtual ~Cv();
    void processFrame(QSMatrix<float> frame);
    int16_t gateCount();

  private:
    //Image processing
    void scaleFrame(QSMatrix<float>& frame);
    float linearInterpolation(float c1, float c2, float v1, float v2, float x);
    void normalise(QSMatrix<float>& frame);
    void updateMean(const QSMatrix<float>& frame);
    void updateVariance(const QSMatrix<float>& frame);
    void updateForeground(const std::vector<Contour>& contours);

    //Contours
    std::vector<Contour> findContours(const QSMatrix<float>& frame);
    uint8_t getRoot(const std::map<uint8_t, std::set<uint8_t> >& equivalentLabels, uint8_t label);
	void saveContour(std::vector<Contour>& contours, uint8_t label, const Point& point);
    void updateContours(std::vector<Contour>& contours);
    void removeSmallContours(std::vector<Contour>& contours);
    void getContourCentroids(std::vector<Contour>& contours);

    //Tracking
    void updateTracks(std::vector<Contour>& contours);
    void saveNewTracks(std::vector<Contour>& contours);

    ///Counting
    bool endedAndValid(const Track& track);
    int crossedDoor(const Track& track);

    //Utilities
    float getDistance(const Point& first, const Point& second);
    Point getVelocity(const Point& first, const Point& second);

    //Printing //TODO template
    void printContourInfo(const std::vector<Contour>& contours);
	void printTrackInfo();
    void printFrame(const QSMatrix<int16_t>& frame, const char* name);
    void printFrame(const QSMatrix<int32_t>& frame, const char* name);
    void printFrame(const QSMatrix<float>& frame, const char* name);
    void printFrame(const QSMatrix<uint8_t>& frame, const char* name);

    //Global variables TODO move to heap
    uint8_t _frameSize = 0;
    uint8_t _scaleFactor = 0;
    uint16_t _currentHistory = 0;
    QSMatrix<uint8_t> _foreground;
    std::vector<Track> _tracks;
    std::vector<Contour> _contours;
};

#endif
