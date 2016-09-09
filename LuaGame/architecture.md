# LuaGame

## Overview

### Mission Statement
The purpose of the LuaGame library is to allow for the rapid prototyping of primarily 3D games. It is designed according to the Lua paradigm of economy and simplicity. Generally, ease of use is preferred over performance. That being said, some performance considerations must be made to allow for the realtively extreme performance requirements of 3D games.

LuaGame is not suited for production, and it is not the best choice for prototyping 2D games, though it can serve that purpose.

## Managers

### graphics_manager
- Manages GLFW window state.

### scene_manager
- Manages a scene graph.
- Draws the scene.

## Rendering

### scene_node
- Manages relative and absolute transform for a scene node.
- Tracks bounding box for frustrum culling. 

### camera_node : *scene_node*
- A unique node whose absolute position controls the view matrix.

### empty_node : *scene_node*
- An empty scene node that can contain other nodes.

### model_node : *scene_node*
- A scene node that contains an instance of a model.

### billboard_node : *scene_node*
- A scene node that contains a billboarded sprite.
- Modes:
  - **BILLBOARD_FULL**: orient the billboard plane so that it is orthogonal to the look vector
  - **BILLBOARD_YAW**: a limited billboard that only adjusts the yaw (rotation about the vertical axis) of the billboard plane.

*Or do I want to have ROLL, PITCH, and YAW flags that adjust how the sprite can adjust...*

*Is BILLBOARD_YAW relative to the world vertical axis or the node vertical axis?*

*Should this be implemented with model still? Is the math done on the graphics card?*

### model
- Contains one or more meshes that are rendered together as a model.
- Holds the functionality that renders the model.

### mesh
- Contains geometry and a reference to the material and textures used to render that geometry.
- Wraps an Array Buffer Object of the vertices in the mesh.
- Has a flag for either triangles or quads.

### material
- Contains the information needed to render a mesh.
- Wraps an OpenGl shader program.

### texture
- Wraps an OpenGl texture.
- Allows for configuration of texture filters and mipmaps.

## Math

Vector math functionality is provided by glm.