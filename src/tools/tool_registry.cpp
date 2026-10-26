// src/tools/tool_registry.cpp — YOU IMPLEMENT (spec §4.3).
//
// A hand-written hash table. No std::unordered_map / std::map / std::set.
// Every function below is a stub so the starter compiles. Replace each body.

#include "tools/tool_registry.h"

ToolRegistry::ToolRegistry() {
    buckets_.resize(16);
}
ToolRegistry::~ToolRegistry() = default;   // unique_ptrs in your buckets free the tools

std::size_t ToolRegistry::hash(std::string_view /*key*/) noexcept {
    return 0;   // TODO: FNV-1a (spec Appendix A) or another real string hash
}

bool ToolRegistry::add(std::unique_ptr<Tool> /*tool*/) {
    return false;   // TODO
}

Tool* ToolRegistry::find(std::string_view /*name*/) const {
    return nullptr;   // TODO
}

std::size_t ToolRegistry::size() const noexcept {
    return count_;
}

void ToolRegistry::for_each(const std::function<void(const Tool&)>& /*fn*/) const {
    // TODO
}

std::size_t ToolRegistry::bucket_count() const noexcept {
    return buckets_.size();
}

double ToolRegistry::load_factor() const noexcept {
    return static_cast<double>(count_) / buckets_.size();
}

std::size_t ToolRegistry::max_probe_length() const {
    return 0;   // TODO
}
