#include <iostream>
#include <fstream>
#include <dpp/dpp.h>

#include <thread>

#include "webhooks/github_webhook_handler.h"
#include "commands/commands.h"
#include "utils/suggestion/suggestion.h"

using json = nlohmann::json;

std::list<cmdStruct> cmdList = {
    { "topic", "Get a topic question", cmd::topicCommand },
    { "coding", "Get a coding question", cmd::codingCommand },
    { "close", "Close a ticket or forum post", cmd::closeCommand },
    { "ticket", "Open a ticket", cmd::ticketCommand },
    { "code", "Formatting code on Discord", cmd::codeCommand },
    { "project", "Get a project idea", cmd::projectCommand }
};

// Function to run the HTTP server
void start_server(dpp::cluster& bot) {
	httplib::Server server;

	// Handle webhooks
	server.Post("/webhooks", [&bot](const httplib::Request& req, httplib::Response& res) {
		handleGitHubWebhook(bot, req, res);
		});

	std::cout << "Starting server on localhost:8080..." << std::endl;
	server.listen("localhost", 8080);  // Use port 8080 for HTTP
}

int main()
{
    std::ifstream configFile("config.json");
    json config = json::parse(configFile);

    dpp::cluster bot(config["token"], dpp::i_default_intents | dpp::i_message_content);

    std::thread server_thread(start_server, std::ref(bot));

    bot.on_ready([&bot](const dpp::ready_t& event) {
        std::cout << "[!] Bot ready" << std::endl;
        bot.set_presence(dpp::presence(dpp::presence_status::ps_online, dpp::activity_type::at_watching, "cppdiscord.com"));

        if (dpp::run_once<struct bulkRegister>())
        {
            std::vector<dpp::slashcommand> slashcommands;
            for (const auto& item : cmdList)
            {
                dpp::slashcommand slashcommand;
                slashcommand.set_name(item.name);
                slashcommand.set_description(item.desc);
                slashcommand.set_application_id(bot.me.id);
                slashcommands.push_back(slashcommand);
            }
            bot.global_bulk_command_create(slashcommands);
        }
    });

    bot.on_slashcommand([&bot](const dpp::slashcommand_t& event) {
        for (const auto& item : cmdList)
        {
            if (item.name == event.command.get_command_name())
            {
                item.function(bot, event);
                return;
            }
        }
    });

    bot.on_message_create([&bot](const dpp::message_create_t& event) {
        const dpp::channel* channel = dpp::find_channel(event.msg.channel_id);

        if (channel && channel->name == "suggestions")
            utils::suggestion::createSuggestion(bot, event);
    });

    bot.on_button_click([&bot](const dpp::button_click_t& event) {
        if (event.custom_id == "delSugguestion")
            utils::suggestion::deleteSuggestion(bot, event);
        else if (event.custom_id == "editSuggestion")
            utils::suggestion::editSuggestion(bot, event);
    });

    bot.on_form_submit([&bot](const dpp::form_submit_t& event) {
        if (event.custom_id == "editModal")
            utils::suggestion::showSuggestionEditModal(bot, event);
    });

    httplib::Server server;

    bot.start(dpp::st_wait);

	// Wait for the server thread to finish (this is not necessary if you want the server to run indefinitely)
	server_thread.join();

    return 0;
}
