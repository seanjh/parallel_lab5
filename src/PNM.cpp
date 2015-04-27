#include <stdlib.h>

#include "PNM.hpp"

PNM::PNM(const std::string& filename) : source_filename_(filename)
{
  // source_filename = std::make_shared<std::string>(filename);
}

void PNM::parse()
{
  std::ifstream infile (source_filename());
  if (infile.is_open()) {
    this->parse_header(infile);
    this->parse_body(infile);
    infile.close();
  } else{
    std::cerr << "Unable to open file " << source_filename() << "\n";
  }
}

PNM::~PNM()
{
    // std::cout << "Cya PNM " << source_filename() << "\n";
}

void PNM::parse_header(std::ifstream& infile)
{
  std::cout << "BEGINNING TO PARSE HEADER\n";
  std::string line;
  int line_number = 0;
  int number;
  dimensions dims;

  while ( line_number < 3 && getline (infile, line) ) {
    if (is_comment_line(line) || is_blank_line(line)) {
      // std::cout << "Skipping line - " << line << "\n";
      continue;
    } else {
      // TODO
      // std::cout << "Header line: " << line << "\n";
      switch (line_number) {
        case 0:
          // Magic number line (e.g., "P2\n")
          number = parse_magic_number(line);
          set_magic_number(number);
          line_number++;
          break;
        case 1:
          // Row and Column count line (e.g., "3 3\n")
          dims = parse_dimensions(line);
          set_columns(dims.columns);
          set_rows(dims.rows);
          line_number++;
          break;
        case 2:
          // Maximum value line (e.g., "255\n")
          number = parse_max_value(line);
          set_max_value(number);
          line_number++;
          break;
      }
    }
  }

  std::cout << "FINISHED PARSING HEADER\n";
}

void PNM::parse_body(std::ifstream& infile)
{
  std::cout << "PNM (Base class) PARSE BODY\n";
}

int PNM::parse_magic_number(const std::string& str)
{
  return std::stoi(str.substr(1, 1));
}

dimensions PNM::parse_dimensions(const std::string& str)
{
  int whitespace_index = 0;
  char c;
  for (size_t i=0; i < str.length(); i++) {
    c = str.at(i);
    if (c == ' ' || c == '\t') {
      whitespace_index = i;
      break;
    }
  }

  std::string col_str = str.substr(0, whitespace_index);
  std::string row_str = str.substr(whitespace_index, str.length());
  dimensions d;

  d.columns = std::stoi(col_str);
  d.rows = std::stoi(row_str);
  std::cout << "columns: " << d.columns  << ", rows: " << d.rows << "\n";

  return d;
}

int PNM::parse_max_value(const std::string& str)
{
  return std::stoi(str);
}

bool PNM::is_comment_line(const std::string& str)
{
  return str.size() > 0 && str.at(0) == '#';
}

bool PNM::is_blank_line(const std::string& str)
{
  char c;
  for (size_t i=0; i < str.size(); i++) {
    c = str.at(i);
    if (c == ' ' || c == '\t' || c == '\n' || c == '\r' || c == 0)
      continue;
    else
      return false;
  }

  return true;
}