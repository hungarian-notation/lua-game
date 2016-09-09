#include "any_key.h"
#include <stdio.h>
#include <conio.h>

void luagame::util::any_key(const char * prompt) {
	printf_s(prompt);
	while (!_kbhit()) { /* await */ }
}