#include "edit.h"

#include <iostream>

namespace edit
{
    void CommandManager::registerCommand(
        const std::string& command,
        CommandCallback callback
    )
    {
        commands[command] = std::move(callback);
    }

    bool CommandManager::handleCommand(
        dpp::cluster& bot,
        const dpp::slashcommand_t& event
    )
    {
        const std::string commandName =
            event.command.get_command_name();

        auto it = commands.find(commandName);

        if (it == commands.end())
            return false;

        it->second(bot, event);

        return true;
    }

    // ---------------------------------------------------------
    // /ping
    // ---------------------------------------------------------

    void ping(
        dpp::cluster& bot,
        const dpp::slashcommand_t& event
    )
    {
        event.reply("🏓 Pong!");
    }

    // ---------------------------------------------------------
    // /hello
    // ---------------------------------------------------------

    void hello(
        dpp::cluster& bot,
        const dpp::slashcommand_t& event
    )
    {
        event.reply("Hello! 👋");
    }

    // ---------------------------------------------------------
    // /status
    // ---------------------------------------------------------

    void status(
        dpp::cluster& bot,
        const dpp::slashcommand_t& event
    )
    {
        event.reply("🟢 Bot is online.");
    }

    // ---------------------------------------------------------
    // /cpp
    // ---------------------------------------------------------

    void cpp(
        dpp::cluster& bot,
        const dpp::slashcommand_t& event
    )
    {
        event.reply(
            "C++ is a powerful compiled programming language."
        );
    }

    // ---------------------------------------------------------
    // Register commands
    // ---------------------------------------------------------

    void registerCommands(CommandManager& manager)
    {
        manager.registerCommand("ping", ping);
        manager.registerCommand("hello", hello);
        manager.registerCommand("status", status);
        manager.registerCommand("cpp", cpp);
    }
}
