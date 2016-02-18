/* skiplist.c
   Matthew Clegg.  October 1990.

   This is the implementation of the skiplist data structure.
   The algorithms we use here are essentially copied directly
   from the article below:

   Pugh, William, "Skip Lists:  A Probabilistic Alternative to
   Balanced Trees," CACM, Vol 33, No 6, pp. 668-676.

*/

#include <stdio.h>

#include "skiplist.h"

void *malloc (int);
void free(void *);

static long Current_Random_Level = 0;

static int Choose_Random_Level (void)
/* Chooses a level between 1 and MaxLevel */
{
	int n;
	long i;

/*	for (n = 1; random (2); n++); */
	n = 1;
	for (i = Current_Random_Level++; (i & 1); i >>= 1) n++;
	return ( (n < MAX_LEVEL)? n: MAX_LEVEL);
};

static void Search_by_Key (SkipList sl, skiplist_finger finger,
  skiplist_delta delta, key_type k)
/* Searches the skip list sl for a record with the given key.  Creates
 .  a search 'finger' which represents the results of the search.
 .  Upon exit, finger[i] points to the largest element of the skip list
 .  which has at least i levels and whose key is strictly less than
 .  the given key k.  Delta[i] points to the index of the i-th element,
 .  where the header element has index 0. If there is an element with
 .  key k in the list, then finger[0]->forward[0] will be that element.
 */
{
	int current, last;
	SkipList_element f;

	f    = sl->header;
	last = 0;
	for (current = sl->level-1; current >= 0; current--) {
	  if ((sl->finger[current]->key != NULL) &&
	      ((*sl->Member_Compare)(k, sl->finger[current]->key) > 0)) {
			f = sl->finger[current];
			delta[current] = sl->distance[current];
	  } else
		delta[current] = last;
	  while ((f->forward[current] != NULL) &&
		((*sl->Member_Compare)(k, f->forward[current]->key) > 0)) {
			delta[current] += f->diff[current];
			f = f->forward[current];
	  };
	  sl->finger[current]   = finger[current] = f;
	  sl->distance[current] = last = delta[current];
	};

};

static int Search_by_Index (SkipList sl, skiplist_finger finger, int n)
/* Searches the skiplist sl for an element with the given index.
 . Upon return, finger[i] points to the highest indexed element whose
 . index is strictly less than n, and f[0]->forward[0] points to
 . the indexed element.  Returns 1 if the indexed element is contained
 . in the list (i.e., n is less than the length of the list), and
 . 0 if there is no element in the list with index n.
 */
{
	int current, place;
	SkipList_element f;

	f = sl->header;
	place = 0;
	for (current = sl->level - 1; current >= 0; current--) {
		if (sl->distance[current] < n) {
			place = sl->distance[current];
			f = sl->finger[current];
		};
		while ((f->forward[current] != NULL) &&
			(f->diff[current] + place < n)) {
				place += f->diff[current];
				f = f->forward[current];
		};
		sl->finger[current] = finger[current] = f;
		sl->distance[current] = place;
	};
	return ((n - place) == 1);

};

static SkipList_element MakeNode (int newLevel, key_type k, data_type d)
{
	SkipList_element n;
	int i;

	n = (struct SkipList_element_struct *)
		malloc (sizeof(struct SkipList_element_struct));
	n->key = k;
	n->data = d;
	n->forward = (SkipList_element *) malloc (newLevel *
					       sizeof(SkipList_element));
	n->diff    = (int *) malloc (newLevel * sizeof(int));
	for (i = 0; i < newLevel; i++) n->diff[i] = 0;
	n->height = newLevel;
	return (n);
};

SkipList SkipList_Create (Deleter d, Comparator c)
/* Creates a new skip list having Deleter d and Comparator c.
   Returns a pointer to the created structure.
*/
{
	SkipList sl;
	int i;

	sl = (struct SkipList_struct *)
		malloc (sizeof(struct SkipList_struct));
	sl->Member_Delete  = d;
	sl->Member_Compare = c;
	sl->header = MakeNode (MAX_LEVEL, NULL, NULL);
	sl->finger = (SkipList_element *) malloc (MAX_LEVEL *
					       sizeof(SkipList_element));
	sl->distance = (int *) malloc (MAX_LEVEL * sizeof(int));
	sl->level  = 1;
	sl->length = 0;
	for (i = 0; i < MAX_LEVEL; i++) {
		sl->header->forward[i] = NULL;
		sl->header->diff[i] = 1;
		sl->finger[i] = sl->header;
		sl->distance[i] = 1;
	};

	return (sl);
};

void SkipList_Free (SkipList sl)
/* Deletes all elements in the skiplist sl and releases the memory
   associated with sl.
*/
{
	SkipList_element m, n;

	n = sl->header;

	while (n->forward[0] != NULL) {
		m = n->forward[0];
		n->forward[0] = m->forward[0];
		(*sl->Member_Delete) (m->key, m->data);
		free (m->forward);
		free (m);
	};
	free (n->forward);
	free (n);
	free (sl->finger);
	free (sl->distance);
	free (sl);
};

void SkipList_Iterate (SkipList sl, Iterator i)
/* Applies the iterator i to each element of the skiplist sl. */
{
	SkipList_element f;

	f = sl->header;
	while (f->forward[0] != NULL) {
		f = f->forward[0];
		(*i) (f->key, f->data);
	};
};

data_type SkipList_Search (SkipList sl, key_type k)
/* Searches the skip list sl for the member with key k.  If the member is
   found, then returns a pointer to the data associated with the member.
   If the member is not found, then returns NULL.
*/
{
	skiplist_finger f;
	skiplist_delta d;

	Search_by_Key (sl, f, d, k);
	if (f[0]->forward[0] == NULL)
		return (NULL);
	else if ((*sl->Member_Compare) (k, f[0]->forward[0]->key))
		return (NULL);
	else
		return (f[0]->forward[0]->data);
};

void SkipList_Insert (SkipList sl, key_type k, data_type m)
/* Inserts the pair (k, m) into the skip list.  */
{
	skiplist_finger  u;
	skiplist_delta   d;
	SkipList_element n;
	int nl;		/* level of n. */
	int i;

	Search_by_Key (sl, u, d, k);

	n = u[0]->forward[0];
	nl = Choose_Random_Level ();
	if (nl > sl->level) {
		for (i = sl->level; i < nl; i++) {
			u[i] = sl->header;
			d[i] = 0;
			u[i]->diff[i] = sl->length + 1;
		};
		sl->level = nl;
	};
	n = MakeNode (nl, k, m);

	/* Update the index counters in the skip list. */
	for (i = 0; i < nl; i++) {
		n->diff[i] = u[i]->diff[i] - d[0] + d[i];
		u[i]->diff[i] = d[0] - d[i] + 1;
	};
	for (i = nl; i < sl->level; i++)
		u[i]->diff[i] += 1;
	sl->length += 1;


	/* Update the forward pointers in the skip list. */
	for (i = 0; i < nl; i++) {
		n->forward[i] = u[i]->forward[i];
		u[i]->forward[i] = n;
	};
};

int SkipList_Delete (SkipList sl, key_type k)
/* Deletes all members of the skip list whose key matches k.
   Returns the number of members which are actually deleted. */
{
	skiplist_finger  u;
	SkipList_element n;
	skiplist_delta   delta;
	int d = 0, i;

	Search_by_Key (sl, u, delta, k);

	n = u[0]->forward[0];
	if ((n != NULL) && ((*sl->Member_Compare)(n->key, k) == 0)) {
		for (i = 0; i < sl->level; i++)
			if (u[i]->forward[i] != n)
				u[i]->diff[i] -= 1;
			else {
				u[i]->diff[i] += n->diff[i] - 1;
				u[i]->forward[i] = n->forward[i];
			};
		(*sl->Member_Delete) (n->key, n->data);
		free (n->forward);
		free (n);
		d++; sl->length--;
		n = u[0]->forward[0];
	};
	if (d > 0)
		while ((sl->level > 1) &&
		  (sl->header->forward[sl->level-1] == NULL))
			sl->level--;
	return (d);
};

int SkipList_Length (SkipList sl)
/* Returns the number of items in the skip list. */
{
	return (sl->length);
};

key_type SkipList_Indexed_Key (SkipList sl, int n)
/* Returns the key of the n-th element of the skip list sl, or
   NULL of sl has fewer than n elements. */
{
	int found;
	skiplist_finger f;

	found = Search_by_Index (sl, f, n);
	if (found)
		return (f[0]->forward[0]->key);
	else
		return (NULL);
};

data_type SkipList_Indexed_Member (SkipList sl, int n)
/* Returns the n-th member of the skip list sl, or NULL if sl
   has fewer than n elements. */
{
	int found;
	skiplist_finger f;

	found = Search_by_Index (sl, f, n);
	if (found)
		return (f[0]->forward[0]->data);
	else
		return (NULL);
};

void SkipList_Indexed_Update (SkipList sl, int n, data_type d)
/* Updates the data associated with the n-th element of the skip list sl. */
{
	int found;
	skiplist_finger f;

	found = Search_by_Index (sl, f, n);
	if (found)
		f[0]->forward[0]->data = d;
};


