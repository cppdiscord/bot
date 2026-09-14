#ifndef TICKET_REGISTRY_H
#define TICKET_REGISTRY_H

#include <dpp/snowflake.h>

#include <cstdint>
#include <functional>
#include <mutex>
#include <optional>
#include <unordered_map>

namespace tickets
{
    class Registry
    {
    public:
        std::optional<dpp::snowflake> find(
            dpp::snowflake guildId,
            dpp::snowflake userId) const;

        bool add(
            dpp::snowflake guildId,
            dpp::snowflake userId,
            dpp::snowflake channelId);

        void removeByChannel(
            dpp::snowflake guildId,
            dpp::snowflake channelId);

    private:
        struct Key
        {
            dpp::snowflake guildId{};
            dpp::snowflake userId{};

            bool operator==(const Key& other) const
            {
                return guildId == other.guildId
                    && userId == other.userId;
            }
        };

        struct KeyHash
        {
            std::size_t operator()(const Key& key) const
            {
                const auto guildHash =
                    std::hash<std::uint64_t>{}(
                        static_cast<std::uint64_t>(key.guildId));

                const auto userHash =
                    std::hash<std::uint64_t>{}(
                        static_cast<std::uint64_t>(key.userId));

                return guildHash
                    ^ (userHash + 0x9e3779b9
                    + (guildHash << 6)
                    + (guildHash >> 2));
            }
        };

        mutable std::mutex mutex;
        std::unordered_map<Key, dpp::snowflake, KeyHash> tickets;
    };

    extern Registry registry;
}

#endif // TICKET_REGISTRY_H
