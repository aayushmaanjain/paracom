int skip = (int) (26/numproc);

vector<int> indices{0};

for (int i = 1; i < numproc; ++i) {
	iterator it=upper_bound(freq.begin(), freq.end(), skip * i);
	--it;
	indices.push_back(it - freq.begin());
}
