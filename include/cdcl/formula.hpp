#pragma once 

///////////////////////////////////////////////////////////////////////////////
// Headers
#include <iostream>
#include <vector>
#include <set>
#include <fstream>
#include <string>

#include <cdcl/clause.hpp>
#include <cdcl/utils.hpp>

///////////////////////////////////////////////////////////////////////////////
// Namespace

namespace cdcl {

///////////////////////////////////////////////////////////////////////////////
// Classes

class Formula {

  /////////////////////////////////////////////////////////////////////////////
  // Members
private:
  std::set<int> m_variables;
  std::set<Clause> m_clauses;

  /////////////////////////////////////////////////////////////////////////////
  // Constructors

public: 

  Formula (std::vector<Clause> clauses) {
    this->m_variables = std::set<int>();
    this->m_clauses = std::set<Clause>();

    for (auto &clause : clauses) {
      this->m_clauses.insert(clause);

      for (auto literal : clause.literals()) {
        this->m_variables.insert(literal.variable());
      }
    }
  }

  Formula (std::set<Clause> clauses) {
    this->m_variables = std::set<int>();
    this->m_clauses = clauses;

    for (auto clause : clauses) {
      for (auto literal : clause.literals()) {
        this->m_variables.insert(literal.variable());
      }
    }
  }

  Formula () {
    this->m_variables = std::set<int>();
    this->m_clauses = std::set<Clause>();
  }

  /////////////////////////////////////////////////////////////////////////////
  // Methods

  std::set<int> variables() {
    return m_variables;
  }

  std::set<Clause>& clauses() {
    return m_clauses;
  }

  void addClause(Clause &clause) {
    this->m_clauses.insert(clause);

    for (auto literal : clause.literals()) {
      this->m_variables.insert(literal.variable());
    }
  }

  /////////////////////////////////////////////////////////////////////////////
  // Static methods

  static Formula* fromDIMACS(const char *path) {
    std::ifstream file(path);

    if (!file.is_open()) {
      std::cerr << "Unable to open file: " << path << std::endl;
      return nullptr;
    }

    std::string line;

    // Parse first line
    getline(file, line);
    
    std::vector<std::string> parsed_line = split(line, " ");
    if (parsed_line.size() != 4) {
      std::cerr << "Invalid DIMACS file." << std::endl;
      return nullptr;
    }

    if (parsed_line[0] != "p" || parsed_line[1] != "cnf") {
      std::cerr << "Only cnf formulas are supported." << std::endl;
      return nullptr;
    } 

    int num_variables = std::atoi(parsed_line[2].c_str());
    int num_clauses = std::atoi(parsed_line[3].c_str());

    if (num_clauses == 0 || num_variables == 0) {
      std::cerr << "Invalid DIMACS file." << std::endl;
      return nullptr;
    }

    std::vector<Clause> clauses;
    while(getline(file, line)) {
      std::vector<std::string> raw_literals = split(line, " ");
      
      if (raw_literals.size() == 0) continue;

      std::vector<Literal> literals = std::vector<Literal>();
      for (auto &raw_literal : raw_literals) {
        // Terminator
        if (raw_literal == "0") {
          break;
        }

        bool negated = raw_literal[0] == '-';
        int variable = negated ? std::atoi(raw_literal.substr(1).c_str()) : std::atoi(raw_literal.c_str());
    
        literals.push_back(Literal(variable, negated));
      }

      clauses.push_back(Clause(literals));
    }

    Formula *formula = new Formula(clauses);

    return formula;
  }

  /////////////////////////////////////////////////////////////////////////////
  // Operators 

  friend std::ostream& operator<< (std::ostream &os, Formula const &f) { 
    int current_clause = 0;

    for (auto &clause : f.m_clauses) {
      os << clause;

      if (current_clause != f.m_clauses.size() - 1) {
        os << " ∧ ";
        current_clause++;
      }
    }

    return os;
  }
  
};

} // cdcl