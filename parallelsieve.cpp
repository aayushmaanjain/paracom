#include <bits/stdc++.h>
#include <stdlib.h>
#include <mpi.h>

using namespace std;

char* createBV(long long int len) {
	long long int size = (len + 7) >> 3;
	char* a = new char[size];
	memset(a, 0, size);
	return a;
}

inline void setBV(char* a, long long k) {
	a[k>>3] = a[k>>3] | 1 << (k&7);
}

inline void resetBV(char* a, long long k) {
	a[k>>3] = a[k>>3] & ~(1 << (k&7));
}

inline bool test(char* a, long long k) {
	return a[k>>3] & 1 << (k&7);
}

int main(int argc, char *argv[])
{
	long long int N = atoll(argv[argc-1]);
	long long int sqrtN;

	char *prime_sqrtN;
	char *prime_prll;

	long long int lower;
	long long int upper;
	long long int temp;
	long long int dividesize;

	int rank;
	int numproc;

	vector<long long int> localprimes;
	vector<long long int> primes;

	long long int size;
	vector <long long int> sizes;
	// int sizein;

	double start,end,end1,end2,end3;

	int mpi_argc = argc - 1;
	MPI_Init(&mpi_argc,&argv);
	MPI_Comm_rank(MPI_COMM_WORLD,&rank);
	MPI_Comm_size(MPI_COMM_WORLD,&numproc);
	// Master gets program parameter N
	// if(rank==0)
	// {
	// 	cout<<"Enter N: ";
	// 	cin>>N;
	// 	//clock_gettime(CLOCK_REALTIME,&timer);
	// 	// start = timer.tv_sec;
	// }
	start = MPI_Wtime();

	// // Broadcast N
	// MPI_Bcast(&N,1,MPI_LONG_LONG,0,MPI_COMM_WORLD);

	sqrtN = (long long int)sqrt(N);

	dividesize = (N - (sqrtN + 1)) / numproc;
	lower = sqrtN + rank * dividesize + 1;
	if(rank == numproc-1)
		upper = N;
	else
		upper = lower + dividesize - 1;
	// cout<<"Rank: "<<rank<<"Lower: "<<lower<<"Upper: "<<upper<<endl;

	prime_sqrtN = createBV(sqrtN+1);
	prime_prll = createBV(upper-lower+1);

	end1 = MPI_Wtime();

	// Marking all prime till sqrt(N)
	long long int i,j;
	int sqrt_sqrtN = (int)sqrt(sqrtN);
	setBV(prime_sqrtN, 0); setBV(prime_sqrtN, 1);	// 0 represents prime, 1 - nonprime.
	for(i=2;i<=sqrt_sqrtN;++i)
		if(!test(prime_sqrtN, i))
			for(j=i*i;j<=sqrtN;j+=i){
				setBV(prime_sqrtN, j);
			}
	// time
	//if(rank==0)
	{
		// clock_gettime(CLOCK_REALTIME,&timer);
		// end = timer.tv_sec;
		end2 = MPI_Wtime();
		// cout<<rank<<"time: "<<end-start<<endl;
	}
	// mark the remaining numbers - parallelized list
	// i = 2;
	j = ((lower + 1) / 2) * 2;
	for(;j<=upper;j+=2)
		setBV(prime_prll,j-lower);
	for(i=3;i<=sqrtN;i+=2)
		if(!test(prime_sqrtN, i))
		{
			// DO: could make it more efficient by starting at i^2 if i^2 > lower
			temp = (lower<i*i)?i*i:lower;
			if(temp%i==0)	// !lower%i not working
				j=temp;
			else
				j=((long long int)lower/i + 1) * i;
			// cout<<"i: "<<i<<"; lower: "<<lower<<"; j: "<<j<<endl;
			if(j%2==0)
				j = j + i;
			for(;j<=upper;j+=i*2){
				if(!test(prime_prll,j-lower))
					setBV(prime_prll, j-lower);
			}
		}
	// time
	// if(rank==0)
	{
		// clock_gettime(CLOCK_REALTIME,&timer);
		// end = timer.tv_sec;
		end3 = MPI_Wtime();
		// cout<<"time"<<rank<<" : "<<end1-start<<'\t'<<end2-start<<'\t'<<end3-start<<endl;
	}
	// list of primes
	// Primes till sqrt(N)

	/*// Remove this
	if(rank==0)
	{
		for(i=2;i<=sqrtN;i++)
			if(!test(prime_sqrtN, i))
				localprimes.push_back(i);
	}*/
	if(rank!=0)
	{
		for(i=lower;i<=upper;i++)
			if(!test(prime_prll, i-lower))
				localprimes.push_back(i);
		free(prime_prll);
	}
	if(rank==0)
	{
		for(i=2;i<=sqrtN;i++)
			if(!test(prime_sqrtN, i))
				primes.push_back(i);
		free(prime_sqrtN);
		for(i=lower;i<=upper;i++)
			if(!test(prime_prll, i-lower))	
				primes.push_back(i);
		free(prime_prll);
		sizes.reserve(numproc);
		sizes[0] = primes.size();
		for(i=1;i<numproc;i++)		// i is rank
		{
			//size=-1;
			MPI_Recv(&sizes[i],1,MPI_LONG_LONG,i,0,MPI_COMM_WORLD,MPI_STATUS_IGNORE);
			sizes[i] += sizes[i-1];
		}
		primes.resize(sizes[numproc-1]);
		for(i=1;i<numproc;i++)
		{
			MPI_Recv(&primes[sizes[i-1]],sizes[i]-sizes[i-1],MPI_LONG_LONG,i,0,MPI_COMM_WORLD,MPI_STATUS_IGNORE);
			// primes.insert(primes.end(),localprimes.begin(),localprimes.end());
		}
	}
	else
	{
		size = localprimes.size();
		MPI_Send(&size,1,MPI_INT,0,0,MPI_COMM_WORLD);
		MPI_Send(&localprimes[0],size,MPI_LONG_LONG,0,0,MPI_COMM_WORLD);
		localprimes.clear();
	}

	// time
	if(rank==0)
	{
		// clock_gettime(CLOCK_REALTIME,&timer);
		// end = timer.tv_sec;
		end = MPI_Wtime();
		cout<<"time: "<<end3-start<<"\t"<<end-start<<endl;
		cout<<"#Primes: "<<primes.size()<<endl;
	}
	// Printing
	/*if(rank==0)
	{
		vector <long long int> :: iterator it;
		for(it = primes.begin();it!= primes.end();it++)
			cout<<rank<<" : "<<*it<<endl;
	}*/
	MPI_Finalize();

	return 0;
}
