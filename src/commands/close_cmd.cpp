#include "commands.h"
#include "../globals/globals.h"

#include <dpp/channel.h>
#include <dpp/permissions.h>
#include <dpp/snowflake.h>
#include <dpp/unicode_emoji.h>

void cmd::closeCommand(dpp::cluster& bot, const dpp::slashcommand_t& event)
{
    const dpp::channel* channel = dpp::find_channel(event.command.channel_id);
    if (!channel)
    {
        event.reply(dpp::message("[!] Channel not found").set_flags(dpp::m_ephemeral));
        return;
    }

    if (channel->get_type() == dpp::channel_type::CHANNEL_PUBLIC_THREAD)
    {
        bot.thread_get(event.command.channel_id, [&bot, event](const dpp::confirmation_callback_t& callback) {
            if (callback.is_error())
                return event.reply(dpp::message("[!] Callback error").set_flags(dpp::m_ephemeral));

            dpp::thread thread = callback.get<dpp::thread>();

            bool isOwner = (thread.owner_id == event.command.member.user_id);
            bool hasManagePermission = event.command.member.has_permission(thread.guild_id, dpp::p_manage_threads);
            
            if (!isOwner && !hasManagePermission)
                return event.reply(dpp::message("You can only close your own posts or you need manage threads permission.").set_flags(dpp::m_ephemeral));

            thread.metadata.locked = true;
            thread.metadata.archived = true;

            const std::string newThreadName = dpp::unicode_emoji::lock + std::string(" ") + thread.name;
            thread.set_name(newThreadName);

            bot.thread_edit(thread, [event](const dpp::confirmation_callback_t& callback2) {
                if (callback2.is_error())
                    return event.reply(dpp::message("[!] Unable to close post.").set_flags(dpp::m_ephemeral));

                const dpp::embed embed = dpp::embed()
                    .set_color(globals::color::defaultColor)
                    .add_field("Closed post!", "");

                const dpp::message message(event.command.channel_id, embed);
                event.reply(message);
            });
        });
    }
    else if (channel->parent_id == globals::category::ticketId)
    {
        bool isOwner = false;
        bool hasManagePermission = event.command.member.has_permission(channel->guild_id, dpp::p_manage_channels);
        
        for (const auto& overwrite : channel->permission_overwrites)
        {
            if (overwrite.id == event.command.member.user_id && overwrite.type == dpp::overwrite_type::ot_member)
            {
                isOwner = true;
                break;
            }
        }
        
        if (!isOwner && !hasManagePermission)
        {
            event.reply(dpp::message("You can only close your own tickets or you need manage channels permission.").set_flags(dpp::m_ephemeral));
            return;
        }

        event.reply(dpp::message("Closed ticket!"));

        // Remove user's access to the ticket
        bot.channel_edit_permissions(*channel, event.command.member.user_id, 0, dpp::p_view_channel, true);
    }
    else
    {
        event.reply(dpp::message("This command only operates within tickets and threads.").set_flags(dpp::m_ephemeral));
    }
}
