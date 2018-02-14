#include <mpi.h>
#include <cmath>

int main(int argc, char* argv[]){
	long long int N;
	int p;

	cout << "Enter N : ";
	cin >> N;

	int sqrtN = (int)sqrt(N);

	bool A[sqrtN] = {0};

	MPI_Init(&argc, &argv);

	return 0;
}
