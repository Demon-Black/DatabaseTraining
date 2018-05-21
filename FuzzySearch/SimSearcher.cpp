#include "SimSearcher.h"
#include <fstream>
#include <cstring>
#include <cstdio>
#include <algorithm>
#include <iostream>
#include <cmath>

#define MAX_LENGTH 256
#define EPS 1E-6

using namespace std;

bool operator < (const sortItem& a, const sortItem& b) {
	return a.size < b.size;	
}


int min(int a, int b) {
	if (a > b) return b;
	return a;
}

int max(int a, int b) {
	if (a > b) return a;
	return b;
}

double max(double a, double b) {
	if (a > b) return a;
	return b;
}

SimSearcher::SimSearcher() {
	tree_dictionary = new Trie();
	string_size = new int[MAX_ITEM];
	string_list = new char*[MAX_ITEM];
    searchList = new int[MAX_ITEM];
   	lists = new sortItem[MAX_ITEM];
   	min_tokens = 400;
   	max_tokens = 0;

	for (int i = 0; i < 257; i++)
		d[i][0] = i;

	for (int j = 0; j < 257; j++)
		d[0][j] = j;
}

SimSearcher::~SimSearcher() {
	delete tree_dictionary;
	delete[] string_list;
	delete[] string_size;
	delete[] searchList;
}

int SimSearcher::createIndex(const char *filename, unsigned q_p) {
	q = q_p;
	string_index = 0;
	ifstream fp(filename);
    char line[260];
    const int buffer_size = 260;
    int count = 0;
    while(fp.getline(line, buffer_size)) {
    	string_tokens.push_back(std::vector<std::string>());
        int length = strlen(line);
        string_list[string_index] = new char[257];
        memcpy(string_list[string_index], line, length);
        string_size[string_index] = length;
      	for (int i = 0; i < length - q + 1; i++)
       		tree_dictionary->insert(line + i, string_index, q);

       	string temperary;
       	for (int i = 0; i < length; i++) {      //get tokens
       		if (line[i] == ' ') {
       			string_tokens[string_index].push_back(temperary);
       			if (token_id.find(temperary) == token_id.end()) {
       				token_id[temperary] = count;
       				count++;
       			}
       			temperary = "";
       			while (line[i] == ' ')
       				i++;
       			i--;
       		} else {
       			temperary += line[i];
       		}
       	}

       	if (temperary.size() > 0)  {
       		if (token_id.find(temperary) == token_id.end()) {
       			token_id[temperary] = count;
       			count++;
       		}
       		string_tokens[string_index].push_back(temperary);
       	}

       	std::sort(string_tokens[string_index].begin(), string_tokens[string_index].end());

       	auto it = std::unique(string_tokens[string_index].begin(), string_tokens[string_index].end());
       	
       	string_tokens[string_index].resize(std::distance(string_tokens[string_index].begin(), it));

       	// token_length[string_tokens[string_index].size()].push_back(string_index);
       	
       	if (string_tokens[string_index].size() < min_tokens)
       		min_tokens = string_tokens[string_index].size();
       	if (string_tokens[string_index].size() > max_tokens)
       		max_tokens = string_tokens[string_index].size();

       	for (auto str = string_tokens[string_index].begin(); str != string_tokens[string_index].end(); str++) {
       		int id = token_id[*str];
       		if (jaccard[string_tokens[string_index].size()][id] == NULL)
       			jaccard[string_tokens[string_index].size()][id] = new std::vector<int>;

       		jaccard[string_tokens[string_index].size()][id]->push_back(string_index);
       	}
        string_index++;
    }

    fp.close();
	return SUCCESS;
}

int SimSearcher::searchJaccard(const char *query, double threshold, vector<pair<unsigned, double> > &result) {
	result.clear();
	std::vector<std::string> query_tokens;
	int length_query = strlen(query);
	std::vector<int> id;
	string temperary;
    for (int i = 0; i < length_query; i++) {
      	if (query[i] == ' ') {
       		query_tokens.push_back(temperary);
       		temperary = "";
       		while (query[i] == ' ')
       			i++;
       		i--;
       	}
       	else
       		temperary += query[i];
    }
    if (temperary.size() > 0)
    	query_tokens.push_back(temperary);

    std::sort(query_tokens.begin(), query_tokens.end());
    auto ending = std::unique(query_tokens.begin(), query_tokens.end());
    query_tokens.resize(std::distance(query_tokens.begin(), ending));

    int grams_query = query_tokens.size();

    for (int i = 0; i < grams_query; i++) {
    	string temperary = query_tokens[i];
    	auto ending = token_id.find(temperary);
       	if (ending == token_id.end())
       		id.push_back(-1);
       	else
       		id.push_back(ending->second);
    }

	double t = max(threshold * grams_query, double(grams_query + min_tokens)) * threshold / (1.0 + threshold);
	std::vector<int> candidates;
	std::vector<int> verifyList;
	std::vector<int>* pointer;
	int t_int = ceil(t);
	int t_max = min(ceil(grams_query / threshold), max_tokens);
	for (int i = t; i <= t_max; i++) {
		common_tokens.clear();
		int lists_index = 0;
		candidates.resize(0);
		verifyList.resize(0);
		for (int j = 0; j < id.size(); j++) {
			if (id[j] < 0) 
				continue;
			pointer = jaccard[i][id[j]];
			if (pointer == NULL) 
				continue;
			lists[lists_index++] = sortItem(pointer);
		}

		if (lists_index < t) 
			continue;

		std::sort(lists, lists + lists_index);

		for (int j = 0; j < lists_index - t_int + 1; j++) {
			pointer = lists[j].data;
			for (auto item = pointer->begin(); item != pointer->end(); item++) {
				if (common_tokens[*item] == 0) 
					candidates.push_back(*item);
				common_tokens[*item]++;
			}
		}

		for (int k = 0; k < candidates.size(); k++) {
			if (common_tokens[candidates[k]] >= t_int) 
				verifyList.push_back(candidates[k]);
			for (int j = lists_index - t_int + 1; j < lists_index; j++) {
				pointer = lists[j].data;
				if (std::binary_search(pointer->begin(), pointer->end(), candidates[k])) {
					common_tokens[candidates[k]]++;
					if (common_tokens[candidates[k]] == t_int) 
						verifyList.push_back(candidates[k]);
				}
				if (common_tokens[candidates[k]] + lists_index - j - 1 < t) 
					break;
			}
		}

		for (int j = 0; j < verifyList.size(); j++) {
			double jaccard = (double)common_tokens[verifyList[j]] / (grams_query + i - common_tokens[verifyList[j]]);
			if (jaccard >= threshold) 
				result.push_back(make_pair(verifyList[j], jaccard));
		}
	}

	std::sort(result.begin(), result.end());
	auto result_size = std::unique(result.begin(), result.end());
	result.resize(std::distance(result.begin(), result_size));
	return SUCCESS;
}

int SimSearcher::searchED(const char *query, unsigned threshold, vector<pair<unsigned, unsigned> > &result) {
	result.clear();
	int threshold_s = threshold;
	int length_query = strlen(query);
	int t = length_query - q + 1 - threshold_s * q;
	int distance;
	
	if (t <= 0) { // brute force
		for (int i = 0; i < string_index; i++) {
			distance = editDistance(query, i, length_query, threshold_s);
	    	if (distance >= 0) 
	    		result.push_back(std::make_pair(i, distance));
		}
		return SUCCESS;
	}


	int lists_index = 0;
	void* pointer;

	for (int i = 0; i < length_query - q + 1; i++) {
       	pointer = tree_dictionary->searchString((char*)query + i, q);
		if (pointer != NULL) {
			sortItem item((std::vector<int>*)pointer);
			if (item.size > 0) 
				lists[lists_index++] = item;
		}
    }

    std::sort(lists, lists + lists_index);
    std::vector<int> verifyList;

	for (int i = 0; i < lists_index - t + 1; i++) {
	  	for (int j = 0; j < lists[i].size; j++) {
	   		int temperary = (*(lists[i].data))[j];
	   		verifyList.push_back(temperary);
	   	}
	}

	std::sort(verifyList.begin(), verifyList.end());
	auto ending = std::unique(verifyList.begin(), verifyList.end());
	verifyList.resize(std::distance(verifyList.begin(), ending));

	for (int i = 0; i < verifyList.size(); i++) {
		int temperary = verifyList[i];
		distance = editDistance(query, temperary, length_query, threshold_s);
		if (distance >= 0) 
			result.push_back(std::make_pair(temperary, distance));
	}
	return SUCCESS;
}

int SimSearcher:: editDistance(const char* query, int item, int length_query, int threshold) {
	char* str = string_list[item];
	int length_of_string = string_size[item];

	if (abs(length_of_string - length_query) > threshold)
		return -1;

	bool exceed = true;
	for (int i = 1; i <= length_of_string; i++) {
		exceed = true;
		int j1 = max(1, i - threshold);
		int j2 = min(length_query, i + threshold);
		for (int j = j1; j <= j2; j++) {
			int t = (str[i - 1] == query[j - 1]) ? 0 : 1;
			int temperary1 = d[i-1][j] + 1;
			int temperary2 = d[i][j-1] + 1;
			if (j + 1 - i > threshold) 
				temperary1 = MAX_LENGTH;
			if (i + 1 - j > threshold) 
				temperary2 = MAX_LENGTH;
			d[i][j] = min(temperary1, temperary2);
			d[i][j] = min(d[i][j], d[i-1][j-1] + t);
			if (d[i][j] <= threshold) 
				exceed = false;
		}
		if (d[i][0] <= threshold) 
			exceed = false;
		if (exceed) 
			return -1;
	}
	if (d[length_of_string][length_query] > threshold) 
		return -1;
	return d[length_of_string][length_query];
}
