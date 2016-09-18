#include "common.h"
#include "binding\lgapi.h"


#include "window_context.h"
#include "shared_cache.h"

using namespace luagame;

struct dummy_prototype {
	int a, b, c;
};

class dummy_type {
public:
	dummy_type(dummy_prototype prototype) : prototype(prototype) {}
	dummy_prototype prototype;
};

int main() {
	int result = 0;
	result = luagame_execute();

	system("pause");
	return result;
}