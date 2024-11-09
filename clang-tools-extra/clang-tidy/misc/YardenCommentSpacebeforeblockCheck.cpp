#include "YardenCommentSpacebeforeblockCheck.h"
#include "clang/AST/ASTContext.h"
#include "clang/Basic/SourceManager.h"

using namespace clang;
using namespace clang::tidy;
using namespace clang::ast_matchers;

namespace clang {
namespace tidy {
namespace misc {

void YardenCommentSpacebeforeblockCheck::registerMatchers(ast_matchers::MatchFinder *Finder) {
  // This check will be applied to the entire translation unit.
  Finder->addMatcher(translationUnitDecl().bind("translationUnit"), this);
}

void YardenCommentSpacebeforeblockCheck::check(const MatchFinder::MatchResult &Result) {
  const SourceManager &SM = *Result.SourceManager;
  const FileID MainFileID = SM.getMainFileID();
  StringRef Buffer = SM.getBufferData(MainFileID);
  size_t BufferLength = Buffer.size();

  for (size_t i = 0; i < BufferLength; ++i) {
    // Detect the start of block comments (/*).
    if (Buffer.substr(i, 2) == "/*") {
      SourceLocation CommentLoc = SM.getLocForStartOfFile(MainFileID).getLocWithOffset(i);

      // Ensure there is an empty line before the comment.
      bool HasEmptyLineBefore = (i == 0); // Allow comment at the start of the file
      for (size_t j = i; j > 0; --j) {
        if (Buffer[j] == '\n') {
          // Check the previous character for another newline (indicating an empty line).
          if (j > 1 && Buffer[j - 1] == '\n') {
            HasEmptyLineBefore = true;
          }
          // Stop checking after finding the line break before the comment.
          break;
        }
      }

      // Report if there's no empty line before the comment and it's not the first line.
      if (!HasEmptyLineBefore && i != 0) {
        diag(CommentLoc, "comment block should have an empty line before it")
            << FixItHint::CreateInsertion(CommentLoc, "\n");
      }

      // Skip to the end of the block comment to avoid redundant checks.
      size_t EndPos = Buffer.find("*/", i);
      if (EndPos != StringRef::npos) {
        i = EndPos + 1; // Move past the end of the comment.
      } else {
        // If no end is found, break out (malformed comment).
        break;
      }
    }
  }
}

// Constructor definition
YardenCommentSpacebeforeblockCheck::YardenCommentSpacebeforeblockCheck(StringRef Name, ClangTidyContext *Context)
    : ClangTidyCheck(Name, Context) {}

} // namespace misc
} // namespace tidy
} // namespace clang
