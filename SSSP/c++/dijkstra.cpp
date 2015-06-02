
#include <map>
#include <vector>
#include <list>
#include <cstdlib>
#include <iostream>
#include <assert.h>
#include <time.h>
#include <fstream>
#include <stdint.h>
#include <unordered_map>

#include "read.hpp"
#include "../../mem.hpp"

using namespace std;

typedef double distance_t;

#define INFINITE_DISTANCE 999999.0

typedef enum {
	WHITE,
	GRAY,
	BLACK
} color_t;

struct edge {
   struct node *target;
   distance_t weight;
};

struct node {
	node_t id;
	list<edge*> back_edges;
   struct node *current_target;
   map<struct node*, distance_t> distances;
	size_t position;
	color_t color;
   distance_t current_distance;
   distance_t get_prio() { return current_distance; }
};

static map<size_t, node*> nodes;
static vector<node*> heap;

static int
left(const int parent)
{
	int i = (parent << 1) + 1;
	return (i < (int)heap.size()) ? i : -1;
}

static int
right(const int parent)
{
	int i = (parent << 1) + 2;
	return (i < (int)heap.size()) ? i : -1;
}

static int
parent(const int child)
{
	if(child != 0)
		return (child - 1) >> 1;
	return -1;
}

#define SET_HEAP(POS, OBJ) do { 	\
	heap[(POS)] = (OBJ); 			\
	(OBJ)->position = (POS);		\
} while(false)

static void
heapifydown(const int index)
{
	int l(left(index));
	int r(right(index));
	const bool hasleft = (l >= 0);
	const bool hasright = (r >= 0);

	if(!hasleft && !hasright)
		return;

	if(hasleft && heap[index]->get_prio() <= heap[l]->get_prio() &&
			((hasright && heap[index]->get_prio() <= heap[r]->get_prio()) || !hasright))
		return;

	int smaller;
	if(hasright && heap[r]->get_prio() <= heap[l]->get_prio())
		smaller = r;
	else
		smaller = l;

	node *obj(heap[index]);

	SET_HEAP(index, heap[smaller]);
	SET_HEAP(smaller, obj);
	heapifydown(smaller);
}

static bool
heapifyup(int index)
{
	bool moved(false);

	while((index > 0) && (parent(index) >= 0) &&
			(heap[parent(index)]->get_prio() > heap[index]->get_prio()))
	{
		node *obj(heap[parent(index)]);

		SET_HEAP(parent(index), heap[index]);
		SET_HEAP(index, obj);

		index = parent(index);
		moved = true;
	}

	return moved;
}

static void
decrease_key(node *el)
{
	heapifyup(el->position);
}

static void
insert_heap(node *el)
{
	heap.push_back(el);
	el->position = heap.size() - 1;
	heapifyup(heap.size() - 1);
}

static node*
pop_heap(void)
{
	node *min(heap.front());

	SET_HEAP(0, heap.back());
	heap.pop_back();
	heapifydown(0);

	return min;
}

static inline bool
empty_heap(void)
{
	return heap.empty();
}

static inline
void
init_node(node *n, node_t id)
{
	n->id = id;
	n->position = 0;
	n->color = WHITE;
}

static void
add_connection(node_t src, node_t dst, int weight)
{
	node *dstp(NULL);
	node *srcp(NULL);

   auto d(nodes.find(dst));
   if(d == nodes.end()) {
      dstp = nodes[dst] = new node();
      init_node(dstp, dst);
   } else
      dstp = d->second;

   auto s(nodes.find(src));
   if(s == nodes.end()) {
      srcp = nodes[src] = new node();
      init_node(srcp, src);
   } else
      srcp = s->second;

	assert(srcp != NULL && dstp != NULL);
   edge *ed(new edge);
   ed->target = srcp;
   ed->weight = (distance_t)weight;
	dstp->back_edges.push_back(ed);
}

int
main(int argc, char **argv)
{
	if(argc != 3) {
		cout << "usage: dijkstra <meld data file> <fraction>" << endl;
		return EXIT_FAILURE;
	}
   initMemory();

   int fraction(atoi(argv[2]));
   //cout << "Using fraction " << fraction << endl;
   ifstream fp(argv[1], ifstream::in);

   int conn(0);
   while(!fp.eof()) {
      uint32_t src, dst, weight;
      fp.read((char *)&src, sizeof(uint32_t));
      fp.read((char *)&dst, sizeof(uint32_t));
      fp.read((char *)&weight, sizeof(uint32_t));
      add_connection(src, dst, weight);
      ++conn;
   }

#if 0
	for(size_t i(0); i < nodes.size(); ++i) {
		node *src(nodes[i]);
		cout << "Node " << i << endl;
		for(size_t j(0); j < src->back_edges.size(); ++j) {
			node *edge(src->back_edges[j].first);
			cout << "\t" << edge->id << endl;
		}
	}
#endif
#if 0
   cout << "Added " << nodes.size() << " nodes\n";
   cout << "Added " << conn << " connections\n";
#endif

   for(auto np : nodes) {
      size_t dest(np.first);
      if(dest % fraction != 0)
         continue;
      readMemory();

      heap.clear();
      heap.reserve(0);
      node *d(nodes[dest]);

      for(auto np2 : nodes) {
         node *n(np2.second);
         assert(n);
         n->color = WHITE;
         n->current_target = d;
         n->current_distance = INFINITE_DISTANCE;
      }

      node *target(nodes[dest]);
      if(target == NULL)
         continue;

      assert(target != NULL);

      target->distances[d] = 0;
      target->current_distance = 0;
      target->color = GRAY;
      insert_heap(target);

      while (!empty_heap()) {
         node *u(pop_heap());
         u->distances[target] = u->current_distance;

         for(edge *e : u->back_edges) {
            node *v(e->target);
            size_t w(e->weight);
            auto ud(u->distances.find(d));
            size_t new_d;
            if(u->current_distance == INFINITE_DISTANCE)
               new_d = INFINITE_DISTANCE;
            else
               new_d = u->current_distance + w;
            size_t vdis(v->current_distance);

            if(new_d < vdis) {
               v->current_distance = new_d;
               if(v->color == WHITE) {
                  v->color = GRAY;
                  insert_heap(v);
               } else if(v->color == GRAY) {
                  decrease_key(v);
               }
            }
         }

         u->color = BLACK;
      }
   }

#if 0
   for(auto np : nodes) {
		node *src(np.second);
      cout << "Node " << src->id << endl;
      for(auto d : src->distances) {
         node *dst(d.first);
         int dis(d.second);
         cout << " to node " << dst->id << ": " << dis << endl;
      }
	}
#endif
   printMemory();

	return EXIT_SUCCESS;
}

