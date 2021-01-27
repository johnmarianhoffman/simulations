#include "LVSimulation.h"

void LVSimulation::initialize_grid(int n_rows, int n_cols){
  m_n_rows       = n_rows;
  m_n_cols       = n_cols;
  m_grid         = std::vector<species>(m_n_rows * m_n_cols, species::Null);
  
  // Random initial conditions
  // 0 -> Empty grid space
  // 1 -> Occupied by A
  // 2 -> Occupied by B
  //for (int i=0; i<m_n_rows * m_n_cols;i++){
  //  m_grid[i] = (species)(rand()%3);
  //  
  //}

  // Only predators
  //for (int i=0; i<m_n_rows * m_n_cols;i++){
  //  if (rand()%3==1)
  //    m_grid[i] = species::B;
  //}

  // Pulse
  //for (int i=0; i<m_n_rows * m_n_cols;i++){
  //  if (i%m_n_cols==0 ||
  //      i%m_n_cols==1)
  //    //i%m_n_cols==2 ||
  //    //i%m_n_cols==3 ||
  //    //i%m_n_cols==4 )
  //    m_grid[i] = species::B;
  //
  //  if (i%m_n_cols==2 ||
  //      i%m_n_cols==3)
  //    m_grid[i] = species::A;
  //}

  // Spiral Wave
  for (int i=0; i<m_n_rows * m_n_cols;i++){
    if (i>(m_n_rows/2*m_n_cols) && (i%m_n_cols == m_n_cols/2 || i%m_n_cols == m_n_cols/2 + 1) )
      m_grid[i] = species::B;
  
    if (i>(m_n_rows/2*m_n_cols) && (i%m_n_cols == m_n_cols/2+2 || i%m_n_cols == m_n_cols/2 + 3) )
      m_grid[i] = species::A;  
  }

}

void LVSimulation::set_reaction_rates(float k1, float k2, float k3, float d1, float d2){
  m_k1 = k1;
  m_k2 = k2;
  m_k3 = k3;
  m_d1 = d1;
  m_d2 = d2;
  m_reaction_rates_are_set = true;
}

void LVSimulation::set_boundary_conditions(BoundaryConditions::type configuration){
  if (m_is_initialized)
    std::cout << "WARNING: Boundary conditions are being set *after* model has already been initialized.  This can result in unexpected behavior." << std::endl;
  m_boundary_conditions.set_boundary_conditions(configuration);
  m_boundary_conditions_are_set = true;
}

void LVSimulation::set_grid(int row, int col, species s){
  m_grid[col + row*m_n_cols] = s;
}

LVSimulation::species LVSimulation::get_grid(int row, int col){
  return m_grid[col + row * m_n_cols];
}