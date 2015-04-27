#ifndef _IMAGE_H
#define _IMAGE_H

#include <memory>
#include <string>

#include "PNM.hpp"
#include "Array2d.hpp"

class Image : public PNM {
public:
  Image(const std::string&);
  Image(const std::string&, int, int);
  Image(const Image&);
  Image(int, int, int, int, std::shared_ptr<Array2d>, std::shared_ptr<Array2d>, std::shared_ptr<Array2d>);
  ~Image();

  void parse() override;

  void save(const std::string& filename);

  // double get(int row, int col);
  // void set(double val, int row, int col);
  // void mult(double scalar);

  std::shared_ptr<Array2d> red_pixels() const { return red; }
  std::shared_ptr<Array2d> green_pixels() const { return green; }
  std::shared_ptr<Array2d> blue_pixels() const { return blue; }

protected:
  void parse_body(std::ifstream&) override;

private:
  // rows, cols, max_value, red, green, blue
  std::shared_ptr<Array2d> red;
  std::shared_ptr<Array2d> green;
  std::shared_ptr<Array2d> blue;

  int kernel_rows_;
  int kernel_columns_;
  int first_row_;
  int first_column_;
  int last_row_;
  int last_column_;

  void parse_image_line(const std::string&, int&, int&);
  void set_sample_value(const double, int&, int&, int&);
  std::shared_ptr<std::string> outputPixel(int row, int col) const;
  void calcuate_offsets();
};

#endif /* _IMAGE_H */