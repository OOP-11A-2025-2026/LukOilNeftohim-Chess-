#ifndef CHESS_ENGINE_UCI_HPP
#define CHESS_ENGINE_UCI_HPP

#include "../core/board.hpp"
#include "../core/move.hpp"

#include <string>
#include <vector>

#ifdef _WIN32
#include <windows.h>
#else
#include <sys/types.h>
#endif

namespace chess
{

    class UCIEngine
    {
    public:
        UCIEngine();
        ~UCIEngine();

        void start(const std::string &path);
        void stop();
        void new_game();

        Move get_bestmove(const BoardState &board, int depth);

    private:
        void send(const std::string &cmd);
        std::string read_line();

#ifdef _WIN32
        HANDLE engine_in = INVALID_HANDLE_VALUE;
        HANDLE engine_out = INVALID_HANDLE_VALUE;
        HANDLE process_handle = nullptr;
#else
        int engine_in = -1;
        int engine_out = -1;
        pid_t engine_pid = -1;
#endif

        std::vector<std::string> move_history;
    };

} // namespace chess

#endif
