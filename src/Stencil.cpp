#include <cassert>
#include <cctype>
#include <cmath>
#include <cstddef>

#include "Stencil.hpp"

const int stencil_range_max = 127;
const int stencil_range_min = -127;

Stencil::Stencil(const std::string& filename) : PNM(filename)
{
}

Stencil::~Stencil()
{
}

void Stencil::parse()
{
  std::ifstream infile (source_filename());
  if (infile.is_open()) {
    parse_header(infile);

    // Both stencil dimensions must be odd
    assert(rows() == columns());
    assert(rows() % 2 != 0);
    assert(columns() % 2 != 0);

    kernel = std::make_shared<Array2d>(rows(), columns());

    parse_body(infile);
    infile.close();

    normalize();
  } else{
    std::cerr << "Unable to open file " << source_filename() << "\n";
  }
}

void Stencil::parse_body(std::ifstream& infile)
{
  std::string line;
  int row = 0;
  while ( getline (infile, line) ) {
    if (is_comment_line(line) || is_blank_line(line)) {
      continue;
    } else {
      parse_stencil_line(line, row);
      row++;
    }
  }
}

void Stencil::parse_stencil_line(const std::string& line, const int row)
{
  const std::string whitespace ("\t\n\r ") ;

  int column = 0;
  double value;
  std::size_t last_index = 0;
  std::size_t index = line.find_first_of(whitespace);
  while (index != std::string::npos)
  {
    if (!isspace(line.at(index + 1))) {
      value = std::stod(line.substr(last_index, index - last_index));
      kernel->set(value, row, column++);
      last_index = index;
    }
    index = line.find_first_of(whitespace, index + 1);
  }

  // Parse the final value on the line
  value = std::stod(line.substr(last_index, line.size() - last_index));
  // std::cout << "New value from line(" << last_index << "," <<  line.size() << ") " << value << "\n";
  kernel->set(value, row, column);
}

void Stencil::normalize()
{
  std::cout << "Source PGM kernel:\n";
  print_kernel();

  double kernel_sum = shift_kernel_range();

  normalize_values(kernel_sum);
  std::cout << "Normalized kernel:\n";
  print_kernel();
}

double Stencil::shift_kernel_range()
{
  // Updates kernel values from 0 --> max_value range to stencil_range_min --> stencil_range_max
  // kernel[i][j] = stencil_range_min + (2 * stencil_range_max * PGM value) / (PGM max_value)

  assert(max_value() != 0);
  double kernel_sum = 0.0;

  double old_value;
  double new_value;

  for (int i=0; i < rows(); i++) {
    for (int j=0; j < columns(); j++) {
      old_value = kernel->get(i, j);
      new_value = (double) stencil_range_min + (2 * stencil_range_max * old_value) / max_value();
      new_value = round(new_value);
      kernel->set(new_value , i, j);
      kernel_sum += new_value;
    }
  }

  return round(kernel_sum);
}

void Stencil::normalize_values(double kernel_sum)
{
  double value;

  if ((int) kernel_sum == 0) {

    // Zero-sum normalization
    double sum_negative = 0.0;
    double sum_positive = 0.0;
    for (int i=0; i < rows(); i++) {
      for (int j=0; j < columns(); j++) {
        value = kernel->get(i, j);
        if (value < 0.0)
          sum_negative -= value;
        else
          sum_positive += value;
      }
    }

    sum_positive = round(sum_positive);
    sum_negative = round(sum_negative);

    // Don't divide by 0!
    assert((int) sum_positive != 0);
    assert((int) sum_negative != 0);

    for (int i=0; i < rows(); i++) {
      for (int j=0; j < columns(); j++) {
        value = kernel->get(i, j);
        if (value < 0.0){
          value = value / sum_negative;
        } else {
          value = value / sum_positive;
        }
        kernel->set(value, i, j);
      }
    }

  } else {

    // regular normalization
    for (int i=0; i < rows(); i++) {
      for (int j=0; j < columns(); j++) {
        value = kernel->get(i, j);
        value = value / kernel_sum;
        kernel->set(value, i, j);
      }
    }

  }
}

void Stencil::print_kernel() const
{
  for (int i=0; i < rows(); i++) {
    for (int j=0; j < columns(); j++) {
      std::cout << kernel->get(i, j) << "\t";
    }
    std::cout << std::endl;
  }
}