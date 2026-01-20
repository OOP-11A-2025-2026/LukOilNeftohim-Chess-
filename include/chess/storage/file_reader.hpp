#ifndef CHESS_STORAGE_FILE_READER_HPP
#define CHESS_STORAGE_FILE_READER_HPP

#include <string>
#include <optional>
#include <vector>
#include <cstdint>

namespace chess
{

    std::optional<std::string> read_text_file(const std::string &filepath);
    std::optional<std::vector<uint8_t>> read_binary_file(const std::string &filepath);

    class FileReader
    {
        void *file_handle; // Скрит implementation detail

    public:
        FileReader(const std::string &filepath);
        ~FileReader();

        FileReader(const FileReader &) = delete;
        FileReader &operator=(const FileReader &) = delete;

        bool is_open() const;
        std::optional<std::string> read_line();
        void close();
    };

} // namespace chess

#endif
