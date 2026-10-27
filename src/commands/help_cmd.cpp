#include "../globals/globals.h"
#include "commands.h"

#include <algorithm>
#include <cctype>
#include <string>
#include <vector>

struct HelpEntry {
    std::string name;
    std::string description;
    std::string usage;
};

const std::vector<HelpEntry> helpEntries = {
    {"topic", "Get a topic question to spark conversation.", "/topic"},
    {"beginner", "Get a beginner's guide to C++ with resources.", "/beginner"},
    {"coding", "Get a random C++ coding question.", "/coding"},
    {"close", "Close a ticket or forum post.", "/close"},
    {"ticket", "Open a private support ticket.", "/ticket [participant]"},
    {"code", "Show how to format code on Discord.", "/code"},
    {"project", "Get a random C++ project idea with a hint button.", "/project"},
    {"rule", "Show a server rule by its number.", "/rule [number]"},
    {"help", "List all commands or show help for one command.", "/help [command]"}};

std::string to_lower(std::string value) {
    std::transform(value.begin(), value.end(), value.begin(),
                   [](unsigned char c) { return static_cast<char>(std::tolower(c)); });
    return value;
}

void cmd::helpCommand(dpp::cluster &bot, const dpp::slashcommand_t &event) {
    const dpp::command_interaction cmdData = event.command.get_command_interaction();

    if (!cmdData.options.empty()) {
        const auto &option = cmdData.options[0];
        if (option.type == dpp::co_string) {
            const std::string query = to_lower(std::get<std::string>(option.value));

            const auto it = std::find_if(helpEntries.begin(), helpEntries.end(),
                                         [&query](const HelpEntry &entry) { return entry.name == query; });

            if (it == helpEntries.end()) {
                const dpp::embed embed = dpp::embed()
                                             .set_color(globals::color::defaultColor)
                                             .set_title("Unknown command")
                                             .add_field("`/" + query + "` does not exist.",
                                                        "Use `/help` to list all available commands.");

                event.reply(dpp::message(event.command.channel_id, embed).set_flags(dpp::m_ephemeral));
                return;
            }

            const dpp::embed embed = dpp::embed()
                                         .set_color(globals::color::defaultColor)
                                         .set_title("Help: /" + it->name)
                                         .add_field("Description", it->description)
                                         .add_field("Usage", "`" + it->usage + "`");

            event.reply(dpp::message(event.command.channel_id, embed));
            return;
        }
    }

    dpp::embed embed = dpp::embed()
                           .set_color(globals::color::defaultColor)
                           .set_title("Bot Commands")
                           .set_description("Use `/help <command>` for details about a specific command.");

    for (const auto &entry : helpEntries)
        embed.add_field("/" + entry.name, entry.description);

    event.reply(dpp::message(event.command.channel_id, embed));
}