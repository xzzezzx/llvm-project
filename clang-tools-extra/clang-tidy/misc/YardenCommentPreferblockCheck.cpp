#include "YardenCommentPreferblockCheck.h"
#include "clang/AST/ASTContext.h"
#include "clang/Lex/Preprocessor.h"
#include "clang/Lex/Token.h"
#include "clang/Lex/Lexer.h"

using namespace clang;
using namespace clang::tidy;
using namespace clang::tidy::misc;

namespace {

class ConsecutiveCommentTracker : public PPCallbacks, public CommentHandler {
public:
  ConsecutiveCommentTracker(ClangTidyCheck *Check, const SourceManager &SM)
      : Check(Check), SM(SM), ConsecutiveCount(0), LastLineNumber(0) {}

  bool HandleComment(Preprocessor &PP, SourceRange CommentRange) override {
    if (SM.isWrittenInMainFile(CommentRange.getBegin())) {
        StringRef CommentText = Lexer::getSourceText(
            CharSourceRange::getTokenRange(CommentRange), SM, PP.getLangOpts());

        unsigned CurrentLineNo = SM.getSpellingLineNumber(SM.getSpellingLoc(CommentRange.getBegin()));
        bool IsSingleLineComment = CommentText.trim().starts_with("//");
        bool IsBlockComment = CommentText.trim().starts_with("/*");

        if (IsSingleLineComment || IsBlockComment) {
            if (LastLineNumber == 0 || CurrentLineNo == LastLineNumber + 1) {
                ++ConsecutiveCount;
            } else {
                // Reset if the line number is not consecutive, start a new block
                ConsecutiveCount = 1;
            }

            // Update the last line number to the current one
            LastLineNumber = CurrentLineNo;
        } else if (!CommentText.trim().empty()) {
            // Reset if a non-comment or non-consecutive comment is found
            ConsecutiveCount = 0;
            LastLineNumber = 0;
        }

        // Issue a warning only if there are enough consecutive comments
        if (ConsecutiveCount >= 2) {  // Adjust the threshold as needed
            Check->diag(CommentRange.getBegin(),
                        "Consider using a block comment (/* ... */) or refactoring for clarity.");
            ConsecutiveCount = 0;  // Reset after issuing a warning
            LastLineNumber = 0;  // Reset the last line number tracker
        }
    }
    return false;  // Allow other handlers to process the comment as well
  }

  void MacroExpands(const Token &Tok, const MacroDefinition &MD,
                    SourceRange Range, const MacroArgs *Args) override {
    // Reset the count on macro expansions (indicates non-comment code)
    ConsecutiveCount = 0;
    LastLineNumber = 0;  // Reset the last line number tracker
  }

  void FileChanged(SourceLocation Loc, FileChangeReason Reason,
                   SrcMgr::CharacteristicKind FileType,
                   FileID PrevFID) override {
    // Reset the count when the file context changes
    ConsecutiveCount = 0;
    LastLineNumber = 0;  // Reset the last line number tracker
  }

private:
  ClangTidyCheck *Check;
  const SourceManager &SM;
  int ConsecutiveCount;
  unsigned LastLineNumber;  // To keep track of the last comment's line number
};

} // namespace

void YardenCommentPreferblockCheck::registerPPCallbacks(const SourceManager &SM,
                                                         Preprocessor *PP,
                                                         Preprocessor *ModuleExpanderPP) {
  auto *Tracker = new ConsecutiveCommentTracker(this, SM);
  PP->addCommentHandler(Tracker);
  PP->addPPCallbacks(std::unique_ptr<PPCallbacks>(Tracker));
}
