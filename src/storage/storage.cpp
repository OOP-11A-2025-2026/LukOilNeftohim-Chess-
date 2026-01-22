#include <fstream>
#include <iostream>
#include <vector>
#include <string>
#include <cctype>

void writefile(std::string filename, std::string text)
{
    std::ofstream file(filename);

    if (!file)
    {
        std::cerr << "Failed to open file for writing.\n";
        return;
    }

    file << text;
    file.close();
}

std::vector<std::string> readFile(std::string filename, std::string &game_specs)
{
    std::vector<std::string> moves;
    std::string tmp = "";
    std::string temp2 = "";
    std::ifstream file(filename);
    if (!file)
    {
        std::cerr << "Failed to open file for writing.\n";
        return {};
    }

    while (std::getline(file, tmp))
    {
        if (tmp.empty() || tmp[0] != '[')
            break;

        game_specs += tmp + '\n';
    }

    while (file >> tmp)
    {

        if (isdigit(tmp[0]))
        {
            for (int i = 1; i < tmp.size(); i++)
            {
                if (i == 1 && isdigit(tmp[i]))
                    continue;
                if (tmp[i] != '.')
                {
                    temp2.push_back(tmp[i]);
                }
            }
            moves.push_back(temp2);
            temp2 = "";
            continue;
        }
        if (tmp[0] == '{')
        {
            continue;
        }

        if (tmp == "1-0" || tmp == "0-1" || tmp == "1/2-1/2")
            break;

        moves.push_back(tmp);
    }

    return moves;
}