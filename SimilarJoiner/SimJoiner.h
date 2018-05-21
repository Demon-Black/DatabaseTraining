#ifndef __SIMJOINER_H__
#define __SIMJOINER_H__

#include <iostream>
#include <fstream>
#include <cmath>
#include <algorithm>
#include <map>
#include <unordered_map>
#include <string>
#include <queue>
#include <functional>
#include <utility>
#include <set>
#include <sstream>
#include <unordered_set>
#include <cstring>
#include <cstdlib>

using namespace std;

template <typename IDType, typename SimType>
struct JoinResult {
    IDType id1;
    IDType id2;
    SimType s;
};

typedef JoinResult<unsigned, double> JaccardJoinResult;
typedef JoinResult<unsigned, unsigned> EDJoinResult;

const int SUCCESS = 0;
const int FAILURE = 1;

#define MAX_THRESHOLD 100

struct CandidateSet{
    vector<int> *common_numbers;
    vector<int> dirty;
    vector<int> candidate;
    int min_size;

    void set_size(int n) {
        common_numbers = new vector<int>(n);
    }

    void clear(int n) {
        min_size = n;
        for(int i = 0; i < dirty.size(); i++)
            (*common_numbers)[dirty[i]] = 0;
        dirty.clear();
        candidate.clear();
    }

    void add(int n) {
        if((*common_numbers)[n])
            (*common_numbers)[n]++;
        else {
            (*common_numbers)[n] = 1;
            dirty.push_back(n);
        }
        
        if((*common_numbers)[n] == min_size) 
            candidate.push_back(n);
    }
};

struct StringIndex {
    unordered_map<string, int> string_index_map;
    static vector<int> frequence;
    int common_token_number;
    
    StringIndex():common_token_number(-1) {}
    
    int get_index(string& s) {
        unordered_map<string, int>::iterator temperary = string_index_map.find(s);
        if(temperary != string_index_map.end())
            return temperary->second;
        else {
            common_token_number++;
            string_index_map[s] = common_token_number;
            frequence.push_back(0);
            return common_token_number;
        }
    }

    int add(string& s) {
        int index = get_index(s);
        frequence[index]++;
        return index;
    }

    int find(string& s) {
        unordered_map<string, int>::iterator temperary = string_index_map.find(s);
        if(temperary != string_index_map.end())
            return temperary->second;
        else
            return -1;
    }
};




class SimJoiner {
public:
    SimJoiner();
    ~SimJoiner();

    int string_index_ed;
    int d[257][257];
    unordered_map<string, vector<int>*> substring_map[260][15];
    int substring_length[260][MAX_THRESHOLD];
    vector<string> string_list;

    void readFile(const char *filename1, const char *filename2);

    int joinJaccard(const char *filename1, const char *filename2, double threshold, std::vector<JaccardJoinResult> &result);
    int joinED(const char *filename1, const char *filename2, unsigned threshold, std::vector<EDJoinResult> &result);

    void addTokens(int n);
    void addResult(int n, std::vector<JaccardJoinResult> &result);


    vector<string> words1;
    vector<string> words2;

    int createIndexED(const char* filename, unsigned threshold);
    int searchED(const char* query, unsigned threshold, vector<pair<unsigned, unsigned> >& result);
    int editDistance(const char* query, int item, int length_query, int threshold);

    //Jaccard
    double jaccard_threshold;
    StringIndex string_index;
    unordered_map<int, vector<int> > token_index[257];
    CandidateSet jaccard_candidate_set;
};

#endif
