#include <fstream>
#include <iterator>
#include "fileio.h"

std::string readFile(const std::string &filename) {
    std::string data;

    std::fstream file{filename, std::ios::in};

    std::copy(std::istream_iterator<char>{file}, std::istream_iterator<char>{}, std::back_inserter(data));

    file.close();

    return data;
}