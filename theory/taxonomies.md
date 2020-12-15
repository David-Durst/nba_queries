# Taxonomies
Taxonomies of the types of queries, the join strategies used in the queries, and the indexes used for those joins.

## Queries
1. **reference** - source in the query, to which others are compared
    1. If the reference is an object in the dataset, the options are:
        1. **stationary** (like the top of the key on the basketball court) or **moving** (like a player)
        2. **number of references**: either a single object (like looking at one player), a series of objects (like looking at all shots)
    2. The reference can be clusters that are defined eithe
        1. As input to the query (like dividing the court into 2ft x 2ft regions)
        2. By the an algorithm (like k-means)
2. How the reference and other objects are compared 
    1. Point query – find all objects at same location as reference 
    2. Range query – find all objects in a range of space relative to the reference 
    3. Nearest-Neighbor – find the k nearest neighbors to the reference
    4. Trajectory intersection – find all objects whose trajectory intersects that of the reference
    5. Trajectory similarity – find most similar trajectories 
3. How the data set changes
    1. Static – only bulk updates are applied to the data set
    2. Chronological – new records are appended, but old records are not updated 
    3. Dynamic – new records are appended, and old records can be changed
    
### Related Work:
1. [Ten Benchmark Database Queries for Location-based Services - 2003 - Theodoridis](https://ieeexplore.ieee.org/abstract/document/8129773)
2. [On Benchmarking a Trajectory DBMS - 2013 - Vodas et al. (including Theodoridis)](http://infolab.cs.unipi.gr/technical_reports/TR-2013-04.pdf)
3. [BerlinMOD: a Benchmark for Moving Object Databases - 2009 - Düntgen et al. (including Güting)](https://link.springer.com/article/10.1007/s00778-009-0142-5)

## Join Strategies
These join strategies are defined for two relations - a **left** and **right** relation. 
Three or more relation joins are done by composing binary joins.

1. Nested Loop Join - for every row in left relation, search the right relation.
    1. The search of the right relation can be a full table scan. This is really slow, but necessary if no relevant indices are found.
    2. Alternatively, the right relation search can just be looking up values in an index, like a B-Tree.
2. Merge Join - Sort each relation, then walk them at the same time.
3. Hash Join - Convert the right relation into a hash table. Then, for each row in the left relation, search the hash table for matches.

### Related Work:
1. [PostgreSQL Join Documentation - 2020](https://www.postgresql.org/docs/current/planner-optimizer.html)

## Indices
1. R-tree - a tree with a bounding box defined at each node. Each box specifies the minimum region containing all leafs below the node.
   Bounding boxes can overlap, even for different parts of the tree where one node isn't the ancestor of another.
    1. The r-tree typically stores spatial, but not temporal data.
    2. This is similar to a BVH in computer graphics.
    3. Extensions of the R-tree to handle time - MV3R-tree
    4. Extensions of the R-tree for trajectory queries include - STR-tree, TB-tree
    5. Extensions of the R-tree for predicting future data and dropping off old points - TPR-tree and R^{exp}-tree
    6. Extensions of the R-tree for nearest-neighbor indexing - native space indexing
2. Quad-tree - a tree with a bounding box defined at each node. Each box does not specify the minimum region containing all leafs below the node. 
   Rather, for each parent node's 2D region R, it's four node divide R into four equally sized quadrants. Bounding boxes cannot overlap unless one node is the ancestor of another.
   1. The quad-tree can be extended to handle time by adding another dimension. Then, it becomes an octree as 8 nodes are necessary to divide a 3D region.
   
### TODO FOR INDICES
1. Further investigate indices listed above that are extensions of R-trees
2. Understand how trajectory databases use quad-trees - see [TrajStore - 2010 - Cudre-Mauroux et al. (including Madden)](https://dspace.mit.edu/bitstream/handle/1721.1/62803/Madden_Trajstore%20an.pdf?sequence%3D1)

### Related Work
1. [Ten Benchmark Database Queries for Location-based Services - 2003 - Theodoridis](https://ieeexplore.ieee.org/abstract/document/8129773)
2. [Novel Approaches to the Indexing of Moving Object Trajectories - 2000 - Pfoser et al. (including Theodoridis)](https://dl.acm.org/doi/10.5555/645926.672019)
3. [TrajStore - 2010 - Cudre-Mauroux et al. (including Madden)](https://dspace.mit.edu/bitstream/handle/1721.1/62803/Madden_Trajstore%20an.pdf?sequence%3D1)
