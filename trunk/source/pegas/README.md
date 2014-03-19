pegas
=====

A fundamental library in c.

The project contains some fundamental data structures and algorithms.  

1.  memory management algorithm, include buddy system, left-lean red black tree based memory manager, single memory pool and multiple memory pool.  
2.  it creates a simple object oriented model, which could dynamic dispatch the function to different object.  
3.  based on the oo model(object oriented model), it implements a container library by the iterator pattern.  
   * unlike STL, which has type information of the elements so it knows how to clone, copy, destroy the elements, pegas make the type information explicitly and dynamiclly.  
   * like STL, pegas uses iterator to decouple algorithm from container.  
   * like STL, pegas has array(vector), list, set, mset(multiset), map, mmap(multimap).  
   * unlike STL, pegas has more than one set implementation, the driver of set could be red-black tree, splay tree, skiplist or hash table.  

