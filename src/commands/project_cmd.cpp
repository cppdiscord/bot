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

    if (data["projects"].empty())
    {
        event.reply("No projects available.");
        return;
    }

    index = index % data["projects"].size();

    const auto& project = data["projects"][index];
    if (!project.contains("title") || !project.contains("description"))
    {
        event.reply("Project data is missing required fields.");
        return;
    }

    const std::string projectTitle = project["title"];
    const std::string projectDescription = project["description"];
    const std::string projectHint = project.contains("hint") ? project["hint"] : "No hint available.";

    const dpp::embed embed = dpp::embed()
        .set_color(globals::color::defaultColor)
        .add_field("Project Idea", projectTitle)
        .add_field("Description", projectDescription);

    dpp::message message(event.command.channel_id, embed);

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
    
    index = (index + 1) % data["projects"].size();
}
