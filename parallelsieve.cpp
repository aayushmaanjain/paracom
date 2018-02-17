#include <mpi.h>
#include <cmath>
#include <time.h>
#include <iostream>
#include <cstdlib>

using namespace std;

int main(int argc, char *argv[])
{
	long long int N;
	int sqrtN;

	int *prime_sqrtN;
	int *prime_prll;

	int lower;
	int upper;
	int dividesize;
	
	int rank;
	int numproc;

	MPI_Init(&argc,&argv);
	MPI_Comm_rank(MPI_COMM_WORLD,&rank);
	MPI_Comm_size(MPI_COMM_WORLD,&numproc);
	// Master gets program parameter N
	if(rank==0)
	{
		cout<<"Enter N: ";
		cin>>N;
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
	cout<<"Rank: "<<rank<<"Lower: "<<lower<<"Upper: "<<upper<<endl;

	prime_sqrtN = (int *)calloc((sqrtN+1),sizeof(int));
	prime_prll = (int *)calloc((upper-lower+1),sizeof(int));
	
	// Marking all prime till sqrt(N)
	int i,j;
	int sqrt_sqrtN = (int)sqrt(sqrtN);
	prime_sqrtN[0]=prime_sqrtN[1]=1;	// 0 represents prime, 1 - nonprime.
	for(i=2;i<=sqrt_sqrtN;i++)
		if(prime_sqrtN[i]==0)
			for(j=i*i;j<=sqrtN;j+=i)
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
				prime_prll[j-lower]=1;

		}

	// Printing
	// cout<<rank<<endl;
	if(rank==0)
		for(int i=1;i<sqrtN+1;i++)
			cout<<i<<": "<<prime_sqrtN[i]<<endl;
	for(int i=lower;i<=upper;i++)
		cout<<"rank: "<<rank<<"\t"<<i<<": "<<prime_prll[i-lower]<<endl;
	cout<<endl;

	MPI_Finalize();

	return 0;
}
