#include <iostream>
#include <string>
#include <unordered_map>
#include <functional>

class Bot {
private:
    std::string name;
    std::unordered_map<std::string, std::function<void()>> commands;

public:
    explicit Bot(std::string botName)
        : name(std::move(botName)) {}

    void registerCommand(const std::string& command,
                         std::function<void()> callback) {
        commands[command] = std::move(callback);
    }

    void handleCommand(const std::string& input) const {
        auto it = commands.find(input);

        if (it != commands.end()) {
            it->second();
        } else {
            std::cout << "Unknown command: " << input << '\n';
        }
    }

    void run() const {
        std::cout << name << " is running.\n";
        std::cout << "Type a command:\n";

        std::string input;

        while (std::cout << "> " && std::getline(std::cin, input)) {
            if (input == "exit")
                break;

            handleCommand(input);
        }
    }
};

int main() {
    Bot bot("Discord Bot");

    bot.registerCommand("ping", [] {
        std::cout << "Pong!\n";
    });

    bot.registerCommand("hello", [] {
        std::cout << "Hello!\n";
    });

    bot.registerCommand("status", [] {
        std::cout << "Bot is online.\n";
    });

    bot.run();

    return 0;
}