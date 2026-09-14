#include "ticket_registry.h"

#include <algorithm>

namespace tickets
{
    std::optional<dpp::snowflake> Registry::find(
        const dpp::snowflake guildId,
        const dpp::snowflake userId) const
    {
        std::scoped_lock lock(mutex);

        const auto ticket = tickets.find({guildId, userId});

        if (ticket == tickets.end())
            return std::nullopt;

        return ticket->second;
    }

    bool Registry::add(
        const dpp::snowflake guildId,
        const dpp::snowflake userId,
        const dpp::snowflake channelId)
    {
        std::scoped_lock lock(mutex);

        return tickets.emplace(
            Key{guildId, userId},
            channelId).second;
    }

    void Registry::removeByChannel(
        const dpp::snowflake guildId,
        const dpp::snowflake channelId)
    {
        std::scoped_lock lock(mutex);

        std::erase_if(tickets, [guildId, channelId](const auto& ticket) {
            return ticket.first.guildId == guildId
                && ticket.second == channelId;
        });
    }

    Registry registry;
}

