#include "common.h"
#include "binding\lgapi.h"

#include <iostream>
#include <set>

#ifdef _WIN32
#include <Windows.h>
static void hide_console() { FreeConsole(); }
#else
static void hide_console() { /* noop */ }
#endif

int main(int argc, char * argv[]) {
	std::set<std::string> flags;
	std::string game_path = ".";

	for (int i = 1; i < argc; i++) {
		std::string arg(argv[i]);

		if (arg.length() > 1 && arg.substr(0, 1) == "-")
			flags.insert(arg.substr(1, arg.length() - 1));
		else if (i == argc - 1)
			game_path = arg;
	}

	bool force_console = false;

	#ifdef _DEBUG
	force_console = true;
	#endif

	if (flags.count("help")) {
		std::cout << "luagame [ -help | -console ] <game_path>" << std::endl;
		system("pause");
	} else {

		if (!(flags.count("console") || force_console))
			hide_console();

		std::cout << "running game from: " << game_path << std::endl;

		int result = 0;
		result = luagame_execute(game_path);
#ifdef _DEBUG
		system("pause");
		#endif
		return result;
	}
}