# Float Node

A single channel floating point value. 

- `Value`: 
    The numeric value of the node. Example: "0.1"
- `UniformName`: 
    The name of the uniform this node is associated with. This allows the value to be dynamically changed at runtime.

# Vec2 Node

A two channel vector value. 

- `Value`: 
    The value of the two channels. Example: "0.1 0.2"
- `UniformName`: 
    The name of the uniform this node is associated with. This allows the value to be dynamically changed at runtime.

# Vec3 Node

A three channel vector value. 

- `Value`: 
    The value of the two channels. Example: "0.1 0.2 0.3"
- `UniformName`: 
    The name of the uniform this node is associated with. This allows the value to be dynamically changed at runtime.

# Vec4 Node

A four channel vector value. 

- `Value`: 
    The value of the two channels. Example: "0.1 0.2 0.3 0.4"
- `UniformName`: 
    The name of the uniform this node is associated with. This allows the value to be dynamically changed at runtime.

# Texture Node

Texture sampling node. Texture slots are assigned in the MaterialAsset and then utilized via this node.

- `Slot`: 
    The texture slot number this texture node is associated with.
- `PremultiplyAlpha`: 
    If true the value output is first multiplied by the textures alpha value.