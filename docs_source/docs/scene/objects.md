# BaseComponent

All components are derived from BaseComponent so all the below properties are available in any component.

- `Position`: 
    Local position within the owner object.
- `Scale`: 
    Local scale of the component.
- `Rotation`: 
    Local rotation of the component.

# AnimationComponent

Takes an animation from MeshAsset and plays it back on the Target MeshComponent at the specified Speed.

- `Speed`: 
    Playback rate of the animation. 1.0 would be regular speed, 0.5 is half speed, etc.
- `Target`: 
    The InternalName of the MeshComponent to apply the animation to. Must be within the same SceneObject.
- `MeshAsset`: 
    Name of the asset to pull the animation data from.

# LightComponent

Represents a point light.

- `Tint`: 
    Controls the color of the light.
- `Attenuation`: 
    Controls the rate at which the light loses power as it gets further from the source.
- `Intensity`: 
    Multiplier of the strength of the light.

# MeshComponent

Used to render a mesh.

- `MeshAsset`: 
    Name of the asset to render.

# PhysicsComponent

This component is the physics representation of the entity. It will apply any transformations it receives to it's owner object.

- `OnCollideFunction`: 
    Name of the TorqueScript function to call when a collision occurs.
- `CollisionType`: 
    Abritrary type string to pass along with OnCollideFunction.
- `Static`: 
    True/False whether the physics component is static or should simulate gravity, etc.

# TextComponent

Used to render simple 3D text. Try to scale the component to match the aspect ratio of TextureWidth and TextureHeight for best quality.

- `Text`: 
    String of text to render.
- `TextColor`: 
    Color of text to render.
- `TextSize`: 
    Size of the text to render in standard font pt.
- `TextureWidth`: 
    Width of the internal texture used to render the text to. Larger = better quality but more memory usage.
- `TextureHeight`: 
    Height of the internal texture used to render the text to. Larger = better quality but more memory usage.
- `UScrollSpeed`: 
    Speed to scroll the text horizontally.
- `VScrollSpeed`: 
    Speed to scroll the text vertically.