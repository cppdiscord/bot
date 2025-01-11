#include "commands.h"
#include "../globals/globals.h"

static bool buttonClicked = false;

void cmd::projectCommand(dpp::cluster& bot, const dpp::slashcommand_t& event)
{
	static int index;
	const std::string project = cmd::utils::readFileLine("res/project.txt", index);

	dpp::embed embed = dpp::embed()
		.set_color(globals::color::defaultColor)
		.add_field("Project Idea", project);

	dpp::message message(event.command.channel_id, embed);

	// Add hint button with index as custom_id
	message.add_component(
		dpp::component().add_component(
			dpp::component()
			.set_label("Hint")
			.set_type(dpp::cot_button)
			.set_style(dpp::cos_primary)
			.set_id("hint_button_" + std::to_string(index))
		)
	);

	event.reply(message);

	// Reset the button click state when the /project command is run
	buttonClicked = false;

	bot.on_button_click([&bot](const dpp::button_click_t& event)
		{
			if (event.custom_id.rfind("hint_button_", 0) == 0 && !buttonClicked)
			{
				// Extract index from custom_id
				int hintIndex = std::stoi(event.custom_id.substr(12)) - 1;
				const std::string hint = cmd::utils::readFileLine("res/hints.txt", hintIndex);

				dpp::embed hintEmbed = dpp::embed()
					.set_color(globals::color::defaultColor)
					.add_field("Hint", hint);

				dpp::message hintMessage(event.command.channel_id, hintEmbed);
				event.reply(hintMessage);

				buttonClicked = true;
			}
		}
	);
}
