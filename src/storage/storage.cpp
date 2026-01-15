#include "chess/storage/storage.hpp"
#include <fstream>
#include <iostream>
#include <vector>
#include <string>
#include <cctype>

void writefile( std::string filename,  std::string text)
{
    std::ofstream file(filename);

    if (!file) {
        std::cerr << "Failed to open file for writing.\n";
        return;
    }

    file << text;
    file.close();
}

 std::vector< std::string> readFile( std::string filename,  std::string& game_specs)
{
     std::vector< std::string> moves; 
     std::string tmp = "";
     std::ifstream file(filename);
    if (!file) 
    {
        std::cerr << "Failed to open file for writing.\n";
        return {};
    }
    
    do
    {
        if(tmp[0] != '[')
        getline(file, tmp);
        game_specs += tmp + '\n';
    }while(tmp[0] == '[');

    while (file >> tmp) {

        if (isdigit(tmp[0]) || tmp[0] == '.')
            continue;

        if (tmp == "1-0" || tmp == "0-1" || tmp == "1/2-1/2")
            break;

        moves.push_back(tmp);
    }

    return moves;
}