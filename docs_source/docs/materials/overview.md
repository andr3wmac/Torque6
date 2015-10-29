# Asset

The material asset definition contains a path referencing the template the material should use as well as references to any textures or additional inputs the material may utilize.

Example:
```
<MaterialAsset
    AssetName="sphereMaterial"
    TemplateFile="sphereMaterial.taml"
    Texture0="^MaterialExample/models/emissive_mask.png"
/>
```

# Template

A material template contains the actual node definitions and how they are connected. This is used by Torque 6 to generate the actual material shaders. 

Example:
```
<MaterialTemplate>
    <DeferredNode
        ColorSrc="ColorVal"
        MetallicSrc="MetalVal"
        RoughnessSrc="RoughVal" />
    <Vec3Node
        InternalName="ColorVal"
        UniformName="sphereColorVal"
        Value="1.0 0.0 0.0"
    />
    <FloatNode
        InternalName="MetalVal"
        UniformName="sphereMetalVal"
        Value="1.0"
    />
    <FloatNode
        InternalName="RoughVal"
        UniformName="sphereRoughVal"
        Value="0.0"
    />
</MaterialTemplate>
```