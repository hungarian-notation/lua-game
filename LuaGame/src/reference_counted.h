#pragma once

#define LUAGAME_TRACK_GLOBAL_REFERENCES

#include "common.h"

#include <stdexcept>
#include <list>

namespace luagame {

	class reference_counted;
	
	// A reference to a reference_counted object that does not prevent deallocation of that object, but is instead
	// notified when the referenced object is deallocated.
	class weak_reference {
	public:
		weak_reference(reference_counted * referenced);
		~weak_reference();

	public:
		// Marks this reference as invalid. This should only be called by the referenced object.
		void invalidate();

		// Returns true if this light reference is still valid.
		bool is_vaid();

		// Gets a pointer to the referenced object.
		//
		// This method throws and exception if this reference is invalid.
		template <class reftype> reftype * get() {
			if (is_valid()) {
				return dynamic_cast<reftype *>(target);
			} else throw std::exception("reference is not valid");
		}
	private:
		reference_counted * target;
	};

#ifdef LUAGAME_TRACK_GLOBAL_REFERENCES
	static int global_referenced_objects = 0;
#endif

	class reference_counted {
	public:

		reference_counted();

		virtual ~reference_counted();

		void acquire();
		bool release();
		void add_light_reference(weak_reference * ref);
		void remove_light_reference(weak_reference * ref);

	private:
		int								references;
		std::list<weak_reference*>	light_references;
	};
}