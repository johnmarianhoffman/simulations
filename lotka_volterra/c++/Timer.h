#pragma once

#include <chrono>
#include <iostream>

class Timer{
public:
  Timer(){};
  ~Timer(){};

  void tic(){
    if (!m_started){
      m_started = true;
      m_start = std::chrono::high_resolution_clock::now();
    }
  }

  double toc(std::string event_name = ""){
    m_started = false;
    m_stop = std::chrono::high_resolution_clock::now();
    if (event_name!="")
      std::cout << event_name <<": " << std::chrono::duration<double,std::milli>(m_stop-m_start).count() << "ms" << std::endl;
    return std::chrono::duration<double,std::milli>(m_stop-m_start).count();
  }

private:
  bool m_started = false;
  std::chrono::high_resolution_clock::time_point m_start;
  std::chrono::high_resolution_clock::time_point m_stop;
};
