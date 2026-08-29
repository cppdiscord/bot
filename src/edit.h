
#pragma once

#include <string>
#include <unordered_map>
#include <functional>
#include <utility>

#include <dpp/dpp.h>

namespace edit
{
    using CommandCallback =
        std::function<void(
            dpp::cluster&,
            const dpp::slashcommand_t&
        )>;

    class CommandManager
    {
    private:
        std::unordered_map<
            std::string,
            CommandCallback
        > commands;

    public:
        void registerCommand(
            const std::string& command,
            CommandCallback callback
        );

        bool handleCommand(
            dpp::cluster& bot,
            const dpp::slashcommand_t& event
        );
    };

    void registerCommands(CommandManager& manager);
}
