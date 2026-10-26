// src/tools/workspace.cpp — YOU IMPLEMENT (spec §4.4).
//
// Workspace::resolve: a purely lexical check, steps 1–6 of the spec.
// Do not touch the filesystem here (no exists(), canonical(), ...).
// The constructor, root() and check_symlinks() are provided in
// workspace_provided.cpp.

#include "tools/workspace.h"

namespace fs = std::filesystem;

bool Workspace::resolve(std::string_view user_path, fs::path* out,
                        std::string* reason) const {

    // Step 1: Reject an empty path
    if (user_path.empty()) {
        *reason = "empty path";
        return false;
    }

    // Step 2: Reject paths containing a null character
if (user_path.find('\0') != std::string_view::npos) {
    *reason = "path contains null character";
    return false;
}

// Temporary until we finish the remaining steps
    *reason = "Workspace::resolve not fully implemented yet";
    return false;
}
