#ifndef _STENCIL_H
#define _STENCIL_H

#include <memory>
#include <string>
#include <iostream>
#include <fstream>

#include "PNM.hpp"
#include "Array2d.hpp"

extern const int stencil_range_max;
extern const int stencil_range_min;

class Stencil : public PNM
{
public:
  Stencil(const std::string&);
  ~Stencil();

  void parse() override;
  void print_kernel() const;

  std::shared_ptr<Array2d> kernel;

protected:
  void parse_body(std::ifstream&) override;

private:
  

  void parse_stencil_line(const std::string&, const int);
  void normalize();
  double shift_kernel_range();
  void normalize_values(double);
};

#endif /* _STENCIL_H */