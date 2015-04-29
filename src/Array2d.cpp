#include <algorithm>
#include <iostream>
#include <cassert>

#include "Array2d.hpp"

Array2d::Array2d(int r, int c) : rows(r), cols(c) {
  // std::cout<<"Creating array: " << rows << " " << cols << std::endl;
  array = new double*[rows];
  for(int i = 0; i < rows; ++i) {
      array[i] = new double[cols];
  }
}

Array2d::Array2d(Array2d& original) : Array2d(original.rows, original.cols)
{
  // int offset = original.rows * original.cols;
  // std::cout << "Copying original Array2d array from " << &original.array[0][0] << " to " << &original.array[0][0] + offset << "\n";

  for (int i=0; i<rows; i++) {
    // std::cout << "Copying original Array2d array row #" << i << " from " << &original.array[i] << " to " << &original.array[i] + original.cols << "\n";
    std::copy(&original.array[i], &original.array[i] + original.cols, &this->array[i]);
  }

  // std::copy(&original.array[0][0], &original.array[0][0] + offset, &this->array[0][0]);
}

Array2d::~Array2d() {
  for(int i = 0; i < rows; ++i) {
      delete [] array[i];
  }
  delete [] array;

  // std::cout<<"Destroying array: " << rows << " " << cols << std::endl;
}

double Array2d::get(int row, int col) {
  // std::cout << "get row" << row << "/" << rows <<
  //   " col=" << col << "/" << cols << "\n";
  // if (row < 0 || row >= rows) {
  //   std::cout << "ERROR get row" << row << "/" << rows <<
  //     " col=" << col << "/" << cols << "\n";
  // }

  // if (col < 0 || col >= cols) {
  //   std::cout << "ERROR get col" << row << "/" << rows <<
  //     " col=" << col << "/" << cols << "\n";
  // }
  assert(row >= 0 && row < rows);
  assert(col >= 0 && col < cols);
  return array[row][col];
}

void Array2d::set(double val, int row, int col) {
  assert(row >= 0 && row < rows);
  assert(col >= 0 && col < cols);
  array[row][col] = val;
}