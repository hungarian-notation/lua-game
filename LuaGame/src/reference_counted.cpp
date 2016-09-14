#include "reference_counted.h"

luagame::weak_reference::weak_reference(reference_counted * referenced) : target(referenced) {
	referenced->add_light_reference(this);
}

luagame::weak_reference::~weak_reference() {
	if (target) target->remove_light_reference(this);
	invalidate();
}

void luagame::weak_reference::invalidate() {
	target = nullptr;
}

// Returns true if this light reference is still valid.

bool luagame::weak_reference::is_vaid() { return target != nullptr; }

luagame::reference_counted::reference_counted() : references(1) {

#ifdef LUAGAME_TRACK_GLOBAL_REFERENCES
	++global_referenced_objects;
	_log("global objects: %d", global_referenced_objects);
#endif

}

luagame::reference_counted::~reference_counted() {
	// -- invalidate registered light references

	std::list<weak_reference *>::iterator it;
	for (it = light_references.begin(); it != light_references.end(); ++it)
		(*it)->invalidate();

#ifdef LUAGAME_TRACK_GLOBAL_REFERENCES
	--global_referenced_objects;
	_log("global objects: %d", global_referenced_objects);
#endif

}

void luagame::reference_counted::acquire() { ++references; }

bool luagame::reference_counted::release() {
	--references;

	if (references <= 0) {
		delete this;
		return true;
	} else {
		return false;
	}
}

void luagame::reference_counted::add_light_reference(weak_reference * ref) {
	light_references.push_back(ref);
}

void luagame::reference_counted::remove_light_reference(weak_reference * ref) {
	light_references.remove(ref);
}
