#include <iostream>

class BoundaryConditions{
public:
    enum class type{periodic, periodic_horizontal, periodic_vertical, non_periodic};
    enum class direction{up,down,left,right};
    enum class result{valid,invalid};

public:
    void set_boundary_conditions(type t);
    result get_nn_coordinates(int row, int col, int& row_out, int& col_out, const int n_rows, const int n_cols, direction dir);
    
private:
    type m_state = type::periodic;
};

void BoundaryConditions::set_boundary_conditions(BoundaryConditions::type t){
    m_state = t;
}

BoundaryCondition::result BoundaryConditions::get_nn_coordinates(int row, int col, int& row_out, int& col_out, const int n_rows, const int n_cols, BoundaryConditions::direction dir){
    result r = result::valid;

    switch (m_state){
        case type::periodic:{
           switch (dir){
               case direction::up:{
                    row_out = (row - 1 < 0) ? (n_rows-1) : (row-1);
                    col_out = col;
                    return;
               }
               case direction::down:{
                    row_out = (row + 1 == n_rows) ? 0 : (row + 1);
                    col_out = col;
                    return;
               }
               case direction::left:{
                   row_out = row;
                   col_out = (col-1 < 0) ? (n_cols - 1) : col - 1;
                   return;
               }
               case direction::right:{
                   row_out = row;
                   col_out = (col+1==n_cols) ? 0 : (col + 1);
                   return;
               }
            }
            break;
        }

        case type::periodic_horizontal:{
            break;
        }

        case type::periodic_vertical:{
            break;
        }

        case type::non_periodic:{
            break;
        }
        default:{
            std::cout << "Error checking boundary conditions" << std::endl;
            r = result::invalid;
        }
    }

    switch (dir){
        case direction::up:{

        }

    }

    return r;
}