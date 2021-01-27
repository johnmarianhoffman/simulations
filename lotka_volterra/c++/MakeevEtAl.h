#pragma once

#include "LVSimulation.h"

class MakeevEtAl : public LVSimulation{
  
  ~MakeevEtAl() override {};

public:
  void initialize_model() override;
  void step() override;
  void refresh() override;

private:
  void compute_W();
  void compute_W_single_site(int row, int col);
  void compute_reaction();
  void handle_site_change(int row, int col);
  void debug_print_W();

private:
  float m_W_sum;
  std::vector<float> m_W;
  std::vector<float> m_W_cumulative;

  
};