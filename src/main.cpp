#include <iostream>
#include <fstream>
#include <string>
#include <vector>

#include <dpp/dpp.h>
#include <nlohmann/json.hpp>

#include "commands/commands.h"
#include "globals/globals.h"
#include "utils/suggestion/suggestion.h"
#include "utils/moderation/moderation.h"

using json = nlohmann::json;

std::vector<cmdStruct> cmdList = {
    {
        "topic",
        "Get a topic question",
        cmd::topicCommand
    },

    {
        "beginner",
        "Get a beginner's guide to C++",
        cmd::beginnerCommand
    },

    {
        "coding",
        "Get a coding question",
        cmd::codingCommand
    },

    {
        "close",
        "Close a ticket or forum post",
        cmd::closeCommand
    },

    {
        "ticket",
        "Open a ticket",
        cmd::ticketCommand,
        {
            dpp::command_option(
                dpp::command_option_type::co_user,
                "participant",
                "Add participant",
                false
            )
        }
    },

    {
        "code",
        "Formatting code on Discord",
        cmd::codeCommand
    },

    {
        "project",
        "Get a project idea",
        cmd::projectCommand
    },

    {
        "rule",
        "Get the server rules",
        cmd::ruleCommand,
        {
            dpp::command_option(
                dpp::command_option_type::co_integer,
                "number",
                "Rule to mention",
                false
            )
        }
    }
};



std::vector<dpp::slashcommand> createSlashCommands(
    const dpp::snowflake& applicationId
)
{
    std::vector<dpp::slashcommand> commands;

    for (const auto& item : cmdList)
    {
        dpp::slashcommand command;

        command
            .set_name(item.name)
            .set_description(item.desc)
            .set_application_id(applicationId);

        for (const auto& option : item.args)
        {
            command.add_option(option);
        }

        if (item.permissions)
        {
            command.set_default_permissions(
                dpp::permission(item.permissions)
            );
        }

        commands.push_back(command);
    }

    return commands;
}


bool executeCommand(
    dpp::cluster& bot,
    const dpp::slashcommand_t& event
)
{
    const std::string commandName =
        event.command.get_command_name();

    for (const auto& item : cmdList)
    {
        if (item.name == commandName)
        {
            item.function(bot, event);
            return true;
        }
    }

    return false;
}


int main()
{
    std::cout << "[*] Starting bot..." << std::endl;



    std::ifstream configFile("config.json");

    if (!configFile.is_open())
    {
        std::cerr
            << "[!] Failed to open config.json"
            << std::endl;

        return 1;
    }

    json config;

    try
    {
        configFile >> config;
    }
    catch (const json::parse_error& error)
    {
        std::cerr
            << "[!] Invalid JSON in config.json: "
            << error.what()
            << std::endl;

        return 1;
    }


    std::string globalsConfigError;

    if (!globals::loadFromConfig(
            config,
            globalsConfigError))
    {
        std::cerr
            << "[!] Invalid configuration: "
            << globalsConfigError
            << std::endl;

        return 1;
    }


    if (!config.contains("token") ||
        !config["token"].is_string() ||
        config["token"].get<std::string>().empty())
    {
        std::cerr
            << "[!] Discord token is missing from config.json"
            << std::endl;

        return 1;
    }

    const std::string token =
        config["token"].get<std::string>();



    dpp::cluster bot(
        token,
        dpp::i_default_intents |
        dpp::i_message_content
    );

    ModerationService moderationService(bot);

    bot.on_ready([&bot](const dpp::ready_t& event)
    {
        std::cout
            << "[+] Bot ready as "
            << bot.me.username
            << std::endl;

        bot.set_presence(
            dpp::presence(
                dpp::presence_status::ps_online,
                dpp::activity_type::at_watching,
                "cppdiscord.com"
            )
        );

        // Register slash commands once.
        if (dpp::run_once<struct bulkRegister>())
        {
            std::cout
                << "[*] Registering slash commands..."
                << std::endl;

            auto slashCommands =
                createSlashCommands(bot.me.id);

            bot.global_bulk_command_create(
                slashCommands,
                [](const dpp::confirmation_callback_t& callback)
                {
                    if (callback.is_error())
                    {
                        std::cerr
                            << "[!] Failed to register commands: "
                            << callback.get_error().message
                            << std::endl;

                        return;
                    }

                    std::cout
                        << "[+] Slash commands registered."
                        << std::endl;
                }
            );
        }
    });


    bot.on_slashcommand(
        [&bot](const dpp::slashcommand_t& event)
        {
            const std::string commandName =
                event.command.get_command_name();

            std::cout
                << "[COMMAND] /"
                << commandName
                << " used by "
                << event.command.get_issuing_user().username
                << std::endl;

            if (!executeCommand(bot, event))
            {
                std::cerr
                    << "[!] Unknown command: /"
                    << commandName
                    << std::endl;
            }
        }
    );



    bot.on_message_create(
        [&bot, &moderationService](const dpp::message_create_t& event)
        {
            // Moderation gets first priority.
            if (moderationService.handleMessage(event))
            {
                return;
            }

            const dpp::channel* channel =
                dpp::find_channel(event.msg.channel_id);

            if (!channel)
            {
                return;
            }

            if (channel->name == "suggestions")
            {
                utils::suggestion::createSuggestion(
                    bot,
                    event
                );
            }
        }
    );


    bot.on_button_click(
        [&bot](const dpp::button_click_t& event)
        {
            const std::string& customId =
                event.custom_id;

            if (customId == "delSuggestion")
            {
                utils::suggestion::deleteSuggestion(
                    bot,
                    event
                );
            }
            else if (customId == "editSuggestion")
            {
                utils::suggestion::editSuggestion(
                    bot,
                    event
                );
            }
            else if (customId.starts_with("hint_button_"))
            {
                cmd::handleProjectHintButton(
                    bot,
                    event
                );
            }
        }
    );

    bot.on_form_submit(
        [&bot](const dpp::form_submit_t& event)
        {
            if (event.custom_id == "editModal")
            {
                utils::suggestion::showSuggestionEditModal(
                    bot,
                    event
                );
            }
        }
    );

    std::cout
        << "[*] Connecting to Discord..."
        << std::endl;

    bot.start(dpp::st_wait);

    return 0;
}
