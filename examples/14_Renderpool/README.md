# vertexpool

System for reducing OpenGL driver overhead without losing generality of drawables.

## Idea

When drawing many copies of the same VBO data, we can use instancing. This works well for e.g. particle systems.

When we want to draw many different models, currently we construct a VAO with VBO for each.

We can reduce the number of draw calls by consolidating all of the data into a single VAO + VBO.

To do this, we need to be able to dynamically update sub-sections of the VAO + VBO in accordance with
the boundaries of the individual model data.

It seems to me the smartest way to do this would be to create a persistently bound VBO (with fencing), the pointer of which we use as a memory pool and make quads / triangles / primitives available as they are needed.

Finally, when adding a model we create a multidraw indirect model which lets us then execute the draw calls
to the currently bound regions of memory.

## To Do
- Multiple Bucket Sizes using Binary Tree?
- Make Demo Showing Chunk Ordering for Blending (Advantage)
  - Far to Near / Near to Far
- Full Chunk Visibility Culling

## Performance:

Performs at about 2/3 time of the naive implementation for just chunk rendering - no updates!
