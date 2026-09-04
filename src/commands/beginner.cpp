#include "commands.h"
#include "../globals/globals.h"

#include <string>

void cmd::beginnerCommand(dpp::cluster& bot, const dpp::slashcommand_t& event)
{
    const std::string content = R"(If you're learning C++, this learning path can help you get started:

**Suggested order**
1. Install a compiler and choose an IDE
2. Learn variables, types, conditions, and loops
3. Learn functions and basic program structure
4. Learn classes, structs, and object-oriented programming
5. Learn the standard library, including strings, vectors, and maps
6. Practice by building small projects and debugging your code

**Recommended resources**
📘 **Learn C++**
https://www.learncpp.com/

📖 **C++ Reference**
https://en.cppreference.com/

**Common advice**
✅ Learn modern C++, rather than treating it as C with classes
✅ Prefer resources that explain why code works
✅ Read and write code regularly
❌ Do not rely on AI to write your code for you

If you're stuck, ask in the help channels: <#1130494190615265342>.
Include your code, error messages, what you tried, and what you expected to happen.)";

    const dpp::embed embed = dpp::embed()
        .set_color(globals::color::defaultColor)
        .add_field("👋 New to C++? Start here!", content);

    const dpp::message message(event.command.channel_id, embed);
    event.reply(message);
}
