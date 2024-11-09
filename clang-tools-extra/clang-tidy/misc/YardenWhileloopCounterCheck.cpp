#include "YardenWhileloopCounterCheck.h"
#include "clang/AST/ASTContext.h"
#include "clang/ASTMatchers/ASTMatchFinder.h"
#include "clang/Basic/SourceManager.h"
#include "clang/Lex/Lexer.h"

using namespace clang;
using namespace clang::ast_matchers;
using namespace clang::tidy;
using namespace clang::tidy::misc;

void YardenWhileloopCounterCheck::registerMatchers(MatchFinder *Finder) {
  // Match while loops with bodies containing statements that could modify a variable.
  Finder->addMatcher(
      whileStmt(hasBody(compoundStmt().bind("loopBody"))).bind("whileLoop"),
      this);
}

void YardenWhileloopCounterCheck::check(const MatchFinder::MatchResult &Result) {
  const auto *WhileLoop = Result.Nodes.getNodeAs<WhileStmt>("whileLoop");
  const auto *LoopBody = Result.Nodes.getNodeAs<CompoundStmt>("loopBody");
  
  if (!WhileLoop || !LoopBody) return;

  // Collect statements inside the loop body.
  const Stmt *FirstStmt = nullptr;
  const Stmt *LastStmt = nullptr;
  if (!LoopBody->body_empty()) {
    FirstStmt = *(LoopBody->body_begin());
    LastStmt = *std::prev(LoopBody->body_end()); // Use std::prev to get the last statement
  }

  // Helper to check if an expression modifies a variable.
  auto isModifyingStmt = [](const Stmt *Stmt) {
    return isa<UnaryOperator>(Stmt) || isa<BinaryOperator>(Stmt);
  };

  for (const Stmt *S : LoopBody->body()) {
    if (S != FirstStmt && S != LastStmt && isModifyingStmt(S)) {
      // Check if this statement modifies a variable.
      const auto *BO = dyn_cast<BinaryOperator>(S);
      if (BO && BO->isAssignmentOp()) {
        diag(S->getBeginLoc(), "Counter should only be modified in the first or last line of the loop body");
      } else if (const auto *UO = dyn_cast<UnaryOperator>(S)) {
        if (UO->isIncrementDecrementOp()) {
          diag(S->getBeginLoc(), "Counter should only be modified in the first or last line of the loop body");
        }
      }
    }
  }
}
