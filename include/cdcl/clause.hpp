#pragma once 

///////////////////////////////////////////////////////////////////////////////
// Headers
#include <iostream>
#include <vector>
#include <set>

#include <cdcl/literal.hpp>

///////////////////////////////////////////////////////////////////////////////
// Namespace

namespace cdcl {

///////////////////////////////////////////////////////////////////////////////
// Classes

class Clause {

  /////////////////////////////////////////////////////////////////////////////
  // Members
private:
  std::set<Literal> m_literals;

  /////////////////////////////////////////////////////////////////////////////
  // Constructors

public:

  Clause (std::vector<Literal> literals) {
    this->m_literals = std::set<Literal>();

    for (auto &literal : literals) {
      this->m_literals.insert(literal);
    }
  }

  Clause (std::set<Literal> literals) {
    this->m_literals = std::set<Literal>();

    for (auto &literal : literals) {
      this->m_literals.insert(literal);
    }
  }

  /////////////////////////////////////////////////////////////////////////////
  // Methods

  std::set<Literal> literals() {
    return this->m_literals;
  }

  /////////////////////////////////////////////////////////////////////////////
  // Operators 

  friend std::ostream& operator<< (std::ostream &os, Clause const &c) { 
    os << "( ";

    int literals_counter = 0;
    for (auto &literal : c.m_literals) {
      os << literal;

      if (literals_counter != c.m_literals.size() - 1) {
        os << " ∨ ";
        literals_counter++;
      }
    }

    os << " )";

    return os;
  }

  friend bool operator==(const Clause &lhs, const Clause &rhs) {
    for (auto &literal : lhs.m_literals) {
      if (rhs.m_literals.find(literal) == rhs.m_literals.end()) {
        return false;
      }
    }

    return true;
  }

  friend bool operator<(const Clause &lhs, const Clause &rhs) {
    int size_diff = lhs.m_literals.size() - rhs.m_literals.size();

    if (size_diff != 0) return size_diff;

    for (auto &literal : lhs.m_literals) {
      if (rhs.m_literals.find(literal) == rhs.m_literals.end()) {
        return -1;
      }
    }

    return +1;
  }
  
};

} // cdcl