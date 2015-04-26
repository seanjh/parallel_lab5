#ifndef _PNM_H
#define _PNM_H

#include <string>
#include <algorithm>
#include <functional>
#include <cctype>
#include <locale>

typedef struct _dimensions {
  int columns;
  int rows;
} dimensions;

class PNM {
public:
  PNM(const std::string&);

  int magic_number() const { return magic_number_; }
  int columns() const { return columns_; }
  int rows() const { return rows_; }
  int max_value() const { return max_value_; }

  virtual ~PNM();

protected:
  void parse_source_file();
  void parse_header(std::ifstream&);
  virtual void parse_body(std::ifstream&);

  static inline bool is_blank_line(const std::string&);
  static inline bool is_comment_line(const std::string&);

  void set_magic_number(int magic_number) { magic_number_ = magic_number; }
  void set_columns(int columns) { columns_ = columns; }
  void set_rows(int rows) { rows_ = rows; }
  void set_max_value(int max_value) { max_value_ = max_value; }

private:
  std::string source_filename;
  int magic_number_;
  int columns_;
  int rows_;
  int max_value_;

  static inline int parse_magic_number(const std::string&);
  static inline dimensions parse_dimensions(const std::string&);
  static inline int parse_max_value(const std::string&);
};

#endif /* _PNM_H */