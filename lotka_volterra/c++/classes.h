const float a = 1.1; // prey growth
const float b = 0.4; // prey death
const float c = 0.1; // pred growth
const float d = 0.4; // pred death

float carrying_capacity_prey = 30.0f;
float carrying_capacity_pred = 11.0f;

float delta_t = 0.01;

class cell {

  float pop_prey = 10.0f;
  float pop_pred = 10.0f;

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

  olc::Pixel get_color(){
    return olc::Pixel(255*std::min(pop_pred / carrying_capacity_pred,1.0f), 0, 255* std::min(pop_prey / carrying_capacity_prey,1.0f ));
  }
};
