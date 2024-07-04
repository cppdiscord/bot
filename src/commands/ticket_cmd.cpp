#include "commands.h"
#include "../globals/globals.h"

#include <dpp/unicode_emoji.h>

void cmd::createTicket(dpp::cluster& bot, const dpp::slashcommand_t event)
{
	std::string name_ = event.command.get_issuing_user().global_name;
	dpp::snowflake user_id = event.command.get_issuing_user().id;
	dpp::snowflake ticket_support_role_id = 0;
	dpp::snowflake guild_id = 0;
	dpp::snowflake category_id = 0;

    bot.channel_create(dpp::channel()
        .set_type(dpp::CHANNEL_TEXT)
        .set_guild_id(guild_id)
        .set_parent_id(category_id)
        .set_name("ticket-" + name_)
        .add_permission_overwrite(user_id, dpp::overwrite_type::ot_member, dpp::p_view_channel, 0)
        .add_permission_overwrite(ticket_support_role_id, dpp::overwrite_type::ot_role, dpp::p_view_channel, 0)
        .add_permission_overwrite(1258203252948729978/*everyone*/, dpp::overwrite_type::ot_role, 0, dpp::p_view_channel),

        [event, &bot, user_id, name_]() {
            dpp::channel new_channel = std::get<dpp::channel>(callback.value);

            dpp::embed embed = dpp::embed()
                .set_color()
                .set_title("Ticket " + name_)
                .set_description("This is your support ticket. Click the button below to close this ticket.");

            dpp::message msg(new_channel.id, embed);
            msg.add_component(
                dpp::component().add_component(
                    dpp::component()
                    .set_label("Close Ticket")
                    .set_emoji("Cross", 1258391903557124116, false) //thats my emoji custom
                    .set_type(dpp::cot_button)
                    .set_style(dpp::cos_secondary)
                    .set_id("closeTicketwyn")
                )
            );
            /*ticketed_ids.push_back(user_id);*/
            bot.message_create(dpp::message(new_channel.id, "<@&" + std::to_string(ticket_support_role_id) + ">"));
            bot.message_create(msg);
            event.reply("Ticket created: " + new_channel.get_mention());
        }
    );
}
