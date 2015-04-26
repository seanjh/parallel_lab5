#include "Array2d.hpp"
#include <iostream>
#include <assert.h>

extern std::shared_ptr<Array2d> convolve(std::shared_ptr<Array2d> source, std::shared_ptr<Array2d> kernel, bool pad = true);

int main()
{
	int rows = 10;
	int cols = 12;
	std::shared_ptr<Array2d> sourceArray = std::make_shared<Array2d>(rows, cols);


	std::cout<<"Setting up source Array" <<std::endl;
	for(int i=0; i<rows; i++)
		for (int j=0; j<cols; j++)
			sourceArray->set(.5, i, j);
	std::shared_ptr<Array2d> kernelArray = std::make_shared<Array2d>(3, 3);
	kernelArray->set(-1.0, 0, 0);
	kernelArray->set(-2.0, 0, 1);
	kernelArray->set(-1.0, 0, 2);
	kernelArray->set(0.0, 1, 0);
	kernelArray->set(0.0, 1, 1);
	kernelArray->set(0.0, 1, 2);
	kernelArray->set(1.0, 2, 0);
	kernelArray->set(2.0, 2, 1);
	kernelArray->set(1.0, 2, 2);
	// for(int i=0; i<kernelArray->rows; i++)
	// 	for (int j=0; j<kernelArray->cols; j++)
	// 		kernelArray->set(2.0, i, j);

	std::shared_ptr<Array2d> destArray = sourceArray;

	int iterations = 1;
	for(int i=0; i<iterations; i++)
		destArray = convolve(destArray, kernelArray);

	assert(destArray->rows == rows && destArray->cols == cols);

	for(int i=0; i<10; i++)
		for (int j=0; j<12; j++)
			std::cout<<destArray->get(i, j)<<std::endl;




	return 0;
}