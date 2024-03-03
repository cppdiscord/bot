#include "commands.h"

#include <dpp/unicode_emoji.h>

void cmd::closeCommand(dpp::cluster& bot, const dpp::slashcommand_t& event)
{
    bot.thread_get(event.command.channel_id, [&bot, event](const dpp::confirmation_callback_t& callback) {
        if (callback.is_error())
            return event.reply(dpp::message("[!] Callback error").set_flags(dpp::m_ephemeral));

        auto thread = callback.get<dpp::thread>();

        if (!thread.is_public_thread())
            return event.reply(dpp::message("This command only operates within forums.").set_flags(dpp::m_ephemeral));

        if (event.command.channel.owner_id != event.command.member.user_id)
            return event.reply(dpp::message("You can only close your own posts.").set_flags(dpp::m_ephemeral));

        thread.metadata.locked = true;

        const std::string newThreadName = dpp::unicode_emoji::lock + std::string(" ") + thread.name;
        thread.set_name(newThreadName);

        bot.thread_edit(thread, [event](const dpp::confirmation_callback_t& callback2) {
            if (callback2.is_error())
                return event.reply(dpp::message("[!] Unable to close post.").set_flags(dpp::m_ephemeral));

            dpp::embed embed = dpp::embed()
                .set_color(COLOR_DEFAULT)
                .add_field("Closed post!", "");

            dpp::message message(event.command.channel_id, embed);
            event.reply(message);
        });
    });
}
