// src/tools/tool_registry.cpp — YOU IMPLEMENT (spec §4.3).
//
// A hand-written hash table. No std::unordered_map / std::map / std::set.
// Every function below is a stub so the starter compiles. Replace each body.

#include "tools/tool_registry.h"
#include <cstdint>

ToolRegistry::ToolRegistry() {
    buckets_.resize(16);
}
ToolRegistry::~ToolRegistry() = default;   // unique_ptrs in your buckets free the tools

std::size_t ToolRegistry::hash(std::string_view key) noexcept {
    std::uint64_t h = 1469598103934665603ULL;

    for (unsigned char c : key) {
        h ^= c;
        h *= 1099511628211ULL;
    }

    return static_cast<std::size_t>(h);
}

bool ToolRegistry::add(std::unique_ptr<Tool> tool) {

    // Reject a null tool
    if (tool == nullptr) {
        return false;
    }

    // Reject duplicate tool names
    if (find(tool->name()) != nullptr) {
        return false;
    }

    // Find the bucket for this tool
    std::size_t index = hash(tool->name()) % buckets_.size();

    // Insert the tool into that bucket
    buckets_[index].push_back(std::move(tool));

    // Increase the number of registered tools
    count_++;

    return true;
}

Tool* ToolRegistry::find(std::string_view name) const {
    // Determine which bucket the tool belongs in
    std::size_t index = hash(name) % buckets_.size();

    // Search through the tools in that bucket
    for (const auto& tool : buckets_[index]) {
        if (tool->name() == name) {
            return tool.get();
        }
    }

    // Tool was not found
    return nullptr;
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
