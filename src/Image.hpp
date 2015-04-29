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

  std::shared_ptr<Array2d> red_pixels() const { return red; }
  std::shared_ptr<Array2d> green_pixels() const { return green; }
  std::shared_ptr<Array2d> blue_pixels() const { return blue; }

protected:
  void parse_body(std::ifstream&) override;

private:
  std::shared_ptr<Array2d> red;
  std::shared_ptr<Array2d> green;
  std::shared_ptr<Array2d> blue;

  void parse_image_line(const std::string&, int&, int&);
  void set_sample_value(const double, int&, int&, int&);
  void output_pixel(std::string&, int row, int col) const;
};

#endif /* _IMAGE_H */