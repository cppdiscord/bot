#include "commands.h"
#include "../globals/globals.h"

#include <dpp/channel.h>
#include <dpp/permissions.h>
#include <dpp/snowflake.h>
#include <dpp/unicode_emoji.h>

void cmd::closeCommand(dpp::cluster& bot, const dpp::slashcommand_t& event)
{
    if (event.command.channel.get_type() == dpp::channel_type::CHANNEL_PUBLIC_THREAD)
    {
        bot.thread_get(event.command.channel_id, [&bot, event](const dpp::confirmation_callback_t& callback) {
            if (callback.is_error())
                return event.reply(dpp::message("[!] Callback error").set_flags(dpp::m_ephemeral));

            auto thread = callback.get<dpp::thread>();

            if (event.command.channel.owner_id != event.command.member.user_id)
                return event.reply(dpp::message("You can only close your own posts.").set_flags(dpp::m_ephemeral));

            thread.metadata.locked = true;

            const std::string newThreadName = dpp::unicode_emoji::lock + std::string(" ") + thread.name;
            thread.set_name(newThreadName);

            bot.thread_edit(thread, [event](const dpp::confirmation_callback_t& callback2) {
                if (callback2.is_error())
                    return event.reply(dpp::message("[!] Unable to close post.").set_flags(dpp::m_ephemeral));

                dpp::embed embed = dpp::embed()
                    .set_color(globals::color::defaultColor)
                    .add_field("Closed post!", "");

                dpp::message message(event.command.channel_id, embed);
                event.reply(message);
            });
        });
    }
    else if (event.command.channel.parent_id == globals::category::ticketId)
    {
        event.reply(dpp::message("Closed ticket!"));

        bot.channel_get(event.command.channel.id, [&bot, event](const dpp::confirmation_callback_t& callback) {
            if (!callback.is_error())
            {
                dpp::channel ticketChannel = std::get<dpp::channel>(callback.value);
                std::vector<dpp::permission_overwrite> overwrites = ticketChannel.permission_overwrites;

                for (const auto& overwrite : overwrites)
                {
                    if (overwrite.type == dpp::overwrite_type::ot_member)
                    {
                        bot.channel_edit_permissions(ticketChannel, overwrite.id, 0, dpp::p_view_channel, true);
                    }
                }
            }
        });
    }
    else
    {
        event.reply(dpp::message("This command only operates within tickets and threads.").set_flags(dpp::m_ephemeral));
    }
}
