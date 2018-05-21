#ifndef __TRIE_H__
#define __TRIE_H__ 

#include <vector>
#include <cstring>
#include <cstdlib>

#define ASCIISIZE 127

struct TrieNode {
	int child_nodes[128];
	std::vector<int> node_list; 
	TrieNode();
	~TrieNode();
};

class Trie {
	TrieNode* pool;
	int node_index;
public:
	Trie();
	~Trie();
	void insert(char* str, int idx, int length);
	void insert(const char* str, int idx);	
	std::vector<int> * searchString(char* str, int length);
	std::vector<int> * searchString(const char* str);
};

#endif