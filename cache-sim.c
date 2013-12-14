/*main function file*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>
#include <tgmath.h>
#include "cache-sim.h"

//test
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
int l1n;
int l2n;
int l3n;

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

//direct mapped cache FIFO


//main program
int main(int argc, char *argv[])
{

	/*parse command line arguments and print them*/

	/*check for valid input and save input parameters*/

	if (argc<2)
	{
		printf("invalid arguments.\n");
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
				printHelp();
				return 0;
			}
			l1size = atoi(argv[i+1]);
			i++;
		}
		else if (strcmp(argv[i], "-l2size")==0)
		{
			if (isdigit(*argv[i+1]) == 0)
			{
				printHelp();
				return 0;
			}
			l2size = atoi(argv[i+1]);
			i++;
		}
		else if (strcmp(argv[i], "-l3size")==0)
		{
			if (isdigit(*argv[i+1]) == 0)
			{
				printHelp();
				return 0;
			}
			l3size = atoi(argv[i+1]);
			i++;
		}
		
		//save associativity setting
		if (debug==1){printf("%s","here1\n");}
		else if (strcmp(argv[i], "-l1assoc")==0)
		{
			if (debug==1){printf("%s","here2\n");}
			int flag = 0;
			char * temp = 0;
			if (strcmp(argv[i+1], "direct")!=0)
			{
				if (strcmp(argv[i+1], "assoc")!=0)
				{
					if (strncmp(argv[i+1],"assoc:",5)!=0)
					{
						printHelp();
						return 0;
					}
					else
					{
						if (debug==1){printf("%s","here3\n");}
						char news[3];
						for (int j = 0; j<strlen(argv[i+1]);j++)
						{
							fgets(temp, 1, (FILE*)argv[i+1]);
							printf("%s",temp);
							if (strcmp(temp,":")==0)
							{
								flag = 1;
							}
							if (flag==1)
							{
								strcat(news,temp);
							}
						}
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
				strcpy(l1assoc,"assoc:");
			}
			i++;
		}
		else if (strcmp(argv[i], "-l2assoc")==0)
		{
			if (strcmp(argv[i+1], "direct")!=0)
			{
				if (strcmp(argv[i+1], "assoc")!=0)
				{
					if (strcmp(argv[i+1], "assoc:n")!=0)
					{	
						printHelp();
						return 0;
					}
				}
			}
			strcpy(l2assoc,argv[i+1]);
			i++;
		}	
		else if (strcmp(argv[i], "-l3assoc")==0)
		{
			if (strcmp(argv[i+1], "direct")!=0)
			{
				if (strcmp(argv[i+1], "assoc")!=0)
				{
					if (strcmp(argv[i+1], "assoc:n")!=0)
					{	
						printHelp();
						return 0;
					}
				}
			}
			strcpy(l3assoc,argv[i+1]);
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
			if (debug == 1)
			{
				printf("filename: %s",argv[i]);
			}
			trace = fopen(argv[i], "r");
		}	 
	}
	

	//print debug values
	if(debug==1)
	{
		printf("l1size: %d\n",l1size);
		printf("l2size: %d\n",l2size);
		printf("l3size: %d\n",l3size);
		printf("l1assoc: %s\n",l1assoc);
		printf("l2assoc: %s\n",l2assoc);
		printf("l3assoc: %s\n",l3assoc);
		printf("blocksize: %d\n",blocksize);
		printf("replacealg: %s\n",replacealg);
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

	if (strcmp(l1assoc,"direct")==0)
	{
		b1bits = llog2(blocksize);
		s1bits = blocksize;
	}
	else if (strcmp(l1assoc,"assoc")==0)
	{
		b1bits = llog2(blocksize);
		s1bits = llog2(1);
	}


	char currAddr[15];
	while (1)
	{
		fscanf(trace, "%s", currAddr); 		//read an address
		if (feof(trace)) 	//end of file
		{
			break;
		}
	}




return 0;
}
		
