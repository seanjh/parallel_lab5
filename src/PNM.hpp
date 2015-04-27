#ifndef _PNM_H
#define _PNM_H

#include <memory>
#include <string>
#include <algorithm>
#include <functional>
#include <cctype>
#include <locale>
#include <iostream>
#include <fstream>

typedef struct _dimensions {
  int columns;
  int rows;
} dimensions;

class PNM {
public:
  PNM(const std::string&);
  virtual void parse();

  const std::string& source_filename() { return source_filename_; }
  int magic_number() const { return magic_number_; }
  int columns() const { return columns_; }
  int rows() const { return rows_; }
  int max_value() const { return max_value_; }

  virtual ~PNM();

protected:
  void parse_header(std::ifstream&);
  virtual void parse_body(std::ifstream&);

  static bool is_blank_line(const std::string&);
  static bool is_comment_line(const std::string&);

  void set_magic_number(int magic_number) { magic_number_ = magic_number; }
  void set_columns(int columns) { columns_ = columns; }
  void set_rows(int rows) { rows_ = rows; }
  void set_max_value(int max_value) { max_value_ = max_value; }

private:
  const std::string source_filename_;
  int magic_number_;
  int columns_;
  int rows_;
  int max_value_;

  static int parse_magic_number(const std::string&);
  static dimensions parse_dimensions(const std::string&);
  static int parse_max_value(const std::string&);
};

#endif /* _PNM_H */