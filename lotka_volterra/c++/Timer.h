#pragma once

#include <chrono>
#include <iostream>

class Timer{
public:
  Timer(){};
  ~Timer(){};

  void tic(){
    m_start = std::chrono::high_resolution_clock::now();
  }

  float toc(std::string event_name = ""){
    m_stop = std::chrono::high_resolution_clock::now();
    if (event_name!="")
      std::cout << event_name <<": " << std::chrono::duration<float,std::milli>(m_stop-m_start).count() << "ms" << std::endl;
    return std::chrono::duration<float,std::milli>(m_stop-m_start).count();
  }

private:
  std::chrono::high_resolution_clock::time_point m_start;
  std::chrono::high_resolution_clock::time_point m_stop;
};
