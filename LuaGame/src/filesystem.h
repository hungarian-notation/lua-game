#pragma once

#include <string>

namespace luagame {
namespace filesystem {

void set_game_path(std::string game_path);

std::string get_game_path();

std::string resolve_resource(std::string resource_name);

}
}