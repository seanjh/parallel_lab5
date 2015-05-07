#include <memory>
#include <cassert>
#include <iostream>

#include "Array2d.hpp"


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
  // assert(kernel->rows == kernel->cols);

  int kernelKVal = (kernel->cols / 2) + 1;
  int sourceRowindex, sourceColindex;
  double sourceVal, kernelVal;

  for (int s_row=0; s_row<rowCount; s_row++)
  {
    for (int s_col=0; s_col<colCount; s_col++)
    {
      double sum = 0.0;

      for(int k_row=0; k_row<kernel->rows; k_row++)
      {
        for(int k_col=0; k_col<kernel->cols; k_col++)
        {
          sourceRowindex = s_row + sourceStartingRow - (kernelKVal-1) + k_row;
          sourceColindex = s_col + sourceStartingCol - (kernelKVal-1) + k_col;

          // std::cout<<s_row<<" "<<s_col<<" "<<l<<" "<<m<<std::endl;
          // std::cout<<sourceRowindex<<" "<<sourceColindex<<std::endl;

          assert(sourceRowindex >= 0 && sourceRowindex < source->rows);
          assert(sourceColindex >= 0 && sourceColindex < source->cols);

          sourceVal = source->get(sourceRowindex, sourceColindex);
          kernelVal = kernel->get(k_row, k_col);
          sum += sourceVal * kernelVal;
          // std::cout<<sourceVal<<" "<<kernelVal<<" "<<sum<<std::endl;
        }
      }
      dest->set(sum, s_row + destStartingRow, s_col + destStartingCol);
    }
  }

}


std::shared_ptr<Array2d> convolve(std::shared_ptr<Array2d> source, std::shared_ptr<Array2d> kernel, bool pad = true)
{
  // assert(kernel->rows == kernel->cols);

  // int kernelKVal = (kernel->cols/2) + 1;
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

}