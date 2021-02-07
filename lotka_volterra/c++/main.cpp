#include <iostream>
#include <thread>
#include <chrono>
#include <ratio>

//#define __GLFW__
#define OLC_PGE_APPLICATION
#include "olcPixelGameEngine.h"

#include "Timer.h"

#include "LVSimulation.h"
#include "MakeevEtAl.h"
#include "ShengTeuber.h"
#include "Hoffman.h"

bool headless = false;
int n_rows = 1024;
int n_cols = 1024;
int iterations = 0;
int max_time = 1000000000;
int measurement_interval = 1;
std::string output_directory = "./";
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
    m_simulation = new MakeevEtAl();
    //m_simulation = new ShengTeuber();
    //m_simulation = new Hoffman();
    
    m_simulation->set_boundary_conditions(BoundaryConditions::type::non_periodic);
    
    m_simulation->set_reaction_rates(0.5f, 0.5f, 0.04f, 0.0f, 0.0f); // Makeev et al 

    //m_simulation->set_reaction_rates(1.0f, 1.0f, 0.025f, 0.0f, 0.0f); // Sheng and Teuber
    m_simulation->initialize_grid(n_rows,n_cols);
    m_simulation->initialize_model();
    
    render(0.0f,0);
    return true;
  }

  bool OnUserUpdate(float fElapsedTime) override
  {
    // Handle any input
    if (GetKey(olc::Key::Q).bHeld){
      delete m_simulation;
      return false;
    }

    if (GetKey(olc::Key::SPACE).bPressed)
      m_is_playing = !m_is_playing;

    if (GetKey(olc::Key::S).bPressed)
      m_simulation->step();      

    if (GetKey(olc::Key::K1).bPressed)
      currently_drawing = 1;

    if (GetKey(olc::Key::K2).bPressed)
      currently_drawing = 2;

    if (GetKey(olc::Key::K3).bPressed)
      currently_drawing = 0;      

    if (GetMouse(0).bHeld){
      olc::vi2d mouse_pos = GetMousePos();
      m_simulation->set_grid(mouse_pos.y,mouse_pos.x,(LVSimulation::species)currently_drawing);
    }
    
    if (GetMouse(0).bReleased)
      m_simulation->refresh();

    // Simulation step
    if (m_is_playing){
      ++simulation_step_counter;
      m_simulation->step();

      if (floor(m_simulation->m_time_mc_total) != m_curr_MC_step){
        m_simulation->record_data(output_directory);
      }
    }
    else{
      //m_simulation.refresh();
    }

    // Check for render timeout
    float target_frame_time = 0.030f;
    //float target_frame_time = 1.0f;
    render_timer += fElapsedTime;
    if (render_timer > target_frame_time){
      render(render_timer,simulation_step_counter);
      render_timer = 0.0f;
      simulation_step_counter = 0;
    }

    // Render HUD (want to call every single frame)
    DrawStringDecal(olc::vf2d(0,0),  "Time (compute step):  " + std::to_string(m_simulation->m_time_compute_step), olc::WHITE, olc::vf2d(scale,scale));
    DrawStringDecal(olc::vf2d(0,7),  "Time (MC step):       " + std::to_string(m_simulation->m_time_mc_step), olc::WHITE, olc::vf2d(scale,scale));
    DrawStringDecal(olc::vf2d(0,14), "Time (MC total):      " + std::to_string(m_simulation->m_time_mc_total), olc::WHITE, olc::vf2d(scale,scale));
    
    return true;
  }

  void simulate_one_reaction(){
    
  }
  
private:
  void render(float fElapsedTime,int simulation_steps){
    ///Clear(olc::Pixel(10,10,10));
    for (int i=0;i<n_rows;i++){
      for (int j=0;j<n_cols;j++){
        if (m_simulation->get_grid(i,j) == LVSimulation::species::A)
          Draw(j,i, olc::Pixel(141,204,136));
        else if (m_simulation->get_grid(i,j) == LVSimulation::species::B)
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

  int m_curr_MC_step = 0;
  
  Timer m_timer;
  
  LVSimulation * __restrict__ m_simulation;
  //LVSimulation * m_simulation;
};

void usage(){

  printf(
         "Usage: LV_KMC [OPTIONS]\n"
         "\n"
         " Run a kinetic Monte Carlo Lattice Lotka-Volterra simulation.\n"
         "\n"
         " Options: \n"
         " -r, --rows [int]          Set the number of rows in the lattice. \n"
         " -c, --cols [int]          Set the number of columns in the lattice. \n"
         " -t, --max_time [int]      Set the maximum number of Monte Carlo steps to simulate. \n"
         " -o, --output [directory]  Set the output directory where data will be saved.  If not set, \n"
         "                             the current directory is used.\n"
         " --headless                Run the simulation in without olc Pixel Game engine. \n"
         "                             Data will be saved to the current directory, or output (see below). \n"
         " --help                    Show this message\n"
         "\n"
         " This software is Copyright (c), John Hoffman 2020. All rights reserved. \n"
         );

}

int main(int argc, char ** argv)
{

  // Parse command line args
  for (int i=1;i<argc;i++){

    std::string curr_arg = argv[i];
    if (curr_arg == "--rows" || curr_arg == "-r"){
      n_rows = std::atoi(argv[++i]);
    }
    
    else if (curr_arg == "--cols" || curr_arg == "-c"){
      n_cols = std::atoi(argv[++i]);
    }

    else if (curr_arg == "--headless"){
      headless = true;
    }

    else if (curr_arg == "--max-time" || curr_arg == "-t"){
      max_time = std::atoi(argv[++i]);
    }

    else if (curr_arg == "--output" || curr_arg == "-o"){
      output_directory = argv[++i];
    }

    else if (curr_arg == "--help"){
      usage();
      exit(1);
    }
    
    else {
      usage();
      exit(1);
    }
  }


  /* Run the simulation without OLC Pixel Game Engine (no visualization) */
  if (headless){
    
    LVSimulation * __restrict__ m_simulation;
    m_simulation = new MakeevEtAl();
    m_simulation->set_boundary_conditions(BoundaryConditions::type::non_periodic);
    m_simulation->set_reaction_rates(0.5f, 0.5f, 0.04f, 0.0f, 0.0f); // Makeev et al     
    m_simulation->initialize_grid(n_rows,n_cols);
    m_simulation->initialize_model();

    // Run
    int curr_mc_step = 0;
    m_simulation->record_data(output_directory);
    while(floor(m_simulation->m_time_mc_total) <= max_time){
      
      m_simulation->step();

      // Whole time step has increased by 1, record data
      if (floor(m_simulation->m_time_mc_total) != curr_mc_step){
        curr_mc_step = floor(m_simulation->m_time_mc_total);
        m_simulation->record_data(output_directory);
      }
    }
    
  }

  /* Run the simulation with OLC Pixel Game Engine */
  else{
    Example demo;
    bool vsync = false;
    if (demo.Construct(n_cols, n_rows, 20, 20, false, vsync, false))
      demo.Start();
  }
  return 0;

}

