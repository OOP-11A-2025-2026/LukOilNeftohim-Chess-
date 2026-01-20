#include "../catch2/catch_amalgamated.hpp"

#include "chess/storage/file_reader.hpp"
#include "chess/storage/file_writer.hpp"
#include "chess/storage/storage.hpp"

#include <fstream>
#include <vector>
#include <string>
#include <cstdio> // for remove()

using namespace chess;

TEST_CASE("Text file write and read")
{
    const std::string filename = "test_text.txt";
    const std::string content = "Hello, Chess!\nLine 2.";

    REQUIRE(write_text_file(filename, content));

    auto read = read_text_file(filename);
    REQUIRE(read.has_value());
    REQUIRE(read.value() == content);

    std::remove(filename.c_str());
}

TEST_CASE("Text file append")
{
    const std::string filename = "test_append.txt";
    const std::string content1 = "Line 1\n";
    const std::string content2 = "Line 2\n";

    REQUIRE(write_text_file(filename, content1));
    REQUIRE(append_text_file(filename, content2));

    auto read = read_text_file(filename);
    REQUIRE(read.has_value());
    REQUIRE(read.value() == content1 + content2);

    std::remove(filename.c_str());
}

TEST_CASE("Binary file write and read")
{
    const std::string filename = "test_bin.bin";
    std::vector<uint8_t> data{1, 2, 3, 4, 5};

    REQUIRE(write_binary_file(filename, data.data(), data.size()));

    auto read = read_binary_file(filename);
    REQUIRE(read.has_value());
    REQUIRE(read.value() == data);

    std::remove(filename.c_str());
}

TEST_CASE("writefile and readFile helpers")
{
    const std::string filename = "test_helper.txt";
    const std::string file_content = "[Event \"Test\"]\n[Site \"Local\"]\n1. e4 e5 2. Nf3 Nc6 1-0";

    writefile(filename, file_content);

    std::string specs;
    auto moves = readFile(filename, specs);

    REQUIRE(specs.find("[Event \"Test\"]") != std::string::npos);
    REQUIRE(specs.find("[Site \"Local\"]") != std::string::npos);

    REQUIRE(moves.size() == 4);
    REQUIRE(moves[0] == "e4");
    REQUIRE(moves[1] == "e5");
    REQUIRE(moves[2] == "Nf3");
    REQUIRE(moves[3] == "Nc6");

    std::remove(filename.c_str());
}

TEST_CASE("FileReader and FileWriter classes")
{
    const std::string filename = "test_class.txt";

    // Test FileWriter
    {
        FileWriter writer(filename);
        REQUIRE(!writer.is_open()); // your implementation is TODO, currently returns false
        // If you implement, we could do:
        // REQUIRE(writer.is_open());
        // REQUIRE(writer.write_line("Hello"));
        // writer.flush();
    }

    // Test FileReader
    {
        FileReader reader(filename);
        REQUIRE(!reader.is_open()); // currently returns false
        // If implemented:
        // REQUIRE(reader.is_open());
        // auto line = reader.read_line();
        // REQUIRE(line.has_value());
        // REQUIRE(line.value() == "Hello");
    }

    std::remove(filename.c_str());
}
