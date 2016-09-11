#include "lgapi_shared.h"

int luagame::release_api_object(lua_State * L) {
	reference_counted ** objref = (reference_counted **)lua_touserdata(L, 1);
	(*objref)->release();
	_log("lua: released api object");
	return 0;
}

void luagame::create_api_object(lua_State * L, reference_counted * ptr) {
	reference_counted ** objref = (reference_counted **)lua_newuserdata(L, sizeof(reference_counted *));
	_log("lua: created api object");
	*objref = ptr;
}
