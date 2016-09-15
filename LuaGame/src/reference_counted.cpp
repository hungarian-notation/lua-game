#include "reference_counted.h"
#include <cassert>

luagame::reference_counted::reference_counted() : references(0), observers() {

#ifdef LUAGAME_TRACK_GLOBAL_REFERENCES
	++global_referenced_objects;
	_log("global objects: %d", global_referenced_objects);
#endif

}

luagame::reference_counted::~reference_counted() {
	_log("dconst reference_counted");
#ifdef LUAGAME_TRACK_GLOBAL_REFERENCES
	--global_referenced_objects;
	_log("global objects: %d", global_referenced_objects);
#endif
}

void luagame::reference_counted::acquire() { ++references; }

bool luagame::reference_counted::release() {
	--references;

	if (references < 0) _err("reference count went < 0");

	if (references <= 0) {

		for (std::vector<reference_observer *>::iterator it = observers.begin(); it != observers.end(); ++it) {
			(*it)->invalidate();
		}

		delete this;
		return true;
	} else {
		return false;
	}
}

void luagame::reference_counted::add_observer(reference_observer * observer) {

	observers.push_back(observer);

}

void luagame::reference_counted::remove_observer(reference_observer * observer) {
	if (this == (reference_counted*)0xDDDDDDDD) _err("I do not exist.");

	for (std::vector<reference_observer *>::iterator it = observers.begin(); it != observers.end(); ++it) {
		if (*it == observer) {
			observers.erase(it);
			return;
		}
	}

	_err("no such observer");
}