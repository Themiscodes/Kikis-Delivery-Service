# Kiki's Delivery Service 

A side-scrolling platformer written in C using raylib. It is based on Hayao Miyazaki's movie of the same title. The goal of the game is for Kiki to collect loaves of bread and deliver them with her broom, while avoiding the enemies of each level.

This is a project for the course of Data Structures and Software Principles of Spring 2021. Aside from implementing the Abstract Data Types required for this course, my aim was to use the low-level abilities and high degree of flexibility that C can offer to optimize performance, which is crucial for game design.

## Execution

- To compile and execute in the terminal, from the directory [Source-Code/game](Source-Code/game/) run: 
```
$ make
$ ./kiki
```

- On a Mac you can also simply double-click on the `Kiki's Delivery Service.app` and the game will begin. You can also play the game online [here](https://k08.chatzi.org/games/2021/competition/KikisDeliveryService/game.html).

## Screenshot



## Abstract Data Types

Aside from the data structures I used for the game, all the abstract data types I implemented for the purposes of this course can be found in the [modules](Source-Code/modules/) folder and are the following.

### ADT Vector using ADT Map

- In [ADTVector.c](Source-Code/modules/UsingADTMap/ADTVector.c) is my implementation of ADT Vector using ADT Map. I simply use the key of the map to position the element in the vector. Given that the map functions are executed in O(logn), it follows that `insert()`, `vector_set_at()`, `remove_last()` are done in O(logn) time, as well as all the other functions for traversal.

### ADT Deque using ADT Map

- In [ADTDeque.c](Source-Code/modules/UsingADTMap/ADTDeque.c) is my implementation of ADT Deque using ADT Map. When I insert at the beginning of the queue, I subtract 1 from the first element for the position. This is because the key can have negative numbers and there is no need to move the elements, by keeping these relative positions. As with my Vector implementation, in the Deque as well apart from `deque_find()` all the other functions have O(logn) complexity.

### ADT Deque using Dynamic Array

- In [ADTDeque.c](Source-Code/modules/UsingDynamicArray/ADTDeque.c) is my implementation of ADT Deque using Dynamic Array. I keep a pointer to the first element head and to the last tail. This way there is no need to move the elements when a new one is inserted or removed. Thus, the complexity compared to using ADTMap is better for `insert` and `remove` since they are done in O(1) time, as well as `set_at`, `get_at` since we have random access to the elements, as opposed to before when this was done in O(logn) time. But the implementation with dynamic array lags, when doing resizes which take O(n) time.

### ADT Vector using Real-Time Dynamic Array

- In [ADTVector.c](Source-Code/modules/UsingRealTimeDynamicArray/ADTVector.c) is my implementation of ADT Vector using Real-Time Dynamic Array. Here, I added `array_X` that holds the old array until all elements have been copied. This way I can achieve O(1) complexity even for resizes. Also, to keep the complexity at O(1) for `set_at` and `get_at`, I keep the `position_X` of the last copied element.

### ADT Set using Binary Search Tree

- In [ADTSet.c](Source-Code/modules/UsingBinarySearchTree/ADTSet.c) is my implementation of ADT Set using BST. To provide random-access I use a counter for each node of the number of nodes on the left subtree. These "weights" are adjusted when an element is inserted on the left hand side, through the path this element followed with the function `set_fix_weights`. `Set_fix_weights_reverse` works in the same way for remove.

- These functions have complexity at most O(h) since they follow the same path down the tree, as the insert or remove. For `set_get_at` I use the `left_weight` counters and keep the position of the minimum element of the tree. So here too complexity is at most O(h), as in `set_set_at`, that works in a similar way.

### ADT Set using AVL Tree

- In [ADTSet.c](Source-Code/modules/UsingHashTable/ADTSet.c) is my implementation of an AVL Tree using separate chaining, but inserting in an ADT Set instead of a List. I modified the order of the elements to be based on the key instead of the value. Both are stored in the `set_node`, while the other functions are inserted as they are except where adaptation was needed such as in insert, remove and find. In the map I keep the table of pointer buckets in sets which I initialize as null and only when I insert an element in this position I create the corresponding set. I also changed the load factor to 0.9.

- The complexity of the insert is O(n) when we need to rehash, but this is rarely done, so amortized time is O(1). For the search with this implementation, the amortized time is O(1) and going directly to the bucket where the key is located to find the element, the complexity improves from O(n) of the classic implementation to O(logn), because of the AVL tree.

### ADT Graph using Adjacency Map

- In [ADTGraph.c](Source-Code/modules/UsingAdjacencyMap/ADTGraph.c) is my implementation where I used the Map from Hash Table. The vertex_node struct contains a vertex with a map of the vertices adjacent to it (i.e. edges). I also added add_the_edge which adds these vertices to this node map when the graph is updated with a new edge. If two vertices are not connected by an edge, I chose not to have them at all in the `vertex_pairs` map of the graph (instead of having them with INT_MAX) to save space. I can find these from the other map. 

- In `graph_shortest_path_lengths()` I simply create a matrix and with a variation of the Floyd-Warshall algorithm, I find in each cell the optimal path between the edges. Since the graph is undirected, the matrix is symmetric. So, based on the same convention that I followed in the `vertex_pairs` map the edges appear once (that is, not both (1,2) and (2,1)) and there is no need to go trough the whole matrix, but only half, the lower "triangle". 

- In `graph_shortest_path_nexts()` I use the same algorithm, but I keep both edges. I also have an additional matrix so that when I find a better path, I can save that node instead. In [ADTGraph_test.c](Source-Code/tests/ADTGraph_test.c) I have a fairly exhaustive test for the subcases of insert, remove and for the two algorithms.


## Game Implementation

- In [kikistate.c](Source-Code/game/kikistate.c) is the implementation of the state of the game using an ADT Set, instead of an ADT Vector, for the objects of the game. This way it is faster on the system and allows for the game to never drop below 60fps.

- The `state_update()` function deals with the movement of the character and enemies, as well as with collisions with objects. To make `state_objects()` faster, instead of searching for all the objects of the set, I create an object with coordinates x_to and I find with the use of `set_find_eq_or_greater()` the next object in the set. 

- The structure of the game is in [kiki.c](Source-Code/game/kiki.c), where I create a state and by calling `interface_init()` the window is created with the screen_width and height given in [kiki_interface.h](Source-Code/game/kiki_interface.h). There the images from the `assets` folder load and by using `state_objects()` with `draw texture()` they are displayed in the window. 

- For the animations I used stop-frames from the movie and with the functions `DrawTextureRec()`, `GetFrameTime()` and a modulo operation calculated which frames to use so that they appear in motion during gameplay.