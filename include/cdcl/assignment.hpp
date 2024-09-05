#pragma once 

///////////////////////////////////////////////////////////////////////////////
// Headers
#include <vector>
#include <unordered_map>
#include <optional>

#include <cdcl/formula.hpp>

///////////////////////////////////////////////////////////////////////////////
// Namespace

namespace cdcl {

///////////////////////////////////////////////////////////////////////////////
// Types

typedef struct {
  bool value;
  std::optional<Clause> antecedent;
  int decision_level;
} Assignment;

typedef std::unordered_map<int, bool> Model;

///////////////////////////////////////////////////////////////////////////////
// Classes

class PartialAssignment {

  /////////////////////////////////////////////////////////////////////////////
  // Members
private:
  int m_decision_level;
  std::unordered_map<int, Assignment> m_assignments;

  /////////////////////////////////////////////////////////////////////////////
  // Constructors
public: 
  PartialAssignment () {
    this->m_decision_level = -1;
    this->m_assignments = std::unordered_map<int, Assignment>();
  }

  /////////////////////////////////////////////////////////////////////////////
  // Methods

  int decisionLevel() {
    return this->m_decision_level;
  }

  void setDecisionLevel(int level) {
    this->m_decision_level = level;
  }

  bool isAssigned(int variable) {
    return this->m_assignments.count(variable);
  }

  bool value(Literal &literal) {
    bool value = this->m_assignments[literal.variable()].value;

    if (literal.negated()) {
      value = !value;
    }

    return value;
  }

  void assign(int variable, bool value, std::optional<Clause> clause) {
    this->m_assignments[variable] = Assignment({value, clause, this->m_decision_level});
  }

  void unassign(int variable) {
    this->m_assignments.erase(variable);
  }

  void clear() {
    this->m_decision_level = -1;
    this->m_assignments.clear();
  }

  int assignedVariables() {
    return this->m_assignments.size();
  }

  std::unordered_map<int, Assignment> getLiteralsAtCurrentDecisionLevel() {
    return this->getLiteralsAtDecisionLevel(this->m_decision_level);
  }

  std::unordered_map<int, Assignment> getLiteralsAtDecisionLevel(int level) {
    std::unordered_map<int, Assignment> assignments_at_level = std::unordered_map<int, Assignment>();

    // Returns all literals assigned at the current decision level that have an antecedent clause
    for (auto &assignment : this->m_assignments) {
      if (assignment.second.decision_level == level && assignment.second.antecedent != std::nullopt) {
        auto pair = std::make_pair(assignment.first, assignment.second);
        assignments_at_level[assignment.first] = assignment.second;
      }
    }
    
    return assignments_at_level;
  }

  void backtrack(int level) {
    auto assignment = this->m_assignments.begin();
    while(assignment != this->m_assignments.end()) {
      if (assignment->second.decision_level > level) {
        assignment = this->m_assignments.erase(assignment);
      } else {
        assignment++;
      }
    }
  }

  Model getModel() {
    std::unordered_map<int, bool> model = std::unordered_map<int, bool>();

    for (auto &literal : this->m_assignments) {
      model[literal.first] = literal.second.value;
    }

    return model;
  }

  /////////////////////////////////////////////////////////////////////////////
  // Operators 

  friend std::ostream& operator<< (std::ostream &os, PartialAssignment const &assignment) { 
    for (auto const& var : assignment.m_assignments) {
      os << "Variable " << var.first << " = " << var.second.value << std::endl;
    }

    return os;
  }
};

} // cdcl