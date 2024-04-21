// Copyright (c) 2023 Fujicoin Developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include "logprintf.h"

#include <clang-tidy/ClangTidyModule.h>
#include <clang-tidy/ClangTidyModuleRegistry.h>

class FujicoinModule final : public clang::tidy::ClangTidyModule
{
public:
    void addCheckFactories(clang::tidy::ClangTidyCheckFactories& CheckFactories) override
    {
        CheckFactories.registerCheck<fujicoin::LogPrintfCheck>("fujicoin-unterminated-logprintf");
    }
};

static clang::tidy::ClangTidyModuleRegistry::Add<FujicoinModule>
    X("fujicoin-module", "Adds fujicoin checks.");

volatile int FujicoinModuleAnchorSource = 0;
