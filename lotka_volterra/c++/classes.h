const float a = 1.001; // prey growth
const float b = 0.4; // prey death
const float c = 0.1; // pred growth
const float d = 0.5; // pred death

const float carrying_capacity_prey = 30.0f;
const float carrying_capacity_pred = 5.0f;

float delta_t = 0.01;

class cell {

  float pop_prey = 0.0f;
  float pop_pred = 0.0f;

public:

  void update(){
    
    float delta_prey = a*pop_prey          - b*pop_prey*pop_pred;
    float delta_pred = c*pop_prey*pop_pred - d*pop_pred;
    
    pop_prey += delta_prey * delta_t;
    pop_pred += delta_pred * delta_t;

    pop_prey = std::max(0.0f, pop_prey);
    pop_pred = std::max(0.0f, pop_pred);
    
  }

  float migrate_prey(){
    float migrating = std::max(0.0f, pop_prey - carrying_capacity_prey);
    return migrating;
  }

  float migrate_pred(){
    float migrating = std::max(0.0f, pop_pred - carrying_capacity_pred);
    return migrating;
  }

  float get_prey(){return pop_prey;}
  float get_pred(){return pop_pred;}

  void adjust_populations(float pred_adjust, float prey_adjust){
    // Add incoming values, truncate anything above carrying capacity
    pop_prey = std::min(pop_prey + prey_adjust, carrying_capacity_prey);
    pop_pred = std::min(pop_pred + pred_adjust, carrying_capacity_pred);
  }

  void set_predators(float pop){pop_pred = pop;}
  void set_prey(float pop){pop_prey = pop;}

  olc::Pixel get_color(){
    return olc::Pixel(255*std::min(pop_pred / carrying_capacity_pred,1.0f), 0, 255* std::min(pop_prey / carrying_capacity_prey, 1.0f ));
  }
};

class environment {
private:
  int m_n_rows;
  int m_n_cols;

  std::vector<cell> m_world;
  
public:
  void initialize(int n_rows, int n_cols){
    m_n_rows = n_rows;
    m_n_cols = n_cols;

    m_world.resize(m_n_rows*m_n_cols);
  }

  std::vector<cell> get_world(){return m_world;}

  void initial_conditions_one_predator(){
    for (int i=0;i<m_n_rows;i++){
      for (int j=0;j<m_n_cols;j++){
        m_world[j+i*m_n_cols].set_prey(10.0f);

        if (i==m_n_rows/2 && j==m_n_cols/2)
          m_world[j+i*m_n_cols].set_predators(10.0f);
      }
    }    
  }

  void initial_conditions_line(){
    for (int i=0;i<m_n_rows;i++){
      for (int j=0;j<m_n_cols;j++){
        m_world[j+i*m_n_cols].set_prey(10.0f);

        if (i==0)
          m_world[j+i*m_n_cols].set_predators(10.0f);
      }
    }
  }

  void initial_conditions_two_colonies(){
    for (int i=0;i<m_n_rows;i++){
      for (int j=0;j<m_n_cols;j++){
        m_world[j+i*m_n_cols].set_prey(10.0f);

        if (i==0 && j==0)
          m_world[j+i*m_n_cols].set_predators(10.0f);

        if (i==m_n_rows/2 && j==m_n_cols/2)
          m_world[j+i*m_n_cols].set_predators(1.0f);
      }
    }
  }

  void initial_conditions_random_colonies(int n_colonies){
    for (int i=0;i<m_n_rows;i++){
      for (int j=0;j<m_n_cols;j++){
        m_world[j+i*m_n_cols].set_prey(10.0f);
      }
    }

    for (int ii=0;ii<n_colonies;ii++){
      int row = rand()%m_n_rows;
      int col = rand()%m_n_cols;
      
      m_world[col+row*m_n_cols].set_prey((float)(rand()%30));
      m_world[col+row*m_n_cols].set_predators((float)(rand()%20));
    }
  }

  void initial_conditions_binary_random(){
    for (int i=0;i<m_n_rows;i++){
      for (int j=0;j<m_n_cols;j++){

        switch (rand()%2){
        case 0:{
          m_world[j+i*m_n_cols].set_prey(10.0f);
          break;
        }
        case 1:{
          m_world[j+i*m_n_cols].set_predators(10.0f);
          m_world[j+i*m_n_cols].set_prey(10.0f);
          break;
        }
        }
      }
    }    
  }

  void update(){
    for (int i=0;i<m_n_rows;i++){
      for (int j=0;j<m_n_cols;j++){
        m_world[j+i*m_n_cols].update();
      }
    }
  }

  void migrate(){

    // Calculate how many incoming pred/prey to each cell
    std::vector<float> migrating_pred(m_n_rows*m_n_cols, 0.0f);
    std::vector<float> migrating_prey(m_n_rows*m_n_cols, 0.0f);

    for (int i=0;i<m_n_rows;i++){
      for (int j=0;j<m_n_cols;j++){

        for (int ii=-1;ii<=1;ii++){
          for (int jj=-1;jj<=1;jj++){

            int curr_row = i+ii;
            int curr_col = j+jj;

            if (curr_row < 0 || curr_row == m_n_rows)
              continue;

            if (curr_col < 0 || curr_col == m_n_cols)
              continue;

            migrating_prey[j + i*m_n_cols] += m_world[curr_col + curr_row*m_n_cols].migrate_prey();
            migrating_pred[j + i*m_n_cols] += m_world[curr_col + curr_row*m_n_cols].migrate_pred();
            
          }
        }

        migrating_prey[j + i*m_n_cols] /= 8.0f;
        migrating_pred[j + i*m_n_cols] /= 8.0f;
        
      }
    }

    // Adjust current totals in the world (add incoming, truncate anything over carrying capacity)
    for (int i=0;i<m_n_rows;i++){
      for (int j=0;j<m_n_cols;j++){
        float incoming_pred = migrating_pred[j + i*m_n_cols];
        float incoming_prey = migrating_prey[j + i*m_n_cols];
        m_world[j + i*m_n_cols].adjust_populations(incoming_pred,incoming_prey);
      }
    }

  }
};