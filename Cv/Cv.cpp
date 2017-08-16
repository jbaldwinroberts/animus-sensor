#ifndef __CV_CPP_
#define __CV_CPP_

#include "Cv.h"

Cv::Cv(uint8_t frameSize, uint8_t scaleFactor) : _frameSize(frameSize), _scaleFactor(scaleFactor),
    _foreground(_frameSize * _scaleFactor, _frameSize * _scaleFactor, 0)
{
    _logger.logMessage(INFO_LOG, "Started CV initialisation");

    if(_settings.getSavedHistoryAvailable())
    {
        _storage.readSavedHistory();
        _currentHistory = _settings.getHistoryLength() + 10;
    }
    else _storage.initialiseHistory(_frameSize, _scaleFactor);

    //printFrame(_storage._average, "Mean");
    //printFrame(_storage._variance, "Variance");
    //printFrame(_storage._standardDeviation, "SD");

    _logger.logMessage(INFO_LOG, "Finished CV initialisation");
}

Cv::~Cv() {}

void Cv::processFrame(QSMatrix<float> frame)
{
    //printFrame(frame, "RAW");

    //Scale the frame
    scaleFrame(frame);

    //Take copy of the frame before normalisation
    QSMatrix<float> scaledFrame(frame);

    //Normalise frame
    normalise(frame);

    //Find contours
    std::vector<Contour> contours = findContours(frame);

    //Remove small contours
    removeSmallContours(contours);

    //Get contour centroids
    getContourCentroids(contours);

    //Update foreground
    updateForeground(contours);

    //Update mean
    updateMean(scaledFrame);

    //Update variance
    updateVariance(scaledFrame);

    if(_currentHistory >= _settings.getHistoryLength())
    {
        //Assign new contours to existing tracks
        if(_tracks.size() != 0) updateTracks(contours);

        //Save new tracks
        saveNewTracks(contours);
    }

    //Print out track information
    //printTrackInfo();

    //Update stored contours
    updateContours(contours);

    //Increment history
    if(_currentHistory < _settings.getHistoryLength()) ++_currentHistory;
    else if(_currentHistory == _settings.getHistoryLength())
    {
        _storage.writeSavedHistory();
        _currentHistory = _settings.getHistoryLength() + 10;

        //printFrame(_storage._average, "Mean");
        //printFrame(_storage._variance, "Variance");
        //printFrame(_storage._standardDeviation, "SD");
    }
}

int16_t Cv::gateCount()
{
    if(_tracks.size() == 0) return 0;

    int16_t counts = 0;
    for(auto track = _tracks.begin(); track != _tracks.end(); ++track)
    {
        if(!endedAndValid(*track)) continue;
        counts += crossedDoor(*track);
    }

    return counts;
}

void Cv::scaleFrame(QSMatrix<float>& frame)
{
    //Check if we need to scale
    if(_scaleFactor < 2) return;

    //Store original frame size
    uint8_t originalRows = frame.get_rows();
    uint8_t originalCols = frame.get_cols();

    //Store scaled frame size
    uint8_t scaledRows = originalRows * _scaleFactor;
    uint8_t scaledCols = originalCols * _scaleFactor;

    //Final frame to be returned
    QSMatrix<float> scaledFrame = QSMatrix<float>(scaledRows, scaledCols, 0.0);

    for(uint8_t newRow = 0; newRow < scaledRows; ++newRow)
	{
        for(uint8_t newCol = 0; newCol < scaledCols; ++newCol)
        {
            //Closest pixel in frame
            float oldRow = (newRow + 0.5) / (float)_scaleFactor - 0.5;
            float oldCol = (newCol + 0.5) / (float)_scaleFactor - 0.5;

            //Check lower boundary
            oldRow = oldRow > 0 ? oldRow : 0;
            oldCol = oldCol > 0 ? oldCol : 0;

            //Set indexes so they are only calculated once
            //Casting to int8_t rounds the number down
            int8_t topIndex = (int8_t)oldRow;
            int8_t bottomIndex = topIndex + 1;
            int8_t leftIndex = (int8_t)oldCol;
            int8_t rightIndex = leftIndex + 1;

            //Check upper boundary
            bottomIndex = bottomIndex < originalRows ? bottomIndex : originalRows - 1;
            rightIndex = rightIndex < originalCols ? rightIndex : originalCols - 1;

            //Get surrounding 4 pixels
            float topLeftPixel = frame(topIndex, leftIndex);
            float topRightPixel = frame(topIndex, rightIndex);
            float bottomLeftPixel = frame(bottomIndex, leftIndex);
            float bottomRightPixel = frame(bottomIndex, rightIndex);

            //Interpolate
            float topInterpolation = linearInterpolation(topLeftPixel, topRightPixel, leftIndex, rightIndex, oldCol);
            float bottomInterpolation = linearInterpolation(bottomLeftPixel, bottomRightPixel, leftIndex, rightIndex, oldCol);
            float finalInterpolation = linearInterpolation(topInterpolation, bottomInterpolation, topIndex, bottomIndex, oldRow);
            scaledFrame(newRow, newCol) = finalInterpolation;
        }
	}

    frame = scaledFrame;
}

float Cv::linearInterpolation(float c1, float c2, float v1, float v2, float x)
{
    if(v1 == v2) return c1;
    return (((c2 - c1) / (v2 - v1)) * (x - v1)) + c1;
}

void Cv::normalise(QSMatrix<float>& frame)
{
    for(uint8_t row = 0, rows = frame.get_rows(); row < rows; ++row)
    {
        for(uint8_t column = 0, columns = frame.get_cols(); column < columns; ++column)
        {
            _storage._standardDeviation(row, column) = sqrt(_storage._variance(row, column));
            frame(row, column) = abs(frame(row, column) - _storage._average(row, column));
            if(_storage._standardDeviation(row, column) != 0)
            {
                frame(row, column) /= _storage._standardDeviation(row, column);
            }
        }
    }
}

void Cv::updateMean(const QSMatrix<float>& frame)
{
    for(uint8_t row = 0, rows = frame.get_rows(); row < rows; ++row)
    {
        for(uint8_t column = 0, columns = frame.get_cols(); column < columns; ++column)
        {
            float rate;
            if(_currentHistory < 10) rate = 1.0;
            else rate = 1.0 / _settings.getHistoryLength();

            if(_currentHistory < _settings.getHistoryLength() || _foreground(row, column) == 0)
            {
                _storage._average(row, column) -= rate * _storage._average(row, column);
                _storage._average(row, column) += rate * frame(row, column);
            }
        }
    }
}

void Cv::updateVariance(const QSMatrix<float>& frame)
{
    for(uint8_t row = 0, rows = frame.get_rows(); row < rows; ++row)
    {
        for(uint8_t column = 0, columns = frame.get_cols(); column < columns; ++column)
        {
            float rate = 1.0 / _settings.getHistoryLength();

            if(_currentHistory < _settings.getHistoryLength() || _foreground(row, column) == 0)
            {
                _storage._variance(row, column) -= (rate * _storage._variance(row, column));
                _storage._variance(row, column) += (rate * pow(frame(row, column) - _storage._average(row, column), 2.0));
            }
        }
    }
}

void Cv::updateForeground(const std::vector<Contour>& contours)
{
    //Clear foreground pixels
    for(uint8_t row = 0, rows = _foreground.get_rows(); row < rows; ++row)
	{
        for(uint8_t col = 0, cols = _foreground.get_cols(); col < cols; ++col)
        {
            _foreground(row, col) = 0;
        }
    }

    //Set foreground pixels
    for(auto contour = contours.begin(); contour != contours.end(); ++contour)
    {
        uint8_t startingRow = contour->minRow;
        uint8_t endingRow = contour->maxRow;
        uint8_t startingCol = contour->minCol;
        uint8_t endingCol = contour->maxCol;

        for(uint8_t row = startingRow; row <= endingRow; ++row)
        {
            for(uint8_t col = startingCol; col <= endingCol; ++col)
            {
                _foreground(row, col) = 1;
            }
        }
    }
}

std::vector<Contour> Cv::findContours(const QSMatrix<float>& frame)
{
    //COntour labels
    QSMatrix<uint8_t> labelledFrame(frame.get_rows(), frame.get_cols(), 0);
    std::map<uint8_t, std::set<uint8_t> > equivalentLabels;
    uint8_t label = 1;

    //Threshold
    float threshold = _settings.getBinaryThreshold() / 100.0;

    //First pass
    for(uint8_t row = 0, rows = frame.get_rows(); row < rows; ++row)
    {
        for(uint8_t col = 0, cols = frame.get_cols(); col < cols; ++col)
        {
            //Pixel is background - skip
            if(frame(row, col) < threshold) continue;

            //If pixel is on top left
            if(row == 0 && col == 0)
            {
                //Save new label
                labelledFrame(row, col) = label;
                equivalentLabels[label] = std::set<uint8_t>{label};
                ++label;
            }
            //If pixel is on top row
            else if(row == 0)
            {
                if(frame(row, col - 1) >= threshold)
                {
                    //Left pixel is foreground so copy label
                    labelledFrame(row, col) = labelledFrame(row, col - 1);
                }
                else
                {
                    //Left pixel is not forground so save new label
                    labelledFrame(row, col) = label;
                    equivalentLabels[label] = std::set<uint8_t>{label};
                    ++label;
                }
            }
            //If pixel is on left column
            else if(col == 0)
            {
                if(frame(row - 1, col) >= threshold)
                {
                    //Above pixel is forground so copy label
                    labelledFrame(row, col) = labelledFrame(row - 1, col);
                }
                else
                {
                    //Above pixel is not foreground so save new label
                    labelledFrame(row, col) = label;
                    equivalentLabels[label] = std::set<uint8_t>{label};
                    ++label;
                }
            }
            //Other positions
            else if(frame(row - 1, col) >= threshold)
            {
                if(frame(row - 1, col - 1) >= threshold)
                {
                    if(frame(row, col - 1) >= threshold)
                    {
                        //Get the smallest and largest label
                        uint8_t minimum = min(min(labelledFrame(row - 1, col), labelledFrame(row - 1, col - 1)), labelledFrame(row, col - 1));
                        uint8_t maximum = max(max(labelledFrame(row - 1, col), labelledFrame(row - 1, col - 1)), labelledFrame(row, col - 1));

                        //Both left, above left and above pixel are foregound so copy label with pixel with the smallest label
                        labelledFrame(row, col) = minimum;

                        //Store the larger label as a child of the smaller label
                        equivalentLabels[minimum].insert(maximum);
                    }
                    else
                    {
                        //Get the smallest and largest label
                        uint8_t minimum = min(labelledFrame(row - 1, col), labelledFrame(row - 1, col - 1));
                        uint8_t maximum = max(labelledFrame(row - 1, col), labelledFrame(row - 1, col - 1));

                        //Both above left and above pixel are foregound so copy label with pixel with the smallest label
                        labelledFrame(row, col) = minimum;

                        //Store the larger label as a child of the smaller label
                        equivalentLabels[minimum].insert(maximum);
                    }
                }
                else if(frame(row, col - 1) >= threshold)
                {
                    //Get the smallest and largest label
                    uint8_t minimum = min(labelledFrame(row - 1, col), labelledFrame(row, col - 1));
                    uint8_t maximum = max(labelledFrame(row - 1, col), labelledFrame(row, col - 1));

                    //Both left and above pixel are foregound so copy label with pixel with the smallest label
                    labelledFrame(row, col) = minimum;

                    //Store the larger label as a child of the smaller label
                    equivalentLabels[minimum].insert(maximum);
                }
                else
                {
                    //Only above pixel is foreground so copy label
                    labelledFrame(row, col) = labelledFrame(row - 1, col);
                }
            }
            else if(frame(row - 1, col - 1) >= threshold)
            {
                if(frame(row, col - 1) >= threshold)
                {
                    //Get the smallest and largest label
                    uint8_t minimum = min(labelledFrame(row - 1, col - 1), labelledFrame(row, col - 1));
                    uint8_t maximum = max(labelledFrame(row - 1, col - 1), labelledFrame(row, col - 1));

                    //Both left and above left pixel are foregound so copy label with pixel with the smallest label
                    labelledFrame(row, col) = minimum;

                    //Store the larger label as a child of the smaller label
                    equivalentLabels[minimum].insert(maximum);
                }
                else
                {
                    //Only above left pixel is foreground so copy label
                    labelledFrame(row, col) = labelledFrame(row - 1, col - 1);
                }
            }
            else if(frame(row, col - 1) >= threshold)
            {
                //Only left pixel is foreground so copy label
                labelledFrame(row, col) = labelledFrame(row, col - 1);
            }
            else
            {
                //Left, above left and above pixel is not foreground so save new label
                labelledFrame(row, col) = label;
                equivalentLabels[label] = std::set<uint8_t>{label};
                ++label;
            }
        }
    }

    //Contours found
    std::vector<Contour> contours;

    //Second pass
    for(uint8_t row = 0, rows = frame.get_rows(); row < rows; ++row)
    {
        for(uint8_t col = 0, cols = frame.get_cols(); col < cols; ++col)
        {
            //Pixel is background - skip
            if(labelledFrame(row, col) == 0) continue;

            //Get pixel label
            label = labelledFrame(row, col);

            //Check whether pixel is root
            uint8_t root = getRoot(equivalentLabels, label);
            while(root != 0)
            {
                //Pixel is not root so keep moving up the chain until the root is found
                label = root;
                root = getRoot(equivalentLabels, label);
            }

            //Save pixel label into root to flatten equivalentLabels
            equivalentLabels[label].insert(labelledFrame(row, col));

            //Set pixel to root label
            labelledFrame(row, col) = label;

            //Save contour
            saveContour(contours, label, Point(row, col));
        }
    }

    //printFrame(labelledFrame, "LABELLED");

    return contours;
}

uint8_t Cv::getRoot(const std::map<uint8_t, std::set<uint8_t> >& equivalentLabels, uint8_t label)
{
    if(equivalentLabels.begin() == equivalentLabels.end()) return 0;

    for(auto i = equivalentLabels.begin(); i != equivalentLabels.end(); ++i)
    {
        //Don't want to check if node exists in its own root - we know it does
        if(label != i->first)
        {
            //If label exists in another set
            if(i->second.find(label) != i->second.end())
            {
                //Return parent
                return i->first;
            }
        }
    }

    //Label does not exist - must be root
    return 0;
}

void Cv::saveContour(std::vector<Contour>& contours, uint8_t label, const Point& point)
{
    //Check if contour has already been created for label
    auto index = contours.end();
    for(auto contour = contours.begin(); contour != contours.end(); ++contour) if(contour->label == label) index = contour;

    //Contour already created
    if(index != contours.end())
    {
        //Save min/max row and point
        index->minRow = min(index->minRow, point.row);
        index->minCol = min(index->minCol, point.col);
        index->maxRow = max(index->maxRow, point.row);
        index->maxCol = max(index->maxCol, point.col);
        index->size += 1;
    }
    //Contour not already created
    else
    {
        //Create new contour
        Contour contour;
        contour.label = label;
        contour.minRow = point.row;
        contour.minCol = point.col;
        contour.maxRow = point.row;
        contour.maxCol = point.col;
        contour.size = 1;
        contour.updated = false;
        contour.deadTime = 0;
        contours.push_back(contour);
    }
}

void Cv::updateContours(std::vector<Contour>& contours)
{
    //Remove all old contours that have been updated
    _contours.erase(std::remove_if(begin(_contours), end(_contours),
    [&](Contour const &t){return t.updated;}), end(_contours));

    //Store contours
    for(auto contour = contours.begin(); contour != contours.end(); ++contour) if(contour->updated == false) _contours.push_back(*contour);

    //Remove all contours that have been alive 3 frames
    _contours.erase(std::remove_if(begin(_contours), end(_contours),
    [&](Contour const &t){return t.deadTime == _settings.getContourDeadTime();}), end(_contours));

    //Increment contour alive time
    for(auto contour = _contours.begin(); contour != _contours.end(); ++contour) contour->deadTime++;
}

void Cv::removeSmallContours(std::vector<Contour>& contours)
{
    contours.erase(std::remove_if(begin(contours), end(contours),
    [&](Contour const &t){return t.size <= _settings.getMinSize();}), end(contours));
}

void Cv::getContourCentroids(std::vector<Contour>& contours)
{
    if(contours.size() == 0) return;

    for(auto contour = contours.begin(); contour != contours.end(); ++contour)
    {
        //Set average of min max as centroid
        float centroidRow  = abs(contour->minRow + contour->maxRow) / 2.0;
        float centroidCol  = abs(contour->minCol + contour->maxCol) / 2.0;
    	contour->centroid = Point(centroidRow, centroidCol);
    }
}

void Cv::updateTracks(std::vector<Contour>& contours)
{
    //Remove dead tracks
    _tracks.erase(std::remove_if(begin(_tracks), end(_tracks),
    [&](Track const &t){return t.deadTime == _settings.getTrackDeadTime();}), end(_tracks));

    //_tracks.erase(std::remove_if(begin(_tracks), end(_tracks),
    //[&](Track const &t){return (t.deadTime == 1 && t.aliveTime == 0);}), end(_tracks));

    Munkres munkres;
    std::vector<Point> trackPositions;
    std::vector<Point> contourPositions;

    for(auto track = _tracks.begin(); track != _tracks.end(); ++track) trackPositions.push_back(track->predicted);
    for(auto contour = contours.begin(); contour != contours.end(); ++contour) contourPositions.push_back(contour->centroid);
    QSMatrix<int16_t> mask = munkres.solve(trackPositions, contourPositions, _settings.getDistanceThreshold());

    for(auto track = _tracks.begin(); track != _tracks.end(); ++track)
    {
        //Set updated to false
        track->updated = false;

        //Try to match contour with existing track
        for(auto contour = contours.begin(); contour != contours.end(); ++contour)
        {
            //Check whether contour has already been dealt with this frame
            if(contour->updated == true) continue;

            //Check whether track has already been dealt with this frame
            if(track->updated == true) continue;

            //Check if pair has been matched
            if(mask(track - _tracks.begin(), contour - contours.begin()) == true)
            {
                //Contour and track has been paired. Check distance is valid
                float distance = getDistance(contour->centroid, track->predicted);

                //Update track if distance is less than threshold
                if(distance <= _settings.getDistanceThreshold())
                {
                    //Set contour updated to true so it is not dealt with twice
                    contour->updated = true;

                    //Get velocity
                    Point velocity = getVelocity(contour->centroid, track->current);

                    //Check if track has passed door
                    if(track->valid == false)
                    {
                        if((track->current.row <= _settings.getDoorPosition() && contour->centroid.row >= _settings.getDoorPosition()) ||
                           (track->current.row >= _settings.getDoorPosition() && contour->centroid.row <= _settings.getDoorPosition()))
                        {
                            double avgPos = (track->current.col + contour->centroid.col) / 2;
                            if(avgPos >= _settings.getMinDoor() && avgPos <= _settings.getMaxDoor()) track->valid = true;
                            if(track->valid && _settings.getDebugPrint()) Serial.printf("Track: %d, Crossed door success, last %2.2f, current %2.2f\r\n", track - _tracks.begin(), track->current.col, contour->centroid.col);
                            else if(_settings.getDebugPrint()) Serial.printf("Track: %d, Crossed door fail, last %2.2f, current %2.2f\r\n", track - _tracks.begin(), track->current.col, contour->centroid.col);
                        }
                    }

                    //Update track
                    track->current = track->kalmanFilter.update(contour->centroid, velocity);
                    track->predicted = track->kalmanFilter.getNextStateEstimate();
                    track->deadTime = 0;
                    track->aliveTime++;
                    track->updated = true;
                }
            }
        }

        //Check whether track has been matched in this frame
        if(track->updated == false)
        {
            //Track not matched in this frame
            track->deadTime++;

            //Track has been dead for dead time and alive for alive time so is a valid track that has ended
            if(track->deadTime == _settings.getTrackDeadTime() && track->aliveTime >= _settings.getTrackAliveTime()) track->end = track->current;
        }
    }
}

void Cv::saveNewTracks(std::vector<Contour>& contours)
{
    if(_contours.size() == 0) return; //No old contours
    else if(contours.size() == 0) return; //No new contours
    else
    {
        Munkres munkres;
        std::vector<Point> oldContourPositions;
        std::vector<Point> newContourPositions;

        for(auto oldContour = _contours.begin(); oldContour != _contours.end(); ++oldContour) oldContourPositions.push_back(oldContour->centroid);
        for(auto newContour = contours.begin(); newContour != contours.end(); ++newContour) newContourPositions.push_back(newContour->centroid);
        QSMatrix<int16_t> mask = munkres.solve(oldContourPositions, newContourPositions, _settings.getDistanceThreshold());

        for(auto oldContour = _contours.begin(); oldContour != _contours.end(); ++oldContour)
        {
            for(auto newContour = contours.begin(); newContour != contours.end(); ++newContour)
            {
                //Check whether contours have already been dealt with
                if(oldContour->updated == true) continue;
                if(newContour->updated == true) continue;

                //Check if pair has been matched
                if(mask(oldContour - _contours.begin(), newContour - contours.begin()) == true)
                {
                    //Old contour and new contour has been paired. Check distance is valid
                    float distance = getDistance(oldContour->centroid, newContour->centroid);

                    //Update track if distance is less than threshold
                    if(distance <= _settings.getDistanceThreshold())
                    {
                        //Set contours updated to true so they are not dealt with twice
                        oldContour->updated = true;
                        newContour->updated = true;

                        //Get velocity
                        Point velocity = getVelocity(newContour->centroid, oldContour->centroid);

                        //Create new track
                        Track track(oldContour->centroid);
                        track.start = oldContour->centroid;
                        track.current = track.kalmanFilter.update(newContour->centroid, velocity);
                        track.predicted = track.kalmanFilter.getNextStateEstimate();
                        track.end = Point(-1, -1);
                        track.deadTime = 0;
                        track.aliveTime = 0;
                        track.updated = true;
                        track.valid = false;
                        _tracks.push_back(track);
                    }
                }
            }
        }
    }
}

bool Cv::endedAndValid(const Track& track)
{
    if(track.end != Point(-1, -1) && track.valid == true) return true;
    else return false;
}

int Cv::crossedDoor(const Track& track)
{
    if(track.start.row < _settings.getDoorPosition() && track.end.row > _settings.getDoorPosition()) return -1;
    else if(track.start.row > _settings.getDoorPosition() && track.end.row < _settings.getDoorPosition()) return 1;
    else return 0;
}

float Cv::getDistance(const Point& first, const Point& second)
{
	float distanceRow = abs(first.row - second.row);
	float distanceCol = abs(first.col - second.col);
	return sqrt(pow(distanceRow, 2) + pow(distanceCol, 2));
}

Point Cv::getVelocity(const Point& first, const Point& second)
{
    float velocityRow = first.row - second.row;
    float velocityCol = first.col - second.col;

	return Point(velocityRow, velocityCol);
}

void Cv::printContourInfo(const std::vector<Contour>& contours)
{
    if(contours.size() == 0) return;

    if(_settings.getDebugPrint()) Serial.printf("Contour information\r\n");
    else return;

    for(auto contour = contours.begin(); contour != contours.end(); ++contour)
    {
        if(_settings.getDebugPrint()) Serial.printf("Contour: %d, centroid:(%2.2f, %2.2f)\r\n", contour - contours.begin(), contour->centroid.row, contour->centroid.col);
    }

    if(_settings.getDebugPrint()) Serial.printf("\r\n");
}

void Cv::printTrackInfo()
{
    if(_tracks.size() == 0) return;

    if(_settings.getDebugPrint()) Serial.printf("Track information\r\n");
    else return;

    for(auto track = _tracks.begin(); track != _tracks.end(); ++track)
    {
        if(_settings.getDebugPrint()) Serial.printf("Track: %d, start:(%2.2f, %2.2f), end:(%2.2f, %2.2f), current:(%2.2f, %2.2f), predicted:(%2.2f, %2.2f), alive time:%d, dead time:%d\r\n",
            track - _tracks.begin(), track->start.row, track->start.col, track->end.row, track->end.col, track->current.row, track->current.col,
            track->predicted.row, track->predicted.col, track->aliveTime, track->deadTime);
    }

    if(_settings.getDebugPrint()) Serial.printf("\r\n");
}

//TODO
void Cv::printFrame(const QSMatrix<int16_t>& frame, const char* name)
{
    int16_t rows = frame.get_rows();
    int16_t columns = frame.get_cols();

    if(_settings.getDebugPrint()) Serial.printf("Frame: %s\r\n", name);
    else return;

    for(int16_t row = 0; row < rows; ++row)
    {
        for(int16_t column = 0; column < columns; ++column)
        {
            if(_settings.getDebugPrint()) Serial.printf("%3d ", frame(row, column));
        }
        if(_settings.getDebugPrint()) Serial.printf("\r\n");
    }
    if(_settings.getDebugPrint()) Serial.printf("\r\n");
}

void Cv::printFrame(const QSMatrix<int32_t>& frame, const char* name)
{
    int16_t rows = frame.get_rows();
    int16_t columns = frame.get_cols();

    if(_settings.getDebugPrint()) Serial.printf("Frame: %s\r\n", name);
    else return;

    for(int16_t row = 0; row < rows; ++row)
    {
        for(int16_t column = 0; column < columns; ++column)
        {
            if(_settings.getDebugPrint()) Serial.printf("%3d ", frame(row, column));
        }
        if(_settings.getDebugPrint()) Serial.printf("\r\n");
    }
    if(_settings.getDebugPrint()) Serial.printf("\r\n");
}

void Cv::printFrame(const QSMatrix<float>& frame, const char* name)
{
    int16_t rows = frame.get_rows();
    int16_t columns = frame.get_cols();

    if(_settings.getDebugPrint()) Serial.printf("Frame: %s\r\n", name);
    else return;

    for(int16_t row = 0; row < rows; ++row)
    {
        for(int16_t column = 0; column < columns; ++column)
        {
            if(_settings.getDebugPrint()) Serial.printf("%3.2f ", frame(row, column));
        }
        if(_settings.getDebugPrint()) Serial.printf("\r\n");
    }
    if(_settings.getDebugPrint()) Serial.printf("\r\n");
}

void Cv::printFrame(const QSMatrix<uint8_t>& frame, const char* name)
{
    int16_t rows = frame.get_rows();
    int16_t columns = frame.get_cols();

    if(_settings.getDebugPrint()) Serial.printf("--------------------------------------------------------------------\r\n|");
    else return;

    //Serial.printf("Frame: %s\r\n", name);
    for(int16_t row = 0; row < rows; ++row)
    {
        for(int16_t column = 0; column < columns; ++column)
        {
            //Serial.printf("%3d ", frame(row, column));
            if(frame(row, column) == 0 && _settings.getDebugPrint()) Serial.printf("  ");
            else if(_settings.getDebugPrint()) Serial.printf("* ");
        }
        if(_settings.getDebugPrint()) Serial.printf("|\r\n|");
    }
    if(_settings.getDebugPrint()) Serial.printf("--------------------------------------------------------------------");
    if(_settings.getDebugPrint()) Serial.printf("\r\n");
}

#endif
