#ifndef LLVM_CLANG_TOOLS_EXTRA_CLANG_TIDY_MISC_YARDENCOMMENTSPACEBEFOREBLOCKCHECK_H
#define LLVM_CLANG_TOOLS_EXTRA_CLANG_TIDY_MISC_YARDENCOMMENTSPACEBEFOREBLOCKCHECK_H

#include "../ClangTidyCheck.h"

namespace clang {
namespace tidy {
namespace misc {

class YardenCommentSpacebeforeblockCheck : public ClangTidyCheck {
public:
  YardenCommentSpacebeforeblockCheck(StringRef Name, ClangTidyContext *Context);
  void registerMatchers(ast_matchers::MatchFinder *Finder) override;
  void check(const ast_matchers::MatchFinder::MatchResult &Result) override;
};

} // namespace misc
} // namespace tidy
} // namespace clang

#endif // LLVM_CLANG_TOOLS_EXTRA_CLANG_TIDY_MISC_YARDENCOMMENTSPACEBEFOREBLOCKCHECK_H
