# Assets

Torque 6 uses managed assets. For each model, texture, etc you bring into the engine an asset definition is generated. This helps to track what's being using in the engine. It helps with exporting and organization.

# EntityTemplateAsset

Points to an Entity Template.

- `TemplateFile`: 
    Path to the entity template file.

# MaterialAsset

Manages a material based on template and also controls the textures supplied to that material template.

- `TemplateFile`: 
    Path to the material template.
- `TextureCount`: 
    Number of textures this material should use. This creates a TextureAssetX and TextureFileX field for each number.
- `TextureAssetX`:
	Asset name of the texture to be used for Slot X where X is a number from 0 to (TextureCount - 1). Note: use TextureAsset OR TextureFile, not both.
- `TextureFileX`:
	Path to the texture file to be used for Slot X where X is a number from 0 to (TextureCount - 1). Note: use TextureAsset OR TextureFile, not both.

# MeshAsset

Used to render a mesh.

- `MeshFile`: 
    Path to the mesh file to load. Supports a wide variety of model formats.

# PluginAsset

Manages an external DLL plugin.

- `PluginFile`: 
    Path to the dll file without extension. The extension will automatically be added based on the platform.

# ShaderAsset

Used to render a mesh.

- `VertexShaderFile`: 
    Path to the vertex portion of this shader.

- `PixelShaderFile`: 
    Path to the pixel portion of this shader.

# TextureAsset

Used to render a mesh.

- `TextureFile`: 
    Path to the texture.