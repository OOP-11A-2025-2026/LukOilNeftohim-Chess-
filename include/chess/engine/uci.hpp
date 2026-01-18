#ifndef CHESS_ENGINE_UCI_HPP
#define CHESS_ENGINE_UCI_HPP

#include "../core/board.hpp"
#include "../core/move.hpp"
#include <string>
#include <vector>

namespace chess
{

    class UCIEngine
    {
    public:
        UCIEngine();
        ~UCIEngine();

        void start(const std::string &path_to_stockfish);
        void stop();

        Move get_bestmove(const BoardState &board, int depth);
        void new_game();

    private:
        void send(const std::string &cmd);
        std::string read_line();

        int engine_in = -1;  // write TO engine
        int engine_out = -1; // read FROM engine
        pid_t engine_pid = -1;

        std::vector<std::string> move_history;
    };

} // namespace chess

#endif