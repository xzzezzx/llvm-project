#ifndef LLVM_CLANG_TOOLS_EXTRA_CLANG_TIDY_MISC_YARDENCOMMENTPREFERBLOCKCHECK_H
#define LLVM_CLANG_TOOLS_EXTRA_CLANG_TIDY_MISC_YARDENCOMMENTPREFERBLOCKCHECK_H

#include "../ClangTidyCheck.h"

namespace clang {
namespace tidy {
namespace misc {

class YardenCommentPreferblockCheck : public ClangTidyCheck {
public:
  YardenCommentPreferblockCheck(StringRef Name, ClangTidyContext *Context)
      : ClangTidyCheck(Name, Context) {}
  void registerPPCallbacks(const SourceManager &SM, Preprocessor *PP,
                           Preprocessor *ModuleExpanderPP) override;
};

} // namespace misc
} // namespace tidy
} // namespace clang

#endif // LLVM_CLANG_TOOLS_EXTRA_CLANG_TIDY_MISC_YARDENCOMMENTPREFERBLOCKCHECK_H
