#ifndef _ARRAY2D_H
#define _ARRAY2D_H

class Array2d {
public:
  Array2d(int, int);
  Array2d(Array2d&);
  ~Array2d();

  double get(int row, int col);
  void set(double val, int row, int col);
  // void mult(double scalar);

  const int rows;
  const int cols;
private:
  double **array;
};

#endif /* _ARRAY2D_H */