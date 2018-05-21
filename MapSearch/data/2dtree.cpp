#include "2dtree.h"
#include <vector>
#include <stack>
#include <cmath>
#include <cstdio>
#include <algorithm>
using namespace std;

vector<Node> v0;
vector<Node> v1;
vector<Node> v2;
vector<Node> v3;

bool operator < (const Node& a, const Node& b) {
	return a.r < b.r;
}

int insert(int n, Node node) {
	if (n == 0) {
		node.d = 0.0;
		v0.push_back(node);
	} else if (n == 1) {
		node.d = sqrt(pow(node.x - node.X[0], 2) + pow(node.y - node.Y[0], 2));
		v1.push_back(node);
	} else if (n == 2) {
		float t1 = sqrt(pow(node.x - node.X[0], 2) + pow(node.y - node.Y[0], 2));
		float t2 = sqrt(pow(node.x - node.X[1], 2) + pow(node.y - node.Y[1], 2));
		float d12 = sqrt(pow(node.X[1] - node.X[0], 2) + pow(node.Y[1] - node.Y[0], 2));
		if (t1 > t2) {
			node.X[0] = node.X[0] + node.X[1];
			node.X[1] = node.X[0] - node.X[1];
			node.X[0] = node.X[0] - node.X[1];
			node.Y[0] = node.Y[0] + node.Y[1];
			node.Y[1] = node.Y[0] - node.Y[1];
			node.Y[0] = node.Y[0] - node.Y[1];
			node.d = t2;
		} else
			node.d = t1;
		v2.push_back(node);
	} else if (n == 3) {
		float t1 = sqrt(pow(node.x - node.X[0], 2) + pow(node.y - node.Y[0], 2));
		float t2 = sqrt(pow(node.x - node.X[1], 2) + pow(node.y - node.Y[1], 2));
		float t3 = sqrt(pow(node.x - node.X[2], 2) + pow(node.y - node.Y[2], 2));
		float d12 = sqrt(pow(node.X[0] - node.X[1], 2) + pow(node.Y[0] - node.Y[1], 2));
		float d13 = sqrt(pow(node.X[0] - node.X[2], 2) + pow(node.Y[0] - node.Y[2], 2));
		float d23 = sqrt(pow(node.X[2] - node.X[1], 2) + pow(node.Y[2] - node.Y[1], 2));
		if (t1 <= t2 && t1 <= t3) {
			if (d12 > d13) {
				node.X[1] = node.X[1] + node.X[2];
				node.X[2] = node.X[1] - node.X[2];
				node.X[1] = node.X[1] - node.X[2];
				node.Y[1] = node.Y[1] + node.Y[2];
				node.Y[2] = node.Y[1] - node.Y[2];
				node.Y[1] = node.Y[1] - node.Y[2];
			}
			node.d = t1;
		} else if (t2 <= t1 && t2 <= t3) {
			node.X[0] = node.X[0] + node.X[1];
			node.X[1] = node.X[0] - node.X[1];
			node.X[0] = node.X[0] - node.X[1];
			node.Y[0] = node.Y[0] + node.Y[1];
			node.Y[1] = node.Y[0] - node.Y[1];
			node.Y[0] = node.Y[0] - node.Y[1];
			if (d12 > d23) {
				node.X[1] = node.X[1] + node.X[2];
				node.X[2] = node.X[1] - node.X[2];
				node.X[1] = node.X[1] - node.X[2];
				node.Y[1] = node.Y[1] + node.Y[2];
				node.Y[2] = node.Y[1] - node.Y[2];
				node.Y[1] = node.Y[1] - node.Y[2];
			}
			node.d = t2;
		} else if (t3 <= t1 && t3 <= t2) {
			node.X[0] = node.X[0] + node.X[2];
			node.X[2] = node.X[0] - node.X[2];
			node.X[0] = node.X[0] - node.X[2];
			node.Y[0] = node.Y[0] + node.Y[2];
			node.Y[2] = node.Y[0] - node.Y[2];
			node.Y[0] = node.Y[0] - node.Y[2];
			if (d13 < d23) {
				node.X[1] = node.X[1] + node.X[2];
				node.X[2] = node.X[1] - node.X[2];
				node.X[1] = node.X[1] - node.X[2];
				node.Y[1] = node.Y[1] + node.Y[2];
				node.Y[2] = node.Y[1] - node.Y[2];
				node.Y[1] = node.Y[1] - node.Y[2];
			}
			node.d = t3;
		}
		v3.push_back(node);
	}
}

void knn_search(float x0, float y0, float x1, float y1, vector<Node> *result) {
	for (int i = 0; i < v0.size(); i++) {
		v0[i].r = sqrt(pow(v0[i].x - x0, 2) + pow(v0[i].y - y0, 2));
		if (v0[i].r < 0.1) {
			result->push_back(v0[i]);
		}
	}

	for (int i = 0;i < v1.size(); i++) {
		float d1 = sqrt(pow(v1[i].x - x0, 2) + pow(v1[i].y - y0, 2));
		float d2 = sqrt(pow(x0 - x1, 2) + pow(y0 - y1, 2));
		float d3 = sqrt(pow(v1[i].X[0] - x1, 2) + pow(v1[i].Y[0] - y1, 2));
		float d4 = sqrt(pow(v1[i].X[0] - x0, 2) + pow(v1[i].Y[0] - y0, 2));
		v1[i].r = d1 + d2 + d3 - v1[i].d;
		if (v1[i].r < 0.1 && d1 < v1[i].d && d2 < v1[i].d && d3 < v1[i].d && d2 < d4) {
			result->push_back(v1[i]);
		}
	}

	for (int i = 0;i < v2.size(); i++) {
		float d1 = sqrt(pow(v2[i].x - x0, 2) + pow(v2[i].y - y0, 2));
		float d2 = sqrt(pow(x0 - x1, 2) + pow(y0 - y1, 2));
		float d3 = sqrt(pow(v2[i].X[0] - x1, 2) + pow(v2[i].Y[0] - y1, 2));
		float d4 = sqrt(pow(v2[i].X[0] - x0, 2) + pow(v2[i].Y[0] - y0, 2));
		v2[i].r = d1 + d2 + d3 - v2[i].d;
		if (v2[i].r < 0.1 && d1 < v2[i].d && d2 < v2[i].d && d3 < v2[i].d && d2 < d4) {
			result->push_back(v2[i]);
		}
	}

	for (int i = 0;i < v3.size(); i++) {
		float d1 = sqrt(pow(v3[i].x - x0, 2) + pow(v3[i].y - y0, 2));
		float d2 = sqrt(pow(x0 - x1, 2) + pow(y0 - y1, 2));
		float d3 = sqrt(pow(v3[i].X[0] - x1, 2) + pow(v3[i].Y[0] - y1, 2));
		float d4 = sqrt(pow(v3[i].X[0] - x0, 2) + pow(v3[i].Y[0] - y0, 2));
		v3[i].r = d1 + d2 + d3 - v3[i].d;
		if (v3[i].r < 0.1 && d1 < v3[i].d && d2 < v3[i].d && d3 < v3[i].d && d2 < d4) {
			result->push_back(v3[i]);
		}
	}

	sort(result->begin(), result->end());
}

float distance(float *a, float *b, int d) {
	float d_ = 0.0;
	for (int i = 0; i < d; i ++) {
		d_ += (a[i] - b[i]) * (a[i] - b[i]);
	}
	return d_;
}

float distance_except_d(float *a, float *b, int except_d) {
	float d = (a[except_d] - b[except_d]) * (a[except_d] - b[except_d]);
	return d;
}

void copy2result(float *a, float *result, int d) {
	for (int i = 0; i < d; i ++) {
		result[i] = a[i];
	}
}

int kdtree_insert(tree_node *&node, float *data, int layer, int d) {
	if (node == NULL) {
		node = (tree_node *)malloc(sizeof(tree_node));
		if (node == NULL) 
			return 1;

		for (int i = 0; i < d; i++)
			(node->data)[i] = data[i];
		node->d = layer % (d);
		node->left = NULL;
		node->right = NULL;
		return 0;
	}
	
	if (data[node->d] <= (node->data)[node->d])
		return kdtree_insert(node->left, data, ++layer, d);
	
	return kdtree_insert(node->right, data, ++layer, d);
}

void search_nearest(tree_node *tree, float *data_search, int dimension, float *result){
	// fprintf(stderr, "\nstart searching....\n");
	stack<tree_node *> Stack;
	
	tree_node *p = tree;
	
	while (p->left != NULL || p->right != NULL) {
		Stack.push(p);
		if (data_search[p->d] <= (p->data)[p->d]) {
			if (p->left == NULL) 
				break;
			p = p->left;
		} else {
			if (p->right == NULL) 
				break;
			p = p->right;
		}
	}
	
	float min_distance = distance(data_search, p->data, dimension);
	// fprintf(stderr, "init: %lf\n", min_distance);
	copy2result(p->data, result, dimension);
	// for (int i = 0; i < dimension; i++)
 //        fprintf(stderr, "%lf\t", result[i]);
 //    fprintf(stderr, "\n");

	float d = 0;
	while (Stack.size() > 0){
		tree_node *q = Stack.top();
		Stack.pop();
		d = distance(data_search, q->data, dimension);
		if (d <= min_distance){
			min_distance = d;
			copy2result(q->data, result, dimension);
		}

		float d_line = distance_except_d(data_search, q->data, q->d);
		if (d_line < min_distance) {
			if (data_search[q->d] > (q->data)[q->d]){
				if (q->left != NULL) 
					q = q->left;
				else 
					q = NULL;
			} else {
				if (q->right != NULL) 
					q = q->right;
				else 
					q = NULL;
			}
			if (q != NULL) {
				while (q->left != NULL || q->right != NULL){
					Stack.push(q);
					if (data_search[q->d] <= (q->data)[q->d]) {
						if (q->left == NULL) 
							break;
						q = q->left;
					} else {
						if (q->right == NULL) 
							break;
						q = q->right;
					}
				}
				if (q->left == NULL && q->right == NULL) 
					Stack.push(q);
			}
		}

	}
}

void kdtree_print(tree_node *tree, int d) {
	if (tree != NULL) {
		fprintf(stderr, "d:%d\n", tree->d);
		for (int i = 0; i < d; i++) {
			fprintf(stderr, "%lf\t", (tree->data)[i]);
		}
		fprintf(stderr, "\n");
		kdtree_print(tree->left, d);
		kdtree_print(tree->right, d);
	}
}

void kdtree_print_in(tree_node *tree, int d) {
	if (tree != NULL) {
		kdtree_print_in(tree->left, d);
		fprintf(stderr, "d:%d\n", tree->d);
		for (int i = 0; i < d; i++) {
			fprintf(stderr, "%lf\t", (tree->data)[i]);
		}
		fprintf(stderr, "\n");
		kdtree_print_in(tree->right, d);
	}
}