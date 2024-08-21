#include "Listener.h"
#include <dpp/dpp.h>
#include <vector>
#include <string>

//taken from dpp github
Listener::Listener(dpp::cluster& bot) : bot(bot) {
    bot.on_message_create([this](const dpp::message_create_t& event) {
        this->on_message_create(event);
    });
}

const std::vector<std::string> keywords = {
    "#include", "import", "std::", "void", "class", "public", "private", "protected",
    "return", "int", "float", "char", "double", "string", "const", "static", "template"
};

//to make sure its rly code and not a conversation
bool count_keywords(const std::string& message) {
    int keyword_count = 0;
    for (const auto& keyword : keywords) {
        if (message.find(keyword) != std::string::npos) {
            keyword_count++;
            if (keyword_count > 2) {
                return true;
            }
        }
    }
    return false;
}

void Listener::on_message_create(const dpp::message_create_t& event) const {
    if (event.msg.author.id == bot.me.id) return;

    if (count_keywords(event.msg.content)) {
        const std::string response = "<@" + std::to_string(event.msg.author.id) + "> ```\n" + event.msg.content + "\n```";

        bot.message_delete(event.msg.id, event.msg.channel_id);
        bot.message_create(dpp::message(event.msg.channel_id, response)
            .set_reference(event.msg.id));
    }
}