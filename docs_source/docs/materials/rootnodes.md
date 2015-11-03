# Deferred Node

This is the most commonly used root node. It outputs into the deferred shading pipeline. This is ideal for performance and if you can use the deferred node to achieve your goals its strongly encouraged.

- `ColorSrc`: 
    Internal name of the node to get the color value from. This is commonly filled by a diffuse or albedo map.
- `NormalSrc`: 
    Internal name of the node to get the normal value from. Normals are usually provided by a normal map.
- `MetallicSrc`: 
    Internal name of the node to get the metallic value from. Roughness controls how reflective or metal-like a material is. Ranges in value from 0 to 1 with 1 being the fully metal.
- `RoughnessSrc`: 
    Internal name of the node to get the roughness value from. Roughness controls how rough a material is. Ranges in value from 0 to 1 with 1 being the most rough.
- `EmissiveSrc`: 
    Internal name of the node to get the emissive value from. Emissive color is unlit and added on top of the material color.
- `WorldPosOffsetSrc`:
	Internal name of the node to get the world position offset from. World Position Offset is a value added to each of the vertices of the model being rendered to offset its position.