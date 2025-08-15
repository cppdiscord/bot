#include <iostream>
#include <list>
#include <fstream>
#include <dpp/dpp.h>

#include "commands/commands.h"
#include "utils/suggestion/suggestion.h"

using json = nlohmann::json;

std::list<cmdStruct> cmdList = {
    { "topic", "Get a topic question", cmd::topicCommand },
    { "coding", "Get a coding question", cmd::codingCommand },
    { "close", "Close a ticket or forum post", cmd::closeCommand },
    { "ticket", "Open a ticket", cmd::ticketCommand },
    { "code", "Formatting code on Discord", cmd::codeCommand },
    { "project", "Get a project idea", cmd::projectCommand },
    { "rule", "Get the server rules", cmd::ruleCommand, { dpp::command_option(dpp::command_option_type::co_integer, "number", "Rule to mention", false) } }
};

int main()
{
    std::ifstream configFile("config.json");
    json config = json::parse(configFile);

    dpp::cluster bot(config["token"], dpp::i_default_intents | dpp::i_message_content);

    bot.on_ready([&bot](const dpp::ready_t& event) {
        std::cout << "[!] Bot ready" << std::endl;
        bot.set_presence(dpp::presence(dpp::presence_status::ps_online, dpp::activity_type::at_watching, "cppdiscord.com"));

        if (dpp::run_once<struct bulkRegister>())
        {
            std::vector<dpp::slashcommand> slashcommands;
            for (const auto& item : cmdList)
            {
                dpp::slashcommand slashCommand;
                slashCommand.set_name(item.name);
                slashCommand.set_description(item.desc);
                slashCommand.set_application_id(bot.me.id);

                for (const dpp::command_option& arg : item.args)
                {
                    slashCommand.add_option(arg);
                }
                slashcommands.push_back(slashCommand);
            }
            bot.global_bulk_command_create(slashcommands);
        }
    });

    bot.on_slashcommand([&bot](const dpp::slashcommand_t& event) {
        for (const auto& item : cmdList)
        {
            if (item.name == event.command.get_command_name())
            {
                item.function(bot, event);
                return;
            }
        }
    });

    bot.on_message_create([&bot](const dpp::message_create_t& event) {
        const dpp::channel* channel = dpp::find_channel(event.msg.channel_id);

        if (channel && channel->name == "suggestions")
            utils::suggestion::createSuggestion(bot, event);
    });

    bot.on_button_click([&bot](const dpp::button_click_t& event) {
        // Handle suggestion buttons
        if (event.custom_id == "delSuggestion")
            utils::suggestion::deleteSuggestion(bot, event);
        else if (event.custom_id == "editSuggestion")
            utils::suggestion::editSuggestion(bot, event);
        // Handle project hint buttons
        else if (event.custom_id.rfind("hint_button_", 0) == 0)
        {
            // Remove button
            dpp::message updatedMsg = event.command.get_context_message();
            updatedMsg.components.clear();
            bot.message_edit(updatedMsg);

            // Parse the index from the button ID
            const int hintButtonIndex = std::stoi(event.custom_id.substr(event.custom_id.rfind('_') + 1));

            // Load project data
            std::ifstream projectFile("res/project.json");
            if (!projectFile.is_open())
            {
                event.reply(dpp::message("Failed to open project file.").set_flags(dpp::m_ephemeral));
                return;
            }

            json data;
            try
            {
                projectFile >> data;
            }
            catch (const json::parse_error& e)
            {
                event.reply(dpp::message("Failed to parse project file.").set_flags(dpp::m_ephemeral));
                return;
            }

            if (!data.contains("projects") || !data["projects"].is_array() || 
                hintButtonIndex >= (int)data["projects"].size() || hintButtonIndex < 0)
            {
                event.reply(dpp::message("Invalid project data or index.").set_flags(dpp::m_ephemeral));
                return;
            }

            const auto& project = data["projects"][hintButtonIndex];
            const std::string hint = project.contains("hint") ? project["hint"] : "No hint available.";

            dpp::embed hintEmbed = dpp::embed()
                .set_color(0x004482) // Using the default color directly since we can't access globals here
                .add_field("Hint", hint);

            dpp::message hintMessage(event.command.channel_id, hintEmbed);
            event.reply(hintMessage);
        }
    });

    bot.on_form_submit([&bot](const dpp::form_submit_t& event) {
        if (event.custom_id == "editModal")
            utils::suggestion::showSuggestionEditModal(bot, event);
    });

    bot.start(dpp::st_wait);
    return 0;
}
