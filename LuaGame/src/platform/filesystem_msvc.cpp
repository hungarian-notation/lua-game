#include "../filesystem.h"

#if defined(_MSC_VER)

#include <filesystem>

using namespace std::tr2::sys;

static path g_game_path;

void luagame::filesystem::set_game_path(std::string game_path) {
	g_game_path = path(game_path);
}

std::string luagame::filesystem::get_game_path() {
	return g_game_path.string();
}

std::string luagame::filesystem::resolve_resource(std::string resource_name) {
	return g_game_path.append(resource_name).string();
}

#endif