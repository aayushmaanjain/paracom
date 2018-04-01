#include <omp.h>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>

using namespace std;

int *search(int **mat, int n, int m, int num, int i, int j, int p, int k, int done)
{
   // printf("Search - n=%d m=%d i=%d j=%d p=%d k=%d\n",n,m,i,j,p,k);
   /*int i = 0, j = n-1;  //set indexes for top right element
   while ( i < n && j >= 0 )
   {
      if ( mat[i][j] == x )
      {
         printf("%d Found at %d, %d\n", n, i, j);
         return 1;
      }
      if ( mat[i][j] > x )
        j--;
      else //  if mat[i][j] < x
        i++;
   }
 
   printf("%d Element not found\n",n);
   return 0;  // if ( i==n || j== -1 )
   */
	int *result = (int *)malloc(2*sizeof(int));
	int r , c;
	for(c=j; (c>(j-p+k)) && (c>=0); c--)
		// printf("%d %d %d\n",i,c,mat[i][c]);
		if(mat[i][c]==num)	// found
		{
			// printf("found: %d %d %d\n",i,c,mat[i][c]);
			// #pragma omp atomic
			// done = 1;
			result[0] = i;
			result[1] = c;
			return result;
		}
	// cout<<endl;
	c++;
	for(r=i+1; r<n && r<= (i+k); r++)
		// printf("%d %d %d\n",r,c,mat[r][c]);
 		if(mat[r][c]==num)	// found
		{
			// printf("found: %d %d %d\n",r,c,mat[r][c]);
			// #pragma omp atomic
			// done = 1;
			result[0] = r;
			result[1] = c;
			return result;
		}
 	// cout<<endl;
 	r--;
 	result[0] = -1;		// denotes not equal
 	if(num<mat[r][c])
 		result[1] = 0;	// denotes smaller than diagonal value
 	
 	else
 		result[1] = 1;	// denotes larger than diagonal value
	// printf("%d %d\n",r,c);
 	return result;
}
int main()
{
	int n = 4, m = 4, key = 33;
	cout<<"n: ";
	cin>>n;
	cout<<"m: ";
	cin>>m;
	cout<<"key: ";
	cin>>key;
	int id;
	int i = 0, j = m-1;
	int done=0,fx,fy;
	/*int mat[4][4] = { {10, 20, 30, 40},
                    {15, 25, 35, 45},
                    {27, 29, 37, 48},
                    {32, 33, 39, 50},
                  };*/
	int **mat = (int **)malloc(n*sizeof(int *));
	for(int a=0; a<n; a++)
		mat[a] = (int *)malloc(m*sizeof(int));
	int count = 0;
	for(int a = 0; a < n; a++)
		for(int b = 0; b < m; b++)
			mat[a][b] = count++;
	/*for(int a = 0; a < n; a++)
	{
		for(int b = 0; b < m; b++)
			cout<<mat[a][b]<<" ";
		cout<<endl;
	}*/
    int *result;
    // int *result = search(mat,4,4,37,2,3,4,3,done);
    // cout<<result[0]<<" "<<result[1]<<endl;
	int numthreads, maxthreads = 4;
	cout<<"#threads: ";
	cin>>maxthreads;
	omp_set_num_threads(maxthreads);
	int *evalthread = (int *)calloc(maxthreads,sizeof(int));
	double start = omp_get_wtime();
	do
	{
		// printf("i: %d\tj: %d\n",i,j);
		 // search(mat, 4, 29);
		#pragma omp parallel private(id,result) shared(i,j,done,fx,fy,evalthread)		// DO: thread creation overhead
		{
			numthreads = omp_get_num_threads();
			// cout<<"numthreads: "<<numthreads<<endl;
			// evalthread = (int *)malloc(numthreads*sizeof(int));
			id = omp_get_thread_num();
			int *result = search(mat,n,m,key,i,j,numthreads,id,done);
	    	// cout<<id<<" : "<<result[0]<<" "<<result[1]<<endl;
	    	if(result[0]>-1)
	    	{
	    		#pragma omp critical
	    		{
	    			done = 1;
	    			fx = result[0];
	    			fy = result[1];	
	    		}
			}
			else	// master reevaluates (i,j)
			{
				evalthread[id] = result[1];
			}
		}
		// for(int k=0;k<numthreads;k++)
		// 	cout<<evalthread[k]<<"\t";
		// cout<<endl;
		int k = 0;
		while(evalthread[k]==1)
			k=k+1;
		// cout<<"zero at: "<<k<<endl;
		i = i + k;
		j = j - numthreads + k;
	}while(i<n && j>=0 && done==0);		// DO: figure out how to end all threads as soon as one thread finds the key
	double end = omp_get_wtime();
	if(done==1)
		printf("%d found at: (%d, %d)\n",key,fx,fy);
	else
		printf("%d not found\n",key);
	cout<<"Time: "<<end-start<<endl;
	return 0;
}