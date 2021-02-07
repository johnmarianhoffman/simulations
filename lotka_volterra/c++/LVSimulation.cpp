#include "LVSimulation.h"
#include "svpng.h"

#include <filesystem>
#include <fstream>
#include <cstring>
#include <cmath>

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

void LVSimulation::record_data(std::string output_directory){

  /* Ensure output directory exists */
  std::filesystem::path output_path(output_directory);
  if (!std::filesystem::exists(output_path))
    std::filesystem::create_directory(output_path);
  
  /* Record the following: */
  // - PNG image of the current grid
  // - Current MC time step
  // - N rows
  // - N cols
  // - Boundary Conditions
  // - Reaction rates
  // - Predator population (Species B)
  // - Prey population  (Species A)

  // Build the data we're going to save
  int curr_whole_mc_time_step = floor(m_time_mc_total);

  uint32_t green = 0xff8dcc8d;
  uint32_t red   = 0xff8d8dcc;
  uint32_t gray  = 0xff0a0a0a;
  std::vector<uint32_t> color_grid(m_n_rows*m_n_cols);

  int pop_A = 0;
  int pop_B = 0;
  for (int i=0;i< m_n_rows;i++){

    for (int j=0;j<m_n_cols;j++){
      int curr_idx = j + i*m_n_cols;
      if (m_grid[curr_idx]==species::A){
        color_grid[curr_idx] = green;
        ++pop_A;
      }
      else if (m_grid[curr_idx]==species::B){
        color_grid[curr_idx] = red;
        ++pop_B;
      }
      else if (m_grid[curr_idx]==species::Null){
        color_grid[curr_idx] = gray;
      }
    }
  }

  // Save the png image
  char pngname[256] = {0};
  sprintf(pngname,"%05d.png",curr_whole_mc_time_step);
  
  std::filesystem::path png_path = output_path / pngname;
  FILE * fp = fopen(png_path.c_str(),"wb");
  svpng(fp,m_n_rows,m_n_cols,(unsigned char *)&color_grid[0],1);
  fclose(fp);

  // Write current row of data to CSV file
  std::filesystem::path csv_path = output_path / "lv_kmc_data.csv";
  if (!std::filesystem::exists(csv_path)){
    std::ofstream output_stream(csv_path.c_str(),std::ios::app);
    output_stream << "time,n_rows,n_cols,boundary_conditions,k1,k2,k3,d1,d2,population_prey,population_predator" << std::endl;
  }
  else{
    std::ofstream output_stream(csv_path.c_str(),std::ios::app);

    char datastring[4096] = {0};
    sprintf(datastring,"%.2f, %d, %d, %s, %.05f, %.05f, %.05f, %.05f, %.05f, %d, %d",
            m_time_mc_total, m_n_rows, m_n_cols, m_boundary_conditions.get_boundary_condition_string().c_str(), m_k1, m_k2, m_k3, m_d1, m_d2, pop_A, pop_B);
    output_stream << datastring << std::endl;
    std::cout << datastring << std::endl;
  }
  
}