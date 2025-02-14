#include "commands.h"
#include "../globals/globals.h"

#include <dpp/appcommand.h>
#include <dpp/dpp.h>
#include <dpp/channel.h>
#include <string>
#include <vector>

void cmd::ruleCommand(dpp::cluster& bot, const dpp::slashcommand_t& event)
{
    std::vector<std::string> rules = {
        "Follow Discord's Terms of Service: Adhere to Discord's Terms of Service (ToS) and Community Guidelines.",
        "Be respectful: Treat others with kindness, respect, and patience. Avoid harassment, personal attacks, or any form of offensive behavior.",
        "Stay on topic: Keep discussions focused on C++ coding and related topics. Avoid excessive off-topic conversations that may disrupt the flow of the server.",
        "No spam or self-promotion: Refrain from spamming, advertising, or excessive self-promotion. This includes sharing unrelated links, excessive emojis, or repeated messages.",
        "Use appropriate language: Ensure your language remains appropriate and professional. Avoid excessive swearing, offensive language, or any content that may violate Discord's guidelines.",
        "Provide helpful assistance: When providing assistance, be constructive and offer accurate information. Help others to the best of your knowledge and avoid misleading advice.",
        "Respect privacy and copyright: Do not share personal information without consent. Respect the intellectual property rights of others and avoid sharing copyrighted material.",
        "Do not provide or request help on projects that may violate terms of service, or that may be deemed inappropriate, malicious, or illegal.",
        "Do not offer or ask for paid work of any kind.",
        "Do not copy and paste answers from ChatGPT or similar AI tools."
    };

    dpp::command_interaction cmdData = event.command.get_command_interaction();
    if (cmdData.options.empty())
        return event.reply(dpp::message("Please follow our <#" + globals::channel::rulesId.str() + ">."));

    const auto option = cmdData.options[0];

    if (option.type != dpp::co_integer)
        return; // should never happen

    const long index = std::get<long>(option.value);

    if ((index < 1 || index > rules.size()))
        return event.reply(dpp::message("Rule number " + std::to_string(index) + " does not exist. Visit <#" + globals::channel::rulesId.str() + "> to see all available rules.").set_flags(dpp::m_ephemeral));

    const std::string rule = rules.at(index-1);
    const dpp::embed embed = dpp::embed()
        .set_color(globals::color::defaultColor)
        .add_field("Rule " + std::to_string(index), rule);
    dpp::message message(event.command.channel_id, embed);
    event.reply(message);
}
