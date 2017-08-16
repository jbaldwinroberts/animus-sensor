#ifndef __QS_MATRIX_CPP
#define __QS_MATRIX_CPP

#include "QSMatrix.h"

//Default Constructor
template<typename T>
QSMatrix<T>::QSMatrix() {}

// Parameter Constructor
template<typename T>
QSMatrix<T>::QSMatrix(unsigned _rows, unsigned _cols, const T& _initial) {
  mat.resize(_rows);
  for (unsigned i=0; i<mat.size(); i++) {
    mat[i].resize(_cols, _initial);
  }
  rows = _rows;
  cols = _cols;
}

// Copy Constructor
template<typename T>
QSMatrix<T>::QSMatrix(const QSMatrix<T>& rhs) {
  mat = rhs.mat;
  rows = rhs.get_rows();
  cols = rhs.get_cols();
}

// (Virtual) Destructor
template<typename T>
QSMatrix<T>::~QSMatrix() {}

// Assignment Operator
template<typename T>
QSMatrix<T>& QSMatrix<T>::operator=(const QSMatrix<T>& rhs) {
  if (&rhs == this)
    return *this;

  unsigned new_rows = rhs.get_rows();
  unsigned new_cols = rhs.get_cols();

  mat.resize(new_rows);
  for (unsigned i=0; i<mat.size(); i++) {
    mat[i].resize(new_cols);
  }

  for (unsigned i=0; i<new_rows; i++) {
    for (unsigned j=0; j<new_cols; j++) {
      mat[i][j] = rhs(i, j);
    }
  }
  rows = new_rows;
  cols = new_cols;

  return *this;
}

// Addition of two matrices
template<typename T>
QSMatrix<T> QSMatrix<T>::operator+(const QSMatrix<T>& rhs) {
  QSMatrix result(rows, cols, 0.0);

  for (unsigned i=0; i<rows; i++) {
    for (unsigned j=0; j<cols; j++) {
      result(i,j) = this->mat[i][j] + rhs(i,j);
    }
  }

  return result;
}

// Cumulative addition of this matrix and another
template<typename T>
QSMatrix<T>& QSMatrix<T>::operator+=(const QSMatrix<T>& rhs) {
  unsigned rows = rhs.get_rows();
  unsigned cols = rhs.get_cols();

  for (unsigned i=0; i<rows; i++) {
    for (unsigned j=0; j<cols; j++) {
      this->mat[i][j] += rhs(i,j);
    }
  }

  return *this;
}

// Subtraction of this matrix and another
template<typename T>
QSMatrix<T> QSMatrix<T>::operator-(const QSMatrix<T>& rhs) {
  unsigned rows = rhs.get_rows();
  unsigned cols = rhs.get_cols();
  QSMatrix result(rows, cols, 0.0);

  for (unsigned i=0; i<rows; i++) {
    for (unsigned j=0; j<cols; j++) {
      result(i,j) = this->mat[i][j] - rhs(i,j);
    }
  }

  return result;
}

// Cumulative subtraction of this matrix and another
template<typename T>
QSMatrix<T>& QSMatrix<T>::operator-=(const QSMatrix<T>& rhs) {
  unsigned rows = rhs.get_rows();
  unsigned cols = rhs.get_cols();

  for (unsigned i=0; i<rows; i++) {
    for (unsigned j=0; j<cols; j++) {
      this->mat[i][j] -= rhs(i,j);
    }
  }

  return *this;
}

// Left multiplication of this matrix and another
template<typename T>
QSMatrix<T> QSMatrix<T>::operator*(const QSMatrix<T>& rhs) {
  unsigned rows = rhs.get_rows();
  unsigned cols = rhs.get_cols();
  QSMatrix result(rows, cols, 0.0);

  for (unsigned i=0; i<rows; i++) {
    for (unsigned j=0; j<cols; j++) {
      for (unsigned k=0; k<rows; k++) {
        result(i,j) += this->mat[i][k] * rhs(k,j);
      }
    }
  }

  return result;
}

// Cumulative left multiplication of this matrix and another
template<typename T>
QSMatrix<T>& QSMatrix<T>::operator*=(const QSMatrix<T>& rhs) {
  QSMatrix result = (*this) * rhs;
  (*this) = result;
  return *this;
}

// Calculate a transpose of this matrix
template<typename T>
QSMatrix<T> QSMatrix<T>::transpose() {
  QSMatrix result(rows, cols, 0.0);

  for (unsigned i=0; i<rows; i++) {
    for (unsigned j=0; j<cols; j++) {
      result(i,j) = this->mat[j][i];
    }
  }

  return result;
}

// Square the matrix
template<typename T>
void QSMatrix<T>::square() {

  for (unsigned i=0; i<rows; i++) {
    for (unsigned j=0; j<cols; j++) {
      this->mat[j][i] = pow(this->mat[j][i], 2);
    }
  }
}

// Calculate a inverse of this matrix
template<typename T>
QSMatrix<T> QSMatrix<T>::inverse() {
  if(this->get_rows() != 4 || this->get_cols() != 4) return *this;

  double determinant =
    this->mat[0][0] * this->mat[1][1] * this->mat[2][2] * this->mat[3][3] + this->mat[0][0] * this->mat[1][2] * this->mat[2][3] * this->mat[3][1] + this->mat[0][0] * this->mat[1][3] * this->mat[2][1] * this->mat[3][2] +
    this->mat[0][1] * this->mat[1][0] * this->mat[2][3] * this->mat[3][2] + this->mat[0][1] * this->mat[1][2] * this->mat[2][0] * this->mat[3][3] + this->mat[0][1] * this->mat[1][3] * this->mat[2][2] * this->mat[3][0] +
    this->mat[0][2] * this->mat[1][0] * this->mat[2][1] * this->mat[3][3] + this->mat[0][2] * this->mat[1][1] * this->mat[2][3] * this->mat[3][0] + this->mat[0][2] * this->mat[1][3] * this->mat[2][0] * this->mat[3][1] +
    this->mat[0][3] * this->mat[1][0] * this->mat[2][2] * this->mat[3][1] + this->mat[0][3] * this->mat[1][1] * this->mat[2][0] * this->mat[3][2] + this->mat[0][3] * this->mat[1][2] * this->mat[2][1] * this->mat[3][0] -

    this->mat[0][0] * this->mat[1][1] * this->mat[2][3] * this->mat[3][2] - this->mat[0][0] * this->mat[1][2] * this->mat[2][1] * this->mat[3][3] - this->mat[0][0] * this->mat[1][3] * this->mat[2][2] * this->mat[3][1] -
    this->mat[0][1] * this->mat[1][0] * this->mat[2][2] * this->mat[3][3] - this->mat[0][1] * this->mat[1][2] * this->mat[2][3] * this->mat[3][0] - this->mat[0][1] * this->mat[1][3] * this->mat[2][0] * this->mat[3][2] -
    this->mat[0][2] * this->mat[1][0] * this->mat[2][3] * this->mat[3][1] - this->mat[0][2] * this->mat[1][1] * this->mat[2][0] * this->mat[3][3] - this->mat[0][2] * this->mat[1][3] * this->mat[2][1] * this->mat[3][0] -
    this->mat[0][3] * this->mat[1][0] * this->mat[2][1] * this->mat[3][2] - this->mat[0][3] * this->mat[1][1] * this->mat[2][2] * this->mat[3][0] - this->mat[0][3] * this->mat[1][2] * this->mat[2][0] * this->mat[3][1];

    if(determinant == 0) return *this;

    QSMatrix b(4, 4, 0.0);

    b(0, 0) = this->mat[1][1] * this->mat[2][2] * this->mat[3][3] + this->mat[1][2] * this->mat[2][3] * this->mat[3][1] + this->mat[1][3] * this->mat[2][1] * this->mat[3][2] - this->mat[1][1] * this->mat[2][3] * this->mat[3][2] - this->mat[1][2] * this->mat[2][1] * this->mat[3][3] - this->mat[1][3] * this->mat[2][2] * this->mat[3][1];
    b(0, 1) = this->mat[0][1] * this->mat[2][3] * this->mat[3][2] + this->mat[0][2] * this->mat[2][1] * this->mat[3][3] + this->mat[0][3] * this->mat[2][2] * this->mat[3][1] - this->mat[0][1] * this->mat[2][2] * this->mat[3][3] - this->mat[0][2] * this->mat[2][3] * this->mat[3][1] - this->mat[0][3] * this->mat[2][1] * this->mat[3][2];
    b(0, 2) = this->mat[0][1] * this->mat[1][2] * this->mat[3][3] + this->mat[0][2] * this->mat[1][3] * this->mat[3][1] + this->mat[0][3] * this->mat[1][1] * this->mat[3][2] - this->mat[0][1] * this->mat[1][3] * this->mat[3][2] - this->mat[0][2] * this->mat[1][1] * this->mat[3][3] - this->mat[0][3] * this->mat[1][2] * this->mat[3][1];
    b(0, 3) = this->mat[0][1] * this->mat[1][3] * this->mat[2][2] + this->mat[0][2] * this->mat[1][1] * this->mat[2][3] + this->mat[0][3] * this->mat[1][2] * this->mat[2][1] - this->mat[0][1] * this->mat[1][2] * this->mat[2][3] - this->mat[0][2] * this->mat[1][3] * this->mat[2][1] - this->mat[0][3] * this->mat[1][1] * this->mat[2][2];
    b(1, 0) = this->mat[1][0] * this->mat[2][3] * this->mat[3][2] + this->mat[1][2] * this->mat[2][0] * this->mat[3][3] + this->mat[1][3] * this->mat[2][2] * this->mat[3][0] - this->mat[1][0] * this->mat[2][2] * this->mat[3][3] - this->mat[1][2] * this->mat[2][3] * this->mat[3][0] - this->mat[1][3] * this->mat[2][0] * this->mat[3][2];
    b(1, 1) = this->mat[0][0] * this->mat[2][2] * this->mat[3][3] + this->mat[0][2] * this->mat[2][3] * this->mat[3][0] + this->mat[0][3] * this->mat[2][0] * this->mat[3][2] - this->mat[0][0] * this->mat[2][3] * this->mat[3][2] - this->mat[0][2] * this->mat[2][0] * this->mat[3][3] - this->mat[0][3] * this->mat[2][2] * this->mat[3][0];
    b(1, 2) = this->mat[0][0] * this->mat[1][3] * this->mat[3][2] + this->mat[0][2] * this->mat[1][0] * this->mat[3][3] + this->mat[0][3] * this->mat[1][2] * this->mat[3][0] - this->mat[0][0] * this->mat[1][2] * this->mat[3][3] - this->mat[0][2] * this->mat[1][3] * this->mat[3][0] - this->mat[0][3] * this->mat[1][0] * this->mat[3][2];
    b(1, 3) = this->mat[0][0] * this->mat[1][2] * this->mat[2][3] + this->mat[0][2] * this->mat[1][3] * this->mat[2][0] + this->mat[0][3] * this->mat[1][0] * this->mat[2][2] - this->mat[0][0] * this->mat[1][3] * this->mat[2][2] - this->mat[0][2] * this->mat[1][0] * this->mat[2][3] - this->mat[0][3] * this->mat[1][2] * this->mat[2][0];
    b(2, 0) = this->mat[1][0] * this->mat[2][1] * this->mat[3][3] + this->mat[1][1] * this->mat[2][3] * this->mat[3][0] + this->mat[1][3] * this->mat[2][0] * this->mat[3][1] - this->mat[1][0] * this->mat[2][3] * this->mat[3][1] - this->mat[1][1] * this->mat[2][0] * this->mat[3][3] - this->mat[1][3] * this->mat[2][1] * this->mat[3][0];
    b(2, 1) = this->mat[0][0] * this->mat[2][3] * this->mat[3][1] + this->mat[0][1] * this->mat[2][0] * this->mat[3][3] + this->mat[0][3] * this->mat[2][1] * this->mat[3][0] - this->mat[0][0] * this->mat[2][1] * this->mat[3][3] - this->mat[0][1] * this->mat[2][3] * this->mat[3][0] - this->mat[0][3] * this->mat[2][0] * this->mat[3][1];
    b(2, 2) = this->mat[0][0] * this->mat[1][1] * this->mat[3][3] + this->mat[0][1] * this->mat[1][3] * this->mat[3][0] + this->mat[0][3] * this->mat[1][0] * this->mat[3][1] - this->mat[0][0] * this->mat[1][3] * this->mat[3][1] - this->mat[0][1] * this->mat[1][0] * this->mat[3][3] - this->mat[0][3] * this->mat[1][1] * this->mat[3][0];
    b(2, 3) = this->mat[0][0] * this->mat[1][3] * this->mat[2][1] + this->mat[0][1] * this->mat[1][0] * this->mat[2][3] + this->mat[0][3] * this->mat[1][1] * this->mat[2][0] - this->mat[0][0] * this->mat[1][1] * this->mat[2][3] - this->mat[0][1] * this->mat[1][3] * this->mat[2][0] - this->mat[0][3] * this->mat[1][0] * this->mat[2][1];
    b(3, 0) = this->mat[1][0] * this->mat[2][2] * this->mat[3][1] + this->mat[1][1] * this->mat[2][0] * this->mat[3][2] + this->mat[1][2] * this->mat[2][1] * this->mat[3][0] - this->mat[1][0] * this->mat[2][1] * this->mat[3][2] - this->mat[1][1] * this->mat[2][2] * this->mat[3][0] - this->mat[1][2] * this->mat[2][0] * this->mat[3][1];
    b(3, 1) = this->mat[0][0] * this->mat[2][1] * this->mat[3][2] + this->mat[0][1] * this->mat[2][1] * this->mat[3][0] + this->mat[0][2] * this->mat[2][0] * this->mat[3][1] - this->mat[0][0] * this->mat[2][2] * this->mat[3][1] - this->mat[0][1] * this->mat[2][0] * this->mat[3][2] - this->mat[0][2] * this->mat[2][1] * this->mat[3][0];
    b(3, 2) = this->mat[0][0] * this->mat[1][2] * this->mat[3][1] + this->mat[0][1] * this->mat[1][0] * this->mat[3][2] + this->mat[0][2] * this->mat[1][1] * this->mat[3][0] - this->mat[0][0] * this->mat[1][1] * this->mat[3][2] - this->mat[0][1] * this->mat[1][2] * this->mat[3][0] - this->mat[0][2] * this->mat[1][0] * this->mat[3][1];
    b(3, 3) = this->mat[0][0] * this->mat[1][1] * this->mat[2][2] + this->mat[0][1] * this->mat[1][2] * this->mat[2][0] + this->mat[0][2] * this->mat[1][0] * this->mat[2][1] - this->mat[0][0] * this->mat[1][2] * this->mat[2][1] - this->mat[0][1] * this->mat[1][0] * this->mat[2][2] - this->mat[0][2] * this->mat[1][1] * this->mat[2][0];

    QSMatrix result = b * (1.0 / determinant);

    return result;
}

// Calculate the sum of this matrix
template<typename T>
double QSMatrix<T>::sum() {
  double sum = 0;
  for (unsigned i=0; i<rows; i++) {
    for (unsigned j=0; j<cols; j++) {
      sum += this->mat[j][i];
    }
  }

  return sum;
}

// Matrix/scalar addition
template<typename T>
QSMatrix<T> QSMatrix<T>::operator+(const T& rhs) {
  QSMatrix result(rows, cols, 0.0);

  for (unsigned i=0; i<rows; i++) {
    for (unsigned j=0; j<cols; j++) {
      result(i,j) = this->mat[i][j] + rhs;
    }
  }

  return result;
}

// Matrix/scalar subtraction
template<typename T>
QSMatrix<T> QSMatrix<T>::operator-(const T& rhs) {
  QSMatrix result(rows, cols, 0.0);

  for (unsigned i=0; i<rows; i++) {
    for (unsigned j=0; j<cols; j++) {
      result(i,j) = this->mat[i][j] - rhs;
    }
  }

  return result;
}

// Matrix/scalar multiplication
template<typename T>
QSMatrix<T> QSMatrix<T>::operator*(const T& rhs) {
  QSMatrix result(rows, cols, 0.0);

  for (unsigned i=0; i<rows; i++) {
    for (unsigned j=0; j<cols; j++) {
      result(i,j) = this->mat[i][j] * rhs;
    }
  }

  return result;
}

// Matrix/scalar division
template<typename T>
QSMatrix<T> QSMatrix<T>::operator/(const T& rhs) {
  QSMatrix result(rows, cols, 0.0);

  for (unsigned i=0; i<rows; i++) {
    for (unsigned j=0; j<cols; j++) {
      result(i,j) = this->mat[i][j] / rhs;
    }
  }

  return result;
}

// Multiply a matrix with a vector
template<typename T>
std::vector<T> QSMatrix<T>::operator*(const std::vector<T>& rhs) {
  std::vector<T> result(rhs.size(), 0.0);

  for (unsigned i=0; i<rows; i++) {
    for (unsigned j=0; j<cols; j++) {
      result[i] = this->mat[i][j] * rhs[j];
    }
  }

  return result;
}

// Obtain a vector of the diagonal elements
template<typename T>
std::vector<T> QSMatrix<T>::diag_vec() {
  std::vector<T> result(rows, 0.0);

  for (unsigned i=0; i<rows; i++) {
    result[i] = this->mat[i][i];
  }

  return result;
}

// Obtain a vector of the row elements
template<typename T>
std::vector<T> QSMatrix<T>::row_vec(const unsigned& row) {
  std::vector<T> result(cols, 0.0);

  for (unsigned i=0; i<cols; i++) {
    result[i] = this->mat[row][i];
  }

  return result;
}

// Obtain a vector of the column elements
template<typename T>
std::vector<T> QSMatrix<T>::col_vec(const unsigned& col) {
  std::vector<T> result(rows, 0.0);

  for (unsigned i=0; i<rows; i++) {
    result[i] = this->mat[i][col];
  }

  return result;
}

//Insert a vector of row elements
template<typename T>
QSMatrix<T>& QSMatrix<T>::insert_row_vec(const std::vector<T>& row, const unsigned& position) {

  for (unsigned i=0; i<row.size(); i++) {
    this->mat[position][i] = row[i];
  }

  return *this;
}

//Insert a vector of column elements
template<typename T>
QSMatrix<T>& QSMatrix<T>::insert_col_vec(const std::vector<T>& col, const unsigned& position) {

  for (unsigned i=0; i<col.size(); i++) {
    this->mat[i][position] = col[i];
  }

  return *this;
}

template<typename T>
int16_t  QSMatrix<T>::get_min()
{
    int16_t matrixMin = std::numeric_limits<int16_t>::max();

    for(int16_t row = 0; row < this->rows; ++row)
    {
        for(int16_t column = 0; column < this->cols; ++column)
        {
            matrixMin = min(matrixMin, this->mat[row][column]);
        }
    }

    return matrixMin;
}

template<typename T>
int16_t  QSMatrix<T>::get_max()
{
    int16_t matrixMax = std::numeric_limits<int16_t>::min();

    for(int16_t row = 0; row < this->rows; ++row)
    {
        for(int16_t column = 0; column < this->cols; ++column)
        {
            matrixMax = max(matrixMax, this->mat[row][column]);
        }
    }

    return matrixMax;
}

template<typename T>
int16_t QSMatrix<T>::get_median()
{
    std::vector<T> medianValues;
    int16_t n = this->rows / 2;

    for(int16_t row = 0; row < this->rows; ++row)
    {
        nth_element(this->mat[row].begin(), this->mat[row].begin()+n, this->mat[row].end());
        medianValues.push_back(this->mat[row][n]);
    }

    n = medianValues.size() / 2;
    nth_element(medianValues.begin(), medianValues.begin()+n, medianValues.end());

    return medianValues[n];
}

// Access the individual elements
template<typename T>
T& QSMatrix<T>::operator()(const unsigned& row, const unsigned& col) {
  return this->mat[row][col];
}

// Access the individual elements (const)
template<typename T>
const T& QSMatrix<T>::operator()(const unsigned& row, const unsigned& col) const {
  return this->mat[row][col];
}

// Get the number of rows of the matrix
template<typename T>
unsigned QSMatrix<T>::get_rows() const {
  return this->rows;
}

// Get the number of columns of the matrix
template<typename T>
unsigned QSMatrix<T>::get_cols() const {
  return this->cols;
}

#endif
