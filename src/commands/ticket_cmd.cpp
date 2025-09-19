#include "commands.h"
#include "../globals/globals.h"

void cmd::ticketCommand(dpp::cluster& bot, const dpp::slashcommand_t& event)
{
    dpp::message message(event.command.channel_id, "Creating ticket...");
    event.reply(message.set_flags(dpp::m_ephemeral));

    dpp::channel ticketChannel = dpp::channel()
        .set_name(event.command.get_issuing_user().username)
        .set_type(dpp::CHANNEL_TEXT)
        .set_guild_id(event.command.guild_id)
        .set_parent_id(globals::category::ticketId)
        .set_permission_overwrite(event.command.guild_id, dpp::overwrite_type::ot_role, 0, dpp::p_view_channel)
        .set_permission_overwrite(event.command.get_issuing_user().id, dpp::overwrite_type::ot_member, dpp::p_view_channel, 0);

    const dpp::command_interaction cmdData = event.command.get_command_interaction();
    if (!cmdData.options.empty())
    {
        if (const auto option = cmdData.options[0]; option.type == dpp::co_user)
        {
            const auto participantId = std::get<dpp::snowflake>(option.value);
            ticketChannel.set_permission_overwrite(participantId, dpp::overwrite_type::ot_member,dpp::p_view_channel, 0);
        }
    }

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
            event.edit_response("Failed to create ticket channel!");
        }
    });
}
