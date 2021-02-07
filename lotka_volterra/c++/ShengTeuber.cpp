#include "ShengTeuber.h"

void ShengTeuber::initialize_model(){
  // Traverse the starting grid and add occupied cells list
  for (int i=0; i<m_n_rows*m_n_cols;i++){
    if (m_grid[i]!=species::Null)
      m_occupied_cells.insert(i);
    
  }

}

void ShengTeuber::step(){

  m_timer.tic();

  // Randomly sample an occupied cell
  auto it = m_occupied_cells.begin();
  std::advance(it,(size_t)(rand() % m_occupied_cells.size()));
  
  int curr_idx = *it;
  int curr_row = curr_idx/m_n_cols;
  int curr_col = curr_idx%m_n_cols;

  // Select operation to use
  float rand_op = ((float)rand()/(float)RAND_MAX);
  
  // Diffusion
  if (rand_op < 0.25f){
  }
  
  // mu: Predator death reaction
  else if (rand_op >= 0.25 && rand_op < 0.5f){
    
  
    if (m_grid[curr_idx]==species::B){
      rand_op = ((float)rand()/(float)RAND_MAX);
      if (rand_op < m_k3){
        m_grid[curr_idx] = species::Null;
        m_occupied_cells.erase(it);
        m_time_compute_step = m_timer.toc();
      }
    }
  }

  // Predator eats prey reaction
  else if (rand_op >= 0.5 && rand_op < 0.75f){
    
    
    int nn_row, nn_col;
    if (m_grid[curr_idx]==species::B){
      BoundaryConditions::result r = m_boundary_conditions.get_random_nn(curr_row, curr_col, nn_row, nn_col, m_n_rows, m_n_cols);
      if (r != BoundaryConditions::result::invalid && m_grid[nn_col + nn_row * m_n_cols] == species::A){
        rand_op = ((float)rand()/(float)RAND_MAX);
        if (rand_op < m_k2){
          m_grid[nn_col + nn_row * m_n_cols] = species::B;
          m_time_compute_step = m_timer.toc();
        }
      }
    }
  }
  
  // Prey reproduction
  else if (rand_op >= 0.75 && rand_op <= 1.0f){    
    int nn_row, nn_col;
    if (m_grid[curr_idx]==species::A){
      BoundaryConditions::result r = m_boundary_conditions.get_random_nn(curr_row, curr_col, nn_row, nn_col, m_n_rows, m_n_cols);
      if (r != BoundaryConditions::result::invalid && m_grid[nn_col + nn_row * m_n_cols] == species::Null){
        rand_op = ((float)rand()/(float)RAND_MAX);
        if (rand_op < m_k1){
          m_grid[nn_col + nn_row * m_n_cols] = species::A;
          m_occupied_cells.insert(nn_col + nn_row * m_n_cols);
          m_time_compute_step = m_timer.toc();
        }
      }
    }
  }

  //m_time_compute_step = m_timer.toc();
  m_time_mc_step = 1.0f / (float)m_occupied_cells.size();
  m_time_mc_total += m_time_mc_step;

}
