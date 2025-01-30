#include "commands.h"
#include "../globals/globals.h"

using json = nlohmann::json;

void cmd::projectCommand(dpp::cluster& bot, const dpp::slashcommand_t& event)
{
    static int index = 0;

    std::ifstream projectFile("res/project.json");
    if (!projectFile.is_open())
    {
        event.reply("Failed to open project file.");
        return;
    }

    json data;
    try
    {
        projectFile >> data;
    }
    catch (const json::parse_error& e)
    {
        event.reply("Failed to parse project file.");
        return;
    }

    if (!data.contains("projects") || !data["projects"].is_array())
    {
        event.reply("Invalid project data.");
        return;
    }

    // If index exceeds the number of projects, reset it to 0

    /* Debugging purposes
    std::cout << index << std::endl;
    std::cout << data["projects"].size() << std::endl;
    */

    if (index >= data["projects"].size())
    {
        index = 0;
    }

    const auto& project = data["projects"][index];
    if (!project.contains("title") || !project.contains("description"))
    {
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

    bot.on_button_click([&bot, &data](const dpp::button_click_t& event) {
        // Ignore if button id does not start with hint_button_
        if (event.custom_id.rfind("hint_button_", 0) != 0)
            return;

        // Remove button
        dpp::message updatedMsg = event.command.get_context_message();
        updatedMsg.components.clear();
        bot.message_edit(updatedMsg);

        json data;
        try
        {
            std::ifstream projectFile("res/project.json");
            projectFile >> data;
        }
        catch (const json::parse_error& e)
        {
            event.reply("Failed to parse project file.");
            return;
        }

        const int hintButtonIndex = std::stoi(event.custom_id.substr(event.custom_id.rfind('_') + 1));
        const auto& project = data["projects"][hintButtonIndex];
        const std::string hint = project.contains("hint") ? project["hint"] : "No hint available.";

        dpp::embed hintEmbed = dpp::embed()
            .set_color(globals::color::defaultColor)
            .add_field("Hint", hint);

        dpp::message hintMessage(event.command.channel_id, hintEmbed);
        event.reply(hintMessage);
    });

    index++;
}
