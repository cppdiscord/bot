#include "commands.h"
#include "../globals/globals.h"

void cmd::ticketCommand(dpp::cluster& bot, const dpp::slashcommand_t& event)
{
    bot.channels_get(event.command.guild_id, [&bot, event](const dpp::confirmation_callback_t& callback) {
        if (callback.is_error()) {
            dpp::message message(event.command.channel_id, "Failed to check existing tickets.");
            return event.reply(message.set_flags(dpp::m_ephemeral));
        }

        const auto channels = callback.get<dpp::channel_map>();
        bool hasExistingTicket = false;
        
        for (const auto& [channel_id, channel] : channels) {
            if (channel.parent_id == globals::category::ticketId && 
                channel.name == event.command.get_issuing_user().username) {
                hasExistingTicket = true;
                break;
            }
        }

        if (hasExistingTicket) {
            dpp::message message(event.command.channel_id, "You already have an open ticket.");
            return event.reply(message.set_flags(dpp::m_ephemeral));
        }

        dpp::message message(event.command.channel_id, "Creating ticket...");
        event.reply(message.set_flags(dpp::m_ephemeral));

        const dpp::channel ticketChannel = dpp::channel()
            .set_name(event.command.get_issuing_user().username)
            .set_type(dpp::CHANNEL_TEXT)
            .set_guild_id(event.command.guild_id)
            .set_parent_id(globals::category::ticketId)
            .set_permission_overwrite(event.command.guild_id, dpp::overwrite_type::ot_role, 0, dpp::p_view_channel)
            .set_permission_overwrite(event.command.get_issuing_user().id, dpp::overwrite_type::ot_member, dpp::p_view_channel, 0);

        bot.channel_create(ticketChannel, [&bot, event](const dpp::confirmation_callback_t& callback) {
            if (!callback.is_error())
            {
                const auto ticketChannel = std::get<dpp::channel>(callback.value);
                const auto pingMessage = dpp::message(ticketChannel.id, event.command.get_issuing_user().get_mention() + " opened this ticket.");
                bot.message_create(pingMessage);

                event.edit_response("Ticket " + ticketChannel.get_mention() + " created!");
            }
            else
            {
                event.edit_response("Failed to create ticket channel: " + callback.get_error().message);
            }
        });
    });
}
