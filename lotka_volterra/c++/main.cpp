#include <iostream>
#include <thread>
#include <chrono>

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
    sAppName = "Example";
  }

public:
  bool OnUserCreate() override
  {

    srand((unsigned) time(&t));    

    // Called once at the start, so create things here
    world.initialize(n_rows,n_cols);
    render();

    // Initial conditions
    //world.initial_conditions_one_predator();
    //world.initial_conditions_binary_random();
    //world.initial_conditions_line();
    //world.initial_conditions_two_colonies();
    world.initial_conditions_random_colonies(30);
    render();
    
    return true;
  }

  bool OnUserUpdate(float fElapsedTime) override
  {

    if (GetKey(olc::Key::Q).bHeld)
      exit(0);

    if (GetKey(olc::Key::SPACE).bPressed)
      m_is_playing = !m_is_playing;

    if (GetKey(olc::Key::F).bHeld)
      delay = std::max(0,delay-1);

    if (GetKey(olc::Key::S).bHeld)
      delay = std::max(0,delay+1);
    
    if (m_is_playing){
      iterations++;
      world.update();
      world.migrate(); 
      render();
    }

    //std::this_thread::sleep_for(std::chrono::milliseconds(delay));

    return true;
  }
  
private:
  bool m_is_playing = false;
  environment world;
  int delay = 5;
  
  void render(){

    std::vector<cell> world_tmp = world.get_world();
    
    // Render the view
    for (int x = 0; x < n_cols; x++)
      for (int y = 0; y < n_rows; y++)
        Draw(x, y, world_tmp[x + y*n_cols].get_color());
  }
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

