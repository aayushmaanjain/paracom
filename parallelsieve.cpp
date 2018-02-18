#include <mpi.h>
#include <cmath>
#include <time.h>
#include <iostream>
#include <cstdlib>
#include <vector>


using namespace std;

int main(int argc, char *argv[])
{
	long long int N;
	int sqrtN;

	char *prime_sqrtN;
	char *prime_prll;

	int lower;
	int upper;
	int dividesize;
	
	int rank;
	int numproc;

	vector<long long int> localprimes;
	vector<long long int> primes;

	int size;

	timespec timer;
	double start,end;

	MPI_Init(&argc,&argv);
	MPI_Comm_rank(MPI_COMM_WORLD,&rank);
	MPI_Comm_size(MPI_COMM_WORLD,&numproc);
	// Master gets program parameter N
	if(rank==0)
	{
		cout<<"Enter N: ";
		cin>>N;
		/*clock_gettime(CLOCK_REALTIME,&timer);
		start = timer.tv_sec;*/
		start = MPI_Wtime();
	}
	// Broadcast N
	MPI_Bcast(&N,1,MPI_LONG_LONG,0,MPI_COMM_WORLD);
	
	sqrtN = (int)sqrt(N);
	
	dividesize = (N - (sqrtN + 1)) / numproc;
	lower = sqrtN + rank * dividesize + 1;
	if(rank == numproc-1)
		upper = N;
	else
		upper = lower + dividesize - 1;
	// cout<<"Rank: "<<rank<<"Lower: "<<lower<<"Upper: "<<upper<<endl;

	prime_sqrtN = (char *)calloc((sqrtN+1),sizeof(char));
	prime_prll = (char *)calloc((upper-lower+1),sizeof(char));
	
	// Marking all prime till sqrt(N)
	int i,j;
	int sqrt_sqrtN = (int)sqrt(sqrtN);
	prime_sqrtN[0]=prime_sqrtN[1]=1;	// 0 represents prime, 1 - nonprime.
	for(i=2;i<=sqrt_sqrtN;i++)
		if(prime_sqrtN[i]==0)
			for(j=i*i;j<=sqrtN;j+=i)
				if(prime_sqrtN[j]==0)
					prime_sqrtN[j]=1;

	// mark the remaining numbers - parallelized list
	for(i=2;i<=sqrtN;i++)
		if(prime_sqrtN[i]==0)
		{
			// DO: could make it more efficient by starting at i^2 if i^2 > lower
			if(lower%i==0)	// !lower%i not working
				j=lower;
			else
				j=((int)lower/i + 1) * i;
			// cout<<"i: "<<i<<"; lower: "<<lower<<"; j: "<<j<<endl;
			for(;j<=upper;j+=i)
				if(prime_prll[j-lower]==0)
					prime_prll[j-lower]=1;

		}


	// list of primes
	// Primes till sqrt(N)
	
	if(rank!=0)
	{
		for(i=lower;i<=upper;i++)
			if(prime_prll[i-lower]==0)
				localprimes.push_back(i);
	}
	if(rank==0)
	{
		for(i=2;i<=sqrtN;i++)
			if(prime_sqrtN[i]==0)
				primes.push_back(i);
		for(i=lower;i<=upper;i++)
			if(prime_prll[i-lower]==0)
				primes.push_back(i);
		for(i=1;i<numproc;i++)		// i is rank
		{
			//size=-1;
			MPI_Recv(&size,1,MPI_INT,i,0,MPI_COMM_WORLD,MPI_STATUS_IGNORE);
			localprimes.resize(size);
			MPI_Recv(&localprimes[0],size,MPI_LONG_LONG,i,0,MPI_COMM_WORLD,MPI_STATUS_IGNORE);
			primes.insert(primes.end(),localprimes.begin(),localprimes.end());
		}
	}
	else
	{
		size = localprimes.size();
		MPI_Send(&size,1,MPI_INT,0,0,MPI_COMM_WORLD);
		MPI_Send(&localprimes[0],size,MPI_LONG_LONG,0,0,MPI_COMM_WORLD);
	}

	// time
	if(rank==0)
	{
		/*clock_gettime(CLOCK_REALTIME,&timer);
		end = timer.tv_sec;*/
		end = MPI_Wtime();
		cout<<"time: "<<end-start<<endl;
	}
	// Printing
	/*if(rank==0)
	{
		vector <long long int> :: iterator it;
		for(it = primes.begin();it!=primes.end();it++)
			cout<<*it<<endl;
	}*/
	MPI_Finalize();

	return 0;
}
