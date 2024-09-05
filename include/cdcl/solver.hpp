#pragma once 

///////////////////////////////////////////////////////////////////////////////
// Headers
#include <algorithm>

#include <cdcl/formula.hpp>
#include <cdcl/assignment.hpp>

///////////////////////////////////////////////////////////////////////////////
// Namespace

namespace cdcl {

///////////////////////////////////////////////////////////////////////////////
// Types

enum ClauseStatus {
  SATISFIED,
  UNSATISFIED,
  UNIT,
  UNRESOLVED,
  CONFLICT
};

typedef struct {
  ClauseStatus status;
  std::optional<Literal> unassigned_literal;
  std::optional<Clause> conflict_clause;
} UnitPropagationResult;


typedef std::pair<ClauseStatus, std::optional<Literal>> ClauseStatusWithUnitLiteral;
typedef std::pair<int, bool> LiteralAssignment;
typedef std::pair<int, std::optional<Clause>> ConflictAnalysisResult;

///////////////////////////////////////////////////////////////////////////////
// Classes

class Solver {

  /////////////////////////////////////////////////////////////////////////////
  // Members
private:
  Formula m_formula;
  PartialAssignment m_assignment;

  /////////////////////////////////////////////////////////////////////////////
  // Constructors

public: 

  Solver (Formula &formula) {
    this->m_formula = formula;
    this->m_assignment = PartialAssignment();
  }

  /////////////////////////////////////////////////////////////////////////////
  // Methods

  bool solve() {
    this->m_assignment.clear();

    // Assign initial values
    UnitPropagationResult result = this->unitPropagate();
    if (result.status == ClauseStatus::CONFLICT) {
      return false;
    }
    // Solve loop
    while(! this->allVariablesAssigned()) {
      LiteralAssignment branch_var = this->branch();
      this->m_assignment.setDecisionLevel(this->m_assignment.decisionLevel() + 1);
      this->m_assignment.assign(branch_var.first, branch_var.second, std::nullopt);
      while(true) {
        UnitPropagationResult unit_result = this->unitPropagate();
        if (unit_result.status != ClauseStatus::CONFLICT) break;

        ConflictAnalysisResult conflict_result = this->conflictAnalysis(unit_result.conflict_clause.value());
        if (conflict_result.first < 0) return false;

        this->m_formula.addClause(conflict_result.second.value());
        this->m_assignment.backtrack(conflict_result.first);
        this->m_assignment.setDecisionLevel(conflict_result.first);
      }
    }

    return true;
  }

  Model getModel() {
    return this->m_assignment.getModel();
  }

  /////////////////////////////////////////////////////////////////////////////
  // Private methods

private:

  // Applies all unit propagations possible
  UnitPropagationResult unitPropagate() {
    bool can_unit = false;
    int satisfied_clauses = 0;

    do {
      can_unit = false;
      satisfied_clauses = 0;

      // For each clause, check its status
      // If it is unresolved or satisfied, we can ignore it
      // If it is possible to unit propagate a value, we do it
      // If it is false, we found a conflict
      
      for (auto clause : this->m_formula.clauses()) {
        ClauseStatusWithUnitLiteral statusWithLiteral = this->clauseStatus(clause);
        ClauseStatus status = statusWithLiteral.first;
        std::optional<Literal> literal_to_unit = statusWithLiteral.second;

        // status = UNRESOLVED  -> ignore clause
        if (status == ClauseStatus::UNRESOLVED || status == ClauseStatus::SATISFIED) continue;

        // status = UNIT -> apply unit propagation
        else if (status == ClauseStatus::UNIT) {
          Literal literal = literal_to_unit.value();
          this->m_assignment.assign(literal.variable(), !literal.negated(), clause);
          can_unit = true;
        }

        // status = UNSATISFIED -> conflict found
        else {
          return UnitPropagationResult({ClauseStatus::CONFLICT, std::nullopt, clause});
        }
      }
    } while (can_unit);

    return UnitPropagationResult({ClauseStatus::UNRESOLVED, std::nullopt, std::nullopt});;
  }

  // 4 states are possible:
  // 1. UNIT - All but one literal are assigned False
  // 2. UNSATISFIED - All literals are assigned False
  // 3. SATISFIED - All literals are assigned True
  // 4. UNRESOLVED - Neither unit, satisfied nor unsatisfied
  ClauseStatusWithUnitLiteral clauseStatus(Clause &clause) {
    int num_of_true = 0;
    int num_of_false = 0;
    int num_of_literals = clause.literals().size();

    std::optional<Literal> literal_to_unit;

    for (auto literal : clause.literals()) {
      if (this->m_assignment.isAssigned(literal.variable())) {
        this->m_assignment.value(literal) ? num_of_true++ : num_of_false++;
      } else {
        literal_to_unit = literal;
      }
    }

    ClauseStatus status = ClauseStatus::UNRESOLVED;

    if (num_of_true >= 1) {
      status = ClauseStatus::SATISFIED;
      literal_to_unit = std::nullopt;
    }

    else if (num_of_literals - num_of_false == 1) status = ClauseStatus::UNIT;
    
    else if (num_of_false == num_of_literals) {
      status = ClauseStatus::UNSATISFIED;
      literal_to_unit = std::nullopt;
    }

    return std::make_pair(status, literal_to_unit);
  }

  bool allVariablesAssigned() {
    return this->m_formula.variables().size() == this->m_assignment.assignedVariables();
  }
  
  // Criterion: pick first unassigned variable and assign it to true
  LiteralAssignment branch() {
    int branch_var = -1;
    for (auto &variable : this->m_formula.variables()) {
      if (! this->m_assignment.isAssigned(variable)) {
        branch_var = variable;
        break;
      }
    }

    return std::make_pair(branch_var, true);
  }

  Clause resolve(Clause &a, Clause &b, int resolvent) {
    std::set<Literal> resolved;

    std::set<Literal> clause_union;

    for (auto &literal : a.literals()) {
      resolved.insert(literal);
    }

    for (auto &literal : b.literals()) {
      resolved.insert(literal);
    }

    resolved.erase(Literal(resolvent, true));
    resolved.erase(Literal(resolvent, false));

    return Clause(resolved);
  }

  int hasResolvent(Clause &a, Clause &b) {
    int resolvent = -1;
    std::set<Literal> clause_union;

    for (auto &literal : a.literals()) {
      clause_union.insert(literal);
    }

    for (auto &literal : b.literals()) {
      clause_union.insert(literal);
    }

    for (auto literal : clause_union) {
      bool contains_literal = clause_union.find(Literal(literal.variable(), false)) != clause_union.end();
      bool contains_negated = clause_union.find(Literal(literal.variable(), true)) != clause_union.end();

      if (contains_negated && contains_literal) {
        return literal.variable();
      }
    }

    return resolvent;
  }

  ConflictAnalysisResult conflictAnalysis(Clause &conflict_clause) {
    if (this->m_assignment.decisionLevel() == 0) return std::make_pair(-1, std::nullopt);

    auto assignments_at_level = this->m_assignment.getLiteralsAtCurrentDecisionLevel();

    Clause learnt_clause = conflict_clause;
    int dl_to_backtrack_to = 0;

    while (assignments_at_level.size() != 1) {
      // Pick first clause that has a resolvent
      std::set<Literal> learnt_clause_literals = learnt_clause.literals();

      // Look for a resolvent and eventually resolve
      auto literals_at_dl = assignments_at_level.begin();
      while (literals_at_dl != assignments_at_level.end()) {
        int resolvent = hasResolvent(learnt_clause, literals_at_dl->second.antecedent.value());
        if (resolvent == -1) {
          literals_at_dl++;
          continue;
        };

        // Resolve
        learnt_clause = this->resolve(learnt_clause, literals_at_dl->second.antecedent.value(), resolvent);
        literals_at_dl = assignments_at_level.erase(literals_at_dl);
      }

    }

    // Learnt clause is now final, according to 1° UIP rule

    // Backtrack to the highest branching point s.t. the stack contains all-but-one
    // literals in the conflict set, and then unit-propagate the unassigned literal 
    // on the learnt clause

    std::set<Literal> conflict_set = std::set<Literal>();
    for (auto literal : learnt_clause.literals()) {
      conflict_set.insert(Literal(literal.variable(), !literal.negated()));
    } 

    int current_dl_in_search = 0;
    while (current_dl_in_search < this->m_assignment.decisionLevel()) {
      assignments_at_level = this->m_assignment.getLiteralsAtDecisionLevel(current_dl_in_search);

      // Do they differ by only one literal?
      int difference = 0;
      for (auto &literal : assignments_at_level) {
        // We don't really care about the polarity of the literal
        bool contains_literal = conflict_set.find(Literal(literal.first, true)) != conflict_set.end();
        contains_literal = contains_literal || (conflict_set.find(Literal(literal.first, false)) != conflict_set.end());
        
        if (! contains_literal) {
          difference++;
        }
      }

      // Is this a candidate dl?
      if (difference == 1) {
        dl_to_backtrack_to = current_dl_in_search;
        break;
      }

      current_dl_in_search++;
    }

    return std::make_pair(dl_to_backtrack_to, learnt_clause);
  }

};

} // cdcl