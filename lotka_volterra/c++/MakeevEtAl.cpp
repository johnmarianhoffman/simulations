#include "LVSimulation.h"
#include "MakeevEtAl.h"

#include <algorithm>

void MakeevEtAl::initialize_model() {
  
  m_W            = std::vector<float>(m_n_rows * m_n_cols * 20 , 0.0f);
  m_W_cumulative = std::vector<float>(m_n_rows * m_n_cols * 20 , 0.0f);
  
  compute_W();

  m_is_initialized = true;
}

void MakeevEtAl::step(){
  if (!m_reaction_rates_are_set)
    std::cout << "ERROR: Attempting to step before reaction rates are set!" << std::endl;

  if (!m_is_initialized)
    std::cout << "ERROR: Attempting to step before simulation is initialized!" << std::endl;

  compute_reaction();
}
void MakeevEtAl::refresh(){
  compute_W();
}

void MakeevEtAl::compute_W(){
  for (int i=0;i<m_n_rows;i++){
    for (int j=0;j<m_n_cols;j++){
      compute_W_single_site(i,j);
    }
  }
}

void MakeevEtAl::compute_W_single_site(int row, int col){
  int i = row;
  int j = col;
  int curr_idx = j + i*m_n_cols;

  // Handle boundary conditions
  int curr_nn_up, curr_nn_down, curr_nn_left, curr_nn_right;
  int nn_row, nn_col;
  BoundaryConditions::result r_up = m_boundary_conditions.get_nn_coordinates(row, col, nn_row, nn_col, m_n_rows, m_n_cols, BoundaryConditions::direction::up);
  curr_nn_up = nn_col + nn_row * m_n_cols;
  BoundaryConditions::result r_down = m_boundary_conditions.get_nn_coordinates(row, col, nn_row, nn_col, m_n_rows, m_n_cols, BoundaryConditions::direction::down);
  curr_nn_down = nn_col + nn_row * m_n_cols;
  BoundaryConditions::result r_left = m_boundary_conditions.get_nn_coordinates(row, col, nn_row, nn_col, m_n_rows, m_n_cols, BoundaryConditions::direction::left);
  curr_nn_left = nn_col + nn_row * m_n_cols;
  BoundaryConditions::result r_right = m_boundary_conditions.get_nn_coordinates(row, col, nn_row, nn_col, m_n_rows, m_n_cols, BoundaryConditions::direction::right);
  curr_nn_right = nn_col + nn_row * m_n_cols;
  
  // Each lattice site has 20 possible reactions (4 nearest neighbors, 5 possible reactions with a nearest neighbor)
  int base_offset = 20 * curr_idx; 
  std::vector<int> nn{curr_nn_up, curr_nn_down, curr_nn_left, curr_nn_right};
  std::vector<BoundaryConditions::result> nn_result{r_up, r_down, r_left, r_right};
  for (int n=0;n<4;++n){
    BoundaryConditions::result r = nn_result[n];
    int nn_idx = nn[n];

    // R1
    if (m_grid[curr_idx]==species::A && m_grid[nn_idx]==species::Null && r==BoundaryConditions::result::valid)
      m_W[base_offset + n*5 + 0] = m_k1;
    else 
      m_W[base_offset + n*5 + 0] = 0.0f;

    // R2
    if (m_grid[curr_idx]==species::B && m_grid[nn_idx]==species::A && r==BoundaryConditions::result::valid)
      m_W[base_offset + n*5 + 1] = m_k2;
    else 
      m_W[base_offset + n*5 + 1] = 0.0f;

    // R3, only stored n==0 since does not depend on a neighbor
    if (n==0 && m_grid[curr_idx]==species::B)
      m_W[base_offset + n*5 + 2] = m_k3; 
    else 
      m_W[base_offset + n*5 + 2] = 0.0f;

    // R4
    if (m_grid[curr_idx]==species::A && m_grid[nn_idx]==species::Null && r==BoundaryConditions::result::valid)
      m_W[base_offset + n*5 + 3] = m_d1;
    else 
      m_W[base_offset + n*5 + 3] = 0.0f;

    // R5
    if (m_grid[curr_idx]==species::B && m_grid[nn_idx]==species::Null && r==BoundaryConditions::result::valid)
      m_W[base_offset + n*5 + 4] = m_d2;
    else 
      m_W[base_offset + n*5 + 4] = 0.0f;
  }
}
void MakeevEtAl::compute_reaction(){

  // Compute the cumulative function

  float W = m_W[0];
  
  //for (int i=1;i<m_n_rows*m_n_cols*20;i++){
  //  m_W_cumulative[i] = m_W_cumulative[i-1] + m_W[i-1];
  //  W += m_W[i];
  //}
  
  for (int i=1;i<m_n_rows*m_n_cols*20;i++){  
    m_W_cumulative[i] = m_W_cumulative[i-1] + m_W[i-1];  
    W += m_W[i];
  }

  m_W_sum = W;
  
  // Throw our random number
  float reaction = W * (float)rand()/(float)RAND_MAX;

  // Determine which reaction occurs
  
  auto it = std::upper_bound(m_W_cumulative.begin(),m_W_cumulative.end(),reaction);
  if (it==m_W_cumulative.end()){
    std::cout << "No more reactions!" << std::endl;
    return;
  }
  

  int reaction_idx = (it - m_W_cumulative.begin()) - 1;
  
  // Translate the reaction idx into what actually happened and where
  
  int reaction_site_idx = reaction_idx/20; // Grid idx of the reaction that occured
  int reaction_nn_idx = (reaction_idx/5)%4; // Produce a number between 0 and 3
  int reaction_specific_idx = reaction_idx%5; // Produce a number between 0 and 4

  // Invoke reaction 
  int curr_row = reaction_site_idx/m_n_cols; // grid coordinates
  int curr_col = reaction_site_idx%m_n_cols; // grid coordinates

  int nn_row;
  int nn_col;
  switch (reaction_nn_idx){ // Indices must match how we laid out our matrix in compute_W
  case 0:{ // up
    nn_col = curr_col;
    if ( curr_row-1 >=0 )
      nn_row = curr_row-1;
    else
      nn_row = m_n_rows-1;
    break;
  }
  case 1: { // down 
    nn_col = curr_col;
    if ( curr_row + 1 < m_n_rows )
      nn_row = curr_row + 1;
    else
      nn_row = 0;
    break;
  }
  case 2:{// left
    nn_row = curr_row;
    if ( curr_col - 1 >= 0 )
      nn_col = curr_col-1;
    else
      nn_col = m_n_cols-1;
    break;
  } 
  case 3:{ // right
    nn_row = curr_row;
    if ( curr_col + 1 < m_n_cols )
      nn_col = curr_col + 1;
    else
      nn_col = 0;
    break;
  }
  default:{
    std::cout << "Something has gone wrong (identifying nearest neighbor)" << std::endl;
  }
  }

  //printf("Reaction R%d occured at site (%d,%d) with NN (%d,%d)\n",
  //        reaction_specific_idx+1,curr_row,curr_col,nn_row,nn_col);

  int nn_idx = nn_col + nn_row * m_n_cols;
  switch (reaction_specific_idx){

  case 0:{ // R1
    m_grid[nn_idx] = species::A;
    handle_site_change(nn_row,nn_col);
    break;
  }

  case 1:{ // R2
    m_grid[nn_idx] = species::B;
    handle_site_change(nn_row,nn_col);
    break;
  }

  case 2:{ // R3
    m_grid[reaction_site_idx] = species::Null;
    handle_site_change(curr_row,curr_col);
    break;
  }

  case 3:{ // R4
    m_grid[reaction_site_idx] = species::Null;
    m_grid[nn_idx] = species::A;
    
    handle_site_change(curr_row,curr_col);
    handle_site_change(nn_row,nn_col);
    break;
  }

  case 4:{ // R5
    m_grid[reaction_site_idx] = species::Null;
    m_grid[nn_idx] = species::B;

    handle_site_change(curr_row,curr_col);
    handle_site_change(nn_row,nn_col);
    break;
  }

  default:{
    std::cout << "Something has gone wrong (identifying which reaction occurred)" << std::endl;
  }
  }
  
}

void MakeevEtAl::handle_site_change(int row, int col){
  // This function addresses updating the W matrix for a single site
  // as well as its nearest neighbors

  compute_W_single_site(row,col);

  int nn_row, nn_col;
  BoundaryConditions::result r_up    = m_boundary_conditions.get_nn_coordinates(row, col, nn_row, nn_col, m_n_rows, m_n_cols, BoundaryConditions::direction::up);
  if (r_up==BoundaryConditions::result::valid)
    compute_W_single_site(nn_row,nn_col);
  
  BoundaryConditions::result r_down  = m_boundary_conditions.get_nn_coordinates(row, col, nn_row, nn_col, m_n_rows, m_n_cols, BoundaryConditions::direction::down);
  if (r_down==BoundaryConditions::result::valid)
    compute_W_single_site(nn_row,nn_col);
  
  BoundaryConditions::result r_left  = m_boundary_conditions.get_nn_coordinates(row, col, nn_row, nn_col, m_n_rows, m_n_cols, BoundaryConditions::direction::left);
  if (r_left==BoundaryConditions::result::valid)
    compute_W_single_site(nn_row,nn_col);
  
  BoundaryConditions::result r_right = m_boundary_conditions.get_nn_coordinates(row, col, nn_row, nn_col, m_n_rows, m_n_cols, BoundaryConditions::direction::right);
  if (r_right==BoundaryConditions::result::valid)
    compute_W_single_site(nn_row,nn_col);
}

void MakeevEtAl::debug_print_W(){
  for (size_t i=0; i<m_W.size();++i){
    std::cout << std::to_string(m_W[i]) << "  ";
    if (i%5==0)
      std::cout << std::endl;
  }
}