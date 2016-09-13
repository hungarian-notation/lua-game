#include "binding.h"

using namespace luagame;

int lgapi_releaseref(lua_State * L) {
	reference_counted ** objref = (reference_counted **)lua_touserdata(L, 1);
	(*objref)->release();
	_log("lua: released api object");
	return 0;
}

void luagame_pushref(lua_State * L, reference_counted * ptr) {
	reference_counted ** objref = (reference_counted **)lua_newuserdata(L, sizeof(reference_counted *));
	_log("lua: created api object");
	*objref = ptr;
}
