#pragma once

#include <vector>
#include <utility>
#include <string>
#include <unordered_map>
#include <unordered_set>

#include "Trie.h"

const int SUCCESS = 0;
const int FAILURE = 1;

#define MAX_ITEM 800100

struct sortItem {
	std::vector<int> *data;
	int size;

	sortItem(std::vector<int> * data) {
		this->data = data;
		size = data->size();
	}

	sortItem(){}
	~sortItem(){}
};

class SimSearcher {
public:
	SimSearcher();
	~SimSearcher();

	Trie* tree_dictionary;
	std::unordered_map<int, int> common_tokens;
	std::unordered_map<std::string, int> token_id;
	std::unordered_map<int, std::vector<int>*> jaccard[257];
	std::vector<std::vector<std::string>> string_tokens;
	// std::vector<int> token_length[257];
	char** string_list;
	int* string_size, *searchList;
	int q;
	int string_index;
	int min_tokens;
	int max_tokens;
	int d[257][257];
	sortItem* lists;

	int createIndex(const char *filename, unsigned q_p);
	int searchJaccard(const char *query, double threshold, std::vector<std::pair<unsigned, double> > &result);
	int searchED(const char *query, unsigned threshold, std::vector<std::pair<unsigned, unsigned> > &result);
	int editDistance(const char *query, int item, int length_query, int threshold);
};

