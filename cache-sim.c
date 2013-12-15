/*main function file*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>
#include <tgmath.h>
#include "cache-sim.h"

//this version successfully parses command line (and probably gets correct bits (t,s,b)

struct Row {

	int first;
	int block;
	int valid;
	int tag;

};

//initialize structs / constructor type stuff

struct Set {

	int nrows;
	struct Row *rows;

};

struct Cache {

	int nrows;
	struct Set *sets;
	int miss;
	int hit;
	int coldmiss;
	int confmiss;
	int capmiss;

};


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
int memaccesses = 0;

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

//hex to binary

void HexToBin(char hex_number, char* bit_number) 
{
	int max = 128;
        for(int i = 63 ; i >-1 ; i--)
        {
            bit_number [i] = (hex_number & max ) ? 1 : 0;
            max >>=1;
        }
}


//main program
int main(int argc, char *argv[])
{
	char filename[strlen(argv[argc-1])];
	/*parse command line arguments and print them*/

	/*check for valid input and save input parameters*/

	if (argc<2)
	{
		fprintf(stderr,"ERROR: invalid arguments.\n");
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
				fprintf(stderr,"ERROR: size arguments must be a power of 2.\n");
				return 0;
			}
			l1size = atoi(argv[i+1]);
			i++;
		}
		else if (strcmp(argv[i], "-l2size")==0)
		{
			if (isdigit(*argv[i+1]) == 0)
			{
				fprintf(stderr,"ERROR: size arguments must be a power of 2.\n");
				return 0;
			}
			l2size = atoi(argv[i+1]);
			i++;
		}
		else if (strcmp(argv[i], "-l3size")==0)
		{
			if (isdigit(*argv[i+1]) == 0)
			{
				fprintf(stderr,"ERROR: size arguments must be a power of 2.\n");
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
						fprintf(stderr,"ERROR: associativity must be either 'direct, assoc, or assoc:n' where n is the set size.\n");
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
						fprintf(stderr,"ERROR: associativity must be either 'direct, assoc, or assoc:n' where n is the set size.\n");
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
						fprintf(stderr,"ERROR: associativity must be either 'direct, assoc, or assoc:n' where n is the set size.\n");
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

	long tag1v;
	long set1v;
	long block1v;
	long tag2v;
	long set2v;
	long block2v;
	long tag3v;
	long set3v;
	long block3v;


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

//t bits

	t1bits = 16-(b1bits+s1bits);
	t2bits = 16-(b2bits+s2bits);
	t3bits = 16-(b3bits+s3bits);


	char tag1[t1bits];
	char set1[s1bits];
	char block1[b1bits];
	char tag2[t2bits];
	char set2[s2bits];
	char block2[b2bits];
	char tag3[t3bits];
	char set3[s3bits];
	char block3[b3bits];
	

	int setsize1 = pow(16,s1bits);
	int setsize2 = pow(16,s2bits);
	int setsize3 = pow(16,s3bits);

	

	struct Cache *l1cache = malloc(sizeof(struct Cache));
	l1cache->sets = malloc((l1size * sizeof(struct Set)));

	l1cache->nrows = l1size/setsize1;
	
	struct Cache *l2cache = malloc(sizeof(struct Cache));
	l2cache->sets = malloc((l2size * sizeof(struct Set)));

	l2cache->nrows = l2size/setsize2;

	struct Cache *l3cache = malloc(sizeof(struct Cache));
	l3cache->sets = malloc((l3size * sizeof(struct Set)));

	l3cache->nrows = l3size/setsize3;

	struct Set *l1set = malloc(sizeof(struct Set));
	l1set->rows = malloc(((l1size/setsize1) * sizeof(struct Row)));

	l1set->nrows = setsize1;

	struct Set *l2set = malloc(sizeof(struct Cache));
	l2set->rows = malloc(((l2size/setsize2) * sizeof(struct Row)));

	l2set->nrows = setsize2;

	struct Set *l3set = malloc(sizeof(struct Cache));
	l3set->rows = malloc(((l3size/setsize3) * sizeof(struct Row)));

	l3set->nrows = setsize3;
	
	int addrlength;
	char currAddr[17];


	l1cache->miss = 0;
	l1cache->hit = 0;
	l1cache->coldmiss = 0;
	l1cache->confmiss = 0;
	l1cache->capmiss = 0;
	
	l2cache->miss = 0;
	l2cache->hit = 0;
	l2cache->coldmiss = 0;
	l2cache->confmiss = 0;
	l2cache->capmiss = 0;

	l3cache->miss = 0;
	l3cache->hit = 0;
	l3cache->coldmiss = 0;
	l3cache->confmiss = 0;
	l3cache->capmiss = 0;


	//start reading addresses
	
	while (1)
	{
		fscanf(trace, "%s", currAddr); 		//read an address

		addrlength = strlen(currAddr)-2;



		
		if (feof(trace)) 	//end of file
		{
			break;
		}
		if (strcmp(currAddr,"#eof")==0)
		{
			break;
		}

		//count mem access
		
		memaccesses++;
		
		//append leading 0s
		
		char tempAddr[15] = "";
		char tempStr[addrlength];
		memcpy(tempStr,&currAddr[2],addrlength);
		tempStr[strlen(tempStr)]=0;

		switch (addrlength)
		{

			case 16:
				break;
			case 15:
				strcat(tempAddr,"0");
				break;
			case 14:
				strcat(tempAddr,"00");
				break;
			case 13: 
				strcat(tempAddr,"000");
				break;
			case 12:
				strcat(tempAddr,"0000");
				break;
			case 11:
				strcat(tempAddr,"00000");
				break;
			case 10:
				strcat(tempAddr,"000000");
				break;
			case 9:
				strcat(tempAddr,"0000000");
				break;
			case 8:
				strcat(tempAddr,"00000000");
				break;
			case 7:
				strcat(tempAddr,"000000000");
				break;
			case 6:
				strcat(tempAddr,"0000000000");
				break;
			case 5:
				strcat(tempAddr,"00000000000");
				break;
			case 4:
				strcat(tempAddr,"000000000000");
				break;
			case 3:
				strcat(tempAddr,"0000000000000");
				break;
			case 2:
				strcat(tempAddr,"00000000000000");
				break;
			case 1:
				strcat(tempAddr,"000000000000000");
				break;
			case 0:
				strcat(tempAddr,"0000000000000000");
				break;
			default:
				break;
				
		}


		
		strcat(tempAddr,tempStr);
		strcpy(currAddr,tempAddr);


		//setup tag, set, and block information.
		char *u;


		memcpy(tag1,&currAddr[0],t1bits);
		memcpy(set1,&currAddr[t1bits],s1bits);
		memcpy(block1,&currAddr[s1bits+t1bits],b1bits);
		
		tag1[t1bits]=0;
		set1[s1bits]=0;
		block1[b1bits]=0;

		tag1v = strtol(tag1,&u,16);
		set1v = strtol(set1,&u,16);
		block1v = strtol(block1,&u,16);
		
		
		memcpy(tag2,&currAddr[0],t2bits);
		memcpy(set2,&currAddr[t2bits],s2bits);
		memcpy(block2,&currAddr[s2bits+t2bits],b2bits);
		
		tag2[t2bits]=0;
		set2[s2bits]=0;
		block2[b2bits]=0;

		tag2v = strtol(tag2,&u,16);
		set2v = strtol(set2,&u,16);
		block2v = strtol(block2,&u,16);


		memcpy(tag3,&currAddr[0],t3bits);
		memcpy(set3,&currAddr[t3bits],s3bits);
		memcpy(block3,&currAddr[s3bits+t3bits],b3bits);

		tag3[t3bits]=0;
		set3[s3bits]=0;
		block3[b3bits]=0;

		tag3v = strtol(tag3,&u,16);
		set3v = strtol(set3,&u,16);
		block3v = strtol(block3,&u,16);
		

		//implementation


		
		int l1pass = 0;
		int l2pass = 0;
		int l3pass = 0;

		

		
		if ((l1pass == 0) && (strcmp(l1assoc,"direct")==0))
		{
		
			l1pass = 0;
			struct Set workingSet = l1cache[set1v];
			for (int i = 0; i<workingSet[i]->nrows; i++)
			{
				if (workingSet[i]->first!=1 && workingSet[i]->first!=0)
				{
					workingSet[i]->first=1;
				}
				if (workingSet[i]->valid!=1 && workingSet[i]->valid!=0)
				{
					workingSet[i]->valid=0;
				}
				if (workingSet->nrows>l1size)
				{
					l1cache->capmiss++;
					break;
				}
				else if (strcmp(workingSet[i]->tag,tag1)==0)
				{
					if (workingSet[i]->valid==1 && strcmp(workingSet[i]->block,block1)==0)
					{
						l1cache->hit++;
						l1pass = 1;
						workingSet[i]->first = 0;
						break;
					}
					else if (workingSet[i]->valid==1 && workingSet[i]->first == 0)
					{
						l1cache->confmiss++
						break;
					}
					else
					{	
						l1cache->coldmiss++;
						strcpy(workingSet[i]->block,block1);
						workingSet[i]->first = 0;
						break;
					}
				}
				else
				{
					continue;
				}
			}
		}
		if ((l1pass = 0) && (l2pass == 0) && (strcmp(l2assoc,"direct")==0))
		{
			l2pass = 0;
			struct workingSet = l2cache[set2v];
			for (int i=0;i<workingSet[i]->nrows;i++)
			{
				if (workingSet[i]->first!=1 && workingSet[i]->first!=0)
				{
					workingSet[i]->first=1;
				}
				if (workingSet[i]->valid!=1 && workingSet[i]->valid!=0)
				{
					workingSet[i]->valid=0;
				}
				if (workingSet->nrows>l2size)
				{
					l2cache->capmiss++;
					break;
				}
				else if (strcmp(workingSet[i]->tag,tag2)==0)
				{
					if (workingSet[i]->valid==1 && strcmp(workingSet[i]->block,block2)==0)
					{
						l2cache->hit++;
						l2pass = 1;
						workingSet[i]->first = 0;
						break;
					}
					else if (workingSet[i]->valid==1 && workingSet[i]->first == 0)
					{
						l2cache->confmiss++
						break;
					}
					else
					{	
						l2cache->coldmiss++;
						strcpy(workingSet[i]->block,block2);
						workingSet[i]->first = 0;
						break;
					}
				}
				else
				{
					continue;
				}
			}
		}
		if ((l2pass = 0) && (l2pass == 0) && (l3pass == 0) && (strcmp(l3assoc,"direct")==0))
		{
			l3pass = 0;
			struct workingSet = l3cache[set3v];
			for (int i=0;i<workingSet[i]->nrows;i++)
			{	
				if (workingSet[i]->first!=1 && workingSet[i]->first!=0)
				{
					workingSet[i]->first=1;
				}
				if (workingSet[i]->valid!=1 && workingSet[i]->valid!=0)
				{
					workingSet[i]->valid=0;
				}
				if (workingSet->nrows>l3size)
				{
					l3cache->capmiss++;
					break;
				}
				else if (strcmp(workingSet[i]->tag,tag3)==0)
				{
					if (workingSet[i]->valid==1 && strcmp(workingSet[i]->block,block3)==0)
					{
						l3cache->hit++;
						l3pass = 1;
						workingSet[i]->first = 0;
						break;
					}
					else if (workingSet[i]->valid==1 && workingSet[i]->first == 0)
					{
						l3cache->confmiss++
						break;
					}
					else
					{	
						l3cache->coldmiss++;
						strcpy(workingSet[i]->block,block3);
						workingSet[i]->first = 0;
						break;
					}
				}
				else
				{
					continue;
				}
			}
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
		printf("t1bits: %d\n",t1bits);
		printf("s1bits: %d\n",s1bits);
		printf("b1bits: %d\n",b1bits);
		printf("t2bits: %d\n",t2bits);
		printf("s2bits: %d\n",s2bits);
		printf("b2bits: %d\n",b2bits);
		printf("t3bits: %d\n",t3bits);
		printf("s3bits: %d\n",s3bits);
		printf("b3bits: %d\n",b3bits);
		printf("setsize1: %d\n",setsize1);
		printf("setsize2: %d\n",setsize2);
		printf("setsize3: %d\n",setsize3);
		printf("l1cache sets: %d\n",l1cache->nrows);
		printf("l2cache sets: %d\n",l2cache->nrows);
		printf("l3cache sets: %d\n",l3cache->nrows);
		printf("l1cache setsize: %d\n",l1set->nrows);
		printf("l2cache setsize: %d\n",l2set->nrows);
		printf("l3cache setsize: %d\n",l3set->nrows);
		printf("l1cache rows: %d\n",l1cache->nrows*l1set->nrows);
		printf("l2cache rows: %d\n",l2cache->nrows*l2set->nrows);
		printf("l3cache rows: %d\n",l3cache->nrows*l3set->nrows);
		printf("last address l1 tag: %s\n",tag1);
		printf("last address l1 set: %s\n",set1);
		printf("last address l1 block: %s\n",block1);
		printf("last address l2 tag: %s\n",tag2);
		printf("last address l2 set: %s\n",set2);
		printf("last address l2 block: %s\n",block2);
		printf("last address l3 tag: %s\n",tag3);
		printf("last address l3 set: %s\n",set3);
		printf("last address l3 block: %s\n",block3);
		printf("last address l1 tag dec: %ld\n",tag1v);
		printf("last address l1 set dec: %ld\n",set1v);
		printf("last address l1 block dec: %ld\n",block1v);
		printf("last address l2 tag dec: %ld\n",tag2v);
		printf("last address l2 set dec: %ld\n",set2v);
		printf("last address l2 block dec: %ld\n",block2v);
		printf("last address l3 tag dec: %ld\n",tag3v);
		printf("last address l3 set dec: %ld\n",set3v);
		printf("last address l3 block dec: %ld\n",block3v);
		printf("memory accesses: %d\n",memaccesses);
#endif


		l1cache->miss = l1cache->coldmiss+l1cache->confmiss+l1cache->capmiss;
		l2cache->miss = l2cache->coldmiss+l2cache->confmiss+l2cache->capmiss;
		l3cache->miss = l3cache->coldmiss+l3cache->confmiss+l3cache->capmiss;

return 0;
}
		
