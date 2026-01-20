#ifndef CHESS_STORAGE_FILE_WRITER_HPP
#define CHESS_STORAGE_FILE_WRITER_HPP

#include <string>

namespace chess
{

    // File writing utilities
    bool write_text_file(const std::string &filepath, const std::string &content);
    bool append_text_file(const std::string &filepath, const std::string &content);
    bool write_binary_file(const std::string &filepath, const uint8_t *data, size_t size);

    class FileWriter
    {
        void *file_handle;

    public:
        FileWriter(const std::string &filepath, bool append = false);
        ~FileWriter();

        FileWriter(const FileWriter &) = delete;
        FileWriter &operator=(const FileWriter &) = delete;

        bool is_open() const;
        bool write_line(const std::string &line);
        bool write(const std::string &content);
        void flush();
        void close();
    };

} // namespace chess

#endif
