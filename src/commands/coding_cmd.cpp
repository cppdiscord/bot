#include "commands.h"
#include "../globals/globals.h"

void cmd::codingCommand(dpp::cluster& bot, const dpp::slashcommand_t& event)
{
    static int index;
    const std::string question = cmd::utils::readFileLine("res/coding.txt", index);

    dpp::embed embed = dpp::embed()
        .set_color(globals::color::defaultColor)
        .add_field(question, "");

    dpp::message message(event.command.channel_id, embed);
    event.reply(message);
}
