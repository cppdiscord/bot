cmake_minimum_required(VERSION 3.3)

project(bot LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(dpp CONFIG REQUIRED)

file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/src/config.json DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/src/res DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

add_executable(bot
	src/main.cpp

	#globals
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

	# utils
	src/utils/suggestion/suggestion.cpp
	src/utils/suggestion/suggestion.h
)

target_link_libraries(bot
	PRIVATE dpp::dpp
)
