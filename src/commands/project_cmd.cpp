#include "commands.h"
#include "../globals/globals.h"

void cmd::projectCommand(dpp::cluster& bot, const dpp::slashcommand_t& event)
{
    static int index;
    const std::string project = cmd::utils::readFileLine("res/project.txt", index);

    dpp::embed embed = dpp::embed()
        .set_color(globals::color::defaultColor)
        .add_field(project, "");

    dpp::message message(event.command.channel_id, embed);
    event.reply(message);
}
