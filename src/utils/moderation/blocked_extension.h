#ifndef BLOCKED_EXTENSION_H
#define BLOCKED_EXTENSION_H

#include <algorithm>
#include <array>
#include <cctype>
#include <string_view>

namespace moderation {
    inline constexpr auto blockedExtensions = std::to_array<std::string_view>({
        ".exe", ".dll", ".bat", ".cmd", ".scr", ".msi", ".ps1"
    });

    inline bool hasBlockedExtension(std::string_view filename)
    {
        const auto dot = filename.find_last_of('.');
        if (dot == std::string_view::npos)
            return false;

        const std::string_view extension = filename.substr(dot);

        return std::ranges::any_of(blockedExtensions,
            [extension](const std::string_view blocked) {
                return extension.size() == blocked.size()
                    && std::ranges::equal(extension, blocked,
                        [](const char lhs, const char rhs) {
                            return std::tolower(static_cast<unsigned char>(lhs))
                                == std::tolower(static_cast<unsigned char>(rhs));
                        });
            });
    }
}

#endif // BLOCKED_EXTENSION_H
