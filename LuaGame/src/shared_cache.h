#pragma once

#include "common.h"

#include <memory>
#include <map>

namespace luagame {

	template <typename T_in, typename T_out>
	struct default_constructor {
		T_out * operator() (const T_in &prototype) { return new T_out(prototype); }
	};

	template <typename T_in, typename T_out, typename T_cons = default_constructor<T_in, T_out>, typename T_compare_in = std::less<T_in>>
	class shared_cache {
	public:

		typedef std::map<T_in, std::weak_ptr<T_out>, T_compare_in> map_type;
	public:
		shared_cache() : cons_functor() {}

		std::shared_ptr<T_out> operator[] (const T_in & prototype) {
			std::weak_ptr<T_out> existing = references[prototype];

			if (!existing.expired()) {
				std::shared_ptr<T_out>	ptr(existing);
				return ptr;
			} else {
				std::shared_ptr<T_out>	ptr(cons_functor(prototype));
				std::weak_ptr<T_out>	monitor(ptr);
				references[prototype] = monitor;
				return ptr;
			}
		}
	private:
		T_cons cons_functor;
		map_type references;
	};



}