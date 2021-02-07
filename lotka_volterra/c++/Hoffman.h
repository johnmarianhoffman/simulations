#pragma once

#include "LVSimulation.h"

#include <unordered_set>

class Hoffman : public LVSimulation {

  ~Hoffman() override {};

public:
  
  void initialize_model() override;
  void step() override;
  void refresh() override {};

private:

  std::unordered_set<int> m_occupied_cells;
  std::vector<float> m_W;
  std::vector<float> m_W_cumulative;

};