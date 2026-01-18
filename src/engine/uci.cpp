#include "chess/engine/uci.hpp"
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <sstream>
#include <cstring>
#include <stdexcept>
#include <iostream>

namespace chess
{

    UCIEngine::UCIEngine() {}

    UCIEngine::~UCIEngine()
    {
        stop();
    }

    void UCIEngine::start(const std::string &path)
    {
        int in_pipe[2];
        int out_pipe[2];

        if (pipe(in_pipe) == -1 || pipe(out_pipe) == -1)
            throw std::runtime_error("pipe failed");

        engine_pid = fork();
        if (engine_pid == -1)
            throw std::runtime_error("fork failed");

        if (engine_pid == 0)
        {
            dup2(in_pipe[0], STDIN_FILENO);
            dup2(out_pipe[1], STDOUT_FILENO);
            dup2(out_pipe[1], STDERR_FILENO);

            close(in_pipe[1]);
            close(out_pipe[0]);

            execl(path.c_str(), path.c_str(), nullptr);
            _exit(1);
        }

        close(in_pipe[0]);
        close(out_pipe[1]);

        engine_in = in_pipe[1];
        engine_out = out_pipe[0];

        send("uci");
        while (read_line().find("uciok") == std::string::npos)
        {
        }

        send("isready");
        while (read_line().find("readyok") == std::string::npos)
        {
        }
    }

    void UCIEngine::stop()
    {
        if (engine_pid != -1)
        {
            send("quit");
            close(engine_in);
            close(engine_out);
            waitpid(engine_pid, nullptr, 0);
            engine_pid = -1;
        }
    }

    void UCIEngine::new_game()
    {
        move_history.clear();
        send("ucinewgame");
        send("isready");
        while (read_line().find("readyok") == std::string::npos)
        {
        }
    }

    void UCIEngine::send(const std::string &cmd)
    {
        std::string line = cmd + "\n";
        write(engine_in, line.c_str(), line.size());
    }

    std::string UCIEngine::read_line()
    {
        char buf[256];
        int i = 0;

        while (i < 255)
        {
            char c;
            if (read(engine_out, &c, 1) <= 0)
                break;
            if (c == '\n')
                break;
            buf[i++] = c;
        }
        buf[i] = '\0';
        return std::string(buf);
    }

    Move UCIEngine::get_bestmove(const BoardState &board, int depth)
    {
        std::stringstream pos;
        pos << "position startpos";
        if (!move_history.empty())
        {
            pos << " moves";
            for (auto &m : move_history)
                pos << " " << m;
        }

        send(pos.str());
        send("go depth " + std::to_string(depth));

        std::string line;
        while (true)
        {
            line = read_line();
            if (line.rfind("bestmove", 0) == 0)
            {
                std::stringstream ss(line);
                std::string tmp, move_str;
                ss >> tmp >> move_str;

                Move mv = string_to_move(board, move_str);
                move_history.push_back(move_str);
                return mv;
            }
        }
    }

} // namespace chess