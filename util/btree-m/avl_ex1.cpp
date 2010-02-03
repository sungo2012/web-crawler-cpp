// Abstract AVL Tree Template Example 1.
// This code is in the public domain.
// Version: 1.5  Author: Walt Karas

// This example shows how to use the AVL template to create the
// env class.  The env class stores multiple variables with string
// names and string values, similar to the environment in a UNIX
// shell.

#include <string.h>
#include <stdio.h>
#include <queue>

#include "avl_tree.h"

using namespace std;
typedef struct{
	long long name;
	int value;
}KV;
// An "environment" of variables and (string) values.
class env {
private:

	struct node {
		// Child pointers.
		node *gt, *lt;

		// First character of variable name string is actually balance factor.
		// Remaining characters are name as nul-terminated string.
		long long name;

		// Value of variable, nul-terminated string.
		int value;
	};

	// Abstractor class for avl_tree template.
	struct abstr {
		typedef node *handle;

		typedef const long long key;

		typedef unsigned size;

		static handle get_less(handle h, bool access) {
			return (h->lt);
		}
		static void set_less(handle h, handle lh) {
			h->lt = lh;
		}
		static handle get_greater(handle h, bool access) {
			return (h->gt);
		}
		static void set_greater(handle h, handle gh) {
			h->gt = gh;
		}

		static long long get_balance_factor(handle h) {
			return (h->name);
		}
		//static void set_balance_factor(handle h, int bf) { h->name[0] = bf; }

		static int compare_key_node(key k, handle h) {
			return (k - h->name);
		}

		static int compare_node_node(handle h1, handle h2) {
			return (h1->name - h2->name);
		}

		static handle null(void) {
			return (0);
		}

		// Nodes are not stored on secondary storage, so this should
		// always return false.
		static bool read_error(void) {
			return (false);
		}
	};

	typedef abstract_container::avl_tree<abstr> tree_t;

	tree_t tree;


public:


	void set(const long long name, const int value) {
		node *n = tree.search(name);

		if (!n) {
			// This variable does not currently exist.  Create a node for it.
			n = new node;
			n->name = name;
			tree.insert(n);
		} else
			// Delete current value.
			n->value = 0;

		if (value) {
			n->value = value;
		} else {
			// Variable is being set to empty string, which deletes it.
			tree.remove(name);
			delete n;
		}
	}

	const int get(const long long name) {
		node *n = tree.search(name);

		return (n ? n->value : 0);
	}

	// Dump environment in ascending order by variable name.
	void dump(void) {
		tree_t::iter it;
		node *n;

		it.start_iter_least(tree);

		for (n = *it; n; it++, n = *it)
			printf("%lld=%d\n", n->name, n->value);

	}

	// Clear environment.
	void clearself(void) {
		tree_t::iter it;
		node *n;

		it.start_iter_least(tree);

		// A useful property of this data structure is the ability to do a
		// "death march" through it.  Once the iterator (forward or backward)
		// has stepped past a node, the node is not accessed again (assuming
		// you don't reverse the direction of iteration).  If you are doing
		// a final iteration in order to destroy the tree, you can release
		// heap memory or other resources allocated by the tree node once the
		// iterator has stepped past it.  If you plan to use the AVL tree
		// instance again after completing this final iteration, you must
		// make it empty (set the root to the null value).

		for (n = *it; n; n = *it) {
			// Step iterator past node n.
			it++;

			// Release node n's resources.

			delete n;
		}

		// Make the tree empty by setting root to null value.
		tree.purge();
	}
	void clearself(queue<KV> &q ) {
			tree_t::iter it;
			node *n;

			it.start_iter_least(tree);

			// A useful property of this data structure is the ability to do a
			// "death march" through it.  Once the iterator (forward or backward)
			// has stepped past a node, the node is not accessed again (assuming
			// you don't reverse the direction of iteration).  If you are doing
			// a final iteration in order to destroy the tree, you can release
			// heap memory or other resources allocated by the tree node once the
			// iterator has stepped past it.  If you plan to use the AVL tree
			// instance again after completing this final iteration, you must
			// make it empty (set the root to the null value).

//			KV* kv = new KV;
			KV kv;
			for (n = *it; n; n = *it) {
				// Step iterator past node n.
				it++;
				kv.name  = n->name;
				kv.value = n->value;
				// Release node n's resources.
//				q.push(*kv);
				q.push(kv);

				delete n;
			}
//			delete kv;
			printf("bug here?\n");
			// Make the tree empty by setting root to null value.
			tree.purge();
		}
};
/*
// Demo main program.
int main(void) {
	env e;
	queue<KV> q;
	e.set(1234, 1);
	e.set(1234, 3);
	e.set(123, 2);
	printf("The value: \n");
	printf("The value of \"123\" is \"%d\"\n\n", e.get(123));
	printf("The value of \"1234\" is \"%d\"\n\n", e.get(1234));
	printf("The value of \"12345\" is \"%d\"\n\n", e.get(12345));


	e.clear(q);


	while (!q.empty()){
		printf("%lld:%d\n", q.front().name, q.front().value);
		q.pop();
	}

	return (0);
}
*/
