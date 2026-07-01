cmake_minimum_required(VERSION 3.20)

project(bot LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)

find_package(dpp CONFIG QUIET)

if (NOT dpp_FOUND)
    message(STATUS "DPP not found, fetching from GitHub...")
    FetchContent_Declare(
        dpp
        GIT_REPOSITORY https://github.com/brainboxdotcc/DPP.git
        GIT_TAG v10.1.4
    )
    FetchContent_MakeAvailable(dpp)

	if (NOT TARGET dpp::dpp AND TARGET dpp)
        add_library(dpp::dpp ALIAS dpp)
    endif()
else()
    message(STATUS "Using system-installed DPP")
endif()

file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/src/config.json DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/src/res DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

add_executable(bot
	src/main.cpp

	#globals
	src/globals/globals.cpp
	src/globals/globals.h

	# commands
	src/commands/commands.h
	src/commands/utils.cpp
	src/commands/topic_cmd.cpp
	src/commands/coding_cmd.cpp
	src/commands/close_cmd.cpp
	src/commands/ticket_cmd.cpp
	src/commands/code_cmd.cpp
	src/commands/project_cmd.cpp
	src/commands/rule_cmd.cpp

	# utils
	src/utils/suggestion/suggestion.cpp
	src/utils/suggestion/suggestion.h
	src/utils/moderation/moderation.cpp
	src/utils/moderation/moderation.h
)

target_link_libraries(bot PRIVATE dpp::dpp)
