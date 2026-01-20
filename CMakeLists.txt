---FILEPATH /CMakeLists.txt
---FIND
```

```
---REPLACE
```
cmake_minimum_required(VERSION 3.10)

project(ProjectName)

# Include Catch2
include(FetchContent)
FetchContent_Declare(
  catch2
  GIT_REPOSITORY https://github.com/catchorg/Catch2.git
  GIT_TAG v2.13.7
)
FetchContent_MakeAvailable(catch2)

# Add test files
enable_testing()
add_executable(tests
    tests/core/board_test.cpp
    tests/core/move_test.cpp
    tests/core/piece_test.cpp
    tests/core/rules_test.cpp
    tests/engine/eval_test.cpp
    tests/engine/search_test.cpp
    tests/engine/uci_test.cpp
    tests/parser/fen_test.cpp
    tests/parser/png_test.cpp
    tests/parser/san_test.cpp
    tests/storage/file_reader_test.cpp
    tests/storage/file_writer_test.cpp
    tests/storage/storage_test.cpp
    tests/main_test.cpp
)

target_link_libraries(tests PRIVATE Catch2::Catch2)

include(CTest)
include(Catch)
```
---COMPLETE