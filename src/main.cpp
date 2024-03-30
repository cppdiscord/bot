#include <iostream>
#include <map>
#include <functional>
#include <fstream>
#include <dpp/dpp.h>

#include "commands/commands.h"

using json = nlohmann::json;

std::list<cmdStruct> cmdList = {
    { "topic", "Get a topic question", cmd::topicCommand },
    { "coding", "Get a coding question", cmd::codingCommand },
    { "close", "Close a forum post", cmd::closeCommand }
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
        dpp::channel* c = dpp::find_channel(event.msg.channel_id);

        if (c && c->name == "suggestions") {
            std::string author = event.msg.author.format_username();
            if (!event.msg.author.is_bot()) { 
                dpp::embed result = dpp::embed()
                    .set_color(dpp::colors::dark_blue)
                    .set_title("Suggestion")
                    .set_author(author, "", event.msg.author.get_avatar_url())
                    .set_description(event.msg.content);

                dpp::message msg(event.msg.channel_id, result);
                bot.message_create( msg, [ &bot ]( const dpp::confirmation_callback_t& callback ) {
                    if ( !callback.is_error()) {
                        dpp::message m = std::get<dpp::message>( callback.value );
                        bot.message_add_reaction( m.id, m.channel_id, "👍" );
                        bot.message_add_reaction( m.id, m.channel_id, "👎" );
                    }
                });
                bot.message_delete(event.msg.id, event.msg.channel_id);
            }
        }
    });

    bot.start(dpp::st_wait);
    return 0;
}
