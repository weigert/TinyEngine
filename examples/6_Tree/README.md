/*
    Tree Growth Model:

    Every tree segment is a "branch".

    When a branch reaches a certain size, it splits into two branches.
    A branch that hasn't split yet is considered to have a leaf at the end.

    When it splits, it can split according to a split ratio X, so the two child
    branches can be evenly or unevenly sized.

    The tree is "fed" at a certain rate every time-step. This constant feed is
    passed down the tree. Every branch it touches takes a fraction of the feed
    and uses it to grow. It then passes the rest to its child branches according
    to the split ratio. This process repeats until the feed is used up.

    Notes:
    Currently, growth

    Grows until a certain length, where it splits after a certain length
    splitting occurs with a split ratio, which divides up the crossectional area
    of the tree between the two branches.

    Then the branches also split according to some angle rule and continue to grow.


    The tree can then be grown based on this model and has a way to be visualized.
    Just make the properties sufficiently modular and it should work quickly.

    We could sample from a ratio distribution

    Aesthetics:
      Leaf Colors sampled from bezier

    Parameters (could be branch-dependent):
      -> Split ratio, in dependency of the branch that is splitting!
      -> Split rate / condition, some probability after which a split might occur.
      -> Choice of split angles!
      -> Growth Rate of the tree is important because it diverts resources
      -> Pass-On Ratio to the child branches!

*/
