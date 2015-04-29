#include <cstdlib>
#include <cassert>
#include <memory>
#include <array>
#include <iostream>
#include <string>
// #include <ommp.h>
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
  std::cerr << "Usage: " << name << " [options] image_ppm stencil_pgm output_ppm output_filename iterations\n"
            << "Options:\n"
            << "\t-t,--threads THREADS\tNumber of threads\n"
            << "\t-h,--help\t\tShow this help dialog\n"
            << std::endl;
}


std::shared_ptr<FilterArguments> parse_arguments(int argc, char* argv[])
{
  auto args = std::make_shared<FilterArguments>();
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
      if (i + 1 < argc) {
        // std::cout << "Arg[" << i << "] " << argument << "\n";
        args->threads = std::stoi(argv[++i]);
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
  // auto image = std::make_shared<Image>(args->image_filename, stencil->rows(), stencil->columns());
  auto image = std::make_shared<Image>(args->image_filename);
  image->parse();

  // std::cout << "Saving image\n";
  // image->save("test.ppm");

  std::cout<<"Input image: " << args->image_filename << std::endl <<
    "\tMagic Num:\t" << image->magic_number() << std::endl <<
    "\tRows:\t\t" << image->rows() << std::endl <<
    "\tColumns:\t" << image->columns() << std::endl <<
    "\tMax Val:\t" << image->max_value() << std::endl <<
    std::endl;

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

  int srcId;
  int dstId;
  // int threads = args->threads != 0 ? args->threads : 4;
  // #pragma omp parallel num_threads(threads)
  {
    // #pragma omp for
    for(int i=0; i<args->iterations; i++)
    {
      //source and dest array flip back and forth based on iteration
      srcId = (i%2);
      dstId = (i+1)%2;

      // std::cout<<"Src="<<srcId<<", Dst="<<dstId<<std::endl;

      convolveNoAlloc(
        rgbBuffers[srcId].r,
        rowOffset,
        image->rows(),
        colOffset,
        image->columns(),
        rgbBuffers[dstId].r,
        rowOffset,
        colOffset,
        stencil->kernel);

      convolveNoAlloc(
        rgbBuffers[srcId].g,
        rowOffset,
        image->rows(),
        colOffset,
        image->columns(),
        rgbBuffers[dstId].g,
        rowOffset,
        colOffset,
        stencil->kernel);

      convolveNoAlloc(
        rgbBuffers[srcId].b,
        rowOffset,
        image->rows(),
        colOffset,
        image->columns(),
        rgbBuffers[dstId].b,
        rowOffset,
        colOffset,
        stencil->kernel);
    }
  }

  // for(int i=0; i<image->rows(); i++)
  // {
  //   for(int j=0; j<image->columns(); j++)
  //   {
  //     std::cout << "R=" << rgbBuffers[dstId].r->get(i+rowOffset, j+colOffset) <<
  //     " G=" << rgbBuffers[dstId].g->get(i+rowOffset, j+colOffset) <<
  //     " B=" << rgbBuffers[dstId].b->get(i+rowOffset, j+colOffset) << "\n";
  //   }
  // }



  std::cout<<"Convolution is complete. Writing output buffer."<<std::endl;

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

      output.r->set(rgbBuffers[dstId].r->get(rowIndex, colIndex), i, j);
      output.g->set(rgbBuffers[dstId].g->get(rowIndex, colIndex), i, j);
      output.b->set(rgbBuffers[dstId].b->get(rowIndex, colIndex), i, j);

      // std::cout << "(row=" << i << ",col=" << j  << ") " <<
      // "R=" << output.r->get(i, j) <<
      // " G=" << output.g->get(i, j) <<
      // " B=" << output.b->get(i, j) << "\n";
    }
  }

  std::cout<<"Processing is complete. Creating output image."<<std::endl;

  auto outputImage = std::make_shared<Image>(
    image->magic_number(),
    image->rows(),
    image->columns(),
    image->max_value(),
    output.r,
    output.g,
    output.b);

  std::cout<<"Output image: " << args->output_filename << std::endl <<
    "\tMagic Num:\t" << outputImage->magic_number() << std::endl <<
    "\tRows:\t\t" << outputImage->rows() << std::endl <<
    "\tColumns:\t" << outputImage->columns() << std::endl <<
    "\tMax Val:\t" << outputImage->max_value() << std::endl <<
    std::endl;

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