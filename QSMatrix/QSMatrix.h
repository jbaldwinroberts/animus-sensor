#ifndef __QS_MATRIX_H
#define __QS_MATRIX_H

#include "application.h"
#include "math.h"
#include <vector>
#include <limits>

template <typename T> class QSMatrix {
 private:
  std::vector<std::vector<T> > mat;
  unsigned rows;
  unsigned cols;

 public:
  QSMatrix();
  QSMatrix(unsigned _rows, unsigned _cols, const T& _initial);
  QSMatrix(const QSMatrix<T>& rhs);
  virtual ~QSMatrix();

  // Operator overloading, for "standard" mathematical matrix operations
  QSMatrix<T>& operator=(const QSMatrix<T>& rhs);

  // Matrix mathematical operations
  QSMatrix<T> operator+(const QSMatrix<T>& rhs);
  QSMatrix<T>& operator+=(const QSMatrix<T>& rhs);
  QSMatrix<T> operator-(const QSMatrix<T>& rhs);
  QSMatrix<T>& operator-=(const QSMatrix<T>& rhs);
  QSMatrix<T> operator*(const QSMatrix<T>& rhs);
  QSMatrix<T>& operator*=(const QSMatrix<T>& rhs);
  QSMatrix<T> transpose();
  QSMatrix<T> inverse();
  void square();
  double sum();


  // Matrix/scalar operations
  QSMatrix<T> operator+(const T& rhs);
  QSMatrix<T> operator-(const T& rhs);
  QSMatrix<T> operator*(const T& rhs);
  QSMatrix<T> operator/(const T& rhs);

  // Matrix/vector operations
  std::vector<T> operator*(const std::vector<T>& rhs);
  std::vector<T> diag_vec();
  std::vector<T> row_vec(const unsigned& row);
  std::vector<T> col_vec(const unsigned& col);
  QSMatrix<T>& insert_row_vec(const std::vector<T>& row, const unsigned& position);
  QSMatrix<T>& insert_col_vec(const std::vector<T>& col, const unsigned& position);
  int16_t get_min();
  int16_t get_max();
  int16_t get_median();

  // Access the individual elements
  T& operator()(const unsigned& row, const unsigned& col);
  const T& operator()(const unsigned& row, const unsigned& col) const;

  // Access the row and column sizes
  unsigned get_rows() const;
  unsigned get_cols() const;

};

#include "QSMatrix.cpp"

#endif
