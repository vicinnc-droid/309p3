// tests/p3/test_p3.cpp — YOUR P3 TEST SUITE.
//
// The five tests below are worked examples. Keep them, and add your own
// until you have AT LEAST 15 (spec §6 lists what must be covered).
// Name every test <area>_<what>: reg_ for the registry, ws_ for the
// workspace, calc_ for the calculator.
//
// Build & run:   cmake --build build && ./build/test_p3
// One area:      ./build/test_p3 reg_        (names starting with "reg_")
// One test:      ./build/test_p3 =reg_add_and_find

#include "../mini_test.h"
#include "../test_helpers.h"     // TempDir, write_text, args1, ...
#include "tools/calculator.h"
#include "tools/tool_registry.h"
#include "tools/workspace.h"
#include <cstdio>
#include <memory>
#include <string>
#include <vector>

namespace fs = std::filesystem;

namespace {

// A tool whose name you choose — use it to fill the registry in tests.
class NamedTool : public Tool {
public:
    explicit NamedTool(std::string n) : n_(std::move(n)), schema_(JsonValue::object()) {}
    std::string name() const override { return n_; }
    std::string description() const override { return "tool being testeed"; }


    const JsonValue& schema() const override { return schema_; }
    ToolResult invoke(const JsonValue&, ToolContext&) override { return ToolResult::success(n_); }
private:

    std::string n_;
    JsonValue   schema_;   // an empty JSON object: {}
};

// Tokenize with the provided tokenizer, then run YOUR calc_evaluate.
bool eval(const std::string& expr, double* value, std::string* error) {
    std::vector<CalcToken> tokens;


    if (!calc_tokenize(expr, &tokens, error)) return false;


    return calc_evaluate(tokens, value, error);
}

}  // namespace

// ---------------------------------------------------------------- examples

TEST(reg_add_and_find) {
    ToolRegistry toolReggy;

    auto t = std::make_unique<NamedTool>("alpha");


    Tool* variable = t.get();                        // remember the address before moving

    REQUIRE(toolReggy.add(std::move(t)));

    CHECK(toolReggy.size() == 1);
    CHECK(toolReggy.find("alpha") == variable);              // same object, not a copy
    CHECK(toolReggy.find("beta") == nullptr);
}

TEST(reg_duplicate_keeps_original) {
    ToolRegistry toolReggy;


    auto first = std::make_unique<NamedTool>("alpha");
    Tool* original = first.get();
    REQUIRE(toolReggy.add(std::move(first)));


    CHECK(!toolReggy.add(std::make_unique<NamedTool>("alpha")));
    CHECK(toolReggy.find("alpha") == original);
    CHECK(toolReggy.size() == 1);
}

TEST(ws_rejects_parent_escape) {
    TempDir tDir;
    Workspace ws(tDir.path());

    fs::path out;

    std::string reasoning;
    REQUIRE(ws.resolve("notes/todo.txt", &out, &reasoning));   // sanity: accepts *something*


    CHECK(out == ws.root() / "notes" / "todo.txt");
    CHECK(!ws.resolve("../etc/passwd", &out, &reasoning));
    CHECK(!reasoning.empty());
}

TEST(calc_precedence) {
    double value = 0;
    std::string err;
    REQUIRE(eval("2 + 3 * 4", &value, &err));

    CHECK(value == 14);
}

TEST(calc_division_by_zero) {
    double value = 0;
    std::string err;
    CHECK(!eval("1 / 0", &value, &err));
    CHECK(err == "division by zero");
}

// ------------------------------------------------------------- your tests
//
// Still to write (spec §6 has the full list). Suggested names:
//   reg_null_rejected, reg_for_each_visits_each_once
//   reg_10000_tools               REQUIRED: print bucket_count / load_factor /
//                                 max_probe_length, check the bound
//   reg_alternative_hash          REQUIRED: longest chain of a weak hash
//   ws_<case>                     REQUIRED: every row of the table in spec §4.4
//   calc_<case>                   left-to-right, parentheses, negatives

//testing tool cannt be added
TEST(reg_null_rejected) {
    ToolRegistry toolReggy;

    CHECK(!toolReggy.add(nullptr));
    CHECK(toolReggy.size() == 0);
    CHECK(toolReggy.find("alpha") == nullptr);
}

// every tool visited one time
TEST(reg_for_each_visits_each_once) {
    ToolRegistry toolReggy;

    REQUIRE(toolReggy.add(std::make_unique<NamedTool>("alpha")));
    REQUIRE(toolReggy.add(std::make_unique<NamedTool>("beta")));
    REQUIRE(toolReggy.add(std::make_unique<NamedTool>("gamma")));



    int count = 0;

    toolReggy.for_each([&](const Tool& tool) {
        CHECK(toolReggy.find(tool.name()) != nullptr);
        count++;
    });

    CHECK(count == 3);
    CHECK(toolReggy.size() == 3);
}

// try 10000 tools
TEST(reg_10000_tools) {
    ToolRegistry toolReggy;

    //first tool is added and obtained its adress
    auto first = std::make_unique<NamedTool>("tool_0");
    Tool* original = first.get();

    REQUIRE(toolReggy.add(std::move(first)));

    // add rest of the tools left
    for (int i = 1; i < 10000; i++) {
        std::string name = "tool_" + std::to_string(i);

        REQUIRE(toolReggy.add(std::make_unique<NamedTool>(name)));

        // Verify the load factor after every insertion
        CHECK(toolReggy.load_factor() <= 1.0);
    }

    // Verify all tools can still be found
    for (int i = 0; i < 10000; i++) {
        std::string name = "tool_" + std::to_string(i);
        CHECK(toolReggy.find(name) != nullptr);
    }

    // verification
    CHECK(toolReggy.find("tool_0") == original);
    CHECK(toolReggy.size() == 10000);
    CHECK(toolReggy.max_probe_length() <= 64);
    std::printf("Bucket count: %zu\n", toolReggy.bucket_count());
    std::printf("Load factor: %.4f\n", toolReggy.load_factor());
    std::printf("Max probe length: %zu\n", toolReggy.max_probe_length());
}

//compare FNV-1a with a weak hash function
TEST(reg_alternative_hash) {
    ToolRegistry toolReggy;

    // add 10000 tools
    for (int i = 0;i < 10000; i++) {
        std::string name = "tool_" + std::to_string(i);
        REQUIRE(toolReggy.add(std::make_unique<NamedTool>(name)));
    }

    // create the buckets for simulation
    std::vector<int> weakBuckets(toolReggy.bucket_count(), 0);

    for (int i= 0; i < 10000;i++) {
        std::string name = "tool_" + std::to_string(i);

        // weak has using first character
        std::size_t weakHash = static_cast<unsigned char>(name[0]) + name.size();



        std::size_t index = weakHash % weakBuckets.size();

        weakBuckets[index]++;
    }

    int weakMax = 0;

    for (int count : weakBuckets) {
        if (count > weakMax) {
            weakMax = count;
        }
    }

    // print
    std::printf("FNV-1a max chain: %zu\n", toolReggy.max_probe_length());
    std::printf("Weak hash max chain: %d\n", weakMax);

    CHECK(toolReggy.max_probe_length() <= 64);
    CHECK(weakMax > static_cast<int>(toolReggy.max_probe_length()));
}

// makeing sure absolute paths are indeed rejected
TEST(ws_rejects_absolute_path) {
    TempDir tDir;
    Workspace ws(tDir.path());

    fs::path out;
    std::string reasoning;

    CHECK(!ws.resolve("/etc/passwd", &out, &reasoning));
    CHECK(!reasoning.empty());
}

// test reltive paths
TEST(ws_normalizes_paths) {
    TempDir tDir;
    Workspace ws(tDir.path());

    fs::path out;
    std::string reasoning;


    REQUIRE(ws.resolve("./notes//todo.txt", &out, &reasoning));
    CHECK(out == ws.root() / "notes" / "todo.txt");
    REQUIRE(ws.resolve("a/../b", &out, &reasoning));
    CHECK(out == ws.root() / "b");
    REQUIRE(ws.resolve("a/..", &out, &reasoning));
    CHECK(out == ws.root());
}

// make sure empty path is not allowed
TEST(ws_rejects_empty_path) {
    TempDir tDir;
    Workspace ws(tDir.path());

    fs::path out;
    std::string reasoning;

    CHECK(!ws.resolve("", &out, &reasoning));
    CHECK(!reasoning.empty());
}

// null character
TEST(ws_rejects_null_character) {
    TempDir tDir;
    Workspace ws(tDir.path());

    fs::path out;
    std::string reasoning;

    std::string badPath = std::string("notes") + '\0' + "/todo.txt";

    CHECK(!ws.resolve(badPath, &out, &reasoning));
    CHECK(!reasoning.empty());
}

// Test that paths cannot escape the workspace
TEST(ws_rejects_multiple_parent_paths) {
    TempDir tDir;
    Workspace ws(tDir.path());

    fs::path out;
    std::string reasoning;

    CHECK(!ws.resolve("a/../../b", &out, &reasoning));
    CHECK(!reasoning.empty());
    CHECK(!ws.resolve("notes/../../ws/notes/todo.txt", &out, &reasoning));
    CHECK(!reasoning.empty());
}

// two period files
TEST(ws_accepts_double_period_filename) {
    TempDir tDir;
    Workspace ws(tDir.path());

    fs::path out;
    std::string reasoning;

    REQUIRE(ws.resolve("my..notes.txt", &out, &reasoning));
    CHECK(out == ws.root() / "my..notes.txt");
}

//test the subtraction
TEST(calc_left_to_right) {
    double value = 0;
    std::string err;

    REQUIRE(eval("10 - 4 - 3", &value, &err));
    CHECK(value == 3);

    REQUIRE(eval("100 / 10 / 5", &value, &err));
    CHECK(value == 2);
}

//test the parentheses
TEST(calc_parentheses) {
    double value = 0;
    std::string err;

    REQUIRE(eval("(2 + 3) * 4", &value, &err));
    CHECK(value == 20);

    REQUIRE(eval("2 * (3 + 4) - 5", &value, &err));
    CHECK(value == 9);
}

// testing negatives
TEST(calc_negative_numbers) {
    double value = 0;
    std::string err;

    REQUIRE(eval("-3 * -3", &value, &err));
    CHECK(value == 9);
}

//testing decimals
TEST(calc_decimals) {
    double value = 0;
    std::string err;

    REQUIRE(eval("7 / 2", &value, &err));
    CHECK(value == 3.5);
}

// div by 0
TEST(calc_parentheses_division_by_zero) {
    double value = 0;
    std::string err;

    CHECK(!eval("1 / (2 - 2)", &value, &err));
    CHECK(err == "division by zero");
}

int main(int argc, char** argv) { return mt::run_all(argc, argv); }
