#pragma once

#define LG_NAMESPACE			"luagame"
#define LG_STATE_KEY			"__API_STATE__"

// -- Api Hooks

#define LG_HOOK_INIT			"initialize"
#define LG_HOOK_UPDATE			"update"
#define LG_HOOK_DRAW			"draw"

// -- Lua Meta Keys

#define LUAMETA_INDEX			"__index"
#define LUAMETA_NEWINDEX		"__newindex"
#define LUAMETA_CALL			"__call"