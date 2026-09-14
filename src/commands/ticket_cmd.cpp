#include "commands.h"
#include "../globals/globals.h"
#include "../ticket_registry.h"

void cmd::ticketCommand(dpp::cluster& bot, const dpp::slashcommand_t& event)
{
    const auto guildId = event.command.guild_id;
    const auto userId = event.command.get_issuing_user().id;

    if (const auto existingTicket = tickets::registry.find(guildId, userId))
    {
        event.reply(dpp::message(
            "You already have an open ticket: <#" + existingTicket->str() + ">")
            .set_flags(dpp::m_ephemeral));
        return;
    }

    dpp::message message(event.command.channel_id, "Creating ticket...");
    event.reply(message.set_flags(dpp::m_ephemeral));

    constexpr int ticketPerms = dpp::p_view_channel
                                 | dpp::p_send_messages
                                 | dpp::p_attach_files
                                 | dpp::p_embed_links;

    dpp::channel ticketChannel = dpp::channel()
        .set_name(event.command.get_issuing_user().username)
        .set_type(dpp::CHANNEL_TEXT)
        .set_guild_id(guildId)
        .set_parent_id(globals::category::ticketId)
        .set_permission_overwrite(guildId, dpp::overwrite_type::ot_role, 0, dpp::p_view_channel)
        .set_permission_overwrite(userId, dpp::overwrite_type::ot_member, ticketPerms, 0)
        .set_permission_overwrite(globals::role::staffId, dpp::overwrite_type::ot_role, ticketPerms, 0);

    const dpp::command_interaction cmdData = event.command.get_command_interaction();
    if (!cmdData.options.empty())
    {
        if (const auto option = cmdData.options[0]; option.type == dpp::co_user)
        {
            const auto participantId = std::get<dpp::snowflake>(option.value);
            ticketChannel.set_permission_overwrite(participantId, dpp::overwrite_type::ot_member,ticketPerms, 0);
        }
    }

    bot.channel_create(ticketChannel, [&bot, event, guildId, userId](const dpp::confirmation_callback_t& callback) {
        if (callback.is_error())
        {
            event.edit_response("Failed to create ticket channel.");
            return;
        }

        const auto createdChannel = std::get<dpp::channel>(callback.value);
        if (!tickets::registry.add(guildId, userId, createdChannel.id))
        {
            bot.channel_delete(createdChannel.id);
            event.edit_response("You already have an open ticket.");
            return;
        }

        const auto pingMessage = dpp::message(
            createdChannel.id,
            event.command.get_issuing_user().get_mention() + " opened this ticket.");

        dpp::message ticketMessage = pingMessage;
        ticketMessage.add_component(
            dpp::component().add_component(
                dpp::component()
                    .set_label("Close ticket")
                    .set_type(dpp::cot_button)
                    .set_style(dpp::cos_danger)
                    .set_id("closeTicket")));

        bot.message_create(ticketMessage);

        event.edit_response("Ticket " + createdChannel.get_mention() + " created!");
    });
}
