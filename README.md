# Kaguya Render

A simple physically based render.

[CPU / GPU development plan](https://github.com/StormPhoenix/kaguya/blob/master/PLAN.md)
   
## Feature

- Offline rendering algorithms
    - [x] Path Tracing
    - [x] Bidirectional Path Tracing
    - [x] Stochastic Progressive Photon Mapping
    - [ ] Metropolis Light Transport

- Material
    - Microfacet material
        - [x] Metal
        - [x] Frosted Glass
    - Diffuse
        - [x] Lambertian
    - Dielectric
        - [x] Glass
    - Subsurface

- Geometry
    - Triangle
    - Cube
    - Sphere
    - Rectangle
    
- Light
    - Point light
    - Spot light
    - Area light
    - Environment light
    - Sun light

- Denoiser
    - [ ] to be continued ...
    
- Filter
    - [ ] to be continued ...

## Example

- Material testball
<img src="https://github.com/StormPhoenix/render-repository-data/blob/master/thumbnail/Metal_testball.jpg" alt="metal-ball" width="70%">

- Dragon
<img src="https://github.com/StormPhoenix/render-repository-data/blob/master/thumbnail/Dragon.jpg" alt="dragon" width="70%">

- Teapot full
<img src="https://github.com/StormPhoenix/render-repository-data/blob/master/thumbnail/Teapot_full.jpg" alt="teapot-full" width="70%">

- Bunny glass
<img src="https://github.com/StormPhoenix/render-repository-data/blob/master/thumbnail/Bunny_glass.jpg" alt="bunny_glass" width="70%">

- Debug PT
<img src="https://github.com/StormPhoenix/render-repository-data/blob/master/thumbnail/Debug_bunny.jpg" alt="dragon" width="70%">
