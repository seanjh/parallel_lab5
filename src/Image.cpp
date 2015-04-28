#include <fstream>
#include <cassert>
#include <cctype>
#include <stdexcept>

#include "Image.hpp"

#define CHARACTERS_PER_PIXEL 3 + 1 + 3 + 1 + 3 + 1
#define MAX_CHARACTERS_PER_LINE 70-1
#define MAX_PIXELS_PER_LINE MAX_CHARACTERS_PER_LINE / CHARACTERS_PER_PIXEL

Image::Image(const std::string& filename, int kernel_rows, int kernel_cols) : PNM(filename), kernel_rows_(kernel_rows), kernel_columns_(kernel_cols)
{
  // int row_padding = kernel_rows_ / 2;
  // int col_padding = kernel_cols / 2;

  // calculate_offsets();
}

Image::Image(const std::string& filename) : Image(filename, 0, 0)
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
  std::shared_ptr<Array2d> blue_a,
  std::shared_ptr<Array2d> green_a
  ) :
  PNM(magic_val, row_count, col_count, max_val)
{
  std::cout << "Making a new image from parameters\n";
  // Get a copy of each color array
  this->red = std::make_shared<Array2d>(*red_a);
  this->green = std::make_shared<Array2d>(*blue_a);
  this->blue = std::make_shared<Array2d>(*green_a);

  // calculate_offsets();
}


Image::~Image()
{
}

void Image::calculate_offsets()
{
  // Requires that rows() and columns() are initialized
  assert(rows());
  assert(columns());

  // Determines any required "padding" around the image to accomodate the kernel
  if (kernel_rows_ != 0 && kernel_columns_ != 0) {
    assert(kernel_columns_ % 2 != 0);
    assert(kernel_rows_ % 2 != 0);

    int row_padding = kernel_rows_ / 2;
    int col_padding = kernel_columns_ / 2;

    first_row_ = row_padding;
    first_column_ = col_padding;
    last_row_ = rows() - 1 + row_padding;
    last_column_ = columns() - 1 + col_padding;
    // std::cout << "Row padding " << row_padding << " Col padding " << col_padding << "\n";
    // std::cout << "First Row " << first_row_ << " First Column " << first_column_ << "\n";
    // std::cout << "Last Row " << last_row_ << " Last Column " << last_column_ << "\n";

  } else {
    // No image padding needed
    first_row_ = 0;
    first_column_ = 0;
    last_row_ = rows() - 1;
    last_column_ = columns() - 1;
  }
}

void Image::parse()
{
  std::cout << "IMAGE PARSE\n";

  std::ifstream infile (source_filename());
  if (infile.is_open()) {
    parse_header(infile);

    calculate_offsets();

    // Row and Column counts must include offsets (if any)
    int row_count = rows() + ((kernel_rows_ / 2) * 2);
    int col_count = columns() + ((kernel_columns_ / 2) * 2);
    red = std::make_shared<Array2d>(row_count, col_count);
    green = std::make_shared<Array2d>(row_count, col_count);
    blue = std::make_shared<Array2d>(row_count, col_count);

    parse_body(infile);

    infile.close();
  } else{
    std::cerr << "Unable to open file " << source_filename() << "\n";
  }
}

void Image::parse_body(std::ifstream& infile)
{
  // std::cout << "IMAGE: BEGINNING TO PARSE BODY\n";

  std::string line;

  int row = first_row_;
  int column = first_column_;
  while ( getline (infile, line) ) {
    if (is_comment_line(line) || is_blank_line(line)) {
      // std::cout << "Skipping line - " << line << "\n";
      continue;
    } else {
      parse_image_line(line, row, column);
    }
  }

  // std::cout << "IMAGE: FINISHED PARSING BODY\n";
}

void Image::parse_image_line(const std::string& line, int &row, int &column)
{
  // std::cout << "Body line is " << line << std::endl;
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
    // std::cout << "VALUE " << value << " value from line at " << last_index << ", len " <<  index - last_index << "\n";
    set_sample_value(value, sample, row, column);
    last_index = index;
    index = line.find_first_of(whitespace, index);
  }

  // std::cout << "DONE LINE\n";
}

void Image::set_sample_value(const double value, int &sample, int &row, int &column)
{
  // std::cout << "Sample " << sample << " Row " << row << " Column " << column << "\n";
  assert(sample < 3);
  assert(row <= last_row_);
  assert(column <= last_column_);
  assert(value < max_value() + 1);

  switch (sample) {
    case 0:
      // RED
      // std::cout << "Updating RED [" << row << "][" << column << "] to " << value << "\n";
      red->set(value, row, column);
      break;
    case 1:
      // GREEN
      // std::cout << "Updating GREEN [" << row << "][" << column << "] to " << value << "\n";
      green->set(value, row, column);
      break;
    case 2:
      // BLUE
      // std::cout << "Updating BLUE [" << row << "][" << column << "] to " << value << "\n";
      blue->set(value, row, column);

      if (column == last_column_) {
        // std::cout << "Starting a new ROW\n";
        // Start a new row here
        row++;
      }
      column = (column == last_column_) ?  first_column_ : (column + 1);
      break;
  }

  // Increment sample number
  sample = (sample + 1) % 3;
}

void Image::save(const std::string& filename)
{
  std::ofstream file;
  file.open(filename);

  calculate_offsets();

  file << "P" << magic_number() << std::endl;

  file << columns() << " " << rows() << std::endl;

  file << max_value() << std::endl;

  int pixelCount = 0;
  std::string buff = std::string();

  std::cout<<"Outputting image."<<std::endl;

  std::cout<<"Columns = ("<<first_column_<<", "<<last_column_<<")"<<std::endl;
  std::cout<<"Rows = ("<<first_row_<<", "<<last_row_<<")"<<std::endl;

  for(int i = first_column_; i <= last_column_; i++)
  {
    for (int j = first_row_; j <= last_row_; j++) {
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
    }
  }

  if(pixelCount > 0)
  {
    file << buff << std::endl;
  }

}

std::shared_ptr<std::string> Image::outputPixel(int row, int col) const
{
  std::string buff = std::string();
  buff += std::to_string((int)(red->get(row, col) / max_value())) + " ";
  buff += std::to_string((int)(green->get(row, col) / max_value())) + " ";
  buff += std::to_string((int)(blue->get(row, col) / max_value())) + " ";

  return std::make_shared<std::string>(buff);
}
