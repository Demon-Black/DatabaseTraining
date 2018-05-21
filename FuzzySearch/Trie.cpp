#include "Trie.h"
#include <cstdio>
#include <iostream>

#define MAXITEM 8000000

TrieNode::TrieNode() {
	for (int i = 0; i < 128; i++)
		child_nodes[i] = 0;
}

TrieNode::~TrieNode() {}

Trie::Trie() {
	pool = new TrieNode[MAXITEM];
	node_index = 1;
}

Trie::~Trie() {
	delete[] pool;
}

void Trie::insert(char* str, int index_p, int length) {
	int node = 0;
	int current = 0;
	while (current < length) {
		int index = str[current];
		if (pool[node].child_nodes[index] == 0) 
			pool[node].child_nodes[index] = node_index++;
		node = pool[node].child_nodes[index];
		current++;
	}
	if (!pool[node].node_list.empty() && pool[node].node_list[pool[node].node_list.size() - 1] == index_p) 
		return;
	pool[node].node_list.push_back(index_p);
}

void Trie::insert(const  char* str, int index_p) {
	int node = 0;
	int current = 0;
	int index = str[current];
	while (index != 0) {
		if (pool[node].child_nodes[index] == 0) 
			pool[node].child_nodes[index] = node_index++;
		node = pool[node].child_nodes[index];
		current++;
		index = str[current];
	}
	if (!pool[node].node_list.empty() && pool[node].node_list[pool[node].node_list.size() - 1] == index_p) 
		return;
	pool[node].node_list.push_back(index_p);
}


std::vector<int> * Trie::searchString(char* str, int length) {
	int node = 0;
	int current = 0;
	while (current < length) {
		int index = str[current];
		node = pool[node].child_nodes[index];
		if (node == 0) 
			return NULL;
		current++;
	}
	return &(pool[node].node_list);
}

std::vector<int> * Trie::searchString(const char* str) {
	int node = 0;
	int current = 0;
	int index = str[current];
	while (index != 0) {
		node = pool[node].child_nodes[index];
		if (node == 0) 
			return NULL;
		current++;
		index = str[current];
	}
	return &(pool[node].node_list);
}
