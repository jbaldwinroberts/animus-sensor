#ifndef __POINT_H_
#define __POINT_H_

struct Point
{
    float row;
    float col;

    //Instantiate
    Point(float row=0, float col=0)
        : row(row), col(col)
    {}

    //Modify
    Point& operator=(const Point& a)
    {
        row=a.row;
        col=a.col;
        return *this;
    }

    Point& operator+=(const Point& a)
    {
       row += a.row;
       col += a.col;
       return *this;
    }

    Point& operator/=(const int& a)
    {
       row /= a;
       col /= a;
       return *this;
    }

    //Compare
    bool operator==(const Point& a) const
    {
        return (row == a.row && col == a.col);
    }

    bool operator!=(const Point& a) const
    {
        return (row != a.row || col != a.col);
    }
};

#endif
