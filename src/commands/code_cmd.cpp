#include "commands.h"
#include "../globals/globals.h"

void cmd::codeCommand(dpp::cluster& bot, const dpp::slashcommand_t& event)
{
    const std::string title = "Formatting C++ Code";
    const std::string content = "Please format your C++ code like this:\n\n\\```cpp\nstd::cout << \"foo bar\";\n\\```";
    dpp::embed embed = dpp::embed()
        .set_color(globals::color::defaultColor)
        .set_title(title)
        .add_field(content, "");

    dpp::message message(event.command.channel_id, embed);
    event.reply(message);
}
