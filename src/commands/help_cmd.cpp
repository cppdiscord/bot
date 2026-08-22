#include "commands.h"
#include "../globals/globals.h"

void cmd::helpCommand(dpp::cluster& bot, const dpp::slashcommand_t& event)
    {
    dpp::embed helpEmbed = dpp::embed()
        .set_color(globals::color::defaultColor)
        .set_title("📚 Command List")
        .set_description("Here are all the available commands:")
        .add_field(
            "💻 **Coding Help**",
            "- /beginner - Get a beginner's guide to C++\n"
            "- /coding - Get a coding question\n"
            "- /topic - Get a topic question\n"
            "- /project - Get a project idea",
            false
        )
        .add_field(
            "🛠️ **Moderation**",
            "- /rule - Get the server rules\n"
            "- /ticket - Open a ticket\n"
            "- /close - Close a ticket",
            false
        )
        .add_field(
            "📝 **Utility**",
            "- /code - Format code on Discord\n"
            "- /help - Show this help message",
            false
        )
        .add_field(
            "💡 **Tips**",
            "- Use /coding difficulty:Advanced for harder questions\n"
            "- Use /rule number:1 to see a specific rule\n"
            "- Use /ticket participant:@user to add someone to your ticket",
            false
        )
        .set_footer(dpp::embed_footer()
            .set_text("Need more help? Ask in <#" + std::to_string(globals::channels::HELP_CHANNEL_ID) + ">"))
        .set_timestamp(dpp::utility::time_f());

    event.reply(dpp::message(event.command.channel_id, helpEmbed));
    }