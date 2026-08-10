#include "commands.h"
#include "../globals/globals.h"

void cmd::beginnerCommand(dpp::cluster& bot, const dpp::slashcommand_t& event)
{
    const dpp::embed embed = dpp::embed()
        .set_color(globals::color::defaultColor)
        .set_title("👋 New to C++? Start here!")
        .set_url("https://www.learncpp.com/")
        .set_description("Essential resources for C++ beginners")
        .add_field("📘 **Best Beginner Tutorial**", 
            "LearnCpp.com is widely considered the best free resource:\nhttps://www.learncpp.com/", false)
        .add_field("📖 **CPP Reference**", 
            "The definitive C++ language reference:\nhttps://en.cppreference.com/", false)
        .add_field("🛠️ **Practice**", 
            "Start with small projects:\n"
            "- Calculator\n"
            "- Guess game\n"
            "- Dice game", false)
        .add_field("✅ **Do this**", 
            "- Learn Modern C++\n"
            "- Use a great IDE\n"
            "- Practice what you learn\n"
            "- Learn OOP basics\n"
            "- Write clean, readable code", true)
        .add_field("❌ **Dont do this**", 
            "- Don't learn from outdated C++ resources\n"
            "- Don't let AI write code for you\n"
            "- Don't use `using namespace std;` (it's bad practice)\n"
            "- Don't ignore compiler warnings", true)
        .set_footer(dpp::embed_footer()
            .set_text("Need help? Ask in <#" + std::to_string(globals::channels::HELP_CHANNEL_ID) + ">"))
        .set_timestamp(dpp::utility::timepoint());

    dpp::message message(event.command.channel_id, embed);
    message.add_component(
        dpp::component()
            .add_component(
                dpp::component()
                    .set_type(dpp::cot_button)
                    .set_label("Learn C++")
                    .set_url("https://www.learncpp.com/")
                    .set_style(dpp::cos_link)
            )
            .add_component(
                dpp::component()
                    .set_type(dpp::cot_button)
                    .set_label("CPP Reference")
                    .set_url("https://en.cppreference.com/")
                    .set_style(dpp::cos_link)
            )
    );
    event.reply(message);
}
