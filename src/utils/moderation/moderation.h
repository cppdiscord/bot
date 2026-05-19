#ifndef MODERATION_H
#define MODERATION_H

#include <dpp/dpp.h>
#include <chrono>
#include <string>
#include <unordered_map>
#include <vector>
#include <mutex>

class ModerationService
{
public:
    explicit ModerationService(dpp::cluster& bot);

    /**
    * @brief Analyzes the message for cross-posting/spam
    *        Duplicate content by same user in:
    *        - 2 channels: Delete the message and dm a warning
    *        - 3+ channels: Delete all messages and jail the user
    * @param event message create event
    * @return true if the message was deleted/handled, false otherwise
    */
    bool handleMessage(const dpp::message_create_t& event);

private:
    static constexpr auto crossPostWindow = std::chrono::seconds{5};
    static constexpr auto cleanupThreshold = std::chrono::seconds{180};

    static constexpr std::string_view warningMsg =
        "You posted the same message on multiple channels. "
        "Your message has been deleted, because we do not allow cross-posting.";

    static constexpr std::string_view jailMsg =
        "You have been jailed as you posted the same message on multiple channels. "
        "Please contact a staff member if you think this is a mistake.";

    struct PostedMessage
    {
        dpp::snowflake channelId{};
        dpp::snowflake messageId{};
    };

    struct UserState
    {
        std::string lastSignature;
        std::chrono::steady_clock::time_point lastPostTime{};
        std::vector<PostedMessage> messages;
    };

    std::string getMessageSignature(const dpp::message& msg) const;
    void cleanupOldEntries(const std::chrono::steady_clock::time_point& now);

    dpp::cluster& bot;
    std::mutex mtx;
    std::unordered_map<dpp::snowflake, UserState> state;
};

#endif // MODERATION_H
