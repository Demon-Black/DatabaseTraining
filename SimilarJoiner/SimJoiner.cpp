#include "SimJoiner.h"

#define MAX_LENGTH 256

using namespace std;

int jaccard_result_compare(const JaccardJoinResult& a, const JaccardJoinResult b) {
	if(a.id1 == b.id1) 
		return a.id2 < b.id2;
	return a.id1 < b.id1;
}

vector<int> StringIndex::frequence;

int frequence_compare(const int&a, const int &b) {
	return StringIndex::frequence[a] < StringIndex::frequence[b];
}

SimJoiner::SimJoiner() {
    for (int i = 0; i < 257; i++)
        d[i][0] = i;
    for (int j = 0; j < 257; j++)
        d[0][j] = j;
}


SimJoiner::~SimJoiner() {}

void SimJoiner::readFile(const char *filename1, const char *filename2) {
	words1.clear();
	words2.clear();
	string s;
	ifstream fin1(filename1);
	while(getline(fin1,s))
		words1.push_back(s);
	fin1.close();
	ifstream fin2(filename2);
	while(getline(fin2,s))
		words2.push_back(s);
	fin2.close();
}

int SimJoiner::joinJaccard(const char *filename1, const char *filename2, double threshold, vector<JaccardJoinResult> &result) {
    jaccard_threshold = threshold;
    result.clear();
    readFile(filename1, filename2);
    jaccard_candidate_set.set_size(words2.size());
    for(int i = 0; i < words2.size(); i++)
    	addTokens(i);
    for(int i=0;i<words1.size();i++)
    	addResult(i, result);
    return SUCCESS;
}

int SimJoiner::joinED(const char *filename1, const char *filename2, unsigned threshold, vector<EDJoinResult> &result) {
    result.clear();
    createIndexED(filename2, threshold);
    ifstream fp(filename1);
    char line[260];
    const int buffer_size = 260;
    vector<pair<unsigned, unsigned>> resultED;
    int id1 = 0;
    while(fp.getline(line, buffer_size)) {
        searchED(line, threshold, resultED);
        for (auto& temperary:resultED) {
            EDJoinResult result_t;
            result_t.id1 = id1;
            result_t.id2 = temperary.first;
            result_t.s = temperary.second;
            result.push_back(result_t);
        }
        id1++;
    }
    fp.close();
    return SUCCESS;
}

int SimJoiner::createIndexED(const char* filename, unsigned threshold) {
    for (int i = 0; i < 257; i++)
        for (int j = 0; j < 15; j++) {
            substring_map[i][j].clear();
            substring_length[i][j] = 0;
        }
    string_list.resize(0);
    ifstream fp(filename);
    char line[260];
    char temperary[MAX_THRESHOLD];
    const int buffer_size = 260;
    string_index_ed = 0;
    while(fp.getline(line, buffer_size)) {
        string_list.push_back(line);
        int length = strlen(line);
        int big_block, small_block;
        if ((length % (threshold + 1)) == 0)
            big_block = small_block = length / (threshold + 1);
        else {
            small_block = length / (threshold + 1);
            big_block = small_block + 1;
        }
        temperary[small_block] = 0;
        int i = 0;
        while (unsigned(i * small_block) < (length - (length % (threshold + 1) * big_block))){
            memcpy(temperary, line + i * small_block, small_block);
            substring_length[length][i] = small_block;
            auto iterator_t = substring_map[length][i].find(temperary);
            if (iterator_t == substring_map[length][i].end()) {
                vector<int> *temperaryVector = new vector<int>();
                temperaryVector->push_back(string_index_ed);
                substring_map[length][i][temperary] = temperaryVector;
            }
            else
                iterator_t->second->push_back(string_index_ed);
            i++;
        }
        int j = 0;
        temperary[big_block] = 0;
        while (i * small_block + j * big_block < length) {
            memcpy(temperary, line + i * small_block + j * big_block, big_block);
            auto iterator_t = substring_map[length][i + j].find(temperary);
            substring_length[length][i+j] = big_block;
            if (iterator_t == substring_map[length][i + j].end()) {
                vector<int> *temperaryVector = new vector<int>();
                temperaryVector->push_back(string_index_ed);
                substring_map[length][i + j][temperary] = temperaryVector;
            }
            else
                iterator_t->second->push_back(string_index_ed);
            j++;
        }
        string_index_ed++;
    }
    fp.close();
    return SUCCESS;
}

int SimJoiner::searchED(const char* query, unsigned threshold, vector<pair<unsigned, unsigned> >& result) {
    result.clear();
    vector<int> candidates;
    int length = strlen(query);
    char temperary[MAX_THRESHOLD];
    int upperbound = min(length + threshold, unsigned(256));
    int lowerbound = max(unsigned(0), length - threshold);
    for (int i = lowerbound; i <= upperbound; i++) {
        int index = 0;
        int p = 0;
        int delta = length - i;
        while (substring_length[i][index] > 0) {
            int offset = max(p - index, p + delta - ((int)threshold - index));
            int offset_bound = min(p + index, p + delta + ((int)threshold - index));
            while (offset + substring_length[i][index] <= length) {
                if (offset >= 0) {
                    memcpy(temperary, query + offset, substring_length[i][index]);
                    temperary[substring_length[i][index]] = 0;
                    auto iterator_t = substring_map[i][index].find(temperary);
                    if (iterator_t != substring_map[i][index].end())
                        for (auto& candidate_t : *iterator_t->second)
                            candidates.push_back(candidate_t);
                }
                offset++;
                if (offset > offset_bound) 
                    break;
            }
            p += substring_length[i][index];
            index++;
        }
    }

    sort(candidates.begin(), candidates.end());
    auto iterator_t = unique(candidates.begin(), candidates.end());
    candidates.resize(distance(candidates.begin(), iterator_t));

    for (int i = 0; i < candidates.size(); i++) {
        int temperary = candidates[i];
        int distance = editDistance(query, temperary, length, threshold);
        if (distance >= 0) 
            result.push_back(make_pair(temperary, distance));
    }   
    return SUCCESS;
}


int SimJoiner:: editDistance(const char* query, int item, int length_query, int threshold) {
    string str = string_list[item];
    int length_of_string = str.size();;

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



void SimJoiner::addTokens(int n) {
	stringstream sin(words2[n]);
	string temperary_tokens;
	unordered_set<int> tokens_set;
	while(sin>>temperary_tokens)
		tokens_set.insert(string_index.add(temperary_tokens));
	int token_number = tokens_set.size();
	for(unordered_set<int>::iterator temperary = tokens_set.begin(); temperary != tokens_set.end();temperary++)
		token_index[token_number][*temperary].push_back(n);
}

void SimJoiner::addResult(int n, vector<JaccardJoinResult> &result) {
	vector<JaccardJoinResult> result_tempeary;
	JaccardJoinResult temperary_result;
	temperary_result.id1 = n;
	stringstream sin(words1[n]);
	string temperary_tokens;
	unordered_set<int> tokens_set;
	vector<int> words_vector;

	while(sin>>temperary_tokens) {
		int find_result = string_index.find(temperary_tokens);
		if(find_result != -1) 
			tokens_set.insert(find_result);
	}

	for(unordered_set<int>::iterator temperary = tokens_set.begin(); temperary != tokens_set.end();temperary++)
		words_vector.push_back(*temperary);
	sort(words_vector.begin(), words_vector.end(), frequence_compare);
	int q_size = words_vector.size();
	int min_size = ceil((double)q_size * jaccard_threshold);
	int max_size = floor((double)q_size / jaccard_threshold);
	max_size = min(256, max_size);

	for(int i = min_size; i <= max_size; i++) {
		int common_number = 0;
		int temperary_min_size =  ceil((q_size + i) * jaccard_threshold / (1 + jaccard_threshold));
		temperary_min_size = max(temperary_min_size, min_size);
		for(vector<int>::iterator temperary = words_vector.begin(); temperary != words_vector.end(); temperary++)
			if(token_index[i].find(*temperary) != token_index[i].end()) 
				common_number++;
		if(common_number < temperary_min_size) 
			continue;

		jaccard_candidate_set.clear(temperary_min_size);
		common_number = 0;
		for(vector<int>::iterator temperary = words_vector.begin(); temperary != words_vector.end(); temperary++) {
			common_number++;
			if(token_index[i].find(*temperary) == token_index[i].end()) 
				continue;
			vector<int> &temperary_vector = token_index[i][*temperary];
			if(common_number <= (q_size - temperary_min_size + 1))
				for(int j = 0; j < temperary_vector.size(); j++)
					jaccard_candidate_set.add(temperary_vector[j]);
			else {
				for(vector<int>::iterator iterator_t = jaccard_candidate_set.dirty.begin(); iterator_t != jaccard_candidate_set.dirty.end();iterator_t++)
					if(binary_search(temperary_vector.begin(), temperary_vector.end(), *iterator_t))
						jaccard_candidate_set.add(*iterator_t);
			}
		}
		for(int j = 0;j < jaccard_candidate_set.candidate.size(); j++) {
			int temperary_rank = jaccard_candidate_set.candidate[j];
			double distance = (*jaccard_candidate_set.common_numbers)[temperary_rank];
			distance = distance / (q_size + i - distance);
			if(distance > jaccard_threshold) {
				temperary_result.id2 = temperary_rank;
				temperary_result.s = distance;
				result_tempeary.push_back(temperary_result);
			}
		}
	}
	sort(result_tempeary.begin(), result_tempeary.end(), jaccard_result_compare);
	for(int i=0;i<result_tempeary.size();i++)
		result.push_back(result_tempeary[i]);
}