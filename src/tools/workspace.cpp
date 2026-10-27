// src/tools/workspace.cpp — YOU IMPLEMENT (spec §4.4).
//
// Workspace::resolve: a purely lexical check, steps 1–6 of the spec.
// Do not touch the filesystem here (no exists(), canonical(), ...).
// The constructor, root() and check_symlinks() are provided in
// workspace_provided.cpp.

#include "tools/workspace.h"

namespace fs = std::filesystem;

bool Workspace::resolve(std::string_view thePath, fs::path* out,
                        std::string* reason) const {
                        

                            
    // do not accept path
    if (thePath.empty()) {
        *reason= "this paht is empty";
        
        return  false;
    }
    // do not accept paths with pointers
if (thePath.find('\0')!=std::string_view::npos) {




    *reason = "null pinter detected!!!";

    return false;
}

    //string becomes file system path
fs::path p{std::string(thePath)};
// rooted path violation
if (p.is_absolute()  || p.has_root_path()) {
    *reason = "this has an absolute path";



   return false;
}

// normalize path
fs::path n = p.lexically_normal();

// path esacping workplace protocall
if (!n.empty() && *n.begin() == "..") {
    *reason = "path escapes workspace";

    return false;
}
// full path creationn
if (n== ".") {
    *out = root();
} 
else {


    *out  = root() / n;
}


return true;// the path works



}
