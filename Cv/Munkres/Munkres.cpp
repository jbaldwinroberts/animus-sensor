#ifndef __MUNKRES_CPP_
#define __MUNKRES_CPP_

#include "Munkres.h"

Munkres::Munkres() {}

Munkres::~Munkres() {}

QSMatrix<int16_t> Munkres::solve(const std::vector<Point>& first, const std::vector<Point>& second, int16_t distanceThreshold)
{
    QSMatrix<float> optimisedDistances = generateOptimisedDistances(first, second, distanceThreshold);

    int16_t size = optimisedDistances.get_rows();
    QSMatrix<int16_t> mask(size, size, 0);
    std::vector<bool> rowMask;
    std::vector<bool> colMask;
    int16_t saveRow = 0;
    int16_t saveCol = 0;

    for(int16_t i = 0; i < size; ++i)
    {
        rowMask.push_back(false);
        colMask.push_back(false);
    }

    minimise(optimisedDistances, size, false);
    minimise(optimisedDistances, size, true);

    // Follow the steps
    int16_t step = 1;
    while(step)
    {
        switch(step)
        {
            case 1:
                step = step1(optimisedDistances, mask, size);
                break;
            case 2:
                step = step2(mask, colMask, size);
                break;
            case 3:
                step = step3(optimisedDistances, mask, rowMask, colMask, saveRow, saveCol, size);
                break;
            case 4:
                step = step4(mask, rowMask, colMask, size, saveRow, saveCol);
                break;
            case 5:
                step = step5(optimisedDistances, rowMask, colMask, size);
                break;
            default:
                break;
        }
    }

    return mask;
}

int16_t Munkres::step1(const QSMatrix<float>& optimisedDistances, QSMatrix<int16_t>& mask, int16_t size)
{
    for(int16_t row = 0; row < size; ++row)
    {
        for(int16_t col = 0; col < size; ++col)
        {
            if(optimisedDistances(row, col) == 0)
            {
                for(int16_t nRow = 0; nRow < row; ++nRow)
                {
                    if(mask(nRow, col) == 1) goto next_col;
                }

            mask(row, col) = 1;
            goto next_row;
        }
        next_col:;
      }
      next_row:;
    }

    return 2;
}

int16_t Munkres::step2(const QSMatrix<int16_t>& mask, std::vector<bool>& colMask, int16_t size)
{
    int16_t covercount = 0;

    for(int16_t row = 0; row < size; ++row)
    {
        for(int16_t col = 0; col < size; ++col)
        {
            if(mask(row, col) == 1)
            {
              colMask[col] = true;
              covercount++;
            }
        }
    }

    if(covercount >= size) return 0;

    return 3;
}

int16_t Munkres::step3(const QSMatrix<float>& optimisedDistances, QSMatrix<int16_t>& mask, std::vector<bool>& rowMask, std::vector<bool>& colMask, int16_t& saveRow, int16_t& saveCol, int16_t size)
{
    if(findUncoveredInMatrix(optimisedDistances, rowMask, colMask, 0, size, saveRow, saveCol)) mask(saveRow, saveCol) = 2;
    else return 5;

    for(int16_t nCol = 0; nCol < size; ++nCol)
    {
        if(mask(saveRow, nCol) == 1)
        {
            rowMask[saveRow] = true;
            colMask[nCol] = false;
            return 3;
        }
    }

    return 4;
}

int16_t Munkres::step4(QSMatrix<int16_t>& mask, std::vector<bool>& rowMask, std::vector<bool>& colMask, int16_t size, int16_t saveRow, int16_t saveCol)
{
    std::list<std::pair<size_t,size_t> > seq;
    std::pair<size_t,size_t> z0(saveRow, saveCol);
    seq.insert(seq.end(), z0);

    std::pair<size_t,size_t> z1(-1, -1);
    std::pair<size_t,size_t> z2n(-1, -1);

    int16_t row = saveCol;
    int16_t col = saveCol;
    bool madepair;

    do
    {
        madepair = false;
        for(row = 0; row < size; ++row)
        {
            if(mask(row, col) == 1)
            {
                z1.first = row;
                z1.second = col;
                if(pairInList(z1, seq)) continue;

                madepair = true;
                seq.insert(seq.end(), z1);
                break;
            }
        }

        if (!madepair) break;

        madepair = false;

        for(col = 0; col < size; ++col)
        {
            if(mask(row, col) == 2)
            {
                z2n.first = row;
                z2n.second = col;
                if(pairInList(z2n, seq)) continue;

                madepair = true;
                seq.insert(seq.end(), z2n);
                break;
            }
        }
    }
    while(madepair);

    for(auto i = seq.begin(); i != seq.end(); ++i)
    {
        if(mask(i->first,i->second) == 1) mask(i->first,i->second) = 0;
        if(mask(i->first,i->second) == 2) mask(i->first,i->second) = 1;
    }

    for(int16_t row = 0; row < size; ++row)
    {
        for(int16_t col = 0; col < size; ++col)
        {
            if(mask(row, col) == 2) mask(row, col) = 0;
        }
    }

    for(int16_t i = 0; i < size; ++i)
    {
        rowMask[i] = false;
        colMask[i] = false;
    }

    return 2;
}

int16_t Munkres::step5(QSMatrix<float>& optimisedDistances, const std::vector<bool>& rowMask, const std::vector<bool>& colMask, int16_t size)
{
    float h = std::numeric_limits<float>::max();

    for(int16_t row = 0; row < size; ++row)
    {
        if (!rowMask[row])
        {
            for(int16_t col = 0; col < size; ++col)
            {
                if(!colMask[col])
                {
                    if(h > optimisedDistances(row, col) && optimisedDistances(row, col) != 0) h = optimisedDistances(row, col);
                }
            }
        }
    }

    for(int16_t row = 0; row < size; ++row)
    {
        if(rowMask[row])
        {
            for(int16_t col = 0; col < size; ++col) optimisedDistances(row, col) += h;
        }
    }

    for(int16_t col = 0; col < size; ++col)
    {
        if(!colMask[col])
        {
            for(int16_t row = 0; row < size; ++row) optimisedDistances(row, col) -= h;
        }
    }

    return 3;
}

QSMatrix<float> Munkres::generateOptimisedDistances(const std::vector<Point>& first, const std::vector<Point>& second, int16_t distanceThreshold)
{
    int16_t penaliseDistance = max(first.size(), second.size()) * distanceThreshold;
    QSMatrix<float> distances(first.size(), second.size(), 0);

    //Calculate distances between all contours and tracks
    for(int16_t row = 0, rows = distances.get_rows(); row < rows; ++row)
    {
        for(int16_t col = 0, cols = distances.get_cols(); col < cols; ++col)
        {
            distances(row, col) = getDistance(first[row], second[col]);
        }
    }

    //Check for any row where all values exceed the distance threshold
    for(int16_t row = 0, rows = distances.get_rows(); row < rows; ++row)
    {
        //Get row
        std::vector<float> completeRow = distances.row_vec(row);

        //Check elements against distance threshold
        if(std::all_of(completeRow.begin(), completeRow.end(), [=](int16_t distance)->bool{return distance > distanceThreshold;}))
        {
            //All elements are greater than distance threshold so assign max number
            completeRow.assign(completeRow.size(), penaliseDistance);

            //Save vector
            distances.insert_row_vec(completeRow, row);
        }
    }

    //Check for any column where all values exced the distance threshold
    for(int16_t col = 0, cols = distances.get_cols(); col < cols; ++col)
    {
        //Get column
        std::vector<float> completeCol = distances.col_vec(col);

        //Check elements against distance threshold
        if(std::all_of(completeCol.begin(), completeCol.end(), [=](int16_t distance)->bool{return distance > distanceThreshold;}))
        {
            //All elements are greater than distance threshold so assign max number
            completeCol.assign(completeCol.size(), penaliseDistance);

            //Save vector
            distances.insert_col_vec(completeCol, col);
        }
    }

    //Penalise distances that exceed distance threshold
    for(int16_t row = 0, rows = distances.get_rows(); row < rows; ++row)
    {
        for(int16_t col = 0, cols = distances.get_cols(); col < cols; ++col)
        {
            if(distanceThreshold < distances(row, col)) distances(row, col) = penaliseDistance;
        }
    }

    //Square off array
    if(distances.get_rows() != distances.get_cols())
    {
        int16_t size = max(distances.get_rows(), distances.get_cols());
        QSMatrix<float> tempDistances(size, size, penaliseDistance);

        for(int16_t row = 0, rows = distances.get_rows(); row < rows; ++row)
        {
            //Get row
            std::vector<float> completeRow = distances.row_vec(row);

            //Insert row
            tempDistances.insert_row_vec(completeRow, row);
        }

        distances = tempDistances;
    }

    return distances;
}

void Munkres::minimise(QSMatrix<float>& optimisedDistances, int16_t size, bool over_cols)
{
    for(int16_t i = 0; i < size; ++i)
    {
        float minimum = over_cols ? optimisedDistances(0, i) : optimisedDistances(i, 0);

        for(int16_t j = 1; j < size && minimum > 0; ++j) minimum = min(minimum, over_cols ? optimisedDistances(j, i) : optimisedDistances(i, j));

        if(minimum > 0)
        {
            for(int16_t j = 0; j < size ; ++j)
            {
                if(over_cols) optimisedDistances(j, i) -= minimum;
                else optimisedDistances(i, j) -= minimum;
            }
        }
    }
}

float Munkres::getDistance(const Point& first, const Point& second)
{
	float distanceRow = abs(first.row - second.row);
	float distanceCol = abs(first.col - second.col);
	return sqrt(pow(distanceRow, 2) + pow(distanceCol, 2));
}

bool Munkres::findUncoveredInMatrix(const QSMatrix<float>& optimisedDistances, const std::vector<bool>& rowMask, const std::vector<bool>& colMask, int16_t item, int16_t size, int16_t& row, int16_t& col)
{
    for(row = 0; row < size; ++row)
    {
        if(!rowMask[row])
        {
            for(col = 0; col < size; ++col)
            {
                if(!colMask[col])
                {
                    if (optimisedDistances(row, col) == item) return true;
                }
            }
        }
    }

    return false;
}

bool Munkres::pairInList(const std::pair<size_t, size_t> &needle, const std::list<std::pair<size_t,size_t> > &haystack)
{
    for(auto i = haystack.begin(); i != haystack.end(); ++i)
    {
        if(needle == *i) return true;
    }

    return false;
}

#endif
