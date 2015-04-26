#include <iostream>
#include <assert.h>
#include <memory>

static void show_usage(std::string name)
{
  std::cerr << "Usage: " << name << " [options] image_ppm stencil_pgm iterations\n"
            << "Options:\n"
            << "\t-h,--help\t\tShow this help dialog\n"
            << std::endl;
}

int main(int argc, char *argv[])
{
  if (argc < 4) {
    show_usage(argv[0]);
    return 1;
  }
  for (int i=1; i< argc; i++) {
    //
  }

  // Parse CLI arguments
  // Parse PGM stencil
  // Parse PPM image
  // for each iteration
  //  preallocate
  //  apply stencil (convolve)
  // write filtered PPM image
}