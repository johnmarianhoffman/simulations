#include "Hoffman.h"

#include <algorithm>

void Hoffman::initialize_model(){
  // Traverse the starting grid and add occupied cells list
  for (int i=0; i<m_n_rows*m_n_cols;i++){
    if (m_grid[i]!=species::Null)
      m_occupied_cells.insert(i); 
  }

  m_W = std::vector<float>(20,0.0f);
  m_W_cumulative = std::vector<float>(20,0.0f);
};

void Hoffman::step(){

  m_timer.tic();

  /* Randomly sample an occupied cell */
  auto it = m_occupied_cells.begin();
  std::advance(it,(size_t)(rand() % m_occupied_cells.size()));
  
  int curr_idx = *it;
  int curr_row = curr_idx/m_n_cols;
  int curr_col = curr_idx%m_n_cols;

  /* Compute the W matrix for the current site */
  // Get nearest neighbors/handle boundary conditions
  int curr_nn_up, curr_nn_down, curr_nn_left, curr_nn_right;
  int nn_row, nn_col;
  BoundaryConditions::result r_up = m_boundary_conditions.get_nn_coordinates(curr_row, curr_col, nn_row, nn_col, m_n_rows, m_n_cols, BoundaryConditions::direction::up);
  curr_nn_up = nn_col + nn_row * m_n_cols;
  BoundaryConditions::result r_down = m_boundary_conditions.get_nn_coordinates(curr_row, curr_col, nn_row, nn_col, m_n_rows, m_n_cols, BoundaryConditions::direction::down);
  curr_nn_down = nn_col + nn_row * m_n_cols;
  BoundaryConditions::result r_left = m_boundary_conditions.get_nn_coordinates(curr_row, curr_col, nn_row, nn_col, m_n_rows, m_n_cols, BoundaryConditions::direction::left);
  curr_nn_left = nn_col + nn_row * m_n_cols;
  BoundaryConditions::result r_right = m_boundary_conditions.get_nn_coordinates(curr_row, curr_col, nn_row, nn_col, m_n_rows, m_n_cols, BoundaryConditions::direction::right);
  curr_nn_right = nn_col + nn_row * m_n_cols;

  // Each lattice site has 20 possible reactions (4 nearest neighbors, 5 possible reactions with a nearest neighbor)
  std::vector<int> nn{curr_nn_up, curr_nn_down, curr_nn_left, curr_nn_right};
  std::vector<BoundaryConditions::result> nn_result{r_up, r_down, r_left, r_right};

  bool reaction_possible = false;
  for (int n=0;n<4;++n){
    BoundaryConditions::result r = nn_result[n];
    int nn_idx = nn[n];

    // R1
    if (m_grid[curr_idx]==species::A && m_grid[nn_idx]==species::Null && r==BoundaryConditions::result::valid && m_k1!=0.0f){
      m_W[n*5 + 0] = m_k1;
      reaction_possible = true;
    }
    else 
      m_W[n*5 + 0] = 0.0f;

    // R2
    if (m_grid[curr_idx]==species::B && m_grid[nn_idx]==species::A && r==BoundaryConditions::result::valid && m_k2!=0.0f){
      m_W[n*5 + 1] = m_k2;
      reaction_possible = true;
    }
    else 
      m_W[n*5 + 1] = 0.0f;

    // R3, only stored n==0 since does not depend on a neighbor
    if (n==0 && m_grid[curr_idx]==species::B && m_k3!=0.0f){
      m_W[n*5 + 2] = m_k3;
      reaction_possible = true;
    }
    else 
      m_W[n*5 + 2] = 0.0f;

    // R4
    if (m_grid[curr_idx]==species::A && m_grid[nn_idx]==species::Null && r==BoundaryConditions::result::valid && m_d1!=0.0f){
      m_W[n*5 + 3] = m_d1;
      reaction_possible = true;
    }
    else 
      m_W[n*5 + 3] = 0.0f;

    // R5
    if (m_grid[curr_idx]==species::B && m_grid[nn_idx]==species::Null && r==BoundaryConditions::result::valid && m_d2!=0.0f){
      m_W[n*5 + 4] = m_d2;
      reaction_possible = true;
    }
    else 
      m_W[n*5 + 4] = 0.0f;
  }

  //std::cout << "=============" << curr_row << " " << curr_col << std::endl;
  //for (int i=0;i<4;i++){
  //  for (int j=0;j<5;j++){
  //    std::cout << m_W[j+i*5] << "   ";
  //  }
  //  std::cout << std::endl;    
  //}

  // Bail out if current cell has no possible updates
  if (!reaction_possible){
    //std::cout << "Returning early" << std::endl;
    return;
  }

  // Otherwise, compute cumulative function for W
  float W = m_W[0];
  for(int i=1;i<20;i++){
    m_W_cumulative[i] = m_W_cumulative[i-1] + m_W[i-1];
    W += m_W[i];
  }

  // Select the reaction that occurs from possible reactions
  float reaction = W * (float)rand()/(float)RAND_MAX;

  auto reaction_it = std::upper_bound(m_W_cumulative.begin(),m_W_cumulative.end(),reaction);
  if (reaction_it==m_W_cumulative.end()){
    std::cout << "ERROR: No possoible reactions.  We should have bailed out sooner..." << std::endl;
    return;
  }
  
  int reaction_idx = (reaction_it - m_W_cumulative.begin()) - 1;
  
  int reaction_nn_idx = reaction_idx/5; // Produce a number between 0 and 3
  int reaction_specific_idx = reaction_idx%5; // Produce a number between 0 and 4

  /* Invoke the reaction on the lattice  */
  // Get the nearest neighbor coordinates
  BoundaryConditions::result r = m_boundary_conditions.get_nn_coordinates(curr_row, curr_col,nn_row, nn_col, m_n_rows, m_n_cols, (BoundaryConditions::direction)reaction_nn_idx);

  if (r==BoundaryConditions::result::invalid)
    std::cout << "ERROR: Something has gone wrong while selecting a reaction" << std::endl;

  int nn_idx = nn_col + nn_row * m_n_cols;

  // Carry out the reaction on the lattice
  switch (reaction_specific_idx){
  case 0:{ // R1
    m_grid[nn_idx] = species::A;
    m_occupied_cells.insert(nn_idx);
    break;
  }

  case 1:{ // R2
    m_grid[nn_idx] = species::B;
    break;
  }

  case 2:{ // R3
    m_grid[curr_idx] = species::Null;
    m_occupied_cells.erase(it);
    break;
  }

  case 3:{ // R4
    m_grid[curr_idx] = species::Null;
    m_grid[nn_idx] = species::A;
    
    m_occupied_cells.erase(it);
    m_occupied_cells.insert(nn_idx);
    
    break;
  }

  case 4:{ // R5
    m_grid[curr_idx] = species::Null;
    m_grid[nn_idx] = species::B;

    m_occupied_cells.erase(it);
    m_occupied_cells.insert(nn_idx);
    
    break;
  }
  
  }
}
