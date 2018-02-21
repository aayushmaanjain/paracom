#include <iostream>
#include <map>
#include <utility>
#include <bits/stdc++.h>
#include <string>
#include <vector>
#include <mpi.h>
#include <unistd.h>

using namespace std;

#define SSTR( x ) static_cast< std::ostringstream & >( \
        ( std::ostringstream() << std::dec << x ) ).str()

char *getstring(string str)
{
	return (char *)str.c_str();
}

vector <pair <string,int> >create_vector(string doc, int freq)
{
	vector <pair <string, int> > v;
	v.push_back(make_pair(doc,freq));
	return v;
}

int main(int argc, char *argv[])
{
	int rank, numproc;

	int i;
	int tag;
	int recvdone;
	int senddone;
	int rem_merge;

	char **recvbuf;
	char **sendbuf;
	char *tempbuf;

	unsigned long int *sendsizes;
	unsigned long int *recvsizes;
	int *sendind;	// stores indices corresponding to nodes to which data has been sent
	int *recvind;	// stores indices corresponding to nodes which have received data
	
	map <string, vector <pair <string,int> > > localindex;	// this should probably be a vector
	map <string, vector <pair <string,int> > > globalindex;

	
	MPI_Init(&argc,&argv);
	MPI_Comm_rank(MPI_COMM_WORLD,&rank);
	MPI_Comm_size(MPI_COMM_WORLD,&numproc);

	MPI_Request sendrequest[numproc];
	MPI_Request sendrequest1[numproc];
	MPI_Request recvrequest[numproc];
	MPI_Request recvrequest1[numproc];
	MPI_Status status[numproc];

	sendsizes = (unsigned long int *)malloc(numproc*sizeof(unsigned long int));
	sendsizes[rank] = 1;
	recvsizes = (unsigned long int *)malloc(numproc*sizeof(unsigned long int));
	recvsizes[rank] = 1;
	recvind = (int *)malloc((numproc)*sizeof(int));
	sendind = (int *)malloc((numproc)*sizeof(int));

	// for(i=0;i<numproc;i++)
	// 	recvind[i]=-1;

	// // replace this with building of local indices
	// if(rank==0)
	// {
	// 	localindex.insert(make_pair("hello",create_vector("doc1",10)));
	// 	// localindex["hello"].push_back(make_pair("doc1",10));
	// 	// localindex.insert(make_pair("aayush",{make_pair("doc1",20)}));
	// 	localindex["hello"].push_back(make_pair("doc2",5));
	// 	localindex.insert(make_pair("aayush",create_vector("doc1",20)));
	// }
	// else
	// {
	// 	localindex.insert(make_pair("aayush",create_vector("doc1",20)));
	// 	localindex["aayush"].push_back(make_pair("doc2",15));
	// 	// localindex.insert(make_pair("aayush",create_vector("doc3",13)));	// didn't work
	// 	localindex["aayush"].push_back(make_pair("doc3",12));
	// }

	// after computing local index
	recvbuf = (char **)malloc(numproc*sizeof(char *));
	if(recvbuf==NULL)
		cout<<"recvbuf failed\n"<<flush;
	sendbuf = (char **)malloc(numproc*sizeof(char *));
	if(sendbuf==NULL)
		cout<<"sendbuf failed\n"<<flush;
	// initialize all pointers to NULL
	for(i=0;i<numproc;i++)	{
		recvbuf[i] = NULL;
		sendbuf[i] = NULL;
		
	}
	// make all receive size requests
	for(i=0;i<numproc;i++)
		if(rank!=i)	{
			tag = i*100 + rank;
			MPI_Irecv(&recvsizes[i],1,MPI_UNSIGNED_LONG,i,tag,MPI_COMM_WORLD,&recvrequest[i]);
		}
	// send size 
	for(i=0;i<numproc;i++)	{
		if(i!=rank)	{
			tag = rank * 100 + i;
			// serialize that subset of the map
			
			// based on serialization result - allocate space
			sendsizes[i] = 100000+i;	// get size of serialized strength;
			MPI_Isend(&sendsizes[i],1,MPI_UNSIGNED_LONG,i,tag,MPI_COMM_WORLD,&sendrequest[i]);
			sendbuf[i] = (char *)malloc(sendsizes[i]*sizeof(char));
			if(sendbuf[i]==NULL)
				cout<<"memory error\n"<<flush;
			string s("HELLO");
			if(rank==0 && i==3)
			strcpy(sendbuf[i],s.c_str());	// copy serialized stream to sendbuf[i]
			// populate it with serialized string
			// strcat(sendbuf[i],getstring("Hello"));	// revertss
		}
		else	{
			sendbuf[i] = (char *)malloc(sendsizes[i]*sizeof(char));
			string s = "";
			strcpy(sendbuf[i],s.c_str());
		}
	
	}
	// if(rank==0)
	// cout<<"send buf: "<<sendbuf[3]<<endl;
	// wait on all sized being received.
	for(i=0;i<numproc;i++)
		if(rank!=i)	{
			MPI_Wait(&recvrequest[i],MPI_STATUS_IGNORE);
			// MPI_Wait(&sendrequest[i],MPI_STATUS_IGNORE);	// optimize => comment

		}
	// for(i=0;i<numproc;i++)
		// cout<<rank<<" : "<<i<<" - recv: "<<recvsizes[i]<<" send: "<<sendsizes[i]<<endl;
	// make all receive requests for serialized stream
	for(i=0;i<numproc;i++)	{
		tag = 100000 + i*100 + rank;
		recvbuf[i] = (char *)malloc(recvsizes[i]*sizeof(char));
		MPI_Irecv(&recvbuf[i][0], recvsizes[i], MPI_CHAR, i, tag, MPI_COMM_WORLD, &recvrequest1[i]);		
		if(rank!=i)	{
			MPI_Wait(&sendrequest[i], MPI_STATUS_IGNORE);	// keep
		}
	}
	free(recvsizes);

	// send serialized streams
	for(i=0;i<numproc;i++)	{
		tag = 100000 + rank * 100 + i;	// div and mod 100 to extract
		MPI_Isend(&sendbuf[i][0], sendsizes[i], MPI_CHAR, i, tag, MPI_COMM_WORLD, &sendrequest1[i]);
	}
	free(sendsizes);
	
	// wait on all serialized streams being received.
	// MPI_Testsome(numproc,recvrequest1,&recvdone,recvind,status);
	// cout<<rank<<" received: "<<recvdone<<endl;
	// for(i=0;i<recvdone;i++)
	// 	cout<<rank<<" : "<<recvind[i]<<"\t";
	// cout<<endl;
	rem_merge = numproc;
	while(rem_merge>0)	{
		// free memory from send buffer if send done
		MPI_Testsome(numproc, sendrequest1, &senddone, sendind, status);
		if(senddone > 0)	{
			for(i=0;i<senddone;i++)	{
				free(sendbuf[sendind[i]]);
				cout<<rank<<" sent: "<<sendind[i]<<"\t"<<flush;
			}
			cout<<endl;
		}
		// cout<<"hello\n"<<flush;
		MPI_Testsome(numproc, recvrequest1, &recvdone, recvind, status);
		if(recvdone > 0)	{
			cout<<rank<<" Received: "<<recvdone<<endl<<flush;
			for(i=0;i<recvdone;i++)	{
				cout<<rank<<" : "<<recvind[i]<<"\t"<<flush;
				// if(recvind[i]!=rank)	{
				// 	//deserialize

				// 	//insert into global index

				// }
				// // free up memory from receive buffer if receive done and added to global list
				// // DO : could move it up but then take care of the recvbuf[i]
				free(recvbuf[recvind[i]]);
			}
			cout<<endl;
			rem_merge -= recvdone;
		}
	}
	
	// cout<<"received: "<<recvdone<<endl<<"receivedlist: ";
	// for(i=0;i<numproc;i++)
	// 	cout<<recvind[i]<<"\t";
	// cout<<endl;

	// DO : figure out best location to place these two calls
	for(i=0;i<numproc;i++)
		if(rank!=i)	{
			MPI_Wait(&recvrequest1[i],MPI_STATUS_IGNORE);
			MPI_Wait(&sendrequest1[i],MPI_STATUS_IGNORE);	// optimize => comment
		}

// if(rank==3)
	// cout<<rank<<" - recvbuf: "<<recvbuf[0]<<endl;
	// cout<<"SELF - Send: "<<sendbuf[rank]<<" Receive: "<<recvbuf[rank]<<endl;

	// i will map to rank-1

	// // Printing
	// // cout<<localindex["aayush"][0].first<<endl;
	// map <string, vector <pair <string,int> > > :: iterator mapitr;
	// vector <pair <string, int> > :: iterator vecitr;
	// for(mapitr = localindex.begin();mapitr!=localindex.end();mapitr++)
	// 	// cout<<itr->first<<" : "<<itr->second.first<<" : "<<itr->second.second<<endl; 
	// 	for(vecitr=mapitr->second.begin();vecitr!=mapitr->second.end();vecitr++)
	// 		cout<<mapitr->first<<" : "<<vecitr->first<<" : "<<vecitr->second<<endl;
	
	// free(recvsizes);
	// free(sendsizes);
	//free up memory
	free(sendbuf);
	free(recvbuf);

	MPI_Finalize();
	return 0;
}