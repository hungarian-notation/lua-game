#pragma once

#define LUAGAME_TRACK_GLOBAL_REFERENCES

#include "common.h"

#include <stdexcept>
#include <vector>

namespace luagame {
	class reference_counted;
	class reference_observer;

#ifdef LUAGAME_TRACK_GLOBAL_REFERENCES
	static int global_referenced_objects = 0;
#endif

	// -- REFERENCE OBSERVER

	// -- REFERENCE COUNTED

	class reference_counted {
	public:

		reference_counted();
		virtual ~reference_counted();

		virtual void acquire();
		virtual bool release();

		void add_observer(reference_observer * observer);
		void remove_observer(reference_observer * observer);

	private:
		int											references;
		std::vector<reference_observer*>	observers;
	};

	class reference_observer {
	public:

		reference_observer(reference_counted * observed) : resource_ptr(observed) {
			if (observed)
				observed->add_observer(this);
		}

		reference_observer() : reference_observer(nullptr) {

		}

		~reference_observer() {
			_log("dcons reference_observer");

			if (is_valid()) {
				if (resource_ptr == (reference_counted*)0xDDDDDDDD) _err("resource does not exist");
				resource_ptr->remove_observer(this);
				resource_ptr = nullptr;
			}
		}

		bool is_valid() {
			return resource_ptr != nullptr;
		}

		template <class rtype> rtype * get() { 
			return dynamic_cast<rtype *>(resource_ptr);
		}

		void invalidate() {
			resource_ptr = nullptr;
		}

	private:
		reference_counted * resource_ptr;
	};

	// -- REFERENCED

	template <class rtype>
	class reference {
	public:
		reference(rtype * ref) : ref(ref) { if (ref) ref->acquire(); }
		~reference() { if (ref) ref->release(); }
		rtype * get() { return ref; }
	private:
		rtype * ref;
	};

	// -- weak_reference

	template <class rtype>
	class weak_reference {
	public:

		weak_reference(rtype * ref) : observer_ptr(new reference_observer(ref)) {}

		weak_reference() : weak_reference(nullptr) {}

		~weak_reference() {
			_log("dcons weak ref");

			if (observer_ptr) {
				if (this == (weak_reference*)0xDDDDDDDD) _err("I do not exist.");
				delete observer_ptr;
				observer_ptr = nullptr;
			}
		}

		bool valid() { return observer_ptr->is_valid(); }
		rtype * get() { return observer_ptr->get<rtype>(); }

	private:
		reference_observer * observer_ptr;
	};
}

