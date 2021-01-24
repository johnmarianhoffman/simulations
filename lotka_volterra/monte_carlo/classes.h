#include <algorithm>
#include <chrono>
#include <ratio>
#include <numeric>

class Timer{
public:
  Timer(){};
  ~Timer(){};

  void tic(){
    m_start = std::chrono::high_resolution_clock::now();
  }

  float toc(std::string event_name){
    m_stop = std::chrono::high_resolution_clock::now();
    //std::cout << event_name <<": " << std::chrono::duration<float,std::milli>(m_stop-m_start).count() << "ms" << std::endl;
    return std::chrono::duration<float,std::milli>(m_stop-m_start).count();
  }

private:
  std::chrono::high_resolution_clock::time_point m_start;
  std::chrono::high_resolution_clock::time_point m_stop;

};

class Simulation{
public: // methods
  Simulation(){};
  ~Simulation(){};
  
  void set_grid(int row, int col, int val);
  void initialize(int n_rows, int n_cols);
  void set_reaction_rates(float k1, float k2, float k3, float d1, float d2);
  float step();
  void refresh();
  int get_grid(int row, int col);

  float get_accumulate_time(){return m_time_accumulate;}
  float get_reaction_search_time(){return m_time_reaction_search;}
  float get_execute_reaction_time(){return m_time_execute_reaction;}
  float get_compute_W_time(){return m_time_compute_W;}
  
private: // methods
  void compute_W();
  void compute_W_single_site(int row, int col);
  void compute_reaction();
  void handle_site_change(int row, int col);

  void debug_print_W();
  
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

  float m_time_accumulate       = 0.0f;
  float m_time_reaction_search  = 0.0f;
  float m_time_execute_reaction = 0.0f;
  float m_time_compute_W        = 0.0f;

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
  
   for (int i=0; i<m_n_rows * m_n_cols;i++){
     if (i%m_n_cols==0 ||
         i%m_n_cols==1)
         //i%m_n_cols==2 ||
         //i%m_n_cols==3 ||
         //i%m_n_cols==4 )
       m_grid[i] = species::B;

     if (i%m_n_cols==2 ||
         i%m_n_cols==3)
       m_grid[i] = species::A;
   }

   compute_W();
}

void Simulation::set_reaction_rates(float k1, float k2, float k3, float d1, float d2){
  m_k1 = k1;
  m_k2 = k2;
  m_k3 = k3;
  m_d1 = d1;
  m_d2 = d2;
  m_reaction_rates_are_set = true;
}

void Simulation::debug_print_W(){

  for (size_t i=0; i<m_W.size();++i){
    
    std::cout << std::to_string(m_W[i]) << "  ";

    if (i%5==0)
      std::cout << std::endl;

  }
  

}

float Simulation::step(){
  if (!m_reaction_rates_are_set)
    return 0.0;

  //compute_W();
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
  Timer t;
  t.tic();
  for (int i=0;i<m_n_rows;i++){
    for (int j=0;j<m_n_cols;j++){
      compute_W_single_site(i,j);
    }
  }
  m_time_compute_W = t.toc("Compute W (whole matrix)");
}

void Simulation::handle_site_change(int row, int col){
  // This function addresses updating the W matrix for a single site
  // as well as its nearest neighbors

  compute_W_single_site(row,col);

  // Up
  int nn_row, nn_col;
  if (row-1>=0)
    nn_row = row-1;
  else
    nn_row = m_n_rows - 1;
  nn_col = col;

  compute_W_single_site(nn_row,nn_col);

  // Down
  if (row+1<m_n_rows)
    nn_row = row+1;
  else
    nn_row = 0;
  nn_col = col;

  compute_W_single_site(nn_row,nn_col);

  // Left
  if (col-1>=0)
    nn_col = col-1;
  else
    nn_col = m_n_cols - 1;
  nn_row = row;

  compute_W_single_site(nn_row,nn_col);
  
  // Right
  if (col+1<m_n_cols)
    nn_col = col+1;
  else
    nn_col = 0;
  nn_row = row;

  compute_W_single_site(nn_row,nn_col);
  
}

void Simulation::compute_reaction(){

  Timer t;
  
  // Compute the cumulative function
  t.tic();
  float W = m_W[0];
  m_W_cumulative[0] = 0.0f;
  
  //for (int i=1;i<m_n_rows*m_n_cols*20;i++){
  //  m_W_cumulative[i] = m_W_cumulative[i-1] + m_W[i-1];
  //  W += m_W[i];
  //}
  
  for (int i=1;i<m_n_rows*m_n_cols*20;i++){  
    m_W_cumulative[i] = m_W_cumulative[i-1] + m_W[i-1];  
    W += m_W[i];
  }

  m_W_sum = W;
  m_time_accumulate = t.toc("Compute cumulative matrix");
  
  // Throw our random number
  float reaction = W * (float)rand()/(float)RAND_MAX;

  // Determine which reaction occurs
  t.tic();
  auto it = std::upper_bound(m_W_cumulative.begin(),m_W_cumulative.end(),reaction);
  if (it==m_W_cumulative.end()){
    std::cout << "No more reactions!" << std::endl;
    return;
  }
  m_time_reaction_search = t.toc("Find reaction");

  int reaction_idx = (it - m_W_cumulative.begin()) - 1;
  
  // Translate the reaction idx into what actually happened and where
  t.tic();
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
  m_time_execute_reaction = t.toc("Execute reaction and update");
  
}

int Simulation::get_grid(int row, int col){
  return (int)m_grid[col + row * m_n_cols];
}
