#include "moderation.h"
#include "../../globals/globals.h"

ModerationService::ModerationService(dpp::cluster& bot) : bot(bot) {}

void ModerationService::cleanupOldEntries(const std::chrono::steady_clock::time_point& now)
{
    const auto threshold = now - cleanupThreshold;

    for (auto it = state.begin(); it != state.end(); )
    {
        if (it->second.lastPostTime < threshold)
            it = state.erase(it);
        else
            ++it;
    }
}

std::string ModerationService::getMessageSignature(const dpp::message& msg) const
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
    const std::string signature = getMessageSignature(event.msg);

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

        userState.messages.push_back({event.msg.channel_id, event.msg.id});

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

        bot.direct_message_create(
            userId,
            dpp::message(std::string(warningMsg))
        );

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

        bot.direct_message_create(
            userId,
            dpp::message(std::string(jailMsg))
        );

        return true;
    }

    return false;
}
