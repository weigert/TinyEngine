## NEW TODO

- Render Chunks Near to Far???
- This would potentially remove occluded pixels
- How would this be possible?
- Order the indirect draw calls by their distance to the camera somehow using a sort,
then update somehow. Only do this when necessary? What would be the execution time?

- Don't render

- Multithread where appropriate?
    Where can I elegantly introduce multithreading?
    In the vertexpool meshing?
    Is the vertexpool somehow intrinsically threadable?

- Do most calculations in the vertex shader,
do all other "shading" or "fragment" shader stuff on a per-pixel basis
from an FBO, because otherwise we are doing it for every fragment
which might well be occluded.
So render first, then reshade with the FBO later.

- Make chunk loading, saving and accessing a member of the chunk,
  then give it derived classes which reimplement the method so that
  we can save chunks in different compression formats and see how it behaves

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

- Memory Pool Data Structure
- Persistent Mapped Buffer Test, write quads somehow in batches
- Multidraw Indirect Test, Make sure we can write quads and package them into draw calls



# Interleaved Data Structure is Complete


The basic memory pool is working.
Now I need to get its pointer to correspond to the persistently mapped pointer.
AND I need to get it to render using indirect so I can specify the locations of subregions
in the pool.





Instead we can construct a single model with interlaced properties in the VBO.

When drawing many

- Order quads for a single chunk by their facing orientation,
  so we can cull quads which face away from the camera.

To do backface culling, basically you need to know which faces are backwards.

We need to be able to selectively deactivate different subcomponents of our vbo.

Then, we also need full chunk draw culling.

This is most efficiently done using a quad pool with a multidrawelementsindirect.



1. Persistent Mapped Buffer on VBO
2. It has an interlaced format that lets me write quids directly in when needed
3. I use multidraw indirect to select which quads are being rendered and which are not
4. That's it!
