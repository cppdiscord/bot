#ifndef GLOBALS_H
#define GLOBALS_H

#include <dpp/snowflake.h>
#include <string>

namespace globals
{
    namespace color
    {
        static constexpr int defaultColor = 0x004482;
    }

    /**
     * @brief Load configured IDs used by the bot.
     * @param config Parsed config JSON object.
     * @param error Output error message when loading fails.
     * @return true when all required IDs were loaded successfully.
     */
    bool loadFromConfig(const nlohmann::json& config, std::string& error);

    namespace emoji
    {
        extern dpp::snowflake yes;
        extern dpp::snowflake no;
    }

    namespace channel
    {
        extern dpp::snowflake rulesId;
        extern dpp::snowflake jailId;
    }

    namespace category
    {
        extern dpp::snowflake ticketId;
    }

    namespace role
    {
        extern dpp::snowflake staffId;
        extern dpp::snowflake jailId;
    }
}

#endif // GLOBALS_H
