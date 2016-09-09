#pragma once

#define LUAGAME_TRACK_GLOBAL_REFERENCES

#include "common.h"
#include <stdexcept>

namespace luagame {

#ifdef LUAGAME_TRACK_GLOBAL_REFERENCES
	static int global_referenced_objects = 0;
#endif

	class reference_counted {
	public:
		reference_counted() : references(1) {

#ifdef LUAGAME_TRACK_GLOBAL_REFERENCES
			++global_referenced_objects;
			_log("global objects: %d", global_referenced_objects);
#endif

		}

		virtual ~reference_counted() {

#ifdef LUAGAME_TRACK_GLOBAL_REFERENCES
			--global_referenced_objects;
			_log("global objects: %d", global_referenced_objects);
#endif

		}

		void acquire() { ++references; }

		bool release() {
			--references;

			if (references <= 0) {
				delete this;
				return true;
			} else {
				return false;
			}
		}
	private:
		int references;
	};
}