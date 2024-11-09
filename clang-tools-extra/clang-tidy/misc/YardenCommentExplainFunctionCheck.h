#ifndef LLVM_CLANG_TOOLS_EXTRA_CLANG_TIDY_MISC_YARDENCOMMENTEXPLAINFUNCTIONCHECK_H
#define LLVM_CLANG_TOOLS_EXTRA_CLANG_TIDY_MISC_YARDENCOMMENTEXPLAINFUNCTIONCHECK_H

#include "../ClangTidyCheck.h"

namespace clang {
namespace tidy {
namespace misc {

class YardenCommentExplainFunctionCheck : public ClangTidyCheck {
public:
  YardenCommentExplainFunctionCheck(StringRef Name, ClangTidyContext *Context);
  void registerMatchers(ast_matchers::MatchFinder *Finder) override;
  void check(const ast_matchers::MatchFinder::MatchResult &Result) override;
};

} // namespace misc
} // namespace tidy
} // namespace clang

#endif // LLVM_CLANG_TOOLS_EXTRA_CLANG_TIDY_MISC_YARDENCOMMENTEXPLAINFUNCTIONCHECK_H
