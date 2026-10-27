// src/tools/tool_registry.cpp — YOU IMPLEMENT (spec §4.3).
//
// A hand-written hash table. No std::unordered_map / std::map / std::set.
// Every function below is a stub so the starter compiles. Replace each body.
//Author: Victor Montalvo
//Created 10/25

#include "tools/tool_registry.h"

#include <cstdint>

ToolRegistry::ToolRegistry() {
    buckets_.resize(16);
}
ToolRegistry::~ToolRegistry() = default;  
std::size_t ToolRegistry::hash(std::string_view key) noexcept {

    std::uint64_t weirdNum= 1469598103934665603ULL;

    for (unsigned char unsignedc : key) {


        weirdNum ^= unsignedc;
        weirdNum *= 1099511628211ULL;
    }

    return static_cast<std::size_t>(weirdNum);
}

//vary is said tool
bool ToolRegistry::add(std::unique_ptr<Tool> vary) {



    // contains tool that is null
    if (vary == nullptr) {
        return false;
    }
    //contins tools with same name
    if (find(vary->name())!= nullptr) {
        return false;
    }
    // must resize when tool factor us .75
if (count_ + 1 > buckets_.size() * 0.75) {

    // 2x buckets
    std::vector<std::vector<std::unique_ptr<Tool>>> newBuckets(
        buckets_.size() *  2


    );

    // put tools into new bucket
    for (auto& bucket : buckets_) {



        for ( auto& prevtool : bucket) {

             // new bucket index
            std::size_t newIndex =
                hash(prevtool->name()) % newBuckets.size();





            // new bucket ownership
            newBuckets[newIndex].push_back(std::move(prevtool));


        }
    }
    buckets_ = std::move(newBuckets);// new table now
}

    //locate tool
    std::size_t index = hash(vary->name()) % buckets_.size();

    //assign tool with specified bucket
    buckets_[index].push_back(std::move(vary));
    count_++;


    return true;
}

Tool* ToolRegistry::find(std::string_view name) const {
    // find bucket tool is in
    std::size_t index =hash(name)% buckets_.size();

    //sift through the tools
    for (const auto& tool : buckets_[index]) {



        if (tool->name()== name) {
            return  tool.get();
        }
    }

    //for when tool is not found \/
    return nullptr;
}

std::size_t ToolRegistry::size() const noexcept {
    return count_;
}

void ToolRegistry::for_each(const std::function<void(const Tool&)>& fn) const {

    // Go through every bucket
    for (const auto& bucket : buckets_) {

        // Go through every tool in that bucket
        for (const auto& tool : bucket) {

            // Call the function on this tool
            fn(*tool);
        }
    }
}

std::size_t ToolRegistry::bucket_count() const noexcept {
    return buckets_.size();
}

double ToolRegistry::load_factor() const noexcept {
    return static_cast<double>(count_) / buckets_.size();
}

std::size_t ToolRegistry::max_probe_length() const {

    //keep track of longest chain
    std::size_t longC = 0;

    // Check every bucket
    for (const auto& bucket : buckets_) {

        // Update longC if this bucket is larger
        if (bucket.size() > longC) {
         longC = bucket.size();
        }
    }

    return longC;
}
