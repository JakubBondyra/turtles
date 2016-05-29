#include <stdio.h>
#include <stdlib.h>
#include "jb_library.h"

/* buffer lengths */
#define FILELINE 128
#define BUFLEN 667
#define COMMANDLEN 32

/* network features */
#define BACKLOG 3

/* message ids */
#define ADDTURTLEID 0
#define ADDTRACKID 1
#define STARTRACEID 2
#define ENDRACEID 3
#define RACENEWSID 4
#define NEWSEASONID 5
#define LIVEREQUESTID 6
#define SEQREQUESTID 7
#define TABREQUESTID 8
#define ERRID 9
#define SUCCESSID 10
#define GETTURTLESID 11
#define GETTRACKSID 12
#define NAMELEN 32

/* structures */
struct turtle{
	int id;
	char name [NAMELEN];
	int age;
	int weight;
	int currPoints;
	int currPos;
};

struct turtle_group {
	int number;
	struct turtle* turtles;
};

struct track {
	char name [NAMELEN];
	int length;
	int checkpoint_number;
	int laps;
};

struct track_group {
	int number;
	struct track* tracks;
};


