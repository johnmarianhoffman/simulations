#pragma once

#include "LVSimulation.h"

#include <unordered_set>

class ShengTeuber : public LVSimulation {

  ~ShengTeuber() override {};

public:
  
  void initialize_model() override;
  void step() override;
  void refresh() override {};

private:

  std::unordered_set<int> m_occupied_cells;

};