#include <cstdlib>
#include <cassert>
#include <memory>
#include <iostream>
#include <string>

#include "Stencil.hpp"
#include "PPM.hpp"

typedef struct _FilterArguments {
  std::string image_filename;
  std::string stencil_filename;
  int iterations;
  int threads;
} FilterArguments;


void show_usage(std::string name)
{
  std::cerr << "Usage: " << name << " [options] image_ppm stencil_pgm iterations\n"
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

  std::string argument;
  for (int i = 1; i < argc; i++) {
    argument = argv[i];
    std::cout << "Arg[" << i << "] " << argument << "\n";
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
    } else {
      args->iterations = std::stoi(argv[i]);
      break;
    }
  }

  std::cout << "Image filename: " << args->image_filename << "\n";
  std::cout << "Stencil filename: " << args->stencil_filename << "\n";
  std::cout << "Iterations: " << args->iterations << "\n";
  std::cout << "Threads: " << args->threads << "\n";
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
  // auto image = std::make_shared<Image>(args->stencil_filename);
  // image->parse();


  // for each iteration
  //  preallocate
  //  apply stencil (convolve)
  // write filtered PPM image

  return EXIT_SUCCESS;
}