#include <iostream>
#include <thread>
#include <chrono>
#include <ratio>

//#define __GLFW__
#define OLC_PGE_APPLICATION
#include "olcPixelGameEngine.h"

#include "classes.h"

int n_rows = 1024;
int n_cols = 1024;
int iterations = 0;
time_t t;

float scale = 1.0;

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
    m_simulation.set_reaction_rates(0.5f, 0.5f, 0.06f, 0.0f, 0.0f);
    m_simulation.initialize(n_rows,n_cols);
    m_simulation.set_boundary_conditions(BoundaryConditions::type::non_periodic);
    render(0.0f,0);
    return true;
  }

  bool OnUserUpdate(float fElapsedTime) override
  {
    // Handle any input
    if (GetKey(olc::Key::Q).bHeld)
      return false;

    if (GetKey(olc::Key::SPACE).bPressed)
      m_is_playing = !m_is_playing;

    if (GetKey(olc::Key::S).bPressed)
      m_simulation.step();      

    if (GetKey(olc::Key::K1).bPressed)
      currently_drawing = 1;

    if (GetKey(olc::Key::K2).bPressed)
      currently_drawing = 2;

    if (GetKey(olc::Key::K3).bPressed)
      currently_drawing = 0;      

    if (GetMouse(0).bHeld){
      olc::vi2d mouse_pos = GetMousePos();
      m_simulation.set_grid(mouse_pos.y,mouse_pos.x,currently_drawing);
    }
    
    if (GetMouse(0).bReleased)
      m_simulation.refresh();

    // Simulation step
    if (m_is_playing){
      ++simulation_step_counter;
      simulation_time_step = m_simulation.step();
    }
    else{
      //m_simulation.refresh();
    }

    // Check for render timeout
    float target_frame_time = 0.030f;
    //float target_frame_time = 1.0f;
    render_timer += fElapsedTime;
    if (render_timer > target_frame_time){
    //if (simulation_step_counter > 0){
      render(render_timer,simulation_step_counter);
      render_timer = 0.0f;
      simulation_step_counter = 0;
    }

    // Render HUD (want to call every single frame)
    DrawStringDecal(olc::vf2d(0,0),  "Time (accumulate):       " + std::to_string(m_simulation.get_accumulate_time()), olc::WHITE, olc::vf2d(scale,scale));
    DrawStringDecal(olc::vf2d(0,7),  "Time (reaction search):  " + std::to_string(m_simulation.get_reaction_search_time()), olc::WHITE, olc::vf2d(scale,scale));
    DrawStringDecal(olc::vf2d(0,14), "Time (execute reaction): " + std::to_string(m_simulation.get_execute_reaction_time()), olc::WHITE, olc::vf2d(scale,scale));
    DrawStringDecal(olc::vf2d(0,21), "Time (compute W):        " + std::to_string(m_simulation.get_compute_W_time()), olc::WHITE, olc::vf2d(scale,scale));
    
    return true;
  }

  void simulate_one_reaction(){
    
  }
  
private:
  void render(float fElapsedTime,int simulation_steps){
    ///Clear(olc::Pixel(10,10,10));
    for (int i=0;i<n_rows;i++){
      for (int j=0;j<n_cols;j++){
        if (m_simulation.get_grid(i,j) == 1)
          Draw(j,i, olc::Pixel(141,204,136));
        else if (m_simulation.get_grid(i,j) == 2)
          Draw(j,i, olc::Pixel(209,141,140));
        else 
          Draw(j,i, olc::Pixel(10,10,10));
      }
    }
  }

private:
  bool m_is_playing = false;
  
  float render_timer = 0.0f;
  int simulation_step_counter = 0;
  float simulation_time_step = 0.0f;
  int currently_drawing = 1;
  
  Simulation m_simulation;
};

int main(int argc, char ** argv)
{

  if (argc>1){
    n_rows = std::atoi(argv[1]);
    n_cols = std::atoi(argv[2]);
    scale = std::atof(argv[3]);
  }
  
  Example demo;
  bool vsync = false;
  if (demo.Construct(n_cols, n_rows, 20, 20, false, vsync, false))
    demo.Start();
  return 0;

}

