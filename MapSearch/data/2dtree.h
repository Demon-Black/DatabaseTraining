#ifndef _2DTREE__
#define _2DTREE__

#include <vector>
using namespace std;
typedef struct node_ {
	int id;
	float x, y;
	int number;
	float d;
	float r = 0.0;
	float X[3] = {0.0};
	float Y[3] = {0.0};
} Node;

typedef struct treenode_ {
	float data[2];
	int d;
	struct treenode_ *left;
	struct treenode_ *right;
} tree_node;

int insert(int n, Node node);
void knn_search(float x0, float y0, float x1, float y1, vector<Node> *result);
int kdtree_insert(tree_node *&node, float *data, int layer, int d);
void search_nearest(tree_node *tree, float *data_search, int dimension, double *result);
void kdtree_print(tree_node *tree, int d);
void kdtree_print_in(tree_node *tree, int d);

#endif