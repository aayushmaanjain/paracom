#include <bits/stdc++.h>
#include <omp.h>
#include <limits.h>

using namespace std;

bool wordfreq_cmp(pair <string, int> &a, pair <string, int> &b)
{
	return 	a.second<b.second;
}

int main(int argc, char** argv)
{
	double start, end;
	int maxthreads;
	cout<<"#threads: ";
	cin>>maxthreads;
	int k;
	cout<<"k: ";
	cin>>k;
	omp_set_num_threads(maxthreads);
	unordered_map<string, int> wordfreq;
	// unordered_map<string, int>:: iterator itr;
	random_device rd;
	mt19937 rng(rd());
	uniform_int_distribution<int> uni(1,1000000);
	ifstream infile;
	infile.open("words_alpha.txt");
	string line;
	int count = 0;
	// int limit = 1000;
	while(/*limit-- &&*/ getline(infile, line))
	{
		// cout<<line<<"\t"<<uni(rng)<<endl;
		wordfreq[line] = uni(rng);
		count++;
	}
	cout<<"#words: "<<count<<endl;
	cout<<"Bucket count: "<<wordfreq.bucket_count()<<endl;
	vector <int> buckets;
	for(int i=0;i<wordfreq.bucket_count();i++)	{
		// cout<<i<<"\t"<<wordfreq.bucket_size(i)<<endl;
		if(wordfreq.bucket_size(i)!=0)
			buckets.push_back(i);
	}
	cout<<"Non empty buckets: "<<buckets.size()<<endl;
	vector <int>::iterator bkt_itr;
	start = omp_get_wtime();
	vector <pair <string, int> > topk_th[maxthreads];
	int id;
	vector < pair <string, int> > locallist;
	#pragma omp parallel private(id, locallist)
	{
		id = omp_get_thread_num();
		#pragma omp for
		for(bkt_itr = buckets.begin(); bkt_itr < buckets.end(); bkt_itr++)
		{	
			// cout<<omp_get_thread_num()<<" - "<<*bkt_itr<<endl;
			// cout<<itr->first<<"\t"<<itr->second<<endl;
			for(auto itr = wordfreq.begin(*bkt_itr); itr != wordfreq.end(*bkt_itr); itr++){
				// cout<<omp_get_thread_num()<<" - "<<itr->first<<" : "<<itr->second<<endl;
				// arr[omp_get_thread_num()]++;
				locallist.push_back(make_pair(itr->first,itr->second));
			}
		}
		make_heap(locallist.begin(),locallist.end(),wordfreq_cmp);
		for(int i = 0; i < k; i++)
		{
			// cout<<"Heap size: "<<locallist.size()<<" "<<locallist.front().first<<" "<<locallist.front().second<<endl;
			topk_th[id].push_back(locallist.front());
			pop_heap(locallist.begin(), locallist.end(), wordfreq_cmp);
			locallist.pop_back();
		}
	}

	vector <pair <string, int> > merge = topk_th[0];
	// topk_th[0].erase(topk_th[0].begin(), topk_th[0].end());
	for(int i=1; i<maxthreads; i++)
	{
		merge.insert(merge.end(), topk_th[i].begin(), topk_th[i].end());
		// topk_th[i].erase(topk_th[i].begin(),topk_th[i].end());
	}
	cout<<"merge: "<<merge.size()<<endl;
	make_heap(merge.begin(), merge.end(), wordfreq_cmp);
	vector <pair <string, int> > topk;
	for(int i=0; i < k; i++)
	{
		topk.push_back(merge.front());
		pop_heap(merge.begin(), merge.end(), wordfreq_cmp);
		merge.pop_back();
	}
	end = omp_get_wtime();
	// Printing Results
	for(int i=0; i<k; i++)
		cout<<topk[i].first<<" "<<topk[i].second<<endl;
	cout<<"TIME: "<<end-start<<endl;
	return 0;
}