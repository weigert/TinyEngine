# vertexpool

System for reducing OpenGL driver overhead without losing generality of drawables.

![Voxel Gif](https://github.com/weigert/TinyEngine/blob/master/examples/14_Renderpool/voxels.gif)

[Link to Blog Post](https://weigert.vsos.ethz.ch/2021/04/04/high-performance-voxel-engine/)

## Idea

When drawing many copies of the same VBO data, we can use instancing. This works well for e.g. particle systems.

When we want to draw many different models, a naive way is to construct a VAO with VBO for each.

We can reduce the number of draw calls by consolidating all of the data into a single VAO + VBO.

To do this, we need to be able to dynamically update sub-sections of the VAO + VBO in accordance with
the boundaries of the individual model data. This is complicated memory handling.

If we take on the burden of handling this memory, there is still the possibility that memory needs shifting
and draw calls need splitting. We are now officially **lost in the sauce**.

It seems to me the smartest way to do this would be to create a persistently bound VBO (with fencing), the pointer of which we use as a memory pool and make quads / triangles / primitives available as they are needed.

Finally, when adding a model we create a multidraw indirect model which lets us then execute the draw calls
to the currently bound regions of memory.

This is effectively a memory pool, and the multidraw indirect lets us do some other neat things.

## To Do

- Multiple Bucket Sizes using Binary Tree?
- Full Visibility Culling Somehow?
- Cleanup vertexpool class to have better error and exception handling

## Performance:

Performs at about 0.6 the time of the naive implementation for just chunk rendering - no updates!
