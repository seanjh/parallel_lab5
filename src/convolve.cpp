#include <memory>
#include "Array2d.hpp"
#include "assert.h"
#include <iostream>


void convolveNoAlloc(
	std::shared_ptr<Array2d> source,
	int sourceStartingRow,
	int rowCount,
	int sourceStartingCol,
	int colCount,
	std::shared_ptr<Array2d> dest,
	int destStartingRow,
	int destStartingCol,
	std::shared_ptr<Array2d> kernel
	)
{
	assert(kernel->rows == kernel->cols);

	int kernelKVal = (kernel->cols/2) + 1;

	for (int i=0; i<rowCount; i++)
	{
		for (int j=0; j<colCount; j++)
		{
			double sum = 0.0;

			for(int l=0; l<kernel->rows; l++)
			{
				for(int m=0; m<kernel->cols; m++)
				{
					int sourceRowindex = i + sourceStartingRow - (kernelKVal-1) + l;
					int sourceColindex = j + sourceStartingCol - (kernelKVal-1) + m;


					std::cout<<i<<" "<<j<<" "<<l<<" "<<m<<std::endl;
					std::cout<<sourceRowindex<<" "<<sourceColindex<<std::endl;
					assert(sourceRowindex >= 0 && sourceRowindex < source->rows);
					assert(sourceColindex >= 0 && sourceColindex < source->cols);
					double sourceVal = source->get(sourceRowindex, sourceColindex);
					double kernelVal = kernel->get(l, m);
					sum += sourceVal * kernelVal;
					std::cout<<sourceVal<<" "<<kernelVal<<" "<<sum<<std::endl;
				}
			}
			dest->set(sum, i+destStartingRow, j+destStartingCol);
		}
	}

}
std::shared_ptr<Array2d> convolve(std::shared_ptr<Array2d> source, std::shared_ptr<Array2d> kernel, bool pad = true)
{
	assert(kernel->rows == kernel->cols);

	int kernelKVal = (kernel->cols/2) + 1;
	std::shared_ptr<Array2d> inputArray = source;
	std::shared_ptr<Array2d> outputArray = std::make_shared<Array2d>(source->rows, source->cols);

	int rowOffset=0;
	int colOffset=0;

	//padding
	if(pad)
	{
		int padding = (kernel->rows/2)*2;
		inputArray = std::make_shared<Array2d>(source->rows + padding, source->cols + padding);
		rowOffset = padding/2;
		colOffset = padding/2;

		for(int i=0; i<source->rows; i++)
			for(int j=0; j<source->cols; j++)
				inputArray->set(source->get(i, j), i+rowOffset, j+colOffset);

	}

	// for (int i=(kernelKVal-1); i<(source->rows-(kernelKVal-1)); i++)
	// {
	// 	for (int j=(kernelKVal-1); j<(source->cols-(kernelKVal-1)); j++)
	// 	{
	// 		double sum = 0.0;

	// 		for(int l=0; l<kernel->rows; l++)
	// 		{
	// 			for(int m=0; m<kernel->cols; m++)
	// 			{
	// 				int sourceYindex = i - (kernelKVal-1) + l;
	// 				int sourceXindex = j - (kernelKVal-1) + m;

	// 				sum += source->get(sourceYindex, sourceXindex) * kernel->get(l, m);
	// 			}
	// 		}
	// 		outputArray->set(sum, i, j);
	// 	}
	// }

	std::cout<<"Convolving"<<std::endl;

	convolveNoAlloc(
		inputArray,
		rowOffset,
		source->rows,
		colOffset,
		source->cols,
		outputArray,
		0,
		0,
		kernel);

	std::cout<<"Done Convolving"<<std::endl;

	return outputArray;

	// if(pad)
	// {
	// 	std::shared_ptr<Array2d> retArray = std::make_shared<Array2d>(source->rows, source->cols);
	// 	for(int i=0; i<source->rows; i++)
	// 	{
	// 		for(int j=0; j<source->cols; j++)
	// 		{
	// 			int yIndex = i + (kernel->rows/2);
	// 			int xIndex = j + (kernel->cols/2);

	// 			retArray->set(outputArray->get(yIndex, xIndex), i, j);
	// 		}
	// 	}

	// 	return retArray;
	// }
	// else
	// {
	// 	return outputArray;
	// }

}