#include "github_webhook_handler.h"
#include <iostream>

using json = nlohmann::json;

const std::string DISCORD_CHANNEL_ID = "DISCORD_CHANNEL_ID";

void sendDiscordMessage(dpp::cluster& bot, const std::string& title, const std::string& url, const std::string& author, const std::string& description) {
	dpp::embed embed = dpp::embed()
		.set_title(title)
		.set_url(url)
		.set_description(description)
		.set_color(0x00ff00);

	dpp::message message(DISCORD_CHANNEL_ID, embed);
	bot.message_create(message);
}

void handleGitHubWebhook(dpp::cluster& bot, const httplib::Request& req, httplib::Response& res) {
	auto event = req.get_header_value("X-GitHub-Event");
	auto payload = json::parse(req.body);

	if (event == "issues" && payload["action"] == "opened") {
		auto issue = payload["issue"];
		sendDiscordMessage(bot, "New Issue: " + issue["title"].get<std::string>(), issue["html_url"].get<std::string>(), issue["user"]["login"].get<std::string>(), issue["body"].get<std::string>());
	}
	else if (event == "pull_request" && payload["action"] == "opened") {
		auto pr = payload["pull_request"];
		sendDiscordMessage(bot, "New Pull Request: " + pr["title"].get<std::string>(), pr["html_url"].get<std::string>(), pr["user"]["login"].get<std::string>(), pr["body"].get<std::string>());
	}

	res.set_content("OK", "text/plain");
}
