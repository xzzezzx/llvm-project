#include "YardenForloopCounterCheck.h"
#include "clang/AST/ASTContext.h"
#include "clang/ASTMatchers/ASTMatchers.h"
#include "clang/ASTMatchers/ASTMatchFinder.h"

using namespace clang::ast_matchers;

namespace clang {
namespace tidy {
namespace misc {

YardenForloopCounterCheck::YardenForloopCounterCheck(StringRef Name, ClangTidyContext *Context)
    : ClangTidyCheck(Name, Context) {}

void YardenForloopCounterCheck::registerMatchers(MatchFinder *Finder) {
  // Match for loops where a counter variable is initialized and potentially modified in the loop body.
  Finder->addMatcher(
      forStmt(
          hasLoopInit(declStmt(hasSingleDecl(varDecl(hasInitializer(expr())).bind("counterVarDecl")))),
          hasBody(hasDescendant(
              binaryOperator(isAssignmentOperator(),
                             hasLHS(ignoringParenImpCasts(declRefExpr(to(varDecl(equalsBoundNode("counterVarDecl")))))))
          ))
      ).bind("forLoop"),
      this);
}

void YardenForloopCounterCheck::check(const MatchFinder::MatchResult &Result) {
  const auto *ForLoop = Result.Nodes.getNodeAs<ForStmt>("forLoop");
  const auto *CounterVarDecl = Result.Nodes.getNodeAs<VarDecl>("counterVarDecl");

  if (!ForLoop || !CounterVarDecl)
    return;

  // Emit a diagnostic warning when a loop counter variable is modified inside the loop body.
  diag(ForLoop->getBeginLoc(),
       "modifying loop counter variable '%0' inside the loop body is disallowed")
      << CounterVarDecl->getName();
}

} // namespace misc
} // namespace tidy
} // namespace clang
