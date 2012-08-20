#ifndef _234TREE_H_
#define _234TREE_H_

typedef struct _234_node {
	unsigned dim;
	struct __node *parent;
	union {
		struct {
			void *first;
			void *second;
			void *third;
		};
		void *keys[3];
	};
	union {
		struct {
			struct __node* left;
			struct __node* midleft;
			struct __node* midright;
			struct __node* right;
		};
		struct __node* children[4];
	};
};

struct _234_node *_234_node_find  (struct _234_node *n, void *key, int *index);
struct _234_node *_234_node_add   (struct _234_node *n, void *key);
struct _234_node *_234_node_delete(struct _234_node *n, int index);

typedef int (*pf_compare_function)(const void *lhs, const void *rhs);
extern inline void _234_node_set_comp(pf_compare_function);

#endif /* _234TREE_H_ */
