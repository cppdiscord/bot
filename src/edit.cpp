#include "edit.h"

#include <dpp/dpp.h>

#include <algorithm>
#include <chrono>
#include <ctime>
#include <iomanip>
#include <random>
#include <sstream>
#include <string>
#include <unordered_map>
#include <vector>

namespace edit
{
    // =========================================================
    // Internal helpers
    // =========================================================

    namespace
    {
        using Clock = std::chrono::steady_clock;

        const auto startTime = Clock::now();

        std::mt19937& rng()
        {
            static std::mt19937 generator{
                std::random_device{}()
            };

            return generator;
        }

        std::string formatDuration(
            std::chrono::seconds seconds
        )
        {
            long long total = seconds.count();

            const long long days = total / 86400;
            total %= 86400;

            const long long hours = total / 3600;
            total %= 3600;

            const long long minutes = total / 60;
            const long long secs = total % 60;

            std::ostringstream output;

            if (days > 0)
                output << days << "d ";

            if (hours > 0 || days > 0)
                output << hours << "h ";

            if (minutes > 0 || hours > 0 || days > 0)
                output << minutes << "m ";

            output << secs << "s";

            return output.str();
        }

        std::string currentTime()
        {
            const std::time_t now = std::time(nullptr);

            std::tm tm{};

#ifdef _WIN32
            localtime_s(&tm, &now);
#else
            localtime_r(&now, &tm);
#endif

            std::ostringstream output;

            output << std::put_time(
                &tm,
                "%Y-%m-%d %H:%M:%S"
            );

            return output.str();
        }

        dpp::embed baseEmbed(
            const std::string& title,
            const std::string& description
        )
        {
            dpp::embed embed;

            embed
                .set_title(title)
                .set_description(description)
                .set_timestamp(time(nullptr));

            return embed;
        }

        void replyEmbed(
            const dpp::slashcommand_t& event,
            const dpp::embed& embed
        )
        {
            event.reply(
                dpp::message()
                    .add_embed(embed)
            );
        }

        bool isAdministrator(
            const dpp::slashcommand_t& event
        )
        {
            return event.command.member.has_permission(
                event.command.guild_id,
                dpp::p_administrator
            );
        }

        bool isModerator(
            const dpp::slashcommand_t& event
        )
        {
            return
                isAdministrator(event) ||
                event.command.member.has_permission(
                    event.command.guild_id,
                    dpp::p_manage_messages
                );
        }

        std::string userName(
            const dpp::slashcommand_t& event
        )
        {
            return event.command.get_issuing_user().username;
        }
    }

    // =========================================================
    // CommandManager
    // =========================================================

    void CommandManager::registerCommand(
        const std::string& command,
        CommandCallback callback
    )
    {
        commands[command] = std::move(callback);
    }

    bool CommandManager::handleCommand(
        dpp::cluster& bot,
        const dpp::slashcommand_t& event
    )
    {
        const std::string commandName =
            event.command.get_command_name();

        const auto it = commands.find(commandName);

        if (it == commands.end())
            return false;

        it->second(bot, event);

        return true;
    }

    // =========================================================
    // /ping
    // =========================================================

    void ping(
        dpp::cluster& bot,
        const dpp::slashcommand_t& event
    )
    {
        event.reply(
            dpp::message("🏓 Pong!")
                .set_flags(dpp::m_ephemeral)
        );
    }

    // =========================================================
    // /hello
    // =========================================================

    void hello(
        dpp::cluster& bot,
        const dpp::slashcommand_t& event
    )
    {
        const std::string name = userName(event);

        event.reply(
            "Hello, **" + name + "**! 👋"
        );
    }

    // =========================================================
    // /status
    // =========================================================

    void status(
        dpp::cluster& bot,
        const dpp::slashcommand_t& event
    )
    {
        const auto uptime =
            std::chrono::duration_cast<std::chrono::seconds>(
                Clock::now() - startTime
            );

        dpp::embed embed =
            baseEmbed(
                "Bot Status",
                "Current status of the bot."
            );

        embed.add_field(
            "Status",
            "🟢 Online",
            true
        );

        embed.add_field(
            "Uptime",
            formatDuration(uptime),
            true
        );

        embed.add_field(
            "Time",
            currentTime(),
            true
        );

        replyEmbed(event, embed);
    }

    // =========================================================
    // /uptime
    // =========================================================

    void uptime(
        dpp::cluster& bot,
        const dpp::slashcommand_t& event
    )
    {
        const auto elapsed =
            std::chrono::duration_cast<std::chrono::seconds>(
                Clock::now() - startTime
            );

        event.reply(
            "⏱️ Uptime: **" +
            formatDuration(elapsed) +
            "**"
        );
    }

    // =========================================================
    // /about
    // =========================================================

    void about(
        dpp::cluster& bot,
        const dpp::slashcommand_t& event
    )
    {
        dpp::embed embed =
            baseEmbed(
                "About the Bot",
                "A C++ Discord bot powered by DPP."
            );

        embed.add_field(
            "Language",
            "C++",
            true
        );

        embed.add_field(
            "Library",
            "DPP",
            true
        );

        embed.add_field(
            "Purpose",
            "Community utilities and moderation.",
            false
        );

        replyEmbed(event, embed);
    }

    // =========================================================
    // /cpp
    // =========================================================

    void cpp(
        dpp::cluster& bot,
        const dpp::slashcommand_t& event
    )
    {
        dpp::embed embed =
            baseEmbed(
                "C++",
                "C++ is a compiled general-purpose programming language."
            );

        embed.add_field(
            "Core topics",
            "Classes, templates, pointers, references, STL, RAII.",
            false
        );

        embed.add_field(
            "Useful STL",
            "`vector`, `string`, `unordered_map`, `optional`.",
            false
        );

        replyEmbed(event, embed);
    }

    // =========================================================
    // /help
    // =========================================================

    void help(
        dpp::cluster& bot,
        const dpp::slashcommand_t& event
    )
    {
        dpp::embed embed =
            baseEmbed(
                "Help",
                "Available utility commands."
            );

        embed.add_field(
            "General",
            "`/ping`\n"
            "`/hello`\n"
            "`/status`\n"
            "`/uptime`\n"
            "`/about`\n"
            "`/cpp`",
            false
        );

        embed.add_field(
            "Fun",
            "`/coinflip`\n"
            "`/roll`\n"
            "`/8ball`\n"
            "`/choose`",
            false
        );

        embed.add_field(
            "Server",
            "`/serverinfo`\n"
            "`/userinfo`\n"
            "`/avatar`",
            false
        );

        replyEmbed(event, embed);
    }

    // =========================================================
    // /coinflip
    // =========================================================

    void coinflip(
        dpp::cluster& bot,
        const dpp::slashcommand_t& event
    )
    {
        std::uniform_int_distribution<int> distribution(0, 1);

        const bool heads = distribution(rng()) == 0;

        event.reply(
            heads
                ? "🪙 **Heads!**"
                : "🪙 **Tails!**"
        );
    }

    // =========================================================
    // /roll
    // =========================================================

    void roll(
        dpp::cluster& bot,
        const dpp::slashcommand_t& event
    )
    {
        int sides = 6;

        const auto parameter =
            event.get_parameter("sides");

        if (std::holds_alternative<int64_t>(parameter))
        {
            sides =
                static_cast<int>(
                    std::get<int64_t>(parameter)
                );
        }

        sides = std::clamp(sides, 2, 1000);

        std::uniform_int_distribution<int> distribution(
            1,
            sides
        );

        const int result = distribution(rng());

        event.reply(
            "🎲 You rolled **" +
            std::to_string(result) +
            "** / " +
            std::to_string(sides)
        );
    }

    // =========================================================
    // /8ball
    // =========================================================

    void eightBall(
        dpp::cluster& bot,
        const dpp::slashcommand_t& event
    )
    {
        static const std::vector<std::string> answers = {
            "Yes.",
            "No.",
            "Definitely.",
            "Probably.",
            "Maybe.",
            "Ask again later.",
            "The outlook is good.",
            "The outlook is unclear."
        };

        std::uniform_int_distribution<size_t> distribution(
            0,
            answers.size() - 1
        );

        event.reply(
            "🎱 " + answers[distribution(rng())]
        );
    }

    // =========================================================
    // /choose
    // =========================================================

    void choose(
        dpp::cluster& bot,
        const dpp::slashcommand_t& event
    )
    {
        const auto first =
            std::get<std::string>(
                event.get_parameter("first")
            );

        const auto second =
            std::get<std::string>(
                event.get_parameter("second")
            );

        std::uniform_int_distribution<int> distribution(
            0,
            1
        );

        const std::string result =
            distribution(rng()) == 0
                ? first
                : second;

        event.reply(
            "🤔 I choose **" +
            result +
            "**."
        );
    }

    // =========================================================
    // /serverinfo
    // =========================================================

    void serverInfo(
        dpp::cluster& bot,
        const dpp::slashcommand_t& event
    )
    {
        const dpp::guild* guild =
            dpp::find_guild(event.command.guild_id);

        if (!guild)
        {
            event.reply(
                dpp::message(
                    "Unable to find this server."
                ).set_flags(dpp::m_ephemeral)
            );

            return;
        }

        dpp::embed embed =
            baseEmbed(
                "Server Information",
                guild->name
            );

        embed.add_field(
            "Members",
            std::to_string(guild->member_count),
            true
        );

        embed.add_field(
            "Channels",
            std::to_string(guild->channels.size()),
            true
        );

        embed.add_field(
            "Roles",
            std::to_string(guild->roles.size()),
            true
        );

        embed.add_field(
            "Server ID",
            std::to_string(guild->id),
            false
        );

        replyEmbed(event, embed);
    }

    // =========================================================
    // /userinfo
    // =========================================================

    void userInfo(
        dpp::cluster& bot,
        const dpp::slashcommand_t& event
    )
    {
        dpp::snowflake userId =
            event.command.get_issuing_user().id;

        const auto parameter =
            event.get_parameter("user");

        if (std::holds_alternative<dpp::snowflake>(parameter))
        {
            userId =
                std::get<dpp::snowflake>(parameter);
        }

        const dpp::user* user =
            dpp::find_user(userId);

        if (!user)
        {
            event.reply(
                dpp::message(
                    "Unable to find that user."
                ).set_flags(dpp::m_ephemeral)
            );

            return;
        }

        dpp::embed embed =
            baseEmbed(
                "User Information",
                user->username
            );

        embed.add_field(
            "Username",
            user->username,
            true
        );

        embed.add_field(
            "ID",
            std::to_string(user->id),
            true
        );

        embed.add_field(
            "Bot",
            user->is_bot() ? "Yes" : "No",
            true
        );

        embed.set_thumbnail(
            user->get_avatar_url(
                256
            )
        );

        replyEmbed(event, embed);
    }

    // =========================================================
    // /avatar
    // =========================================================

    void avatar(
        dpp::cluster& bot,
        const dpp::slashcommand_t& event
    )
    {
        const dpp::user& user =
            event.command.get_issuing_user();

        dpp::embed embed =
            baseEmbed(
                "Avatar",
                user.username
            );

        embed.set_image(
            user.get_avatar_url(1024)
        );

        replyEmbed(event, embed);
    }

    // =========================================================
    // /echo
    // =========================================================

    void echo(
        dpp::cluster& bot,
        const dpp::slashcommand_t& event
    )
    {
        const auto text =
            std::get<std::string>(
                event.get_parameter("text")
            );

        if (text.empty())
        {
            event.reply(
                dpp::message(
                    "You need to provide some text."
                ).set_flags(dpp::m_ephemeral)
            );

            return;
        }

        event.reply(text);
    }

    // =========================================================
    // /announce
    // =========================================================

    void announce(
        dpp::cluster& bot,
        const dpp::slashcommand_t& event
    )
    {
        if (!isModerator(event))
        {
            event.reply(
                dpp::message(
                    "You don't have permission to use this command."
                ).set_flags(dpp::m_ephemeral)
            );

            return;
        }

        const auto text =
            std::get<std::string>(
                event.get_parameter("text")
            );

        dpp::message message(
            text
        );

        bot.message_create(
            dpp::snowflake(event.command.channel_id),
            message
        );

        event.reply(
            dpp::message(
                "Announcement sent."
            ).set_flags(dpp::m_ephemeral)
        );
    }

    // =========================================================
    // /clear
    // =========================================================

    void clear(
        dpp::cluster& bot,
        const dpp::slashcommand_t& event
    )
    {
        if (!isModerator(event))
        {
            event.reply(
                dpp::message(
                    "You don't have permission to use this command."
                ).set_flags(dpp::m_ephemeral)
            );

            return;
        }

        const auto amount =
            std::get<int64_t>(
                event.get_parameter("amount")
            );

        const int count =
            static_cast<int>(
                std::clamp<int64_t>(
                    amount,
                    1,
                    100
                )
            );

        event.reply(
            dpp::message(
                "Clear command received for " +
                std::to_string(count) +
                " messages."
            ).set_flags(dpp::m_ephemeral)
        );
    }

    // =========================================================
    // /lock
    // =========================================================

    void lock(
        dpp::cluster& bot,
        const dpp::slashcommand_t& event
    )
    {
        if (!isModerator(event))
        {
            event.reply(
                dpp::message(
                    "You don't have permission to use this command."
                ).set_flags(dpp::m_ephemeral)
            );

            return;
        }

        event.reply(
            dpp::message(
                "🔒 Channel lock requested."
            ).set_flags(dpp::m_ephemeral)
        );
    }

    // =========================================================
    // /unlock
    // =========================================================

    void unlock(
        dpp::cluster& bot,
        const dpp::slashcommand_t& event
    )
    {
        if (!isModerator(event))
        {
            event.reply(
                dpp::message(
                    "You don't have permission to use this command."
                ).set_flags(dpp::m_ephemeral)
            );

            return;
        }

        event.reply(
            dpp::message(
                "🔓 Channel unlock requested."
            ).set_flags(dpp::m_ephemeral)
        );
    }

    // =========================================================
    // /modcheck
    // =========================================================

    void modCheck(
        dpp::cluster& bot,
        const dpp::slashcommand_t& event
    )
    {
        if (isModerator(event))
        {
            event.reply(
                dpp::message(
                    "✅ You have moderator permissions."
                ).set_flags(dpp::m_ephemeral)
            );
        }
        else
        {
            event.reply(
                dpp::message(
                    "❌ You don't have moderator permissions."
                ).set_flags(dpp::m_ephemeral)
            );
        }
    }

    // =========================================================
    // Register all edit.cpp commands
    // =========================================================

    void registerCommands(CommandManager& manager)
    {
        manager.registerCommand(
            "ping",
            ping
        );

        manager.registerCommand(
            "hello",
            hello
        );

        manager.registerCommand(
            "status",
            status
        );

        manager.registerCommand(
            "uptime",
            uptime
        );

        manager.registerCommand(
            "about",
            about
        );

        manager.registerCommand(
            "cpp",
            cpp
        );

        manager.registerCommand(
            "help",
            help
        );

        manager.registerCommand(
            "coinflip",
            coinflip
        );

        manager.registerCommand(
            "roll",
            roll
        );

        manager.registerCommand(
            "8ball",
            eightBall
        );

        manager.registerCommand(
            "choose",
            choose
        );

        manager.registerCommand(
            "serverinfo",
            serverInfo
        );

        manager.registerCommand(
            "userinfo",
            userInfo
        );

        manager.registerCommand(
            "avatar",
            avatar
        );

        manager.registerCommand(
            "echo",
            echo
        );

        manager.registerCommand(
            "announce",
            announce
        );

        manager.registerCommand(
            "clear",
            clear
        );

        manager.registerCommand(
            "lock",
            lock
        );

        manager.registerCommand(
            "unlock",
            unlock
        );

        manager.registerCommand(
            "modcheck",
            modCheck
        );
    }
}
