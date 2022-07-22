<div align="center">

# 💡 DistEngine

**Personal real-time global illumination toy engine made with DirectX 12**

[![DIST](https://img.shields.io/badge/dist-open%20source-blueviolet.svg)]()
[![DIST](https://img.shields.io/badge/demo-20220722-brightgreen)]()
[![DIST](https://img.shields.io/static/v1?label=license&message=MIT&color=green)](https://opensource.org/licenses/MIT)
</div>


This is an open source rendering engine for learning. In the future,various rendering functions will be added continuously. As it is a project for learning, performance overhead will not be considered.

`DistEngine` will include various lighting models and rendering functions in the future, such as real physics rendering, cartoon rendering, and water rendering. DXR ray tracing, SSGI, SH and other global lighting technologies.

<p align="left">
  <img height="438px" width="802px" src="https://imgur.com/Oeovi71.png">
</p>

_Simple scene rendered in DitEngine. 

<p align="left">
  <img height="438px" width="802px" src="https://imgur.com/hS6ojTH.png">
</p>

_Simple screen space global illumination in DitEngine.[Scene]() by ConelBox

## Features `done`

* Fbx Obj mesh loading
* Atmospheric rendering skybox
* Deferred rendering path and forward rendering path
* Standard PBR with GGX and roughness/metalness
  * Energy-preserving multi-scattering BRDF
* Fast approximate anti-Aliasing
* Dynamic global illumination
  * Screen-space global illumination
* Global fog system
* FidelityFX Super Resolution 1.0
* Imgui editor manager
* PostProcess
  * RGB-split feature
  * Radial blur feature
  * Vignette feature
  * Decolor feature
  * Brightness feature
  * HSV feature
  * Screen mosaic feature
  * Sharpen feature
  * White balance feature
  * Oil paint feature
  * Relief feature
  * Robert edge detection feature
  * Screen-space ambient occlusion

## Features `todo`

* Temporal super-resolution and anti-aliasing
* Natural tone mapping
* Basic motion blur
* Cluster based light culling
* Dynamic global illumination
  * Voxel global illumination
* Nvidia Optix Denoiser
* Direct X ray tracing
* Volumetric Lighting
* Carton rendering shader
* Proceduralcontent generation terrain mesh
* Water Rendering
* Jolit physics system add

## Controls in the `view` app

* WSAD - movement
* Mouse + RMB - rotate the camera
* Mouse + LMB - select game object

## Technical details

* Repository highlights:
  * HLSL shaders: [`DistEngine/Asset/Shaders`](DistEngine/Asset/Shaders)
  * Model meshs: [`DistEngine/Asset/Mesh`](DistEngine/Asset/Mesh)
  * Textures: [`DistEngine/Asset/Textures`](DistEngine/Asset/Textures)
  * Core code: [`Core`](Core)
  * Component code: [`Component`](Component)
  * Logic code: [`System`](System)
  * Rendering code: [`Render`](Render)
  * Editor code: [`Editor`](Editor)

## Primary platforms

`DistEngine` currently works on a limited range of operating systems and hardware.

Hardware:

* Nvidia RTX series
* Nvidia GTX 1060 and newer _with 6+ GB of VRAM_ (slow: driver-emulated ray-tracing)
* AMD Radeon RX 6000 series

Operating systems and language:

* Windows
* C++ 17

# Community
Technical exchange can visit me from this link ：https://www.zhihu.com/people/kirk-54-38.
