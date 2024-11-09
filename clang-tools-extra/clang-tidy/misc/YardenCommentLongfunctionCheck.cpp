#include "YardenCommentLongfunctionCheck.h"
#include "clang/AST/ASTContext.h"
#include "clang/ASTMatchers/ASTMatchers.h"
#include "clang/ASTMatchers/ASTMatchFinder.h"
#include "clang/Lex/Lexer.h"

using namespace clang::ast_matchers;

namespace clang {
namespace tidy {
namespace misc {

YardenCommentLongfunctionCheck::YardenCommentLongfunctionCheck(StringRef Name, ClangTidyContext *Context)
    : ClangTidyCheck(Name, Context) {}

void YardenCommentLongfunctionCheck::registerMatchers(MatchFinder *Finder) {
  // Match function declarations that have a body (i.e., are definitions).
  Finder->addMatcher(functionDecl(isDefinition()).bind("functionDecl"), this);
}

void YardenCommentLongfunctionCheck::check(const MatchFinder::MatchResult &Result) {
  const auto *Func = Result.Nodes.getNodeAs<FunctionDecl>("functionDecl");
  if (!Func || !Func->hasBody())
    return;

  const SourceManager &SM = *Result.SourceManager;
  const CompoundStmt *Body = dyn_cast<CompoundStmt>(Func->getBody());
  if (!Body)
    return;

  // Calculate function length in lines.
  SourceLocation StartLoc = SM.getSpellingLoc(Body->getLBracLoc());
  SourceLocation EndLoc = SM.getSpellingLoc(Body->getRBracLoc());
  unsigned StartLine = SM.getSpellingLineNumber(StartLoc);
  unsigned EndLine = SM.getSpellingLineNumber(EndLoc);

  if (EndLine - StartLine <= 20)
    return; // Function is 20 lines or fewer, no check needed.

  // Get the source code text following the closing brace to look for a comment.
  SourceLocation AfterRBrace = EndLoc.getLocWithOffset(1);  // Location after the closing brace
  const char *AfterRBraceData = SM.getCharacterData(AfterRBrace);
  StringRef AfterRBraceText(AfterRBraceData, 10);  // Read 10 characters following the brace

  // Use 'starts_with' instead of 'startswith' to check for a comment.
  if (!AfterRBraceText.starts_with("//") && !AfterRBraceText.starts_with("/*")) {
    diag(EndLoc, "function longer than 20 lines requires a closing comment after the function");
  }
}

} // namespace misc
} // namespace tidy
} // namespace clang
