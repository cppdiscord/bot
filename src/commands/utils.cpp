#include "commands.h"

#include <string>
#include <fstream>

std::string cmd::utils::readFileLine(const std::string& path, int &index)
{
    std::ifstream file(path);

    std::string line;
    if (file.is_open())
    {
        for (int i = 0; std::getline(file, line) && i < index; i++);

        // validate next line
        std::string next;
        if (!getline(file, next) || line.length() == 0)
            index = 0;
        else
            index++;

        file.close();
    }
    else
        line = "[!] Could not open " + path;
    return line;
}
