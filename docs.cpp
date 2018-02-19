#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <map>
#include <utility>
#include <iterator>
#include <vector>
#include <algorithm>
#include <cstring>
// #include <mpi.h>

using namespace std;

bool cmp(pair<string, int> a, pair<string, int> b) {
	return a.second > b.second;
}

int main() {
	vector<string> fnames;
	fnames.push_back("file1.txt");
	fnames.push_back("file2.txt");
	vector<string>::iterator iter;
	map<string, map<string, int> > doc_list;

	for (iter = fnames.begin(); iter != fnames.end(); ++iter) {
		std::ifstream t(*iter);

		stringstream stream1;
		stream1 << t.rdbuf();

		string temp = stream1.str();
		char* line = (char*)temp.c_str();

		const char* delimiters = "-, \n";

		char* word = strtok(line, delimiters);

		while (word != NULL) {
			temp = string(word);
		 	if (doc_list[*iter].find(temp) == doc_list[*iter].end()) {
		 		doc_list[*iter][temp] = 1;
		 	}
		 	else {
		 		doc_list[*iter][temp] += 1;
		 	}
			word = strtok(NULL, "-, \n");
		}
	}

	map<string, vector<pair<string, int> > > local_list;

	map<string, map<string, int> >::iterator doc_iter;

	map<string, int>::iterator word_iter;

	for (doc_iter = doc_list.begin(); doc_iter != doc_list.end(); ++doc_iter) {
		for (word_iter = doc_iter->second.begin(); word_iter != doc_iter->second.end();  word_iter++) {
			local_list[word_iter->first].push_back(make_pair(doc_iter->first, word_iter->second));
		}
	}

	map<string, vector<pair<string, int> > >::iterator local_iter;

	for (local_iter = local_list.begin(); local_iter != local_list.end(); ++local_iter) {
		sort(local_iter->second.begin(), local_iter->second.end(), cmp);
	}

	vector<pair<string, int> >::iterator vec_iter;

	for (local_iter = local_list.begin(); local_iter != local_list.end(); ++local_iter) {
		cout << local_iter->first << '\n';
		for (vec_iter = local_iter->second.begin(); vec_iter != local_iter->second.end(); vec_iter++) {
			cout << vec_iter->first << '\t' << vec_iter->second << '\n';
		}
		cout << '\n';
	}

	return 0;
}
