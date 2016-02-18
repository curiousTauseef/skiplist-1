/* skiplist.h
   Matthew Clegg.  October 1990.

   This file contains the specification of an abstract implementation
   of the skiplist data structure.  Skiplists are dynamically
   balancing data structures which support rapid insertion, retrieval
   and deletion of items.  For details about skiplists, see

   Pugh, William, "Skip Lists:  A Probabilistic Alternative to
   Balanced Trees," CACM, Vol 33, No 6, pp. 668-676.

   Our implementation is abstract in the sense that it operates
   on any ordered data type.  When a skip list is initialized,
   procedures are provided for comparing two elements and for
   deleting an element.  Elements are represented by pointers.

*/

/* The following two typedefs abstractly define the data types
   upon which the skip list operates.  The 'key' data type is
   an ordered type which forms the basis for indexing the skiplist.
   The 'member' data type represents the data which is carried
   by the 'key'.
*/
typedef void *key_type;
typedef void *data_type;

/* The following typedef specifies a generic function for deleting
   an element of a skiplist.  This function might be instantiated,
   for example, with a pointer to a routine which calls
   dispose. */
/* typedef void (*Deleter)(key_type, data_type); */
typedef void (*Deleter) ();

/* The following typedef specifies a generic function for comparing
   two elements of a skiplist.  This function should return the value
   -1,0,1 according respectively as to whether the first argument
   is less than, equal to or greater than the second. */
/* typedef int (*Comparator)(key_type, key_type); */
typedef int (*Comparator) ();

/* The following typedef specifies a generic function for iterating
   over the elements of a skiplist.  */
/* typedef void (*Iterator)(key_type, data_type); */
typedef void (*Iterator) ();


#define MAX_LEVEL             20

typedef struct SkipList_element_struct *SkipList_element;
typedef struct SkipList_struct *SkipList;

typedef SkipList_element skiplist_finger [MAX_LEVEL];
	/* A forward pointer to later elements in the skip list. */

typedef int skiplist_delta [MAX_LEVEL];
	/* The k-th element of this array is the distance from the
	   current element to the first following element of height k
	   or greateer. */

/* typedef */ struct SkipList_element_struct {
	key_type    	  key;	/* search key for this element. */
	data_type   	  data;	/* data associated with this element. */
	int		  height;
	SkipList_element *forward;
	int		 *diff; /* distance to next el't at this level. */
};

/* typedef */ struct SkipList_struct {
	Deleter          	Member_Delete;
	Comparator       	Member_Compare;
	SkipList_element 	header;	 /* a dummy header element. */
	SkipList_element       *finger;  /* the pointers resulting from
					    the last search. */
	int		       *distance;/* distances of elements in last
					    search. */
	int              	level;   /* the number of levels in use */
	int			length;  /* length of the skip list. */
};


SkipList SkipList_Create (Deleter d, Comparator c);
/* Creates a new skip list having Deleter d and Comparator c.
   Returns a pointer to the created structure.
*/

void SkipList_Free (SkipList sl);
/* Deletes all elements in the skiplist sl and releases the memory
   associated with sl.
*/

void SkipList_Iterate (SkipList sl, Iterator i);
/* Applies the iterator i to each element of the skiplist sl. */

data_type SkipList_Search (SkipList sl, key_type k);
/* Searches the skip list sl for the member with key k.  If the member is
   found, then returns a pointer to the data associated with the member.
   If the member is not found, then returns NULL.
*/

void SkipList_Insert (SkipList sl, key_type k, data_type m);
/* Inserts the pair (k, m) into the skip list.  */

int SkipList_Delete (SkipList sl, key_type k);
/* Deletes all members of the skip list whose key matches k.
   Returns the number of members which are actually deleted. */

int SkipList_Length (SkipList sl);
/* Returns the number of items in the skip list. */

key_type SkipList_Indexed_Key (SkipList sl, int n);
/* Returns the key of the n-th element of the skip list sl, or
   NULL of sl has fewer than n elements. */

data_type SkipList_Indexed_Member (SkipList sl, int n);
/* Returns the n-th member of the skip list sl, or NULL if sl
   has fewer than n elements. */

void SkipList_Indexed_Update (SkipList sl, int n, data_type d);
/* Updates the data associated with the n-th element of the skip list sl. */


