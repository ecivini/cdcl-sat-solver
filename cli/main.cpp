#include <iostream>

#include <cdcl/formula.hpp>
#include <cdcl/solver.hpp>

using namespace cdcl;

int main(int argc, char **argv) {
  if (argc != 2) {
    std::cout << "Usage: ./cdcl <DIMAC FILE>" << std::endl;
    return EXIT_FAILURE;
  }

  // Read CNF formula from file
  Formula *formula = Formula::fromDIMACS(argv[1]);
  if (formula == nullptr) {
    return EXIT_FAILURE;
  }

  std::cout << "Formula: " << *formula << std::endl << std::endl;

  // Solve
  Solver solver = Solver(*formula);
  bool sat = solver.solve();

  if (sat) {
    std::cout << "Formula is SAT" << std::endl;

    Model model = solver.getModel();

    for (auto &assignment : model) {
      std::cout << "\tVariable " << assignment.first << " = " << (assignment.second ? "⊤" : "⊥") << std::endl;
    }
  } else {
    std::cout << "Formula is UNSAT." << std::endl;;
  }

  // Free memory
  if (formula !=nullptr) {
    delete formula;
  }

  return EXIT_SUCCESS;
}

