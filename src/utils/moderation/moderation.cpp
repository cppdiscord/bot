#include "moderation.h"
#include "../../globals/globals.h"

ModerationService::ModerationService(dpp::cluster& bot) : bot(bot) {}

void ModerationService::cleanupOldEntries(const std::chrono::steady_clock::time_point& now)
{
    const auto threshold = now - cleanupThreshold;

    std::erase_if(state, [&](const auto& entry)
    {
        return entry.second.lastPostTime < threshold;
    });
}

std::string ModerationService::makeMessageSignature(const dpp::message& msg)
{
    std::string signature = msg.content;

    for (const auto& att : msg.attachments)
    {
        signature += '|';
        signature += att.filename;
        signature += '|';
        signature += std::to_string(att.size);
    }

    for (const auto& emb : msg.embeds)
    {
        signature += '|';
        signature += emb.title;
        signature += '|';
        signature += emb.description;
        signature += '|';
        signature += emb.url;
    }

    return signature;
}

bool ModerationService::handleMessage(const dpp::message_create_t& event)
{
    if (event.msg.author.is_bot())
        return false;

    if (event.msg.content.empty() &&
        event.msg.attachments.empty() &&
        event.msg.embeds.empty())
        return false;

    const auto now = std::chrono::steady_clock::now();
    const auto userId = event.msg.author.id;
    const std::string signature = makeMessageSignature(event.msg);

    bool warn = false;
    bool jail = false;
    std::vector<PostedMessage> toDelete;
    dpp::snowflake targetChannel{};

    {
        std::scoped_lock lock(mtx);
        cleanupOldEntries(now);

        auto& userState = state[userId];
        const bool sameMessage = (signature == userState.lastSignature);
        const bool withinWindow = (now - userState.lastPostTime) <= crossPostWindow;
        userState.lastPostTime = now;

        if (!sameMessage || !withinWindow)
        {
            userState.lastSignature = signature;
            userState.messages.clear();
        }

        const dpp::snowflake currentChannel = event.msg.channel_id;
        const bool channelAlreadySeen = std::any_of(
            userState.messages.begin(), userState.messages.end(),
            [&](const PostedMessage& m) { return m.channelId == currentChannel; });

        if (!channelAlreadySeen)
            userState.messages.push_back({currentChannel, event.msg.id});

        const std::size_t count = userState.messages.size();

        if (count == 2)
        {
            warn = true;
            targetChannel = event.msg.channel_id;
        }
        else if (count >= 3)
        {
            jail = true;
            toDelete = std::move(userState.messages);
            state.erase(userId);
        }
    }

    if (warn)
    {
        bot.message_delete(event.msg.id, targetChannel);

        dpp::message warnMessage(targetChannel,
            dpp::utility::user_mention(userId) + " " + std::string(warningMsg));
        warnMessage.set_allowed_mentions(false, false, false, false, {userId}, {});
        bot.message_create(warnMessage);

        return true;
    }

    if (jail)
    {
        for (const auto& m : toDelete)
        {
            bot.message_delete(m.messageId, m.channelId);
        }

        bot.guild_member_add_role(
            event.msg.guild_id,
            userId,
            globals::role::jailId
        );

        dpp::message jailMessage(globals::channel::jailId,
            dpp::utility::user_mention(userId) + " " + std::string(jailMsg));
        jailMessage.set_allowed_mentions(false, false, false, false, {userId}, {});
        bot.message_create(jailMessage);

        return true;
    }

    return false;
}
