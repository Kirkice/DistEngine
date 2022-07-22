<div align="center">

# 💡 DistEngine

**Personal real-time global illumination toy engine made with DirectX 12**

[![DIST](https://img.shields.io/badge/dist-open%20source-blueviolet.svg)]()
[![DIST](https://img.shields.io/badge/demo-20220722-brightgreen)]()
[![DIST](https://img.shields.io/static/v1?label=license&message=MIT&color=green)](https://opensource.org/licenses/MIT)
</div>


This is an open source rendering engine for learning. In the future,various rendering functions will be added continuously. As it is a project for learning, performance overhead will not be considered.

`DistEngine` will include various lighting models and rendering functions in the future, such as real physics rendering, cartoon rendering, and water rendering. DXR ray tracing, SSGI, SH and other global lighting technologies.


![image (5)](https://imgur.com/Oeovi71.png)
_Simple scene rendered in DitEngine. 

## Features(done)

* Fbx Obj mesh loading
* Atmospheric rendering skybox
* Deferred rendering path and forward rendering path
* Standard PBR with GGX and roughness/metalness
  * Energy-preserving multi-scattering BRDF
* Fast approximate anti-Aliasing
* Screen-space ambient occlusion
* Screen-space global illumination
* Global fog system
* FidelityFX Super Resolution 1.0
* PostProcess

## Features(todo)

* Cluster based light culling
* Direct X ray tracing
* Water Rendering
* Jolit physics system


## Technical details

* Repository highlights:
  * HLSL shaders: [`DistEngine/Asset/Shaders`](DistEngine/Asset/Shaders)
  * Core code: [`Core`](Core)
  * Main render graph passes: [`Render`](Render)

## Primary platforms

`kajiya` currently works on a limited range of operating systems and hardware.

Hardware:

* Nvidia RTX series
* Nvidia GTX 1060 and newer _with 6+ GB of VRAM_ (slow: driver-emulated ray-tracing)
* AMD Radeon RX 6000 series

Operating systems:

* Windows

# Community
Technical exchange can visit me from this link ：https://www.zhihu.com/people/kirk-54-38.
