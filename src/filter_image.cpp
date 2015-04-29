#include <cstdlib>
#include <cassert>
#include <memory>
#include <array>
#include <iostream>
#include <string>
#include <omp.h>
#include <assert.h>

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
  std::shared_ptr<Array2d> r;
  std::shared_ptr<Array2d> g;
  std::shared_ptr<Array2d> b;
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
    // std::cout << "Arg[" << i << "] " << argument << "\n";
    if (argument == "-h" || argument == "--help") {
      show_usage(argv[0]);
      exit(EXIT_SUCCESS);
    } else if (argument == "-t" || argument == "--threads") {
      // Make certain there are more args to ingest
      if (++i < argc) {
        // std::cout << "Arg[" << i << "] " << argument << "\n";
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
  // std::cout << "Image filename: " << args->image_filename << "\n";
  // std::cout << "Stencil filename: " << args->stencil_filename << "\n";
  // std::cout << "Output filename: " << args->output_filename << "\n";
  // std::cout << "Iterations: " << args->iterations << "\n";
  // std::cout << "Threads: " << args->threads << "\n";
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
  for(int i=0; i<2; i++)
  {
    assert(stencil->rows() == stencil->columns());
    int padding = (stencil->rows()/2)*2;
    rgbBuffers[i].r = std::make_shared<Array2d>(image->rows() + padding, image->columns() + padding);
    rgbBuffers[i].g = std::make_shared<Array2d>(image->rows() + padding, image->columns() + padding);
    rgbBuffers[i].b = std::make_shared<Array2d>(image->rows() + padding, image->columns() + padding);
  }

  std::shared_ptr<Array2d> source;
  std::shared_ptr<Array2d> dest;

  const int rowOffset = (stencil->rows()/2);
  const int colOffset = (stencil->columns()/2);
  // std::cout << "rowOffset=" << rowOffset << " colOffset=" << colOffset << "\n";

  // source = image.r;
  // dest = rgbBuffers[0].r;
  for(int i=0; i<image->rows(); i++)
  {
    for(int j=0; j<image->columns(); j++)
    {
      rgbBuffers[0].r->set(image->red_pixels()->get(i, j), i+rowOffset, j+colOffset);
      rgbBuffers[0].g->set(image->green_pixels()->get(i, j), i+rowOffset, j+colOffset);
      rgbBuffers[0].b->set(image->blue_pixels()->get(i, j), i+rowOffset, j+colOffset);
    }
  }

  for(int i=0; i<image->rows(); i++)
  {
    for(int j=0; j<image->columns(); j++)
    {
      rgbBuffers[1].r->set(0, i+rowOffset, j+colOffset);
      rgbBuffers[1].g->set(0, i+rowOffset, j+colOffset);
      rgbBuffers[1].b->set(0, i+rowOffset, j+colOffset);
    }
  }

  std::cout << "Applying image filter across " << args->threads << " total threads\n";
  int rows_per_thread = image->rows() / args->threads;
  std::cout << "Each thread will process " << rows_per_thread << " rows each iteration\n";

  int thread_id, first_row, this_iter, source_buff_id, dest_buff_id;
  #pragma omp parallel num_threads(args->threads) private(thread_id, first_row, this_iter, source_buff_id, dest_buff_id)
  {
    thread_id = omp_get_thread_num();

    source_buff_id = (this_iter % 2);
    dest_buff_id = (this_iter + 1) % 2;
    first_row = (thread_id * rows_per_thread) + rowOffset;
    std::cout << "t" << thread_id << std::endl <<
      "iter" << this_iter << std::endl;

    // for(int i=0; i<args->iterations; i++)
    this_iter = 0;
    while (this_iter < args->iterations)
    {
      //source and dest array flip back and forth based on iteration
      std::cout << "Thread:" << thread_id << " first row: " << first_row << std::endl;
      convolveNoAlloc(
        rgbBuffers[source_buff_id].r,
        first_row,
        rows_per_thread,
        colOffset,
        image->columns(),
        rgbBuffers[dest_buff_id].r,
        rowOffset,
        colOffset,
        stencil->kernel
      );

      convolveNoAlloc(
        rgbBuffers[source_buff_id].g,
        first_row,
        rows_per_thread,
        colOffset,
        image->columns(),
        rgbBuffers[dest_buff_id].g,
        rowOffset,
        colOffset,
        stencil->kernel
      );

      convolveNoAlloc(
        rgbBuffers[source_buff_id].b,
        first_row,
        rows_per_thread,
        colOffset,
        image->columns(),
        rgbBuffers[dest_buff_id].b,
        rowOffset,
        colOffset,
        stencil->kernel
      );
      std::cout << "Thread " << thread_id << " done iteration " << this_iter << std::endl;

      // #pragma omp single
      // {
      //   source_buff_id = (iterations % 2);
      //   dest_buff_id = (iterations + 1) % 2;
      //   iterations++;
      // }

      this_iter++;
      source_buff_id = (this_iter % 2);
      dest_buff_id = (this_iter + 1) % 2;

      #pragma omp barrier

    }

  }

  // for(int i=0; i<image->rows(); i++)
  // {
  //   for(int j=0; j<image->columns(); j++)
  //   {
  //     std::cout << "R=" << rgbBuffers[dest_buff_id].r->get(i+rowOffset, j+colOffset) <<
  //     " G=" << rgbBuffers[dest_buff_id].g->get(i+rowOffset, j+colOffset) <<
  //     " B=" << rgbBuffers[dest_buff_id].b->get(i+rowOffset, j+colOffset) << "\n";
  //   }
  // }

  std::cout<<"Convolutions are complete. Writing output buffer."<<std::endl;

  RGBArraySet output;

  output.r = std::make_shared<Array2d>(image->rows(), image->columns());
  output.g = std::make_shared<Array2d>(image->rows(), image->columns());
  output.b = std::make_shared<Array2d>(image->rows(), image->columns());

  // std::shared_ptr<Array2d> retArray = std::make_shared<Array2d>(source->rows, source->cols);
  for(int i=0; i<image->rows(); i++)
  {
    for(int j=0; j<image->columns(); j++)
    {
      int rowIndex = i + rowOffset;
      int colIndex = j + colOffset;

      output.r->set(rgbBuffers[dest_buff_id].r->get(rowIndex, colIndex), i, j);
      output.g->set(rgbBuffers[dest_buff_id].g->get(rowIndex, colIndex), i, j);
      output.b->set(rgbBuffers[dest_buff_id].b->get(rowIndex, colIndex), i, j);

      // std::cout << "(row=" << i << ",col=" << j  << ") " <<
      // "R=" << output.r->get(i, j) <<
      // " G=" << output.g->get(i, j) <<
      // " B=" << output.b->get(i, j) << "\n";
    }
  }

  std::cout<<"Processing is complete. Writing output image."<<std::endl;

  auto outputImage = std::make_shared<Image>(
    image->magic_number(),
    image->rows(),
    image->columns(),
    image->max_value(),
    output.r,
    output.g,
    output.b);

  outputImage->save(args->output_filename);

  // for(int i=0; i<image->rows(); i++)
  // {
  //   for(int j=0; j<image->columns(); j++)
  //   {
  //     std::cout << "(row=" << i << ",col=" << j  << ") " <<
  //     "R=" << output.r->get(i, j) <<
  //     " G=" << output.g->get(i, j) <<
  //     " B=" << output.b->get(i, j) << "\n";
  //   }
  // }


  // Copy an image
  // auto image2 = std::make_shared<Image>(*image);

  // Preallocate work

  // To iterate through the actual Image pixels (excluding padding)
  // for (int row = image.first_row(); row <= image.last_row(); row++) {
  //   for (int col = image.first_column(); col <= image.last_column(); col++) {
  //     // pixel[row][col] is one pixel in the image
  //   }
  // }

  // for each iteration
  //  preallocate
  //  apply stencil (convolve)
  // write filtered PPM image

  return EXIT_SUCCESS;
}