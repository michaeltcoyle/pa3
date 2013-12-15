/*main function file*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>
#include <tgmath.h>
#include "cache-sim.h"

//this version successfully parses command line


//debug
int debug = 1;

//global variables
int l1size;
int l2size;
int l3size;
int blocksize;
char l1assoc[8];
char l2assoc[8];
char l3assoc[8];
char replacealg[4];
int l1n = -1;
int l2n = -1;
int l3n = -1;

FILE *trace;

//helper functions

void printHelp()
{
	puts("usage: cache-sim [-h] -l1size <L1 size> -l1assoc <L1 assoc> -l2size <L2 size> -l2assoc <L2 assoc> -l3size <L3 size> -l3assoc <L3 assoc> <block size> <replace alg> <trace file> \n");
}
//string compare ignoring case
int strcicmp(char const *a, char const *b)
{
    for (;; a++, b++) {
        int d = tolower(*a) - tolower(*b);
        if (d != 0 || !*a)
            return d;
    }
}
// Calculates log2 of number.  
int llog2(int n)  
{  
    // log(n)/log(2) is log2.  
    return log10(n)/log10(2);  
}




//main program
int main(int argc, char *argv[])
{
	char filename[strlen(argv[argc-1])];
	/*parse command line arguments and print them*/

	/*check for valid input and save input parameters*/

	if (argc<2)
	{
		printf("ERROR: invalid arguments.\n");
		return 0;
	}
	for (int i=1; i<argc; i++)
	{
		if (strcmp(argv[i],"-h")==0)
		{
			printHelp();
			return 0;
		}
		else if (strcmp(argv[i], "-l1size")==0)
		{
			if (isdigit(*argv[i+1]) == 0)
			{
				printf("ERROR: size arguments must be a power of 2.\n");
				return 0;
			}
			l1size = atoi(argv[i+1]);
			i++;
		}
		else if (strcmp(argv[i], "-l2size")==0)
		{
			if (isdigit(*argv[i+1]) == 0)
			{
				printf("ERROR: size arguments must be a power of 2.\n");
				return 0;
			}
			l2size = atoi(argv[i+1]);
			i++;
		}
		else if (strcmp(argv[i], "-l3size")==0)
		{
			if (isdigit(*argv[i+1]) == 0)
			{
				printf("ERROR: size arguments must be a power of 2.\n");
				return 0;
			}
			l3size = atoi(argv[i+1]);
			i++;
		}
		else if (strcmp(argv[i], "-l1assoc")==0)
		{
			int flag = 0;
			if (strcmp(argv[i+1], "direct")!=0)
			{
				if (strcmp(argv[i+1], "assoc")!=0)
				{
					if (strncmp(argv[i+1],"assoc:",6)!=0)
					{
						printf("ERROR: associativity must be either 'direct, assoc, or assoc:n' where n is the set size.\n");
						return 0;
					}
					else
					{
						flag = 1;
						char news[1];
						memcpy(news,&(argv[i+1])[6],1);
						l1n = atoi(news);
					}
				}
			}
			if (flag==0)
			{
				strcpy(l1assoc,argv[i+1]);
			}
			else
			{
				strcpy(l1assoc,"assoc:n");
			}
			i++;
		}
		else if (strcmp(argv[i], "-l2assoc")==0)
		{
			int flag = 0;
			if (strcmp(argv[i+1], "direct")!=0)
			{
				if (strcmp(argv[i+1], "assoc")!=0)
				{
					if (strncmp(argv[i+1],"assoc:",6)!=0)
					{
						printf("ERROR: associativity must be either 'direct, assoc, or assoc:n' where n is the set size.\n");
						return 0;
					}
					else
					{
						flag = 1;
						char news[1];
						memcpy(news,&(argv[i+1])[6],1);
						l2n = atoi(news);
					}
				}
			}
			if (flag==0)
			{
				strcpy(l2assoc,argv[i+1]);
			}
			else
			{
				strcpy(l2assoc,"assoc:n");
			}
			i++;
		}	
		else if (strcmp(argv[i], "-l3assoc")==0)
		{
			int flag = 0;
			if (strcmp(argv[i+1], "direct")!=0)
			{
				if (strcmp(argv[i+1], "assoc")!=0)
				{
					if (strncmp(argv[i+1],"assoc:",6)!=0)
					{
						printf("ERROR: associativity must be either 'direct, assoc, or assoc:n' where n is the set size.\n");
						return 0;
					}
					else
					{
						flag = 1;
						char news[1];
						memcpy(news,&(argv[i+1])[6],1);
						l3n = atoi(news);
					}
				}
			}
			if (flag==0)
			{
				strcpy(l3assoc,argv[i+1]);
			}
			else
			{
				strcpy(l3assoc,"assoc:n");
			}
			i++;
		}
		else if (i == argc-3)
		{
			if (isdigit(*argv[i]) == 0)
			{
				printHelp();
				return 0;
			}
			blocksize = atoi(argv[i]);
		}	
		else if (i == argc-2)
		{
			if ( (strcicmp(argv[i],"FIFO")==0) || (strcicmp(argv[i],"LRU")==0) )
			{
				strcpy(replacealg,argv[i]);
			}
			else
			{
				printHelp();
				return 0;	
			}
		}
		else if (i == argc-1)
		{
			strcpy(filename,argv[i]);
			trace = fopen(argv[i], "r");
		}	 
	}
	



	//begin simulation

	//find tag,set index,block offset bit sizes

	int b1bits;
	int s1bits;
	int t1bits;
	int b2bits;
	int s2bits;
	int t2bits;
	int b3bits;
	int s3bits;
	int t3bits;

//l1 assoc 

	if (strcmp(l1assoc,"direct")==0)
	{
		b1bits = llog2(blocksize);
		s1bits = 1;
	}
	else if (strcmp(l1assoc,"assoc")==0)
	{
		b1bits = llog2(blocksize);
		s1bits = llog2(1);
	}
	else if (strcmp(l1assoc,"assoc:n")==0)
	{
		b1bits = llog2(blocksize);
		s1bits = l1n;
	}

//l2 assoc 

	if (strcmp(l2assoc,"direct")==0)
	{
		b2bits = llog2(blocksize);
		s2bits = 1;
	}
	else if (strcmp(l2assoc,"assoc")==0)
	{
		b2bits = llog2(blocksize);
		s2bits = llog2(1);
	}
	else if (strcmp(l2assoc,"assoc:n")==0)
	{
		b2bits = llog2(blocksize);
		s2bits = l2n;
	}

//l3 assoc 

	if (strcmp(l3assoc,"direct")==0)
	{
		b3bits = llog2(blocksize);
		s3bits = 1;
	}
	else if (strcmp(l3assoc,"assoc")==0)
	{
		b3bits = llog2(blocksize);
		s3bits = llog2(1);
	}
	else if (strcmp(l3assoc,"assoc:n")==0)
	{
		b3bits = llog2(blocksize);
		s3bits = l3n;
	}

	int getlength = 1;
	int addrlength;
	char currAddr[15];
	printf("here");
	while (1)
	{

		fscanf(trace, "%s", currAddr); 		//read an address
		if (getlength == 1)
		{
			addrlength = strlen(currAddr);
			getlength = 0;
			t1bits = addrlength-(b1bits+s1bits+2);
			t2bits = addrlength-(b2bits+s2bits+2);
			t3bits = addrlength-(b3bits+s3bits+2);
		}
		if (feof(trace)) 	//end of file
		{
			break;
		}
		
	}


	//print debug values
#ifdef DEBUG

		printf("l1size: %d\n",l1size);
		printf("l2size: %d\n",l2size);
		printf("l3size: %d\n",l3size);
		printf("l1assoc: %s\n",l1assoc);
		printf("l2assoc: %s\n",l2assoc);
		printf("l3assoc: %s\n",l3assoc);
		printf("blocksize: %d\n",blocksize);
		printf("replacealg: %s\n",replacealg);
		printf("filename: %s\n",filename);
		if (l1n!=-1){printf("l1n: %d\n",l1n);}
		if (l2n!=-1){printf("l2n: %d\n",l2n);}
		if (l3n!=-1){printf("l3n: %d\n",l3n);}
		printf("b1bits: %s",b1bits);
		printf("s1bits: %s",s1bits);
		printf("t1bits: %s",t1bits);
		printf("b2bits: %s",b2bits);
		printf("s2bits: %s",s2bits);
		printf("t2bits: %s",t2bits);
		printf("b3bits: %s",b3bits);
		printf("s3bits: %s",s3bits);
		printf("t3bits: %s",t3bits);
		
#endif

return 0;
}
		
