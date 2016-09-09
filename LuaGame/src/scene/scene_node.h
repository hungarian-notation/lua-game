#pragma once

#include "..\common.h"
#include "..\graphics\graphics_math.h"

#include <list>

namespace luagame {

	class scene_manager; // forward declaration

	enum node_type {
		empty_node_type, mesh_node_type, camera_node_type
	};

	// -- Auxiliary Types

	class scene_node;

	typedef std::list<scene_node *> scene_node_list;
	typedef scene_node_list sn_list;

	// -- Main Class Definition

	class scene_node : virtual public reference_counted {
	public:

		scene_node(scene_manager * scene = nullptr);

		virtual ~scene_node();

		virtual node_type type() { return node_type::empty_node_type; }

	protected:

		virtual void set_parent(scene_node * parent);

		virtual void update_hierarchy();

		virtual void update_transform();

	public:

		// -- Transform Manipulation

		glm::mat4 get_transform();

		void invalidate_transform();

		void rotate(float angle, glm::vec3 axis);

		void rotate(glm::vec3 euler) { rotate(glm::quat(euler)); }

		void rotate(glm::quat rotation);

		void set_rotation(float angle, glm::vec3 axis) { set_rotation(glm::quat(angle, axis)); }

		void set_rotation(glm::vec3 euler) { set_rotation(glm::quat(euler)); }

		void set_rotation(const glm::quat &rotation);

		void translate(glm::vec3 translation);

		void set_translation(glm::vec3 translation);

		void scale(glm::vec3 scale);

		void set_scale(glm::vec3 scale);

		// -- Scene Graph Manipulation

		/// <summary>Retrieves the parent of this node.</summary>
		luagame::scene_node * get_parent() const;

		/// <summary>Remove the node from the scene.</summary>
		void remove();

		/// <summary>Add a old_child to this scene node.</summary>
		void add_child(luagame::scene_node * new_child);

		/// <summary>Remove a specific old_child node from this scene node.</summary>
		void remove_child(luagame::scene_node * old_child);

		/// <summary>Remove any children of this node from the scene.</summary>
		void remove_children();

		/// <summary>Gets the number of children directly under this node.</summary>
		size_t size();

		/// <summary>Recursively gets the total number of nodes under this node.</summary>
		size_t full_size();

		const sn_list & get_children() { return children; }

	protected:

		luagame::scene_manager *	scene;
		luagame::scene_node *		parent;
		luagame::sn_list			children;

		glm::quat					rel_rotation;
		glm::vec3					rel_translation,
									rel_scale;

		glm::mat4					abs_transform;
		bool						dirty_transform;
	};
}