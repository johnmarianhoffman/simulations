#include "BoundaryConditions.h"

void BoundaryConditions::set_boundary_conditions(BoundaryConditions::type t){
  m_state = t;
}

BoundaryConditions::result BoundaryConditions::get_nn_coordinates(int row, int col, int& row_out, int& col_out, const int n_rows, const int n_cols, BoundaryConditions::direction dir){
  
  result r = result::valid;

  switch (m_state){
  case type::periodic:{
    switch (dir){
    case direction::up:{
      row_out = (row - 1 + n_rows)%n_rows;
      col_out = col;
      break;
    }
    case direction::down:{
      row_out = (row + 1)%n_rows;
      col_out = col;
      break;
    }
    case direction::left:{
      row_out = row;
      col_out = (col - 1 + n_cols)%n_cols;
      break;
    }
    case direction::right:{
      row_out = row;
      col_out = (col+1)%n_cols;
      break;
    }
    }
    break;
  }

  case type::periodic_horizontal:{

    switch (dir){
    case direction::up:{
      row_out = row - 1;
      col_out = col;
      break;
    }
    case direction::down:{
      row_out = row + 1;
      col_out = col;
      break;
    }
    case direction::left:{
      row_out = row;
      col_out = (col - 1 + n_cols)%n_cols;
      break;
    }
    case direction::right:{
      row_out = row;
      col_out = (col+1)%n_cols;
      break;
    }
    }

    if (row_out < 0 || row_out == n_rows)
      r = result::invalid;
    
    break;
  }

  case type::periodic_vertical:{


    switch (dir){
    case direction::up:{
      row_out = (row - 1 + n_rows) % n_rows;
      col_out = col;
      break;
    }
    case direction::down:{
      row_out = (row + 1) % n_rows;
      col_out = col;
      break;
    }
    case direction::left:{
      row_out = row;
      col_out = col - 1;
      break;
    }
    case direction::right:{
      row_out = row;
      col_out = col + 1;
      break;
    }
    }

    if (col_out < 0 || col_out == n_cols)
      r = result::invalid;
    
    break;
  }

  case type::non_periodic:{
    switch (dir){
    case direction::up:{
      row_out = row - 1;
      col_out = col;
      break;
    }
    case direction::down:{
      row_out = row + 1;
      col_out = col;
      break;
    }
    case direction::left:{
      row_out = row;
      col_out = col - 1;
      break;
    }
    case direction::right:{
      row_out = row;
      col_out = col + 1;
      break;
    }
    }

    if ( row_out<0 || col_out <0 || row_out==n_rows || col_out==n_cols )
      r = result::invalid;
    
    break;
  }
  default:{
    std::cout << "Error checking boundary conditions" << std::endl;
    r = result::invalid;
  }
  }

  return r;
}