#include "globals.h"

#include <cstdint>
#include <dpp/nlohmann/json.hpp>
#include <utility>

namespace
{
    bool parseSnowflake(const nlohmann::json& config, const char* key, dpp::snowflake& out)
    {
        if (!config.contains(key))
            return false;

        const auto& value = config.at(key);

        if (value.is_string())
        {
            out = dpp::snowflake(value.get<std::string>());
            return true;
        }

        if (value.is_number_unsigned())
        {
            out = dpp::snowflake(value.get<uint64_t>());
            return true;
        }

        return false;
    }
}

namespace globals
{
    namespace emoji
    {
        dpp::snowflake yes{};
        dpp::snowflake no{};
    }

    namespace channel
    {
        dpp::snowflake rulesId{};
        dpp::snowflake jailId{};
    }

    namespace category
    {
        dpp::snowflake ticketId{};
    }

    namespace role
    {
        dpp::snowflake staffId{};
        dpp::snowflake jailId{};
    }

    bool loadFromConfig(const nlohmann::json& config, std::string& error)
    {
        constexpr std::pair<const char*, dpp::snowflake&> configEntries[] =
        {
            {"emoji_yes_id", emoji::yes},
            {"emoji_no_id", emoji::no},
            {"channel_rules_id", channel::rulesId},
            {"channel_jail_id", channel::jailId},
            {"category_ticket_id", category::ticketId},
            {"role_staff_id", role::staffId},c 
            {"role_jail_id", role::jailId}
        };

        for (const auto& [name, id] : configEntries)
        {
            if (!parseSnowFlake(config, name, id))
            {
                error = "Missing or invalid config key: " + std::string(name);
                return false;
            }
        }

        return true;
    }
}
