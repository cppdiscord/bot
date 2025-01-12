#ifndef GITHUB_WEBHOOK_HANDLER_H
#define GITHUB_WEBHOOK_HANDLER_H

#include <dpp/dpp.h>
#include <httplib.h>
#include <nlohmann/json.hpp>

void sendDiscordMessage(dpp::cluster& bot, const std::string& title, const std::string& url, const std::string& author, const std::string& description);
void handleGitHubWebhook(dpp::cluster& bot, const httplib::Request& req, httplib::Response& res);

#endif // GITHUB_WEBHOOK_HANDLER_H