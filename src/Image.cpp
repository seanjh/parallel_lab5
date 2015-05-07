#include <fstream>
#include <cassert>
#include <cctype>
#include <stdexcept>

#include "Image.hpp"

// #define CHARACTERS_PER_PIXEL (3 + 1 + 3 + 1 + 3 + 1)
// #define MAX_CHARACTERS_PER_LINE (70 - 1)
// #define MAX_PIXELS_PER_LINE MAX_CHARACTERS_PER_LINE / CHARACTERS_PER_PIXEL

const int samples_per_pixel = 3;
const int CHARACTERS_PER_PIXEL = (3 + 1 + 3 + 1 + 3 + 1);
const int MAX_CHARACTERS_PER_LINE = (70 - 1); // Defined by Netpbm spec
const int MAX_PIXELS_PER_LINE = MAX_CHARACTERS_PER_LINE / CHARACTERS_PER_PIXEL;

Image::Image(const std::string& filename) : PNM(filename)
{
}

Image::Image(const Image &img) :
  Image(
    img.magic_number(), img.rows(), img.columns(), img.max_value(),
    img.red_pixels(), img.blue_pixels(), img.green_pixels()
    )
{
  std::cout << "Making a copy of image\n";
  // Intentionally blank
}

Image::Image(
  int magic_val, int row_count, int col_count, int max_val,
  std::shared_ptr<Array2d> red_a,
  std::shared_ptr<Array2d> green_a,
  std::shared_ptr<Array2d> blue_a
  ) : PNM(magic_val, row_count, col_count, max_val)
{
  // std::cout << "Making a new image from parameters\n";
  // Get a copy of each color array
  this->red = red_a;
  this->green = green_a;
  this->blue = blue_a;
}


Image::~Image()
{
}

void Image::parse()
{
  std::cout << "IMAGE PARSE\n";

  std::ifstream infile (source_filename());
  if (infile.is_open()) {
    parse_header(infile);

    red = std::make_shared<Array2d>(rows(), columns());
    green = std::make_shared<Array2d>(rows(), columns());
    blue = std::make_shared<Array2d>(rows(), columns());

    parse_body(infile);

    infile.close();
  } else{
    std::cerr << "Unable to open file " << source_filename() << "\n";
  }
}

void Image::parse_body(std::ifstream& infile)
{
  std::string line;

  int row = 0;
  int column = 0;
  while ( getline (infile, line) ) {
    if (is_comment_line(line) || is_blank_line(line)) {
      continue;
    } else {
      parse_image_line(line, row, column);
    }
  }

}

void Image::parse_image_line(const std::string& line, int &row, int &column)
{
  std::cout << "Body line is " << line << std::endl;
  const std::string whitespace ("\t\n\r ") ;

  int sample_num = 0;
  double value;
  std::size_t last_index = 0;
  std::size_t index = line.find_first_of(whitespace);
  while (index != std::string::npos)
  {
    // Skip over any interal whitespace
    while (index < line.size() && isspace(line.at(index))) {
      index++;
    }

    value = std::stod(line.substr(last_index, index - last_index));
    set_sample_value(value, sample_num, row, column);
    last_index = index;
    index = line.find_first_of(whitespace, index);
  }

}

void Image::set_sample_value(const double value, int &sample_num, int &row, int &column)
{
  assert(sample_num < samples_per_pixel);
  assert(row < rows());
  assert(column < columns());
  assert(value < max_value() + 1);

  switch (sample_num) {
    case 0:
      // RED
      std::cout << "Updating RED [" << row << "][" << column << "] to " << value << "\n";
      red->set(value, row, column);
      break;
    case 1:
      // GREEN
      std::cout << "Updating GREEN [" << row << "][" << column << "] to " << value << "\n";
      green->set(value, row, column);
      break;
    case 2:
      // BLUE
      std::cout << "Updating BLUE [" << row << "][" << column << "] to " << value << "\n";
      blue->set(value, row, column);

      if (column == columns() - 1) {
        std::cout << "Starting a new ROW\n";
        // Start a new row here
        row++;
      }
      // column = (column == last_column_) ?  first_column_ : (column + 1);
      column = (column + 1) % columns();
      break;
  }

  // Increment sample number
  sample_num = (sample_num + 1) % samples_per_pixel;
}

void Image::save(const std::string& filename)
{
  std::ofstream file;
  file.open(filename);

  file << "P" << magic_number() << std::endl;
  file << columns() << " " << rows() << std::endl;
  file << max_value() << std::endl;

  int pixelCount = 0;
  std::string buff = std::string();

  // std::cout<<"Saving image\n";
  // std::cout<<"Rows=" << rows() <<std::endl;
  // std::cout<<"Columns=" << columns() <<std::endl;

  for(int row=0; row < rows(); row++)
  {
    for (int col=0; col < columns(); col++)
    {
      // buff += *(outputPixel(row, col));
      output_pixel(buff, row, col);
      pixelCount++;

      if(pixelCount >= MAX_PIXELS_PER_LINE) {
        // Dump to file
        file << buff << std::endl;
        pixelCount = 0;
        buff.clear();
      }

    } // end for cols
  } // end for rows

  if(pixelCount > 0) {
    // Push any remaining data in the buffer to file
    file << buff << std::endl;
  }

}

void Image::output_pixel(std::string& buff, int row, int col) const
{
  buff += std::to_string((unsigned char) round(red->get(row, col))) + " ";
  buff += std::to_string((unsigned char) round(green->get(row, col))) + " ";
  buff += std::to_string((unsigned char) round(blue->get(row, col))) + " ";
}
