#include "commands.h"
#include "../globals/globals.h"
#include <fstream>
#include <random>
#include <algorithm>
#include <map>
#include <vector>
#include <string>

namespace cmd
    {
    namespace coding
        {
        const std::map<std::string, std::string> difficultyFiles = {
            {"Beginner", "src/res/coding/beginner.txt"},
            {"Intermediate", "src/res/coding/intermediate.txt"},
            {"Advanced", "src/res/coding/advanced.txt"},
            {"Expert", "src/res/coding/expert.txt"},
            {"Master", "src/res/coding/master.txt"}
            };

        std::map<std::string, std::vector<std::string>> questionCache;
        bool loaded = false;

        void loadQuestions() {
            if (loaded) return;

            for (const auto& [difficulty, filepath] : difficultyFiles) {
                std::ifstream file(filepath);

                if (!file.is_open()) {
                    std::cerr << "Failed to open: " << filepath << std::endl;
                    continue;
                    }

                std::vector<std::string> questions;
                std::string line;

                while (std::getline(file, line)) {
                    if (!line.empty()) {
                        questions.push_back(line);
                        }
                    }
                file.close();

                std::random_device rd;
                std::mt19937 gen(rd());
                std::shuffle(questions.begin(), questions.end(), gen);

                questionCache[difficulty] = questions;
                }
            loaded = true;
            }

        std::string getRandomQuestion(const std::string& difficulty) {
            auto it = questionCache.find(difficulty);
            if (it == questionCache.end() || it->second.empty()) {
                return "No questions available for " + difficulty + " difficulty.";
                }

            static std::map<std::string, int> indices;
            int& index = indices[difficulty];
            const std::vector<std::string>& questions = it->second;

            std::string question = questions[index % questions.size()];
            index++;

            return question;
            }
        }
    }

void cmd::codingCommand(dpp::cluster& bot, const dpp::slashcommand_t& event)
    {
    coding::loadQuestions();
    std::string difficulty = "Beginner";
    try {
        auto param = event.get_parameter("difficulty");
        if (!std::holds_alternative<std::monostate>(param)) {
            difficulty = std::get<std::string>(param);
            }
        }
    catch (...) {}

    std::string question = coding::getRandomQuestion(difficulty);
    dpp::embed embed = dpp::embed()
        .set_color(globals::color::defaultColor)
        .set_title("Coding Challenge - " + difficulty)
        .set_description(question)
        .add_field("Difficulty", difficulty, true)
        .add_field("Need help?", "Ask in <#" + std::to_string(globals::channels::HELP_CHANNEL_ID) + ">", true)
        .set_footer(dpp::embed_footer().set_text("Good luck! Share your solution in #code-review"))
        .set_timestamp(time(0));

    event.reply(embed);
    }