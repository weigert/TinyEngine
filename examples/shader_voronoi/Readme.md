# TinyEngine
## GPU Accelerated Voronoi Textures and Filters

This demo program will generate real-time voronoi diagrams with N = \[256, 2^14\]. The voronoi diagram is rendered to an FBO so the texture can be used in another shader. It is sufficiently fast to have real-time voronoi based filters, running at over 60 FPS.

This demo includes two small image filters (mosaic, foam) that are parameterized to see how it behaves and give a demo of what this can do.

I have a blog article about how this works [here](https://weigert.vsos.ethz.ch/2020/08/01/gpu-accelerated-voronoi-textures-and-filters/). More information about performance and method there.
