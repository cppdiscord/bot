#include "commands.h"

#include <string>
#include <fstream>
#include <vector>

std::string cmd::utils::readFileLine(const std::string& path, int &index)
{
    std::ifstream file(path);
    std::string line;

    if (file.is_open())
    {
        std::vector<std::string> lines;
        std::string currentLine;
        while (std::getline(file, currentLine))
        {
            if (!currentLine.empty())
            {
                lines.push_back(currentLine);
            }
        }
        file.close();

        if (lines.empty())
        {
            return "[!] No valid lines found in " + path;
        }

        index = index % lines.size();
        line = lines[index];
        index = (index + 1) % lines.size(); // Prepare for next call
    }
    else
    {
        line = "[!] Could not open " + path;
    }
    return line;
}
