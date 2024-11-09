#include "YardenCommentPreferblockCheck.h"
#include "clang/AST/ASTContext.h"
#include "clang/Lex/Preprocessor.h"
#include "clang/Lex/Token.h"
#include "clang/Lex/Lexer.h"

using namespace clang;
using namespace clang::tidy;
using namespace clang::tidy::misc;

namespace {

class ConsecutiveCommentTracker : public CommentHandler {
public:
  ConsecutiveCommentTracker(ClangTidyCheck *Check, const SourceManager &SM)
      : Check(Check), SM(SM), ConsecutiveCount(0) {}

  bool HandleComment(Preprocessor &PP, SourceRange CommentRange) override {
    if (SM.isWrittenInMainFile(CommentRange.getBegin())) {
      StringRef CommentText = Lexer::getSourceText(
          CharSourceRange::getTokenRange(CommentRange), SM, PP.getLangOpts());

      if (CommentText.starts_with("//")) {
        ++ConsecutiveCount;
      } else {
        ConsecutiveCount = 0;
      }

      if (ConsecutiveCount >= 5) { // Adjust threshold as needed
        Check->diag(CommentRange.getBegin(),
                    "Consider using a block comment (/* */) for consecutive "
                    "single-line comments.");
      }
    }
    return false; // Let other handlers process the comment as well
  }

private:
  ClangTidyCheck *Check;
  const SourceManager &SM;
  int ConsecutiveCount;
};

} // namespace

void YardenCommentPreferblockCheck::registerPPCallbacks(const SourceManager &SM, 
                                                         Preprocessor *PP, 
                                                         Preprocessor *ModuleExpanderPP) {
  PP->addCommentHandler(new ConsecutiveCommentTracker(this, SM));
}
