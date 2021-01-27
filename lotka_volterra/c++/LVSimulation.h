#pragma once

#include "BoundaryConditions.h"
#include "Timer.h"

#include <vector>

class LVSimulation{

public:
  LVSimulation(){};
  virtual ~LVSimulation() = default;

  enum class species{Null,A,B};

  void initialize_grid(int n_rows, int n_cols);
  void set_reaction_rates(float k1, float k2, float k3, float d1, float d2);
  void set_boundary_conditions(BoundaryConditions::type configuration);

  void set_grid(int row, int col, species s);
  species  get_grid(int row, int col);
  
  virtual void initialize_model() = 0;
  virtual void step() = 0;
  virtual void refresh() = 0;

protected:
  
  BoundaryConditions m_boundary_conditions;

  bool m_reaction_rates_are_set = false;
  bool m_is_initialized = false;
  bool m_boundary_conditions_are_set = false;

  float m_k1, m_k2, m_k3, m_d1, m_d2;
  int m_n_rows;
  int m_n_cols;

  std::vector<species> m_grid;

  float m_time_compute_step = 0.0f;
  float m_time_mc_step = 0.0f;
  
};
