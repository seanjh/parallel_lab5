#include <iostream>
#include <cassert>

#include "Array2d.hpp"

Array2d::Array2d(int r, int c) : rows(r), cols(c) {
	array = new double*[rows];
	for(int i = 0; i < rows; ++i) {
	    array[i] = new double[cols];
	}

	std::cout<<"Creating array: " << rows << " " << cols << std::endl;
}

Array2d::~Array2d() {
	for(int i = 0; i < rows; ++i) {
	    delete [] array[i];
	}
	delete [] array;

	std::cout<<"Destroying array: " << rows << " " << cols << std::endl;
}

double Array2d::get(int row, int col) {
	assert(row >= 0 && row < rows);
	assert(col >= 0 && col < cols);
	return array[row][col];
}

void Array2d::set(double val, int row, int col) {
	assert(row >= 0 && row < rows);
	assert(col >= 0 && col < cols);
	array[row][col] = val;
}