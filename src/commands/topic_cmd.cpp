#include "commands.h"

void cmd::topicCommand(const dpp::cluster& bot, const dpp::slashcommand_t& event)
{
    static int index;
    std::string topic = cmd::utils::readFileLine("res/topic.txt", index);

    dpp::embed embed = dpp::embed()
        .set_color(COLOR_DEFAULT)
        .add_field(topic, "");

    dpp::message message(event.command.channel_id, embed);
    event.reply(message);
}
