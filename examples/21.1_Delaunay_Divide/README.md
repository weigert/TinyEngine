# delaunay

delaunay triangulation texture shader


Basic idea:

We need to take multiple steps...
we can take individual disjoint simplices and determine which triangles we can instantly discard.

Often we can take two triangles, compare them and determine that one triangle will be discarded.

So what if I do the following:

Split the group of N by 4.
For each set of 4, I generate a simplex.

For each simplex, decide which 2 or 3 sides are visible.
(because either 1 or 2 sides are always not visible)

or: 1 or 0 lines are not visible.
if a line is not visible, then any other simplices with that line can not exist

Simultaneously, what we are really doing is checking for every line whether it is being crossed over by another line.

There are N^2 possible lines. If any one line is crossed by another line, that line is immediately disqualified.

That means I would have N^4 comparisons.

So what we can also do is a divie and conquer line intersection test.

The thing is though: I don't just have N^2 lines and cross them with N^2 other lines.

but rather from any 4 points I can almost always find 2 lines which WILL intersect.
