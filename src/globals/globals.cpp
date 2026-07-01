#include "globals.h"

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

        if (value.is_number_unsigned() || value.is_number_integer())
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
        if (!parseSnowflake(config, "emoji_yes_id", emoji::yes))
        {
            error = "Missing or invalid config key: emoji_yes_id";
            return false;
        }

        if (!parseSnowflake(config, "emoji_no_id", emoji::no))
        {
            error = "Missing or invalid config key: emoji_no_id";
            return false;
        }

        if (!parseSnowflake(config, "channel_rules_id", channel::rulesId))
        {
            error = "Missing or invalid config key: channel_rules_id";
            return false;
        }

        if (!parseSnowflake(config, "channel_jail_id", channel::jailId))
        {
            error = "Missing or invalid config key: channel_jail_id";
            return false;
        }

        if (!parseSnowflake(config, "category_ticket_id", category::ticketId))
        {
            error = "Missing or invalid config key: category_ticket_id";
            return false;
        }

        if (!parseSnowflake(config, "role_staff_id", role::staffId))
        {
            error = "Missing or invalid config key: role_staff_id";
            return false;
        }

        if (!parseSnowflake(config, "role_jail_id", role::jailId))
        {
            error = "Missing or invalid config key: role_jail_id";
            return false;
        }

        return true;
    }
}
