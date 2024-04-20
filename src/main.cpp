#include <iostream>
#include <map>
#include <functional>
#include <fstream>
#include <dpp/dpp.h>

#include "commands/commands.h"
#include "message_manager/message_handler.h"

using json = nlohmann::json;

std::list<cmdStruct> cmdList = {
    { "topic", "Get a topic question", cmd::topicCommand },
    { "coding", "Get a coding question", cmd::codingCommand },
    { "close", "Close a forum post", cmd::closeCommand },
};
uint64_t intents = dpp::i_default_intents | dpp::i_message_content;

int main()
{
    std::ifstream configFile("config.json");
    json config = json::parse(configFile);

    dpp::cluster bot(config["token"], intents);
    
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

    bot.on_message_create([&bot](const dpp::message_create_t& event) {
        const dpp::channel* c = dpp::find_channel(event.msg.channel_id);
        if (c && c->name == "suggestions")
            MsgHandler::createSuggestion(bot, event);
    });

    bot.on_button_click([&bot](const dpp::button_click_t& event) {
        if (event.custom_id == "delSugguestion")
            MsgHandler::Btns::deleteSuggestionBtn(bot, event);
        else if (event.custom_id == "editSuggestion")
            MsgHandler::Btns::editSuggestionBtn(bot, event);
    });

    bot.on_form_submit([&bot](const dpp::form_submit_t& event) {
        if (event.custom_id == "editModal")
            MsgHandler::ModalForms::showSuggestionEditModal(bot, event);
    });

    bot.start(dpp::st_wait);
    return 0;
}
