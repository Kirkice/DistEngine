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
* Fast Approximate Anti-Aliasing and anti-aliasing
* GlobalFog System(Linear Fog、Height Fog、Volume Fog)
* PostProcess
  * Natural tone mapping
  * RGBSplit
  * RadialBlur
  * Vignette
  * Decolor
  * Brightness
  * HSV
  * Mosaic
  * Sharpen
  * Spherize
  * WhiteBalance
  * OilPaint
  * Relief
  * EdgeDetection


# Dependencies
- Visual Studio 2019
- DirectX 3D 12

# Community
Technical exchange can visit me from this link ：https://www.zhihu.com/people/kirk-54-38.
