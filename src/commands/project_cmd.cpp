#include "commands.h"
#include "../globals/globals.h"
#include "nlohmann/json.hpp"

using json = nlohmann::json;

static bool buttonClicked = false;

static bool buttonClicked = false;

void cmd::projectCommand(dpp::cluster& bot, const dpp::slashcommand_t& event)
{
    static int index = -1;
    index++;

    std::ifstream projectFile("res/project.json");
    if (!projectFile.is_open()) {
        event.reply("Failed to open project file.");
        return;
    }

    json data;
    try {
        projectFile >> data;
    } catch (const json::parse_error& e) {
        event.reply("Failed to parse project file.");
        return;
    }

    if (!data.contains("projects") || !data["projects"].is_array() || index >= data["projects"].size()) {
        event.reply("Invalid project data or index out of bounds.");
        return;
    }

    const auto& project = data["projects"][index];
    if (!project.contains("title") || !project.contains("description")) {
        event.reply("Project data is missing required fields.");
        return;
    }

    const std::string projectTitle = project["title"];
    const std::string projectDescription = project["description"];
    const std::string projectHint = project.contains("hint") ? project["hint"] : "No hint available.";


    dpp::embed embed = dpp::embed()
        .set_color(globals::color::defaultColor)
        .add_field("Project Idea", projectTitle)
        .add_field("Description", projectDescription);

    dpp::message message(event.command.channel_id, embed);

    // Add hint button with index as custom_id
    message.add_component(
        dpp::component().add_component(
            dpp::component()
            .set_label("Hint")
            .set_type(dpp::cot_button)
            .set_style(dpp::cos_primary)
            .set_id("hint_button_" + std::to_string(index))
        )
    );

    event.reply(message);

    // Reset the button click state when the /project command is run
    buttonClicked = false;

    bot.on_button_click([&bot, &data](const dpp::button_click_t& event)
        {
            json data;
            try {
                std::ifstream projectFile("res/project.json");
                projectFile >> data;
            }
            catch (const json::parse_error& e) {
                event.reply("Failed to parse project file.");
                return;
            }
            
            const auto& project = data["projects"][index];
            const std::string hint = project.contains("hint") ? project["hint"] : "No hint available.";
            if (event.custom_id.rfind("hint_button_", 0) == 0 && !buttonClicked)
            {

                dpp::embed hintEmbed = dpp::embed()
                    .set_color(globals::color::defaultColor)
                    .add_field("Hint", hint);

                dpp::message hintMessage(event.command.channel_id, hintEmbed);
                event.reply(hintMessage);

                buttonClicked = true;
            }
        }
    );
}
