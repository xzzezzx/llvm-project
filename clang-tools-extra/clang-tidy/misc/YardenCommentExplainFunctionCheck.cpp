#include "YardenCommentExplainFunctionCheck.h"
#include "clang/AST/ASTContext.h"
#include "clang/ASTMatchers/ASTMatchers.h"
#include "clang/ASTMatchers/ASTMatchFinder.h"
#include "clang/Basic/SourceManager.h"
#include "clang/Lex/Lexer.h"

using namespace clang::ast_matchers;

namespace clang {
namespace tidy {
namespace misc {

YardenCommentExplainFunctionCheck::YardenCommentExplainFunctionCheck(StringRef Name, ClangTidyContext *Context)
    : ClangTidyCheck(Name, Context) {}

void YardenCommentExplainFunctionCheck::registerMatchers(MatchFinder *Finder) {
  // Match function definitions in the source code.
  Finder->addMatcher(functionDecl(isDefinition()).bind("functionDecl"), this);
}

void YardenCommentExplainFunctionCheck::check(const MatchFinder::MatchResult &Result) {
  const auto *FuncDecl = Result.Nodes.getNodeAs<FunctionDecl>("functionDecl");
  if (!FuncDecl)
    return;

  const SourceManager &SM = *Result.SourceManager;
  SourceLocation FuncStart = FuncDecl->getBeginLoc();
  if (FuncStart.isInvalid() || !SM.isWrittenInMainFile(FuncStart))
    return;

  // Get the start of the line where the function begins.
  SourceLocation LineStart = SM.getExpansionLoc(FuncStart);
  unsigned StartOffset = SM.getFileOffset(LineStart);
  const char *FuncStartData = SM.getCharacterData(FuncStart);

  // Move backwards in the source code to check for comments above the function.
  bool HasComment = false;
  for (int Offset = -1; Offset >= -500 && (StartOffset + Offset) > 0; --Offset) {  // Limit to 500 characters back for efficiency.
    const char *CharData = FuncStartData + Offset;
    if (*CharData == '/' && (*(CharData - 1) == '/' || *(CharData - 1) == '*')) {
      HasComment = true;
      break;
    }
    // Stop if a non-comment, non-whitespace character is encountered.
    if (!isspace(*CharData)) {
      break;
    }
  }

  if (!HasComment) {
    diag(FuncStart, "function '%0' should have a comment block explaining its purpose")
        << FuncDecl->getNameAsString();
  }
}

} // namespace misc
} // namespace tidy
} // namespace clang
