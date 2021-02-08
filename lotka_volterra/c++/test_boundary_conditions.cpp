#include <iostream>
#include <vector>
#include "BoundaryConditions.h"

bool check_test_results(std::vector<BoundaryConditions::result> expected_result,
                        std::vector<int> expected_nn_row,
                        std::vector<int> expected_nn_col,
                        std::vector<BoundaryConditions::result> test_result_results,
                        std::vector<int> test_result_nn_row,
                        std::vector<int> test_result_nn_col);

int main(int argc, char ** argv){

  const int n_rows = 20;
  const int n_cols = 20;
  
  BoundaryConditions test;

  const std::vector<BoundaryConditions::direction> directions{BoundaryConditions::direction::up,
                                                              BoundaryConditions::direction::down,
                                                              BoundaryConditions::direction::left,
                                                              BoundaryConditions::direction::right};
  std::vector<BoundaryConditions::result> results(4);
  
  // Periodic tests
  // Test (0,0)
  {
    std::vector<BoundaryConditions::result> expected_result{BoundaryConditions::result::valid,
                                                            BoundaryConditions::result::valid,
                                                            BoundaryConditions::result::valid,
                                                            BoundaryConditions::result::valid};
    std::vector<int> expected_nn_row{19,1,0,0};
    std::vector<int> expected_nn_col{0,0,19,1};

    std::vector<BoundaryConditions::result> test_result_results(4);
    std::vector<int> test_result_nn_row(4);
    std::vector<int> test_result_nn_col(4);
  
    test.set_boundary_conditions(BoundaryConditions::type::periodic);
    for (size_t i=0;i<directions.size();i++){
      test_result_results[i] = test.get_nn_coordinates(0 ,0 ,test_result_nn_row[i], test_result_nn_col[i], n_rows, n_cols, directions[i]);
    }

    bool test_passes = check_test_results(expected_result,expected_nn_row,expected_nn_col,
                                          test_result_results, test_result_nn_row, test_result_nn_col);

    if (test_passes)
      std::cout << "Boundary conditions (periodic, (0,0)): \033[1;32mPASS\033[0m" << std::endl;
    else
      std::cout << "Boundary conditions (periodic, (0,0)): \033[1;31mFAIL\033[0m" << std::endl;

  }

  // Periodic tests
  // Test (19,19)
  {
    std::vector<BoundaryConditions::result> expected_result{BoundaryConditions::result::valid,
                                                            BoundaryConditions::result::valid,
                                                            BoundaryConditions::result::valid,
                                                            BoundaryConditions::result::valid};
    std::vector<int> expected_nn_row{18,0,19,19};
    std::vector<int> expected_nn_col{19,19,18,0};

    std::vector<BoundaryConditions::result> test_result_results(4);
    std::vector<int> test_result_nn_row(4);
    std::vector<int> test_result_nn_col(4);
  
    test.set_boundary_conditions(BoundaryConditions::type::periodic);
    for (size_t i=0;i<directions.size();i++){
      test_result_results[i] = test.get_nn_coordinates(19,19 ,test_result_nn_row[i], test_result_nn_col[i], n_rows, n_cols, directions[i]);
    }

    bool test_passes = check_test_results(expected_result,expected_nn_row,expected_nn_col,
                                          test_result_results, test_result_nn_row, test_result_nn_col);

    if (test_passes)
      std::cout << "Boundary conditions (periodic, (19,19)): \033[1;32mPASS\033[0m" << std::endl;
    else
      std::cout << "Boundary conditions (periodic, (19,19)): \033[1;31mFAIL\033[0m" << std::endl;

  }

  // Non periodic tests
  // Test (0,0)
  {
    std::vector<BoundaryConditions::result> expected_result{BoundaryConditions::result::invalid,
                                                            BoundaryConditions::result::valid,
                                                            BoundaryConditions::result::invalid,
                                                            BoundaryConditions::result::valid};
    std::vector<int> expected_nn_row{-1,1,0,0};
    std::vector<int> expected_nn_col{0,0,-1,1};

    std::vector<BoundaryConditions::result> test_result_results(4);
    std::vector<int> test_result_nn_row(4);
    std::vector<int> test_result_nn_col(4);
  
    test.set_boundary_conditions(BoundaryConditions::type::non_periodic);
    for (size_t i=0;i<directions.size();i++){
      test_result_results[i] = test.get_nn_coordinates(0,0 ,test_result_nn_row[i], test_result_nn_col[i], n_rows, n_cols, directions[i]);
    }

    bool test_passes = check_test_results(expected_result,expected_nn_row,expected_nn_col,
                                          test_result_results, test_result_nn_row, test_result_nn_col);

    if (test_passes)
      std::cout << "Boundary conditions (non-periodic, (0,0)): \033[1;32mPASS\033[0m" << std::endl;
    else
      std::cout << "Boundary conditions (non-periodic, (0,0)): \033[1;31mFAIL\033[0m" << std::endl;

  }

  // Test (19,19)
  {
    std::vector<BoundaryConditions::result> expected_result{BoundaryConditions::result::valid,
                                                            BoundaryConditions::result::invalid,
                                                            BoundaryConditions::result::valid,
                                                            BoundaryConditions::result::invalid};
    std::vector<int> expected_nn_row{18,20,19,19};
    std::vector<int> expected_nn_col{19,19,18,20};
    
    std::vector<BoundaryConditions::result> test_result_results(4);
    std::vector<int> test_result_nn_row(4);
    std::vector<int> test_result_nn_col(4);
  
    test.set_boundary_conditions(BoundaryConditions::type::non_periodic);
    for (size_t i=0;i<directions.size();i++){
      test_result_results[i] = test.get_nn_coordinates(19,19 ,test_result_nn_row[i], test_result_nn_col[i], n_rows, n_cols, directions[i]);
    }

    bool test_passes = check_test_results(expected_result,expected_nn_row,expected_nn_col,
                                          test_result_results, test_result_nn_row, test_result_nn_col);

    if (test_passes)
      std::cout << "Boundary conditions (non-periodic, (19,19)): \033[1;32mPASS\033[0m" << std::endl;
    else
      std::cout << "Boundary conditions (non-periodic, (19,19)): \033[1;31mFAIL\033[0m" << std::endl;

  }

  // Horizontal periodic tests
  // Test (0,0)
  {
    std::vector<BoundaryConditions::result> expected_result{BoundaryConditions::result::invalid,
                                                            BoundaryConditions::result::valid,
                                                            BoundaryConditions::result::valid,
                                                            BoundaryConditions::result::valid};
    std::vector<int> expected_nn_row{-1,1,0,0};
    std::vector<int> expected_nn_col{0,0,19,1};

    std::vector<BoundaryConditions::result> test_result_results(4);
    std::vector<int> test_result_nn_row(4);
    std::vector<int> test_result_nn_col(4);
  
    test.set_boundary_conditions(BoundaryConditions::type::periodic_horizontal);
    for (size_t i=0;i<directions.size();i++){
      test_result_results[i] = test.get_nn_coordinates(0,0 ,test_result_nn_row[i], test_result_nn_col[i], n_rows, n_cols, directions[i]);
    }

    bool test_passes = check_test_results(expected_result,expected_nn_row,expected_nn_col,
                                          test_result_results, test_result_nn_row, test_result_nn_col);

    if (test_passes)
      std::cout << "Boundary conditions (periodic-horizontal, (0,0)): \033[1;32mPASS\033[0m" << std::endl;
    else
      std::cout << "Boundary conditions (periodic-horizontal, (0,0)): \033[1;31mFAIL\033[0m" << std::endl;

  }

  // Test (19,19)
  {
    std::vector<BoundaryConditions::result> expected_result{BoundaryConditions::result::valid,
                                                            BoundaryConditions::result::invalid,
                                                            BoundaryConditions::result::valid,
                                                            BoundaryConditions::result::valid};
    std::vector<int> expected_nn_row{18,20,19,19};
    std::vector<int> expected_nn_col{19,19,18,0};

    std::vector<BoundaryConditions::result> test_result_results(4);
    std::vector<int> test_result_nn_row(4);
    std::vector<int> test_result_nn_col(4);
  
    test.set_boundary_conditions(BoundaryConditions::type::periodic_horizontal);
    for (size_t i=0;i<directions.size();i++){
      test_result_results[i] = test.get_nn_coordinates(19,19 ,test_result_nn_row[i], test_result_nn_col[i], n_rows, n_cols, directions[i]);
    }

    bool test_passes = check_test_results(expected_result,expected_nn_row,expected_nn_col,
                                          test_result_results, test_result_nn_row, test_result_nn_col);

    if (test_passes)
      std::cout << "Boundary conditions (periodic-horizontal, (19,19)): \033[1;32mPASS\033[0m" << std::endl;
    else
      std::cout << "Boundary conditions (periodic-horizontal, (19,19)): \033[1;31mFAIL\033[0m" << std::endl;

  }

  // Vertical periodic tests
  // Test (0,0)
  {
    std::vector<BoundaryConditions::result> expected_result{BoundaryConditions::result::valid,
                                                            BoundaryConditions::result::valid,
                                                            BoundaryConditions::result::invalid,
                                                            BoundaryConditions::result::valid};
    std::vector<int> expected_nn_row{19,1,0,0};
    std::vector<int> expected_nn_col{0,0,-1,1};

    std::vector<BoundaryConditions::result> test_result_results(4);
    std::vector<int> test_result_nn_row(4);
    std::vector<int> test_result_nn_col(4);
  
    test.set_boundary_conditions(BoundaryConditions::type::periodic_vertical);
    for (size_t i=0;i<directions.size();i++){
      test_result_results[i] = test.get_nn_coordinates(0,0 ,test_result_nn_row[i], test_result_nn_col[i], n_rows, n_cols, directions[i]);
    }

    bool test_passes = check_test_results(expected_result,expected_nn_row,expected_nn_col,
                                          test_result_results, test_result_nn_row, test_result_nn_col);

    if (test_passes)
      std::cout << "Boundary conditions (periodic-vertical, (0,0)): \033[1;32mPASS\033[0m" << std::endl;
    else
      std::cout << "Boundary conditions (periodic-vertical, (0,0)): \033[1;31mFAIL\033[0m" << std::endl;

  }

  // Test (19,19)
  {
    std::vector<BoundaryConditions::result> expected_result{BoundaryConditions::result::valid,
                                                            BoundaryConditions::result::valid,
                                                            BoundaryConditions::result::valid,
                                                            BoundaryConditions::result::invalid};
    std::vector<int> expected_nn_row{18,0,19,19};
    std::vector<int> expected_nn_col{19,19,18,20};

    std::vector<BoundaryConditions::result> test_result_results(4);
    std::vector<int> test_result_nn_row(4);
    std::vector<int> test_result_nn_col(4);
  
    test.set_boundary_conditions(BoundaryConditions::type::periodic_vertical);
    for (size_t i=0;i<directions.size();i++){
      test_result_results[i] = test.get_nn_coordinates(19,19 ,test_result_nn_row[i], test_result_nn_col[i], n_rows, n_cols, directions[i]);
    }

    bool test_passes = check_test_results(expected_result,expected_nn_row,expected_nn_col,
                                          test_result_results, test_result_nn_row, test_result_nn_col);

    if (test_passes)
      std::cout << "Boundary conditions (periodic-vertical, (19,19)): \033[1;32mPASS\033[0m" << std::endl;
    else
      std::cout << "Boundary conditions (periodic-vertical, (19,19)): \033[1;31mFAIL\033[0m" << std::endl;

  }


  return 0;

}

bool check_test_results(std::vector<BoundaryConditions::result> expected_result,
                        std::vector<int> expected_nn_row,
                        std::vector<int> expected_nn_col,
                        std::vector<BoundaryConditions::result> test_result_results,
                        std::vector<int> test_result_nn_row,
                        std::vector<int> test_result_nn_col){

  bool test_passes = true;
  for (size_t i=0;i<expected_result.size();i++){

    if (expected_result[i] != test_result_results[i]){
      std::cout << "Failed expected result" << std::endl;
      test_passes = false;
    }

    if (expected_nn_row[i] != test_result_nn_row[i]){
      printf("Failed NN row (direction %d, expected %d and got %d)\n",(int)i,expected_nn_row[i],test_result_nn_row[i]);
      //std::cout << "Failed nearest neighbor row" << std::endl;
      test_passes = false;
    }

    if (expected_nn_col[i] != test_result_nn_col[i]){
      printf("Failed NN column (direction %d, expected %d and got %d)\n",(int)i,expected_nn_col[i],test_result_nn_col[i]);
      //std::cout << "Failed nearest neighbor column" << std::endl;
      test_passes = false;
    }
    
  }

  return test_passes;
}