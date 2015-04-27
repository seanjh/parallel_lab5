#include <fstream>
#include <memory>
#include <cassert>
#include <cctype>
#include <stdexcept>

#include "Image.hpp"

#define CHARACTERS_PER_PIXEL 3 + 1 + 3 + 1 + 3 + 1
#define MAX_CHARACTERS_PER_LINE 70-1
#define MAX_PIXELS_PER_LINE MAX_CHARACTERS_PER_LINE / CHARACTERS_PER_PIXEL

Image::Image(const std::string& filename) : PNM(filename)
{
}

// PPM::PPM(int r, int c, int m, std::sharedptr<Array2d> redArray, std::sharedptr<Array2d> greenArray, std::sharedptr<Array2d> blueArray) :
// maxVal(m), rows(r), cols(c), red(redArray), green(greenArray), blue(blueArray)
// {
// }

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
  std::cout << "STENCIL BEGINNING TO PARSE BODY\n";

  std::string line;
  int row = 0;
  int column = 0;
  while ( getline (infile, line) ) {
    if (is_comment_line(line) || is_blank_line(line)) {
      // std::cout << "Skipping line - " << line << "\n";
      continue;
    } else {
      parse_image_line(line, row, column);
    }
  }

  std::cout << "FINISHED PARSING STENCIL BODY\n";
}

void Image::parse_image_line(const std::string& line, int &row, int &column)
{
  std::cout << "Body line is " << line << std::endl;
  const std::string whitespace ("\t\n\r ") ;

  int sample = 0;
  double value;
  std::size_t last_index = 0;
  std::size_t index = line.find_first_of(whitespace);
  while (index != std::string::npos)
  {
    // Skip over any interal whitespace
    while (index < line.size() && isspace(line.at(index))) {
      // std::cout << "Skipping whitespace at index " << index << "\n";
      index++;
    }

    value = std::stod(line.substr(last_index, index - last_index));
    std::cout << "VALUE " << value << " value from line at " << last_index << ", len " <<  index - last_index << "\n";
    set_sample_value(value, sample, row, column);
    last_index = index;
    index = line.find_first_of(whitespace, index);
  }

  // std::cout << "DONE LINE\n";
}

void Image::set_sample_value(const double value, int &sample, int &row, int &column)
{
  assert(sample < 3);
  assert(row < rows());
  assert(column < columns());

  switch (sample) {
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
        // Start a new row
        row++;
      }
      column = (column + 1) % columns();
      break;
  }

  // Increment sample number
  sample = (sample + 1) % 3;
}

void Image::save(const std::string filename)
{
  std::ofstream file;
  file.open(filename);

  file << "P3" << std::endl;

  file << columns() << " " << rows() << std::endl;

  file << max_value() << std::endl;


  int pixelCount = 0;
  std::string buff = std::string();
  for(int i=0; i < rows(); i++)
  {
    int j=0;
    while(j < columns())
    {
      buff += *(outputPixel(i, j));
      pixelCount += 1;

      if(pixelCount >= MAX_PIXELS_PER_LINE)
      {
        //dump to file
        file << buff << std::endl;

        //reset pixel count and buffer
        pixelCount = 0;
        buff = std::string();
      }

      j+=1;
    }

  }

  if(pixelCount > 0)
  {
    file << buff << std::endl;
  }

}

// unsigned char convert(double val)
// {
//   return (unsigned char) val;
// }

std::shared_ptr<std::string> Image::outputPixel(int row, int col) const
{
  std::string buff = std::string();
  // buff += std::to_string(convert(red->get(row, col))) + " ";
  // buff += std::to_string(convert(green->get(row, col))) + " ";
  // buff += std::to_string(convert(blue->get(row, col))) + " ";
  buff += std::to_string(red->get(row, col)) + " ";
  buff += std::to_string(green->get(row, col)) + " ";
  buff += std::to_string(blue->get(row, col)) + " ";

  return std::make_shared<std::string>(buff);
}
