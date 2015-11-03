# DirectionalLight

Provides a directional light across the entire scene with shadows.

- `Color`: 
    Controls the color of the directional light.
- `Direction`: 
    Direction of the light.
- `SplitDistribution`: 
    Used to control how shadowmap cascades are distributed. 
- `FarPlane`:
	Controls the furthest distance shadows can be rendered.

# DLAA

Directional Anti-Aliasing. Removes the jagged edges from a scene.

# FXAA

Fast Approximate Anti-Aliasing. Removes the jagged edges from a scene. Faster than DLAA, but worse quality.

# HDR

This effect will tonemap high dynamic range colors into a visible range as well as apply blur to colors that exceed the normal range. This provides a bloom effect.

- `MiddleGray`: 
    The point a color is considerer the middle gray. Half way between white and black.
- `WhitePoint`: 
    The point a color is considered white.
- `Threshold`: 
    After a color channel exceeds this value it has blur applied.

# SkyLight

Generates radiance and irradiance from an input cubemap and applies it as ambient lighting.

- `SkyCube`: 
    Cubemap texture to generate radiance and irradiance from.

# SkyBox

Displays an input cubemap as the scene background.

- `Texture`: 
    Cubemap texture to display as the sky.

# SSAO

Applies ambient occlusion effect to the scene.

- `Bias`: 
    Difference between depth before its considered close enough for ambient occlusion to occur.
- `Intensity`: 
    Multiplier of the overall effect.
- `Scale`: 
    Scale the sampling area.
- `Radius`: 
    How far to sample from each point.