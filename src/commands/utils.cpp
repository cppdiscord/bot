#include "commands.h"

#include <random>
#include <string>
#include <fstream>
#include <unordered_map>

std::string cmd::utils::readFileLine(const std::string& path, int &index)
{
    std::ifstream file(path);

    std::string line;
    if (file.is_open())
    {
        for (int i = 0; std::getline(file, line) && i < index; i++);

        // validate next line
        if (std::string next; !getline(file, next) || line.length() == 0)
            index = 0;
        else
            index++;

        file.close();
    }
    else
        line = "[!] Could not open " + path;
    return line;
}

std::string cmd::utils::readFileLineCached(const std::string& path, int index)
{
    static std::unordered_map<std::string, std::vector<std::string>> seen_files;
    if (seen_files.find(path) == seen_files.end())
    {
        seen_files[path] = {};
        std::ifstream file(path);

        if (!file.is_open())
        {
            return "[!] Could not open " + path;
        }

        std::string line;
        while (std::getline(file, line)) {
            seen_files[path].push_back(line);
        }

        file.close();

        std::vector<std::string> lines = seen_files.at(path);
        if (index >= lines.size())
        {
            return "[!] Out of range";
        }

        return lines.at(index);
    }
    else
    {
        std::vector<std::string> lines = seen_files.at(path);
        if (index >= lines.size())
        {
            return "[!] Out of range";
        }

        return lines.at(index);
    }
}

int cmd::utils::getRandomNumber(int min, int max)
{
    if (min > max)
    {
        return 0;
    }

    static std::random_device rd;
    static std::mt19937 gen(rd());
    std::uniform_int_distribution<int> distrib(min, max);

    return distrib(gen);
}
