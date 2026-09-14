#include "suggestion.h"
#include "../../globals/globals.h"

#include <exception>
#include <optional>
#include <string_view>

namespace
{
    constexpr std::string_view authorFooterPrefix = "author_id:";

    std::optional<dpp::snowflake> getSuggestionAuthorId(const dpp::message& message)
    {
        if (message.embeds.empty() || !message.embeds.front().footer)
            return std::nullopt;

        const auto& footer = message.embeds.front().footer;
        if (!footer->text.starts_with(authorFooterPrefix))
            return std::nullopt;

        try
        {
            return dpp::snowflake(footer->text.substr(authorFooterPrefix.size()));
        }
        catch (const std::exception&)
        {
            return std::nullopt;
        }
    }
}

void utils::suggestion::createSuggestion(dpp::cluster& bot, const dpp::message_create_t& event)
{
    dpp::user user = event.msg.author;
    if (!user.is_bot())
    {
        bot.message_delete(event.msg.id, event.msg.channel_id);
        if (event.msg.content.empty())
        {
            bot.message_delete(event.msg.id, event.msg.channel_id);
            bot.message_create(dpp::message(event.msg.channel_id, 
                "You cannot send an empty suggestion. Please add text to your message."),
                [&bot](const dpp::confirmation_callback_t& cb) {
                    if (!cb.is_error()) {
                        const auto& msg = std::get<dpp::message>(cb.value);
                        bot.start_timer([&bot, msg](dpp::timer timer) {
                            bot.message_delete(msg.id, msg.channel_id);
                            bot.stop_timer(timer);
                        }, 5);
                    }
                });
            return;
        }
        dpp::embed result = dpp::embed()
            .set_color(globals::color::defaultColor)
            .set_title("Suggestion")
            .set_author(user.format_username(), "", user.get_avatar_url())
            .set_footer("author_id:" + user.id.str())
            .set_description(event.msg.content);

        dpp::message msg(event.msg.channel_id, result);

        msg.add_component(
            dpp::component().add_component(
                dpp::component()
                .set_label("Delete")
                .set_type(dpp::cot_button)
                .set_style(dpp::cos_danger)
                .set_id("delSuggestion")
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
                const dpp::message msg = std::get<dpp::message>(callback.value);
                const dpp::snowflake messageId = msg.id;
                const dpp::snowflake channelId = msg.channel_id;

                const auto yesEmoji = dpp::find_emoji(globals::emoji::yes);
                const auto noEmoji = dpp::find_emoji(globals::emoji::no);

                if (yesEmoji && noEmoji)
                {
                    const std::string yesEmojiText = yesEmoji->format();
                    const std::string noEmojiText = noEmoji->format();

                    bot.message_add_reaction(messageId, channelId, yesEmojiText, [&bot, messageId, channelId, noEmojiText](const dpp::confirmation_callback_t& reactionCallback) {
                        if (!reactionCallback.is_error())
                            bot.message_add_reaction(messageId, channelId, noEmojiText);
                    });
                }
                else
                {
                    // fallback
                    bot.message_add_reaction(messageId, channelId, "👍", [&bot, messageId, channelId](const dpp::confirmation_callback_t& reactionCallback) {
                        if (!reactionCallback.is_error())
                            bot.message_add_reaction(messageId, channelId, "👎");
                    });
                }
            }
        });
    }
}

void utils::suggestion::deleteSuggestion(dpp::cluster& bot, const dpp::button_click_t& event)
{
    const auto originalAuthorId = getSuggestionAuthorId(event.command.msg);

    if (originalAuthorId && event.command.get_issuing_user().id == *originalAuthorId)
        bot.message_delete(event.command.msg.id, event.command.msg.channel_id);
    else
        event.reply(dpp::message("You can only delete your own suggestions.").set_flags(dpp::m_ephemeral));
}

void utils::suggestion::editSuggestion(dpp::cluster& bot, const dpp::button_click_t& event)
{
    const auto originalAuthorId = getSuggestionAuthorId(event.command.msg);

    if (originalAuthorId && event.command.get_issuing_user().id == *originalAuthorId)
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
    std::string value;
    if ( std::holds_alternative< std::string >( event.components[0].value ) )
        value = std::get< std::string >( event.components[0].value );

    if ( value.empty() ) {
        event.reply(dpp::message("Please fill in the suggestion input field.").set_flags(dpp::m_ephemeral));
        return;
    }

    bot.message_get(event.command.msg.id, event.command.msg.channel_id, [&bot, event, value](const dpp::confirmation_callback_t& callback) {
        if (!callback.is_error())
        {
            dpp::message msg = std::get<dpp::message>(callback.value);
            dpp::embed embed = event.command.msg.embeds[0];

            embed.set_description(value);
            msg.embeds[0] = embed;

            bot.message_edit(msg, [&bot, event, embed](const dpp::confirmation_callback_t& callback) {
                if (!callback.is_error())
                    event.reply(dpp::message("Edited!").set_flags(dpp::m_ephemeral));
            });
        }
    });
}
