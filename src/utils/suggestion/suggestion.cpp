#include "suggestion.h"
#include "../../globals/globals.h"

void utils::suggestion::createSuggestion(dpp::cluster& bot, const dpp::message_create_t& event)
{
    dpp::user user = event.msg.author;
    if (!user.is_bot())
    {
        dpp::embed result = dpp::embed()
            .set_color(globals::color::default)
            .set_title("Suggestion")
            .set_author(user.format_username(), "", user.get_avatar_url())
            .set_description(event.msg.content);

        dpp::message msg(event.msg.channel_id, result);

        msg.add_component(
            dpp::component().add_component(
                dpp::component()
                .set_label("Delete")
                .set_type(dpp::cot_button)
                .set_style(dpp::cos_danger)
                .set_id("delSugguestion")
            )
        );

        msg.add_component(
            dpp::component().add_component(
                dpp::component()
                .set_label("Edit")
                .set_type(dpp::cot_button)
                .set_style(dpp::cos_primary)
                .set_id("editSuggestion")
            )
        );

        bot.message_create(msg, [&bot](const dpp::confirmation_callback_t& callback) {
            if (!callback.is_error())
            {
                dpp::message m = std::get<dpp::message>(callback.value);
                
                bot.message_add_reaction(m.id, m.channel_id, dpp::find_emoji(globals::emoji::yes)->format());
                bot.message_add_reaction(m.id, m.channel_id, dpp::find_emoji(globals::emoji::no)->format());
            }
        });
        bot.message_delete(event.msg.id, event.msg.channel_id);
    }
}

void utils::suggestion::deleteSuggestion(dpp::cluster& bot, const dpp::button_click_t& event)
{
    std::string clicker = event.command.get_issuing_user().format_username();
    std::string originalAuthor = event.command.msg.embeds[0].author->name;

    if (clicker == originalAuthor)
        bot.message_delete(event.command.msg.id, event.command.msg.channel_id);
    else
        event.reply(dpp::message("You can only delete your own suggestions.").set_flags(dpp::m_ephemeral));
}

void utils::suggestion::editSuggestion(dpp::cluster& bot, const dpp::button_click_t& event)
{
    std::string clicker = event.command.get_issuing_user().format_username();
    std::string originalAuthor = event.command.msg.embeds[0].author->name;

    if (clicker == originalAuthor)
    {
        dpp::interaction_modal_response modal("editModal", "Edit suggestion");

        modal.add_component(
            dpp::component()
            .set_label("Edit suggestion")
            .set_id("editedSuggestion")
            .set_type(dpp::cot_text)
            .set_placeholder("Please enter your improved suggestion here")
            .set_min_length(1)
            .set_max_length(2000)
            .set_text_style(dpp::text_paragraph)
        );

        event.dialog(modal);
    }
    else
        event.reply(dpp::message("You can only edit your own suggestions.").set_flags(dpp::m_ephemeral));
}

void utils::suggestion::showSuggestionEditModal(dpp::cluster& bot, const dpp::form_submit_t& event)
{
    std::string v = std::get<std::string>(event.components[0].components[0].value);

    bot.message_get(event.command.msg.id, event.command.msg.channel_id, [&bot, event, v](const dpp::confirmation_callback_t& callback) {
        if (!callback.is_error())
        {
            dpp::message msg = std::get<dpp::message>(callback.value);
            dpp::embed embed = event.command.msg.embeds[0];

            embed.set_description(v);
            msg.embeds[0] = embed;

            bot.message_edit(msg, [&bot, event, embed](const dpp::confirmation_callback_t& callback) {
                if (!callback.is_error())
                    event.reply(dpp::message("Edited!").set_flags(dpp::m_ephemeral));
            });
        }
    });
}
