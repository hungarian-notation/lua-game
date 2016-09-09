#include "scene_node.h"

using namespace luagame;

scene_node::scene_node(scene_manager * scene) : scene(scene) {
	rel_rotation = glm::quat();
	rel_translation = glm::vec3(0, 0, 0);
	rel_scale = glm::vec3(1, 1, 1);
}

scene_node::~scene_node() {
	remove_children();
	if (parent) remove();
}

void luagame::scene_node::set_parent(scene_node * parent) {
	if (parent && this->parent) _err("node already has a parent in the scene graph");

	this->parent = parent;
	this->invalidate_transform();
}

void luagame::scene_node::update_hierarchy() {
	if (parent && parent->scene)
		this->scene = parent->scene;
	else this->scene = nullptr;

	for (sn_list::const_iterator it = children.begin(), end = children.end(); it != end; ++it)
		(*it)->update_hierarchy();
}

glm::mat4 scene_node::get_transform() {
	if (dirty_transform) update_transform();
	return abs_transform;
}

void scene_node::update_transform() {
	glm::mat4 loc_transform;

	loc_transform = glm::translate(loc_transform, rel_translation);
	loc_transform = loc_transform * glm::mat4_cast(rel_rotation);
	loc_transform = glm::scale(loc_transform, rel_scale);

	if (parent) abs_transform = parent->get_transform();
	else abs_transform = glm::mat4();

	abs_transform = abs_transform * loc_transform;
	dirty_transform = false;
}

void scene_node::invalidate_transform() {
	dirty_transform = true;

	for (sn_list::const_iterator it = children.begin(), end = children.end(); it != end; ++it)
		(*it)->invalidate_transform();
}

void scene_node::rotate(float angle, glm::vec3 axis) {
	rel_rotation = glm::normalize(glm::rotate(rel_rotation, angle, axis));
	invalidate_transform();
}

void scene_node::rotate(glm::quat rotation) {
	rel_rotation = glm::normalize(rel_rotation * rotation);
	invalidate_transform();
}

void scene_node::set_rotation(const glm::quat & rotation) {
	rel_rotation = glm::normalize(rotation);
	invalidate_transform();
}

void scene_node::translate(glm::vec3 translation) {
	rel_translation += translation;
	invalidate_transform();
}

void scene_node::set_translation(glm::vec3 translation) {
	rel_translation = translation;
	invalidate_transform();
}

void scene_node::scale(glm::vec3 scale) {
	rel_scale *= scale;
	invalidate_transform();
}

void scene_node::set_scale(glm::vec3 scale) {
	rel_scale = scale;
	invalidate_transform();
}

/// <summary>Retrieves the parent of this node.</summary>

scene_node * scene_node::get_parent() const {
	return parent;
}

/// <summary>Remove the node from the scene.</summary>

void scene_node::remove() {
	if (!parent) _err("can not remove orphan (root?) node");
	else parent->remove_child(this);
}

/// <summary>Add a old_child to this scene node.</summary>

void scene_node::add_child(scene_node * new_child) {
	new_child->acquire();
	children.push_back(new_child);

	new_child->set_parent(this);
}

/// <summary>Remove a specific old_child node from this scene node.</summary>

void scene_node::remove_child(scene_node * old_child) {
	if (old_child->parent != this) _err("supplied node is not a child of this node");
	else {
		old_child->set_parent(nullptr);

		children.remove(old_child);
		old_child->release();
	}
}

/// <summary>Remove any children of this node from the scene.</summary>

void scene_node::remove_children() {
	sn_list to_remove(children);

	for (sn_list::const_iterator it = to_remove.begin(), end = to_remove.end(); it != end; ++it)
		remove_child(*it);
}

/// <summary>Gets the number of children directly under this node.</summary>

size_t scene_node::size() {
	return children.size();
}

/// <summary>Recursively gets the total number of nodes under this node.</summary>

size_t scene_node::full_size() {
	size_t acc = 0;

	for (sn_list::const_iterator it = children.begin(), end = children.end(); it != end; ++it)
		acc += (*it)->full_size() + 1;

	return acc;
}
