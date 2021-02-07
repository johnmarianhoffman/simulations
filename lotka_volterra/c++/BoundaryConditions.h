#pragma once

#include <iostream>

class BoundaryConditions{
public:
  enum class type{periodic, periodic_horizontal, periodic_vertical, non_periodic};
  enum class direction{up,down,left,right};
  enum class result{valid,invalid};

public:
  void set_boundary_conditions(type t);
  result get_nn_coordinates(int row, int col, int& row_out, int& col_out, const int n_rows, const int n_cols, direction dir);
  result get_random_nn(int row, int col, int& row_out, int& col_out, const int n_rows, const int n_cols);

  std::string get_boundary_condition_string();
    
private:
  type m_state = type::periodic;
};
