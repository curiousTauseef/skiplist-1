#### Skiplist
###### Copyright (C) 1990 Matthew Clegg.  All rights reserved.
This code is made available under the GNU Public License (version 3).

This file repository contains a C implementation of the skiplist
data structure.  Skiplists are dynamically balancing data structures 
which support rapid insertion, retrieval and deletion of items.  
For details about skiplists, see

Pugh, William, "Skip Lists:  A Probabilistic Alternative to
Balanced Trees," CACM, Vol 33, No 6, pp. 668-676.

This implementation is abstract in the sense that it operates
on any ordered data type.  When a skip list is initialized,
procedures are provided for comparing two elements and for
deleting an element.  Elements are represented by pointers.

