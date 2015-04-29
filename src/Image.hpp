#ifndef _IMAGE_H
#define _IMAGE_H

#include <memory>
#include <string>

#include "PNM.hpp"
#include "Array2d.hpp"

extern const int samples_per_pixel;

class Image : public PNM {
public:
  Image(const std::string&);
  // Image(const std::string&, int, int);
  Image(const Image&);
  Image(int, int, int, int, std::shared_ptr<Array2d>, std::shared_ptr<Array2d>, std::shared_ptr<Array2d>);
  ~Image();

  void parse() override;
  void save(const std::string& filename);

  // int first_row() const { return first_row_; }
  // int first_column() const { return first_column_; }
  // int last_row() const { return last_row_; }
  // int last_column() const { return last_column_; }

  std::shared_ptr<Array2d> red_pixels() const { return red; }
  std::shared_ptr<Array2d> green_pixels() const { return green; }
  std::shared_ptr<Array2d> blue_pixels() const { return blue; }

protected:
  void parse_body(std::ifstream&) override;

private:
  std::shared_ptr<Array2d> red;
  std::shared_ptr<Array2d> green;
  std::shared_ptr<Array2d> blue;

  // int kernel_rows_;
  // int kernel_columns_;

  // Covers sub-section of the arrays with the real image data (i.e., excluding the kernel "padding")
  // int first_row_;
  // int first_column_;
  // int last_row_;
  // int last_column_;

  void parse_image_line(const std::string&, int&, int&);
  void set_sample_value(const double, int&, int&, int&);
  std::shared_ptr<std::string> outputPixel(int row, int col) const;
  // void calculate_offsets();
};

#endif /* _IMAGE_H */