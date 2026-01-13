#ifndef STORAGE_HPP
#define STORAGE_HPP


#include <fstream>
#include <string>
#include <iostream>
#include <vector>

void writefile( std::string filename,  std::string text);

 std::vector< std::string> readFile( std::string filename,  std::string& game_specs);

#endif