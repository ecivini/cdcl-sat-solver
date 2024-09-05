#pragma once 

///////////////////////////////////////////////////////////////////////////////
// Headers
#include <iostream>

///////////////////////////////////////////////////////////////////////////////
// Namespace

namespace cdcl {

///////////////////////////////////////////////////////////////////////////////
// Classes

class Literal {

  /////////////////////////////////////////////////////////////////////////////
  // Members
private:
  int m_variable;
  bool m_negated;

  /////////////////////////////////////////////////////////////////////////////
  // Constructors

public: 

  Literal (int variable, bool negated = false) {
    this->m_variable = variable;
    this->m_negated = negated;
  }

  /////////////////////////////////////////////////////////////////////////////
  // Methods

  int variable() {
    return this->m_variable;
  }

  bool negated() {
    return this->m_negated;
  }

  /////////////////////////////////////////////////////////////////////////////
  // Operators 

  friend std::ostream& operator<< (std::ostream &os, Literal const &l) { 
    if (l.m_negated) {
      os << "¬";
    }

    os << l.m_variable;

    return os;
  }

  friend bool operator==(const Literal &lhs, const Literal &rhs) {
    return (lhs.m_variable == rhs.m_variable && lhs.m_negated == rhs.m_negated);
  }

  friend bool operator<(const Literal &lhs, const Literal &rhs) {
    return std::tie(lhs.m_variable, lhs.m_negated) < std::tie(rhs.m_variable, rhs.m_negated);
  }
  
};

}