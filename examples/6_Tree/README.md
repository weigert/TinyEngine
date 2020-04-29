# TinyEngine

## Tree Growth Model
This was a small idea I had to grow trees, but it didn't really constitute its own repository. Since I built it with TinyEngine in about 6 hours, I decided to include it as an example program here.

This was based off of an observation I had while walking in the forest, that when a tree branch splits, it seems the sum of the cross-section of the two branches it splits into is conserved.

The model is capable of growing different shapes of trees, from deciduous to evergreen. Some example screenshots are shown below.

![Pine Tree Example Growth](https://github.com/weigert/TinyEngine/blob/master/examples/6_Tree/screenshots/Pine.png)

Example growth of a pine tree. This comes from using imbalanced parameters for growth (explained below).

### How it Works
Every tree segment is a "branch". When a branch reaches a certain size, it splits into two sub-branches. A branch that hasn't split yet is considered to have a leaf at the end.

When a branch splits, it splits according to a split ratio X, so the two sub-branches can be evenly or unevenly sized.

The tree is "fed" at a certain rate every time-step. This constant feed is passed down the tree. Every branch it touches takes a fraction of the feed (`passratio`) and uses it to grow itself. It then passes the rest to its sub-branches according to the split ratio. This process repeats until the feed is used up.

Feed and growth is volume based, so the branch length and diameter are computed directly from the branch property `size` using the cube root and square root respectively.

Branches also have a direction property. The direction of each branch is computed as a weighted sum between the parent branches direction and a random normal vector to the parent branch. The weight is given by the split ratio, so that thicker child branches are more likely to grow straight, and small ones are more likely to grow perpendicular. A random normal vector can be computed by taking a random vector in 3D space and computing the cross product.

All of these properties can be controlled directly from the control panel.

#### Parameters

Note that these parameters are currently fixed, but could potentially be sampled from distributions instead (e.g. normal distribution around a mean) to give more realism.

### Visualization
The leaves are a particle system. They use the image `leaf.png`, and they always face the camera. You can alter color, opacity, size and spread (you can have multiple leaves around a branch).

The tree is meshed by a bunch of cylinders that correspond to the branches. A few parameters then make it look nice (for isntance a taper, the scaling of length and width).

All colors can be edited. Optionally, a wire-mesh can be displayed on top. Leaves and the tree itself can be turned off. 

#### Reading

The model itself is implemented in `tree.h`. Here you will also find how the mesh for the tree and particle system for the leaves are generated.

To see how the interface and event handler are made, read `model.h`.

Everything is wrapped in `main.cpp`.

Overall the code to generate the trees is very brief.

### Usage

Compile with make
        
    make all
        
Control Panel:

    Toggle Pause - P
    Toggle Auto-Rotate - A
    Toggle Control Panel - ESC

### Screenshots

![Naked Tree](https://github.com/weigert/TinyEngine/blob/master/examples/6_Tree/screenshots/Naked.png)

Tree without leaves, example growth with relatively balanced parameters.

![Tree with Leaves](https://github.com/weigert/TinyEngine/blob/master/examples/6_Tree/screenshots/Leaves.png)

The same tree but with leaves.
