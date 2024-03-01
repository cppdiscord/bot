#ifndef COMMANDS_H
#define COMMANDS_H

#include <iostream>
#include <dpp/dpp.h>

#define COLOR_DEFAULT 0x004482

namespace cmd
{
	/**
	 * @brief Replies with a question in the chat to change the topic
	 * @param bot
	 * @param slashcommand event
	 */
	void topicCommand(const dpp::cluster& bot, const dpp::slashcommand_t& event);

	/**
	 * @brief Replies with a C++ related coding question
	 * @param bot
	 * @param slashcommand event
	 */
	void codingCommand(const dpp::cluster& bot, const dpp::slashcommand_t& event);

	namespace utils
	{
		/**
		 * @brief Read next line of file, jump to beginning if no next line
		 * @param path to the file
		 * @param index
		 * @return content of next line
		 */
		std::string readFileLine(const std::string& path, int& index);
	}
}

struct cmdStruct
{
	std::string name;
	std::string desc;

	typedef std::function<void(dpp::cluster&, dpp::slashcommand_t)> cmdFunc;
	cmdFunc function;
};

#endif // COMMANDS_H
