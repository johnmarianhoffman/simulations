#include <iostream>
#include <thread>
#include <chrono>

//#define __GLFW__
#define OLC_PGE_APPLICATION
#include "olcPixelGameEngine.h"

#include "classes.h"

const int n_rows = 64;
const int n_cols = 64;

int iterations = 0;

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
    // Called once at the start, so create things here
    world.resize(n_rows*n_cols);
    render();

    // Initial conditions
    
    return true;
  }

  bool OnUserUpdate(float fElapsedTime) override
  {

    if (GetKey(olc::Key::Q).bHeld)
      exit(0);
    
    iterations++;
    
    // Update the simulation
    for (int x = 0; x < n_cols; x++)
      for (int y = 0; y < n_rows; y++){
        if (x==0 && y==0){
          std::cout << world[x + y*n_cols].get_prey() << "," << world[x + y*n_cols].get_pred() << std::endl;
          //std::cout << "Iteration " << iterations << std::endl;
          //std::cout << "Prey:     "  << world[x + y*n_cols].get_prey() << std::endl;
          //std::cout << "Predator: "  << world[x + y*n_cols].get_pred() << std::endl;
          //std::cout << "================" << std::endl;

          if (world[x + y*n_cols].get_prey()<0.001f &&
              world[x + y*n_cols].get_pred()<0.001f)
            exit(0);
        }

        world[x + y*n_cols].update();

      }

    render();

    //std::this_thread::sleep_for(std::chrono::milliseconds(33));

    return true;
  }
  
private:
  std::vector<cell> world;

  void render(){
    // Render the view
    for (int x = 0; x < n_cols; x++)
      for (int y = 0; y < n_rows; y++)
        Draw(x, y, world[x + y*n_cols].get_color());
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

