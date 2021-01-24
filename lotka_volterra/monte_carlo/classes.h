#include<algorithm>

class Simulation{
public: // methods
  Simulation(){};
  ~Simulation(){};
  
  void set_grid(int row, int col, int val);
  void initialize(int n_rows, int n_cols);
  void set_reaction_rates(float k1, float k2, float k3, float d1, float d2);
  float step();
  void refresh();
  std::vector<int> get_int_grid();

private: // methods
  void compute_W();
  void compute_W_single_site(int row, int col);
  void compute_reaction();
  void handle_site_change(int row, int col);
  
private: // properties
  enum class species{Null,A,B};

  bool m_reaction_rates_are_set = false;
  
  float m_k1, m_k2, m_k3, m_d1, m_d2;
  int m_n_rows;
  int m_n_cols;

  std::vector<species> m_grid;
  float m_W_sum;
  std::vector<float> m_W;
  std::vector<float> m_W_cumulative;

};

void Simulation::refresh(){
  compute_W();
}

void Simulation::set_grid(int row, int col, int val){
  m_grid[col + row*m_n_cols] = (species)val;
}

void Simulation::initialize(int n_rows, int n_cols){
  m_n_rows       = n_rows;
  m_n_cols       = n_cols;
  m_grid         = std::vector<species>(m_n_rows * m_n_cols, species::Null);
  m_W            = std::vector<float>(m_n_rows * m_n_cols * 20 , 0.0f);
  m_W_cumulative = std::vector<float>(m_n_rows * m_n_cols * 20 , 0.0f);
  
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
  
  // for (int i=0; i<m_n_rows * m_n_cols;i++){
  //   if (i%m_n_cols==0 ||
  //       i%m_n_cols==1)
  //       //i%m_n_cols==2 ||
  //       //i%m_n_cols==3 ||
  //       //i%m_n_cols==4 )
  //     m_grid[i] = species::B;

  //   if (i%m_n_cols==2 ||
  //       i%m_n_cols==3)
  //     m_grid[i] = species::A;
  // }
}

void Simulation::set_reaction_rates(float k1, float k2, float k3, float d1, float d2){
  m_k1 = k1;
  m_k2 = k2;
  m_k3 = k3;
  m_d1 = d1;
  m_d2 = d2;
  m_reaction_rates_are_set = true;
}

float Simulation::step(){
  if (!m_reaction_rates_are_set)
    return 0.0;

  compute_W();
  compute_reaction();
  
  return 1.0f/m_W_sum;
}

void Simulation::compute_W_single_site(int row, int col){
  int i = row;
  int j = col;
  int curr_idx = j + i*m_n_cols;

  // Periodic boundary conditions
  int curr_nn_up;
  if (i-1>=0)
    curr_nn_up = j + (i-1)*m_n_cols;
  else
    curr_nn_up = j + (m_n_rows-1)*m_n_cols;

  int curr_nn_down;
  if (i+1<m_n_rows)
    curr_nn_down = j + (i+1)*m_n_cols;
  else
    curr_nn_down = j + (0)*m_n_cols;

  int curr_nn_left;
  if (j-1>=0)
    curr_nn_left = (j-1) + i*m_n_cols;
  else
    curr_nn_left = (m_n_cols-1) + i*m_n_cols;
      
  int curr_nn_right;
  if (j+1<m_n_cols)
    curr_nn_right = (j+1) + i*m_n_cols;
  else
    curr_nn_right = 0 + i*m_n_cols;

  // Each lattice site has 20 possible reactions (4 nearest neighbors, 5 possible reactions with a nearest neighbor)
  int base_offset = 20 * curr_idx; 
  std::vector<int> nn{curr_nn_up, curr_nn_down, curr_nn_left, curr_nn_right};
  for (int n=0;n<4;++n){
    int nn_idx = nn[n];

    // R1
    if (m_grid[curr_idx]==species::A && m_grid[nn_idx]==species::Null)
      m_W[base_offset + n*5 + 0] = m_k1;
    else 
      m_W[base_offset + n*5 + 0] = 0.0f;

    // R2
    if (m_grid[curr_idx]==species::B && m_grid[nn_idx]==species::A)
      m_W[base_offset + n*5 + 1] = m_k2;
    else 
      m_W[base_offset + n*5 + 1] = 0.0f;

    // R3, only stored n==0 since does not depend on a neighbor
    if (n==0 && m_grid[curr_idx]==species::B)
      m_W[base_offset + n*5 + 2] = m_k3; 
    else 
      m_W[base_offset + n*5 + 2] = 0.0f;

    // R4
    if (m_grid[curr_idx]==species::A && m_grid[nn_idx]==species::Null)
      m_W[base_offset + n*5 + 3] = m_d1;
    else 
      m_W[base_offset + n*5 + 3] = 0.0f;

    // R5
    if (m_grid[curr_idx]==species::B && m_grid[nn_idx]==species::Null)
      m_W[base_offset + n*5 + 4] = m_d2;
    else 
      m_W[base_offset + n*5 + 4] = 0.0f;
  }
}

void Simulation::compute_W(){
  for (int i=0;i<m_n_rows;i++){
    for (int j=0;j<m_n_cols;j++){
      compute_W_single_site(i,j);
    }
  }
}

void Simulation::handle_site_change(int row, int col){

}

void Simulation::compute_reaction(){  
  // Compute the cumulative function
  float W = m_W[0];
  m_W_cumulative[0] = 0.0f;
  
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
    break;
  }

  case 1:{ // R2
    m_grid[nn_idx] = species::B;
    break;
  }

  case 2:{ // R3
    m_grid[reaction_site_idx] = species::Null;
    break;
  }

  case 3:{ // R4
    m_grid[reaction_site_idx] = species::Null;
    m_grid[nn_idx] = species::A;
    break;
  }

  case 4:{ // R5
    m_grid[reaction_site_idx] = species::Null;
    m_grid[nn_idx] = species::B;
    break;
  }

  default:{
    std::cout << "Something has gone wrong (identifying which reaction occurred)" << std::endl;
  }
  }
  
}

std::vector<int> Simulation::get_int_grid(){
  std::vector<int> copy(m_grid.size());
  for (int i=0;i<m_grid.size();i++)
    copy[i] = (int)m_grid[i];
  return copy;
}