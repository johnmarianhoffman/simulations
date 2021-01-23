#include <iostream>
#include <thread>
#include <chrono>
#include <ratio>

//#define __GLFW__
#define OLC_PGE_APPLICATION
#include "olcPixelGameEngine.h"

#include "classes.h"

const int n_rows = 256;
const int n_cols = 256;
int iterations = 0;
time_t t;

// Override base class with your custom functionality
class Example : public olc::PixelGameEngine
{
public:
  Example()
  {
    // Name you application
    sAppName = "Lattice Lotka Volterra";
  }

public:
  bool OnUserCreate() override
  {
    m_simulation.initialize(n_rows,n_cols);
    m_simulation.set_reaction_rates(.5f,.5f,0.0f,0.0f,0.0f);
    
    render(0.0f,0);
    return true;
  }

  bool OnUserUpdate(float fElapsedTime) override
  {
    // Handle any input
    if (GetKey(olc::Key::Q).bHeld)
      exit(0);

    if (GetKey(olc::Key::SPACE).bPressed)
      m_is_playing = !m_is_playing;

    if (GetKey(olc::Key::F).bHeld)
      delay = std::max(0,delay-1);

    if (GetKey(olc::Key::S).bHeld)
      delay = std::max(0,delay+1);

    // Simulation step
    if (m_is_playing){
      ++simulation_step_counter;
      m_simulation.step();
    }

    // Check for render timeout
    float target_frame_time = 0.033f;
    render_timer += fElapsedTime;
    if (render_timer > target_frame_time){
      render(render_timer,simulation_step_counter);
      render_timer = 0.0f;
      simulation_step_counter = 0;
    }
    
    return true;
  }

  void simulate_one_reaction(){
    
  }
  
private:
  void render(float fElapsedTime,int simulation_steps){
    Clear(olc::Pixel(81,81,81));

    std::vector<int> current_state = m_simulation.get_int_grid();
    for (int i=0;i<n_rows;i++){
      for (int j=0;j<n_cols;j++){
        if (current_state[j + i*n_cols] == 1)
          Draw(j,i, olc::Pixel(141,204,136));
        
        if (current_state[j + i*n_cols] == 2)
          Draw(j,i, olc::Pixel(209,141,140));
      }
    }

    DrawString(5,5, "Frame time:       " + std::to_string(fElapsedTime) + "s", olc::WHITE, 1);
    DrawString(5,20,"Simulation steps: " + std::to_string(simulation_steps), olc::WHITE, 1);

  }

private:
  bool m_is_playing = false;
  
  int delay = 5;
  float render_timer = 0.0f;
  int simulation_step_counter = 0;
  
  Simulation m_simulation;
};

int main()
{

  float xScale = 1.0f, yScale = 1.0f;


#if defined(__GLFW__)
  glfwInit();
  glfwGetMonitorContentScale(glfwGetPrimaryMonitor(), &xScale, &yScale);
  glfwTerminate();
#endif

  Example demo;
  if (demo.Construct(n_rows, n_cols, xScale*3, yScale*3))
    demo.Start();
  return 0;

}

