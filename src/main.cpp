#include <iostream>
#include <map>
#include <functional>
#include <fstream>
#include <dpp/dpp.h>

#include "commands/commands.h"

using json = nlohmann::json;

std::list<cmdStruct> cmdList = {
    { "topic", "Get a topic question", cmd::topicCommand },
    { "coding", "Get a coding question", cmd::codingCommand }
};

int main()
{
    std::ifstream configFile("config.json");
    json config = json::parse(configFile);

    dpp::cluster bot(config["token"]);

    bot.on_ready([&bot](const dpp::ready_t& event) {
        std::cout << "[!] Bot ready" << std::endl;

        if (dpp::run_once<struct bulkRegister>())
        {
            std::vector<dpp::slashcommand> slashcommands;
            for (const auto& item : cmdList)
            {
                dpp::slashcommand slashcommand;
                slashcommand.set_name(item.name);
                slashcommand.set_description(item.desc);
                slashcommand.set_application_id(bot.me.id);
                slashcommands.push_back(slashcommand);
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

    bot.start(dpp::st_wait);
    return 0;
}
