# TinyEngine

## Tree Growth Model
This was a small idea I had to grow trees, but it didn't really constitute its own repository. Since I built it with TinyEngine in about 6 hours, I decided to include it as an example program here.

This was based off of an observation I had while walking in the forest, that when a tree branch splits, it seems the sum of the cross-section of the two branches it splits into is conserved.

The model is capable of growing different shapes of trees, from deciduous to evergreen. Some example screenshots are shown below.

![Cherry Tree Example Growth](https://github.com/weigert/TinyEngine/blob/master/examples/6_Tree/screenshots/Cherry.png)

Example of a deciduous tree, with some nice color variation set manually.

![Pine Tree Example Growth](https://github.com/weigert/TinyEngine/blob/master/examples/6_Tree/screenshots/Pine.png)

Example growth of a pine tree. This comes from using imbalanced parameters for growth (explained below).

### How it Works
Every tree segment is a "branch". When a branch reaches a certain size (`splitsize`), it splits into two sub-branches. A branch that hasn't split yet is considered to have a leaf at the end.

When a branch splits, it splits according to a split ratio X (`ratio`), so the two sub-branches can be evenly or unevenly sized. One branch grows proportional to X, the other to (1-X), leading to the size-conservation effect that is desired.

The tree is "fed" at a certain rate every time-step. This constant feed is passed down the tree. Every branch it touches takes a fraction of the feed (`passratio`) and uses it to grow itself. It then passes the rest to its sub-branches according to the split ratio. This process repeats until the feed is used up.

Feed and growth is volume based, so the branch length and diameter are computed directly from the branch property `size` using the cube root and square root respectively. Length and girth are then scaled arbitrarily for visualization.

Branches also have a direction property. The direction of each branch is computed as a weighted sum between the parent branches direction and a random normal vector to the parent branch. The weight is given by the split ratio, so that thicker child branches are more likely to grow straight, and small ones are more likely to grow perpendicular. Additionally, a `spread` parameter adds extra weight to the normal vector if desired so that branches tend more outwards or straight. A random normal vector can be computed by taking a random vector in 3D space and computing the cross product.

All of these properties can be controlled directly from the control panel.

#### Parameters

        Growth Rate - Rate of Tree Feeding
        Split Ratio - Value of X when a branch splits (how lopsided is the split)
        Pass Ratio - How much of the feed does a branch consume vs. pass on
        Branch Spread - How much do branches prefer to grow perpendicular vs. straight
        Split Size - Critical size when a branch stops being a leaf node and splits

Note that these parameters are currently fixed, but could potentially be sampled from distributions instead (e.g. normal distribution around a mean) to give more realism.

### Visualization
The leaves are a particle system. They use the image `leaf.png`, and they always face the camera. You can alter color, opacity, size and spread (you can have multiple leaves around a branch).

The tree is meshed by a bunch of cylinders that correspond to the branches. A few parameters then make it look nice (for instance a taper, the scaling of length and width).

All colors can be edited. Optionally, a wire-mesh can be displayed on top. Leaves and the tree itself can be turned off. 

Note that in order to make sure the leaf positions are randomized but don't change every frame, their position offset is hashed. This is done by giving every branch a random ID (which stays the same every tick), which can be used to hash the leaves position offset. This allows for generating a particle cloud that doesn't change all the time and stays fixed in 3D space.

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
