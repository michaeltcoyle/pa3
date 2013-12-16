/*main function file*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>
#include <tgmath.h>
#include "cache-sim.h"

//this version successfully parses command line (and probably gets correct bits (t,s,b)

typedef struct obRow {

	int first;
	int block;
	int valid;
	int tag;

} Row;


//initialize structs / constructor type stuff

typedef struct obSet {

	int nrows;
	Row *row;

} Set;

typedef struct obCache {

	int nsets;
	Set *set;
	int miss;
	int hit;
	int coldmiss;
	int confmiss;
	int capmiss;

} Cache;


Cache* NewCache(void) {
  Cache *lcache = malloc(sizeof(Cache)); 
  if (lcache){
  	lcache->nsets = 0;
  	lcache->miss = 0;
  	lcache->hit = 0;
  	lcache->coldmiss = 0;
  	lcache->confmiss = 0;
  	lcache->capmiss = 0;
  	lcache->set = NULL;
  }
  return lcache;
}

Row NewRow(void) {
  if (lrow){
  	lrow.first = 1;
  	lrow.block = 0;
  	lrow.valid = 0;
  	lrow.tag = 0;
  }
  return lrow;
 }

Set* NewSet(void) {
  Set *lset = malloc(sizeof(Set));
  if (lset){
  	lset->nrows = 0;
  	lset->row = NULL;
  }
  return lset;
 }

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


	Cache *l1cache = NewCache();
	Cache *l2cache = NewCache();
	Cache *l3cache = NewCache();
//l1 assoc 

	if (strcmp(l1assoc,"direct")==0)
	{
		b1bits = llog2(blocksize);
		s1bits = llog2(l1size/blocksize);
		Set *l1sets[l1size/blocksize];
		l1cache->nsets = 1;
	}
	else if (strcmp(l1assoc,"assoc")==0)
	{
		b1bits = llog2(blocksize);
		s1bits = llog2(1);
		
	}
	else if (strcmp(l1assoc,"assoc:n")==0)
	{
		b1bits = llog2(blocksize);
		s1bits = llog2(l1n);
	}

//l2 assoc 

	if (strcmp(l2assoc,"direct")==0)
	{
		b2bits = llog2(blocksize);
		s2bits = llog2(l2size/blocksize);
		Set *l2sets[l2size/blocksize];
		l2cache->nsets = 1;
	}
	else if (strcmp(l2assoc,"assoc")==0)
	{
		b2bits = llog2(blocksize);
		s2bits = llog2(1);
	}
	else if (strcmp(l2assoc,"assoc:n")==0)
	{
		b2bits = llog2(blocksize);
		s2bits = llog2(l2n);
	}

//l3 assoc 

	if (strcmp(l3assoc,"direct")==0)
	{
		b3bits = llog2(blocksize);
		s3bits = llog2(l3size/blocksize);
		Set *l3sets[l3size/blocksize];
		l3cache->nsets = 1;
	}
	else if (strcmp(l3assoc,"assoc")==0)
	{
		b3bits = llog2(blocksize);
		s3bits = llog2(1);
	}
	else if (strcmp(l3assoc,"assoc:n")==0)
	{
		b3bits = llog2(blocksize);
		s3bits = llog2(l3n);
	}

//t bits

	t1bits = 64-(b1bits+s1bits);
	t2bits = 64-(b2bits+s2bits);
	t3bits = 64-(b3bits+s3bits);


	char tag1[t1bits];
	char set1[s1bits];
	char block1[b1bits];
	char tag2[t2bits];
	char set2[s2bits];
	char block2[b2bits];
	char tag3[t3bits];
	char set3[s3bits];
	char block3[b3bits];
	


	
/*
	struct Cache *l1cache = malloc(sizeof(struct Cache));
	l1cache->sets = malloc((l1size*sizeof(struct Set)));
	
	struct Cache *l2cache = malloc(sizeof(struct Cache));
	l2cache->sets = malloc((l2size*sizeof(struct Set)));

	struct Cache *l3cache = malloc(sizeof(struct Cache));
	l3cache->sets = malloc((l3size*sizeof(struct Set)));

*/

	




	
	
	

	


	/*struct Set *l1set = malloc(sizeof(struct Set));
	l1set->rows = malloc(((l1size/setsize1) * sizeof(struct Row)));

	l1set->nrows = setsize1;

	struct Set *l2set = malloc(sizeof(struct Set));
	l2set->rows = malloc(((l2size/setsize2) * sizeof(struct Row)));

	l2set->nrows = setsize2;

	struct Set *l3set = malloc(sizeof(struct Set));
	l3set->rows = malloc(((l3size/setsize3) * sizeof(struct Row)));

	l3set->nrows = setsize3;*/
	
	int addrlength;
	char currAddr0x[19];


/*	l1cache->miss = 0;
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
	l3cache->capmiss = 0;*/






	//start reading addresses
	
	while (1)
	{
		fscanf(trace, "%s", currAddr0x); 		//read an address

		addrlength = strlen(currAddr0x)-2;



		
		if (feof(trace)) 	//end of file
		{
			break;
		}
		if (strcmp(currAddr0x,"#eof")==0)
		{
			break;
		}

		//count mem access
		
		memaccesses++;
		
		//append leading 0s
		
		char tempAddr[65] = "";
		char tempStr[addrlength];
		memcpy(tempStr,&currAddr0x[2],addrlength);
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



		char currAddr16[17];
		
		strcat(tempAddr,tempStr);
		strcpy(currAddr16,tempAddr);

		strcpy(tempAddr,"");

		//convert addr to binary
		for (int i = 0; currAddr16[i] != '\0'; i++)
		{
			switch (currAddr16[i])
			{
				case '0':
					strcat(tempAddr,"0000");
					break;
				case '1':
					strcat(tempAddr,"0001");
					break;
				case '2':
					strcat(tempAddr,"0010");
					break;
				case '3':
					strcat(tempAddr,"0011");
					break;
				case '4':
					strcat(tempAddr,"0100");
					break;
				case '5':
					strcat(tempAddr,"0101");
					break;
				case '6':
					strcat(tempAddr,"0110");
					break;
				case '7':
					strcat(tempAddr,"0111");
					break;
				case '8':
					strcat(tempAddr,"1000");
					break;
				case '9':
					strcat(tempAddr,"1001");
					break;
				case 'A':
					strcat(tempAddr,"1010");
					break;
				case 'a':
					strcat(tempAddr,"1010");
					break;
				case 'B':
					strcat(tempAddr,"1011");
					break;
				case 'b':
					strcat(tempAddr,"1011");
					break;
				case 'C':
					strcat(tempAddr,"1100");
					break;
				case 'c':
					strcat(tempAddr,"1100");
					break;
				case 'D':
					strcat(tempAddr,"1101");
					break;
				case 'd':
					strcat(tempAddr,"1101");
					break;
				case 'E':
					strcat(tempAddr,"1110");
					break;
				case 'e':
					strcat(tempAddr,"1110");
					break;
				case 'F':
					strcat(tempAddr,"1111");
					break;
				case 'f':
					strcat(tempAddr,"1111");
					break;
				default:
					fprintf(stderr,"ERROR: invalid address (can't convert hex to binary).\n");
					#ifdef DEBUG
					printf("%c\n",currAddr16[i]);
					#endif
					return 0;
			}
		}

		char currAddr[64] = "";

		strcpy(currAddr,tempAddr);
		

		//setup tag, set, and block information.
		char *u;


		memcpy(tag1,&currAddr[0],t1bits);
		memcpy(set1,&currAddr[t1bits],s1bits);
		memcpy(block1,&currAddr[s1bits+t1bits],b1bits);
		
		tag1[t1bits]=0;
		set1[s1bits]=0;
		block1[b1bits]=0;

		tag1v = strtol(tag1,&u,2);
		set1v = strtol(set1,&u,2);
		block1v = strtol(block1,&u,2);
		
		
		memcpy(tag2,&currAddr[0],t2bits);
		memcpy(set2,&currAddr[t2bits],s2bits);
		memcpy(block2,&currAddr[s2bits+t2bits],b2bits);
		
		tag2[t2bits]=0;
		set2[s2bits]=0;
		block2[b2bits]=0;

		tag2v = strtol(tag2,&u,2);
		set2v = strtol(set2,&u,2);
		block2v = strtol(block2,&u,2);


		memcpy(tag3,&currAddr[0],t3bits);
		memcpy(set3,&currAddr[t3bits],s3bits);
		memcpy(block3,&currAddr[s3bits+t3bits],b3bits);

		tag3[t3bits]=0;
		set3[s3bits]=0;
		block3[b3bits]=0;

		tag3v = strtol(tag3,&u,2);
		set3v = strtol(set3,&u,2);
		block3v = strtol(block3,&u,2);
		




		//implementation


		
		int l1pass = 0;
		int l2pass = 0;
		int l3pass = 0;






	
	
		if ((l1pass == 0) && (strcmp(l1assoc,"direct")==0))
		{
		

			l1pass = 0;

		  	Set l1set = l1cache->set[set1v];
		  	Row l1row = NewRow();
		  	l1set.row[0] = l1row;


			if (1>l1size)
			{
				l1cache->capmiss++;
				l1cache->miss++;
				l1row.block=block1v;
				l1row.tag=tag1v;
				l1row.first = 0;
				l1row.valid = 1;
			}
			else if (l1row.tag == tag1v)
			{	
				if (l1row.valid==1)
				{
					l1cache->hit++;
					l1pass = 1;
				}
				else if (l1row.first == 1)
				{
					l1cache->miss++;
					l1row.block=block1v;
					l1row.tag=tag1v;
					l1row.first = 0;
					l1row.valid = 1;
				}
				else
				{	
					l1cache->miss++;
					l1row.block=block1v;
					l1row.tag=tag1v;
					l1row.valid = 1;
				}
			}
			else
			{
				if (l1row.valid==1)
				{
					l1cache->hit++;
					l1pass = 1;
				}
				else if (l1row.first == 1)
				{
					l1cache->miss++;
					l1row.block=block1v;
					l1row.tag=tag1v;
					l1row.first = 0;
					l1row.valid = 1;
				}
				else
				{	
					l1cache->miss++;
					l1row.block=block1v;
					l1row.tag=tag1v;
					l1row.valid = 1;
				}
				
			}
			
		}
		if ((l2pass == 0) && (l1pass == 0) && (strcmp(l2assoc,"direct")==0))
		{
		

			l2pass = 0;

			Set l2set = l1cache->set[set2v];
		  	Row l2row = NewRow();
		  	l2set.row[0] = l2row;

		  	

			if (1>l2size)
			{
				l2cache->capmiss++;
				l1cache->miss++;
				l2row.block=block2v;
				l2row.tag=tag2v;
				l2row.first = 0;
				l2row.valid = 1;
			}
			else if (l2row.tag == tag2v)
			{	
				if (l2row.valid==1)
				{
					l2cache->hit++;
					l2pass = 1;
				}
				else if (l2row.first == 1)
				{
					l2cache->miss++;
					l2row.block=block2v;
					l2row.tag=tag2v;
					l2row.first = 0;
					l2row.valid = 1;
				}
				else
				{	
					l2cache->miss++;
					l2row.block=block2v;
					l2row.tag=tag2v;
					l2row.valid=1;
				}
			}
			else
			{
				if (l2row.valid==1)
				{
					l2cache->hit++;
					l2pass = 1;
				}
				else if (l2row.first == 1)
				{
					l2cache->miss++;
					l2row.block=block2v;
					l2row.tag=tag2v;
					l2row.first = 0;
					l2row.valid = 1;
				}
				else
				{	
					l2cache->miss++;
					l2row.block=block2v;
					l2row.tag=tag2v;
					l2row.valid=1;
				}
			}
		}
		if ((l3pass == 0) && (l2pass == 0) && (l1pass == 0) && (strcmp(l3assoc,"direct")==0))
		{
		

			l3pass = 0;

			Set l3set = l3cache->set[set3v];
		  	Row l3row = NewRow();
		  	l3set.row[0] = l3row;
		  	

			if (1>l3size)
			{
				l3cache->capmiss++;
				l3cache->miss++;
				l3row.block=block3v;
				l3row.tag=tag3v;
				l3row.first = 0;
				l3row.valid = 1;
			}
			else if (l3row.tag == tag3v)
			{	
				if (l3row.valid==1)
				{
					l3cache->hit++;
					l3pass = 1;
				}
				else if (l3row.first == 1)
				{
					l3cache->miss++;
					l3row.block=block3v;
					l3row.tag=tag3v;
					l3row.first = 0;
					l3row.valid = 1;
				}
				else
				{	
					l3cache->miss++;
					l3row.block=block3v;
					l3row.tag=tag3v;
					l3row.valid = 1;
				}
			}
			else
			{
				if (l3row.valid==1)
				{
					l3cache->hit++;
					l3pass = 1;
				}
				else if (l3row.first == 1)
				{
					l3cache->miss++;
					l3row.block=block3v;
					l3row.tag=tag3v;
					l3row.first = 0;
					l3row.valid = 1;
				}
				else
				{	
					l3cache->miss++;
					l3row.block=block3v;
					l3row.tag=tag3v;
					l3row.valid = 1;
				}
			}
		}
		/*
		if ((l1pass == 0) && (l2pass==0) && (strcmp(l1assoc,"direct")==0))
		{
		
			l2cache->nsets = 1;
			l2pass = 0;
		  
				
			if (l2cache->sets[set2v].rows[0].first!=1 && l2cache->sets[set2v].rows[0].first!=0)
				l2cache->sets[set2v].rows[0].first=1;
			if (l2cache->sets[set2v].rows[0].valid!=1 && l2cache->sets[set2v].rows[0].valid!=0)
				l2cache->sets[set2v].rows[0].valid=0;
			l2cache->sets[set2v].rows[0].block=0;	
			l2cache->sets[set2v].rows[0].tag=0;

			if (1>l2size)
			{
				l2cache->capmiss++;
			}
			if ((long)l2cache->sets[set2v].rows[0].tag == tag2v)
			{	
				if (l2cache->sets[set2v].rows[0].valid==1 && l2cache->sets[set2v].rows[0].first==1)
				{
					l2cache->hit++;
					l2pass = 1;
					l2cache->sets[set2v].rows[0].first = 0;
				}
				else if (l2cache->sets[set2v].rows[0].valid==1 && l2cache->sets[set2v].rows[0].first == 0)
				{
					l2cache->confmiss++;
					break;
				}
				else
				{	
					l2cache->coldmiss++;
					l2cache->sets[set2v].rows[0].block=block2v;
					l2cache->sets[set2v].rows[0].first = 0;
				}
			}
		}
		if ((l1pass == 0) && (l2pass==0) && (l3pass == 0) && (strcmp(l1assoc,"direct")==0))
		{
		
			l3cache->nsets = 1;
			l3pass = 0;
		  
				
			if (l3cache->sets[set3v].rows[0].first!=1 && l3cache->sets[set3v].rows[0].first!=0)
				l3cache->sets[set3v].rows[0].first=1;
			if (l3cache->sets[set3v].rows[0].valid!=1 && l3cache->sets[set3v].rows[0].valid!=0)
				l3cache->sets[set3v].rows[0].valid=0;
			l3cache->sets[set3v].rows[0].block=0;	
			l3cache->sets[set3v].rows[0].tag=0;

			if (1>l3size)
			{
				l3cache->capmiss++;
			}
			if ((long)l3cache->sets[set3v].rows[0].tag == tag3v)
			{	
				if (l3cache->sets[set3v].rows[0].valid==1 && l3cache->sets[set3v].rows[0].first==1)
				{
					l3cache->hit++;
					l3pass = 1;
					l3cache->sets[set3v].rows[0].first = 0;
				}
				else if (l3cache->sets[set3v].rows[0].valid==1 && l3cache->sets[set3v].rows[0].first == 0)
				{
					l3cache->confmiss++;
					break;
				}
				else
				{	
					l3cache->coldmiss++;
					l3cache->sets[set3v].rows[0].block=block3v;
					l3cache->sets[set3v].rows[0].first = 0;
				}
			}
		}*/
		/*
		if ((l1pass = 0) && (l2pass == 0) && (strcmp(l2assoc,"direct")==0))
		{
			int flag = 0;
			//struct Set *l2set = malloc((1*sizeof(struct Row))+sizeof(struct Set));
			l2cache->nsets = 1;
			l2pass = 0;
			for (int i=0; i<1;i++)
			{
				
				if (flag == 0)
				{
					l2cache->sets[set2v].rows[i].first=1;
					l2cache->sets[set2v].rows[i].valid=0;
					l2cache->sets[set2v].rows[i].block=0;
					l2cache->sets[set2v].rows[i].tag=0;
					flag = 1;
				}
				if (1>l2size)
				{
					l2cache->capmiss++;
					break;
				}
				if (l2cache->sets[set2v].rows[i].tag == tag2v)
				{
					if (l2cache->sets[set2v].rows[i].valid==1 && l2cache->sets[set2v].rows[i].block==block2v)
					{
						l2cache->hit++;
						l2pass = 1;
						l2cache->sets[set2v].rows[i].first = 0;
						break;
					}
					else if (l2cache->sets[set2v].rows[i].valid==1 && l2cache->sets[set2v].rows[i].first == 0)
					{
						l2cache->confmiss++;
						break;
					}
					else
					{	
						l2cache->coldmiss++;
						l2cache->sets[set2v].rows[i].block=block2v;
						l2cache->sets[set2v].rows[i].first = 0;
						break;
					}
				}
			}
		}
		if ((l2pass = 0) && (l2pass == 0) && (l3pass == 0) && (strcmp(l3assoc,"direct")==0))
		{
			//struct Set *l3set = malloc((1*sizeof(struct Row))+sizeof(struct Set));
			l3cache->nsets = 1;
			l3pass = 0;
			int flag = 0;
			for (int i=0;i<1;i++)
			{	
				if (flag == 0)
				{
					l3cache->sets[set3v].rows[i].first=1;
					l3cache->sets[set3v].rows[i].valid=0;
					l3cache->sets[set3v].rows[i].block=0;
					l3cache->sets[set3v].rows[i].tag=0;
					flag = 1;
				}
				if (1>l3size)
				{
					l3cache->capmiss++;
					break;
				}
				if (l3cache->sets[set3v].rows[i].tag == tag3v)
				{
					if (l3cache->sets[set3v].rows[i].valid==1 && l3cache->sets[set3v].rows[i].block==block3v)
					{
						l3cache->hit++;
						l3pass = 1;
						l3cache->sets[set3v].rows[i].first = 0;
						break;
					}
					else if (l3cache->sets[set3v].rows[i].valid==1 && l3cache->sets[set3v].rows[i].first == 0)
					{
						l3cache->confmiss++;
						break;
					}
					else
					{	
						l3cache->coldmiss++;
						l3cache->sets[set3v].rows[i].block=block3v;
						l3cache->sets[set3v].rows[i].first = 0;
						break;
					}
				}
			}
		}*/
			
		
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
		printf("l1cache sets: %d\n",l1cache->nsets);
		printf("l2cache sets: %d\n",l2cache->nsets);
		printf("l3cache sets: %d\n",l3cache->nsets);
		printf("l1cache setsize: %s\n","---");
		printf("l2cache setsize: %s\n","---");
		printf("l3cache setsize: %s\n","---");
		printf("l1cache rows: %s\n","---");
		printf("l2cache rows: %s\n","---");
		printf("l3cache rows: %s\n","---");
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
		printf("l1 cache hits: %d\n",l1cache->hit);
		printf("l1 cache misses: %d\n",l1cache->miss);
		printf("l1 cache coldmisses: %d\n",l1cache->coldmiss);
		printf("l1 cache confmisses: %d\n",l1cache->confmiss);
		printf("l2 cache hits: %d\n",l2cache->hit);
		printf("l2 cache misses: %d\n",l2cache->miss);
		printf("l2 cache coldmisses: %d\n",l2cache->coldmiss);
		printf("l2 cache confmisses: %d\n",l2cache->confmiss);
		printf("l3 cache hits: %d\n",l3cache->hit);
		printf("l3 cache misses: %d\n",l3cache->miss);
		printf("l3 cache coldmisses: %d\n",l3cache->coldmiss);
		printf("l3 cache confmisses: %d\n",l3cache->confmiss);

#endif



return 0;
}
		
