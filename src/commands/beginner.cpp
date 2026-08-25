#include "commands.h"
#include "../globals/globals.h"

void cmd::beginnerCommand(dpp::cluster& bot, const dpp::slashcommand_t& event)
{
    const dpp::embed embed = dpp::embed()
        .set_color(globals::color::defaultColor)
        .add_field("👋 New to C++? Start here!", "If you're learning C++, we recommend these resources:\n\n📘 **Learn C++ (Best beginner tutorial)**\n- https://www.learncpp.com/\n\n📖 **CPP Reference (Language & Standard library reference)**\n- https://en.cppreference.com/\n\n🛠️ **Practice**\n1. Build small projects\n2. Read and write lots of code\n\n**Common advice:**\n* ✅ Learn modern C++, not C with classes\n* ✅ Avoid outdated books, videos, and random blog posts that teach old C++ practices\n* ❌ Don't ask ChatGPT or other AI to write your code\n\nIf you're stuck on something specific, ask in the help channels: <#1130494190615265342>.\n\nBe sure to include your code, any error messages, what you've tried already, and what you expected to happen.");

    const dpp::message message(event.command.channel_id, embed);
    event.reply(message);
}
