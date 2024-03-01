#include "commands.h"

void cmd::codingCommand(const dpp::cluster& bot, const dpp::slashcommand_t& event)
{
	static int index;
	std::string question = cmd::utils::readFileLine("res/coding.txt", index);

	dpp::embed embed = dpp::embed()
		.set_color(COLOR_DEFAULT)
		.add_field(question, "");
	
	dpp::message message(event.command.channel_id, embed);
	event.reply(message);
}
