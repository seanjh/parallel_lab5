#include <cstdlib>
#include <cassert>
#include <memory>
#include <array>
#include <iostream>
#include <string>

#include <omp.h>

#include "Stencil.hpp"
#include "Image.hpp"

extern void convolveNoAlloc(
  std::shared_ptr<Array2d> source,
  int sourceStartingRow,
  int rowCount,
  int sourceStartingCol,
  int colCount,
  std::shared_ptr<Array2d> dest,
  int destStartingRow,
  int destStartingCol,
  std::shared_ptr<Array2d> kernel
  );

typedef struct _FilterArguments {
  std::string image_filename;
  std::string stencil_filename;
  std::string output_filename;
  int iterations;
  int threads;
} FilterArguments;

typedef struct _RGBArraySet {
  std::shared_ptr<Array2d> red;
  std::shared_ptr<Array2d> green;
  std::shared_ptr<Array2d> blue;
} RGBArraySet;

void show_usage(std::string name)
{
  std::cerr << "Usage: " << name << " [options] IMAGE_PPM STENCIL_PGM OUTPUT_PPM [ITERATIONS] [-t THREADS]\n"
            << "Options:\n"
            << "\tITERATIONS\t\tNumber of filter iterations to apply (default: 1)\n"
            << "\t-t,--threads THREADS\tNumber of threads (default: 1)\n"
            << "\t-h,--help\t\tShow this help dialog\n"
            << std::endl;
}


std::shared_ptr<FilterArguments> parse_arguments(int argc, char* argv[])
{
  auto args = std::make_shared<FilterArguments>();
  // Defaults
  args->iterations = 1;
  args->threads = 1;

  bool image_set = false;
  bool stencil_set = false;
  bool output_set = false;

  std::string argument;
  for (int i = 1; i < argc; i++) {
    argument = argv[i];
    if (argument == "-h" || argument == "--help") {
      show_usage(argv[0]);
      exit(EXIT_SUCCESS);
    } else if (argument == "-t" || argument == "--threads") {
      // Make certain there are more args to ingest
      if (++i < argc) {
        args->threads = std::stoi(argv[i]);
      } else {
        std::cerr << "--threads option requires one argument." << std::endl;
        exit(EXIT_FAILURE);
      }
    } else if (!image_set) {
      args->image_filename = argv[i];
      image_set = true;
    } else if (!stencil_set) {
      args->stencil_filename = argv[i];
      stencil_set = true;
    } else if (!output_set) {
      args->output_filename = argv[i];
      output_set = true;
    } else {
      args->iterations = std::stoi(argv[i]);
      break;
    }
  }
  return args;
}


int main(int argc, char* argv[])
{
  // Parse CLI arguments
  if (argc < 4) {
    show_usage(argv[0]);
    return EXIT_FAILURE;
  }
  auto args = parse_arguments(argc, argv);

  // Parse PGM stencil
  auto stencil = std::make_shared<Stencil>(args->stencil_filename);
  stencil->parse();

  // Parse PPM image
  auto image = std::make_shared<Image>(args->image_filename);
  image->parse();

  RGBArraySet rgbBuffers[2];
  int row_padding = (stencil->rows() / 2) * 2;
  int col_padding = (stencil->columns() / 2) * 2;
  for(int i=0; i<2; i++)
  {
    rgbBuffers[i].red = std::make_shared<Array2d>(image->rows() + row_padding, image->columns() + col_padding);
    rgbBuffers[i].green = std::make_shared<Array2d>(image->rows() + row_padding, image->columns() + col_padding);
    rgbBuffers[i].blue = std::make_shared<Array2d>(image->rows() + row_padding, image->columns() + col_padding);
  }

  const int rowOffset = (stencil->rows()/2);
  const int colOffset = (stencil->columns()/2);

  // Initialize update buffers with image pixels
  for(int i=0; i<image->rows(); i++)
  {
    for(int j=0; j<image->columns(); j++)
    {
      rgbBuffers[0].red->set(image->red_pixels()->get(i, j), i+rowOffset, j+colOffset);
      rgbBuffers[0].green->set(image->green_pixels()->get(i, j), i+rowOffset, j+colOffset);
      rgbBuffers[0].blue->set(image->blue_pixels()->get(i, j), i+rowOffset, j+colOffset);
    }
  }

  for(int i=0; i<image->rows(); i++)
  {
    for(int j=0; j<image->columns(); j++)
    {
      rgbBuffers[1].red->set(0, i+rowOffset, j+colOffset);
      rgbBuffers[1].green->set(0, i+rowOffset, j+colOffset);
      rgbBuffers[1].blue->set(0, i+rowOffset, j+colOffset);
    }
  }


  int rows_per_thread = image->rows() / args->threads;
  int last_thread_id = args->threads - 1;
  int leftover_rows = image->rows() % args->threads;
  std::cout << "Applying image filter across " << args->threads << " total threads\n";
  std::cout << "Each thread will process " << rows_per_thread << " rows each iteration\n";

  int thread_id = 0;
  int first_row = 0;
  int this_iter = 0;
  int source_buff_id = 0;
  int dest_buff_id = 0;
  #pragma omp parallel private(thread_id, first_row, this_iter, source_buff_id, dest_buff_id) shared(rgbBuffers, last_thread_id, leftover_rows) num_threads(args->threads)
  {
    thread_id = omp_get_thread_num();
    this_iter = 0;
    source_buff_id = (this_iter % 2);
    dest_buff_id = (this_iter + 1) % 2;
    first_row = (thread_id * rows_per_thread) + rowOffset;

    #pragma omp critical
    {
      if (thread_id == last_thread_id && leftover_rows != 0) {
        // When rows/threads has a remainder, process these remaining rows on the last thread
        std::cout << "Thread " << thread_id << " is processing rows " <<
          first_row << " to " << first_row + rows_per_thread - 1 <<
          "+" << leftover_rows << " and all " << image->columns() << " columns" << std::endl;
        rows_per_thread += leftover_rows;
      } else {
        std::cout << "Thread " << thread_id << " is processing rows " <<
          first_row << " to " << first_row + rows_per_thread - 1 <<
          " and all " << image->columns() << " columns" << std::endl;
      }
    }

    #pragma omp barrier
    while (this_iter < args->iterations)
    {
      //source and dest array flip back and forth based on iteration
      source_buff_id = (this_iter % 2);
      dest_buff_id = (this_iter + 1) % 2;

      convolveNoAlloc(
        rgbBuffers[source_buff_id].red,
        first_row,
        rows_per_thread,
        colOffset,
        image->columns(),
        rgbBuffers[dest_buff_id].red,
        first_row,
        colOffset,
        stencil->kernel
      );

      convolveNoAlloc(
        rgbBuffers[source_buff_id].green,
        first_row,
        rows_per_thread,
        colOffset,
        image->columns(),
        rgbBuffers[dest_buff_id].green,
        first_row,
        colOffset,
        stencil->kernel
      );

      convolveNoAlloc(
        rgbBuffers[source_buff_id].blue,
        first_row,
        rows_per_thread,
        colOffset,
        image->columns(),
        rgbBuffers[dest_buff_id].blue,
        first_row,
        colOffset,
        stencil->kernel
      );

      this_iter++;

      // Synchronize between each iteration
      #pragma omp barrier
    }

  }

  // Output buffer depends on whether iterations is odd (0) or even (1)
  int final_destination = args->iterations % 2 == 0 ? 0 : 1;

  std::cout<<"Convolutions are complete. Writing output buffer."<<std::endl;

  RGBArraySet output;

  output.red = std::make_shared<Array2d>(image->rows(), image->columns());
  output.green = std::make_shared<Array2d>(image->rows(), image->columns());
  output.blue = std::make_shared<Array2d>(image->rows(), image->columns());

  // std::shared_ptr<Array2d> retArray = std::make_shared<Array2d>(source->rows, source->cols);
  for(int i=0; i<image->rows(); i++)
  {
    for(int j=0; j<image->columns(); j++)
    {
      int rowIndex = i + rowOffset;
      int colIndex = j + colOffset;

      output.red->set(rgbBuffers[final_destination].red->get(rowIndex, colIndex), i, j);
      output.green->set(rgbBuffers[final_destination].green->get(rowIndex, colIndex), i, j);
      output.blue->set(rgbBuffers[final_destination].blue->get(rowIndex, colIndex), i, j);

    }
  }

  std::cout<<"Processing is complete. Writing output image."<<std::endl;

  auto outputImage = std::make_shared<Image>(
    image->magic_number(),
    image->rows(),
    image->columns(),
    image->max_value(),
    output.red,
    output.green,
    output.blue);

  outputImage->save(args->output_filename);

  return EXIT_SUCCESS;
}