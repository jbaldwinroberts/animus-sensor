#ifndef __MUNKRES_H_
#define __MUNKRES_H_

#include "application.h"
#include "QSMatrix.h"
#include "math.h"
#include "Point.h"

#include <list>
#include <algorithm>
#include <utility>

class Munkres
{
  public:
    Munkres();
    virtual ~Munkres();
    QSMatrix<int16_t> solve(const std::vector<Point>& first, const std::vector<Point>& second, int16_t distanceThreshold);

  private:
    int16_t step1(const QSMatrix<float>& optimisedDistances, QSMatrix<int16_t>& mask, int16_t size);
    int16_t step2(const QSMatrix<int16_t>& mask, std::vector<bool>& colMask, int16_t size);
    int16_t step3(const QSMatrix<float>& optimisedDistances, QSMatrix<int16_t>& mask, std::vector<bool>& rowMask, std::vector<bool>& colMask, int16_t& saveRow, int16_t& saveCol, int16_t size);
    int16_t step4(QSMatrix<int16_t>& mask, std::vector<bool>& rowMask, std::vector<bool>& colMask, int16_t size, int16_t saveRow, int16_t saveCol);
    int16_t step5(QSMatrix<float>& optimisedDistances, const std::vector<bool>& rowMask, const std::vector<bool>& colMask, int16_t size);
    QSMatrix<float> generateOptimisedDistances(const std::vector<Point>& first, const std::vector<Point>& second, int16_t distanceThreshold);
    void minimise(QSMatrix<float>& optimisedDistances, int16_t size, bool over_cols);
    float getDistance(const Point& first, const Point& second);
    bool findUncoveredInMatrix(const QSMatrix<float>& optimisedDistances, const std::vector<bool>& rowMask, const std::vector<bool>& colMask, int16_t item, int16_t size, int16_t& row, int16_t& col);
    bool pairInList(const std::pair<size_t, size_t> &needle, const std::list<std::pair<size_t,size_t> > &haystack);
};

#endif
