#ifndef GLOBALS_H
#define GLOBALS_H

#include <dpp/snowflake.h>

namespace globals
{
    namespace color
    {
        static constexpr int defaultColor = 0x004482;
    }

    namespace emoji
    {
        static constexpr dpp::snowflake yes = 1226134958872199229;
        static constexpr dpp::snowflake no = 1226134940006219817;
    }

    namespace category
    {
        static constexpr dpp::snowflake ticketId = 1234179713182732374;
    }
}

#endif // GLOBALS_H
