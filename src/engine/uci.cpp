#include "chess/engine/uci.hpp"

#include <sstream>
#include <stdexcept>
#include <iostream>
#include <cstring>

#ifdef _WIN32
#include <windows.h>
#else
#include <unistd.h>
#include <sys/wait.h>
#endif

namespace chess
{

    UCIEngine::UCIEngine() {}

    UCIEngine::~UCIEngine()
    {
        stop();
    }

    void UCIEngine::start(const std::string &path)
    {
#ifdef _WIN32
        SECURITY_ATTRIBUTES sa{};
        sa.nLength = sizeof(sa);
        sa.bInheritHandle = TRUE;

        HANDLE in_r, in_w;
        HANDLE out_r, out_w;

        if (!CreatePipe(&out_r, &out_w, &sa, 0))
            throw std::runtime_error("CreatePipe failed");
        if (!SetHandleInformation(out_r, HANDLE_FLAG_INHERIT, 0))
            throw std::runtime_error("SetHandleInformation failed");

        if (!CreatePipe(&in_r, &in_w, &sa, 0))
            throw std::runtime_error("CreatePipe failed");
        if (!SetHandleInformation(in_w, HANDLE_FLAG_INHERIT, 0))
            throw std::runtime_error("SetHandleInformation failed");

        STARTUPINFOA si{};
        si.cb = sizeof(si);
        si.dwFlags = STARTF_USESTDHANDLES;
        si.hStdInput = in_r;
        si.hStdOutput = out_w;
        si.hStdError = out_w;

        PROCESS_INFORMATION pi{};
        std::string cmd = path;

        if (!CreateProcessA(
                nullptr,
                cmd.data(),
                nullptr,
                nullptr,
                TRUE,
                0,
                nullptr,
                nullptr,
                &si,
                &pi))
            throw std::runtime_error("CreateProcess failed");

        CloseHandle(in_r);
        CloseHandle(out_w);

        engine_in = in_w;
        engine_out = out_r;
        process_handle = pi.hProcess;
        CloseHandle(pi.hThread);

#else
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
#endif

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
#ifdef _WIN32
        if (process_handle)
        {
            send("quit");
            CloseHandle(engine_in);
            CloseHandle(engine_out);
            WaitForSingleObject(process_handle, INFINITE);
            CloseHandle(process_handle);

            process_handle = nullptr;
            engine_in = engine_out = INVALID_HANDLE_VALUE;
        }
#else
        if (engine_pid != -1)
        {
            send("quit");
            close(engine_in);
            close(engine_out);
            waitpid(engine_pid, nullptr, 0);
            engine_pid = -1;
        }
#endif
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

#ifdef _WIN32
        DWORD written;
        WriteFile(engine_in, line.c_str(), (DWORD)line.size(), &written, nullptr);
#else
        write(engine_in, line.c_str(), line.size());
#endif
    }

    std::string UCIEngine::read_line()
    {
        std::string line;
        char c;

#ifdef _WIN32
        DWORD read;
        while (ReadFile(engine_out, &c, 1, &read, nullptr) && read == 1)
        {
            if (c == '\n')
                break;
            if (c != '\r')
                line += c;
        }
#else
        while (read(engine_out, &c, 1) == 1)
        {
            if (c == '\n')
                break;
            if (c != '\r')
                line += c;
        }
#endif

        return line;
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

        while (true)
        {
            std::string line = read_line();

            if (line.rfind("bestmove", 0) == 0)
            {
                std::stringstream ss(line);
                std::string tmp, move_str;
                ss >> tmp >> move_str;

                Move mv = string_to_move(board, move_str);

                std::cout << "Bestmove : " << move_str << std::endl;
                uint8_t from = move_from(mv);
                uint8_t to = move_to(mv);

                std::cout << "MV: " << move_to_string(board, mv) << std::endl;

                if (mv == 0)
                    throw std::runtime_error("Illegal UCI move: " + move_str);

                move_history.push_back(move_str);

                return mv;
            }
        }
    }

} // namespace chess
