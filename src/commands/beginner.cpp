#include "commands.h"
#include "../globals/globals.h"
#include <string>

void cmd::beginnerCommand(dpp::cluster& bot, const dpp::slashcommand_t& event)
{
    std::string dtxt = R"md(
If you're learning C++, we recommend these resources:

📘 **Learn C++ (Best beginner tutorial)**\n- https://www.learncpp.com/

📖 **CPP Reference (Language & Standard library reference)**
- https://en.cppreference.com/\n\n🛠️ **Practice**
1. Build small projects
2. Read and write lots of code

**Common advice:**
* ✅ Learn modern C++, not C with classes
* ✅ Avoid outdated books, videos, and random blog posts that teach old C++ practices
* ❌ Don't ask ChatGPT or other AI to write your code
If you're stuck on something specific, ask in the help channels: <#1130494190615265342>.
Be sure to include your code, any error messages, what you've tried already, and what you expected to happen.
    )md";
    const dpp::embed embed = dpp::embed()
        .set_color(globals::color::defaultColor)
        .add_field("👋 New to C++? Start here!", 
            dtxt.c_str()/* incase */);

    const dpp::message message(event.command.channel_id, embed);
    event.reply(message);
}
