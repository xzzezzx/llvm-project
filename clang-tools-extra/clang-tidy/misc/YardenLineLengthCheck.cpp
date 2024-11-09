#include "YardenLineLengthCheck.h"
#include "clang/AST/ASTContext.h"
#include "clang/ASTMatchers/ASTMatchers.h"
#include "clang/ASTMatchers/ASTMatchFinder.h"
#include "clang/Basic/SourceManager.h"
#include "clang/Lex/Lexer.h"
#include "llvm/ADT/StringRef.h"

using namespace clang::ast_matchers;

namespace clang {
namespace tidy {
namespace misc {

YardenLineLengthCheck::YardenLineLengthCheck(StringRef Name, ClangTidyContext *Context)
    : ClangTidyCheck(Name, Context) {}

void YardenLineLengthCheck::registerMatchers(MatchFinder *Finder) {
  // Match the translation unit to trigger the check for the entire file.
  Finder->addMatcher(translationUnitDecl().bind("translationUnit"), this);
}

void YardenLineLengthCheck::check(const MatchFinder::MatchResult &Result) {
  const SourceManager &SM = *Result.SourceManager;
  FileID MainFileID = SM.getMainFileID();
  StringRef FileContent = SM.getBufferData(MainFileID);

  size_t LineStart = 0;
  unsigned LineNumber = 1;

  while (LineStart < FileContent.size()) {
    size_t LineEnd = FileContent.find('\n', LineStart);
    if (LineEnd == StringRef::npos) {
      LineEnd = FileContent.size();
    }

    StringRef Line = FileContent.slice(LineStart, LineEnd);
    if (Line.size() > 120) {
      SourceLocation LineStartLoc = SM.translateLineCol(MainFileID, LineNumber, 1);
      diag(LineStartLoc, "line exceeds 120 characters (length: %0)") << Line.size();
    }

    LineStart = LineEnd + 1;
    ++LineNumber;
  }
}

} // namespace misc
} // namespace tidy
} // namespace clang
