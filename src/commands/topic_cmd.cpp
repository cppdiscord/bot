#include "commands.h"
#include "../globals/globals.h"

void cmd::topicCommand(dpp::cluster& bot, const dpp::slashcommand_t& event)
{
    static int index;
    int maxLoadAttempts = 10;
    
    for(int i = 0; i < maxLoadAttempts; i++) // check if topic string is empty up to x amount of attempts
    {
        const std::string topic = cmd::utils::readFileLine("res/topic.txt", index);
        if(topic.length() > 0)
        {
          break;
        }
    }
       

    const dpp::embed embed = dpp::embed()
        .set_color(globals::color::defaultColor)
        .add_field(topic, "");

    const dpp::message message(event.command.channel_id, embed);
    event.reply(message);
}
