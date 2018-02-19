#include <mpi.h>
#include <cmath>
#include <time.h>
#include <iostream>
#include <cstdlib>


using namespace std;

int main(int argc, char* argv[]){
	//int *ptr = (int *)malloc(10000000000*sizeof(int));

	long long int N;
	int p;

	cout << "Enter N : ";
	cin >> N;
	timespec timer;
	clock_gettime(CLOCK_REALTIME,&timer);
	double start = timer.tv_nsec;
	int sqrtN = (int)sqrt(N);

	char *A = (char*)calloc((sqrtN+1),sizeof(char));	// 0 -> prime 
	A[0]=1;A[1]=1;
	int rtrtN = (int)sqrt(sqrtN);
	int i,j;
	for (i=2;i<=rtrtN;i++)
	{
		if(A[i]==0)
		{
			for(j=i*i;j<=sqrtN;j+=i)
				A[j] = 1;
		}
		
	}
	clock_gettime(CLOCK_REALTIME,&timer);
	double end = timer.tv_nsec;
	//for(int i=1;i<=sqrtN;i++)
		//cout<<i<<" "<<A[i]<<endl;
	cout<<end-start<<endl;
	//MPI_Init(&argc, &argv);
	

	return 0;
}
