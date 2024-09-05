# A simple C++ Header-Only implementation of a CDCL SAT solver

![License: MIT](https://img.shields.io/badge/License-MIT-green.svg)
![C++](https://img.shields.io/badge/C%2B%2B-17%2B-blue.svg)

## Overview

This is a C++ header-only library that implements the Conflict-Driven Clause Learning (CDCL) algorithm, a powerful method used in modern SAT solvers. The library requires no external dependencies and has been implemented with the primary goal of learning more how SAT solvers work.

### Improvements

As of now this library is still very basic in its functionalities, in fact many things can be improved. Here is a short list of them:

- [ ] Introduce the 2-watched literal data structure
- [ ] Improve the branching strategy (as of now it picks the first unassigned literal and assigns it to true)
- [ ] Introduce random restarts
- [ ] Reduce the amount of copy operations in memory

## Getting Started

### Prerequisites

- A C++17-compatible compiler (e.g., GCC, Clang, MSVC)
- CMake (optional, for building the CLI tool)

### Installation

Since this is a header-only library, you can directly include it in your project. Follow the steps below:

1. Clone the repository:

    ```bash
    $ git clone git@github.com:ecivini/cdcl-sat-solver.git
    ```

2. Add the headers path to your compiler settings, then include the main header files in your project:

    ```cpp
    #include <cdcl/formula.hpp>
    #include <cdcl/solver.hpp>
    ```

3. You're ready to use this CDCL SAT solver in your project! Here is a simple example:

    ```cpp
    #include <iostream>

    #include <cdcl/formula.hpp>
    #include <cdcl/solver.hpp>

    using namespace cdcl;

    int main() {

        Literal l1 = Literal(1, false); // 1
        Literal l1n = Literal(1, true); // ¬1
        Literal l2n = Literal(2, true); // ¬2
        Literal l3n = Literal(3, true); // ¬3

        // 1 v ¬2
        std::set<Literal> first_clause_literals = std::set<Literal>();
        first_clause_literals.insert(l1);
        first_clause_literals.insert(l2n);

        // ¬1 v ¬2 v ¬3
        std::set<Literal> second_clause_literals = std::set<Literal>();
        second_clause_literals.insert(l1n);
        second_clause_literals.insert(l2n);
        second_clause_literals.insert(l3n);

        Clause c1 = Clause(first_clause_literals);
        Clause c2 = Clause(second_clause_literals);

        std::set<Clause> formula_clauses = std::set<Clause>();
        formula_clauses.insert(c1);
        formula_clauses.insert(c2);

        // (1 v ¬2) ∧ (¬1 v ¬2 v ¬3)
        Formula formula = Formula(formula_clauses);

        // Solve
        Solver solver = Solver(formula);
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

        return EXIT_SUCCESS;
    }
    ```

### Building the CLI tool

It's possible to build a simple CDCL SAT solver tool that is able to solve CNF formulas in DIMACS format.
To do so, after cloning this repository, run:

1. Build the executable:

    ```bash
    $ cd cdcl-sat-solver
    $ mkdir build
    $ cd build
    $ cmake ..
    $ make
    ```

2. Now there is an executable file called `cdcl`. You can check if a formula is satisfiable by running:

    ```bash
    $ ./cdcl <path to DIMACS file>
    ```

## Contributing

Contributions are welcome! Feel free to open an issue or to create a pull request if want to add features to this project.
