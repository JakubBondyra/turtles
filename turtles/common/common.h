#include <stdio.h>
#include <stdlib.h>
#include "jb_library.h"

/* buffer lengths */
#define FILELINE 128
#define BUFLEN 667
#define COMMANDLEN 32

/* network features */
#define BACKLOG 3

/* message utils */
#define ADDTURTLEID 10
#define ADDTRACKID 11
#define STARTRACEID 12
#define ENDRACEID 13
#define RACENEWSID 14
#define NEWSEASONID 15
#define LIVEREQUESTID 16
#define SEQREQUESTID 17
#define TABREQUESTID 18
#define ERRID 19
#define SUCCESSID 20
#define GETTURTLESID 21
#define GETTRACKSID 22
#define COMMERROR 99
#define NAMELEN 32
#define MSG_DELIMITER '$'

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

ssize_t dollar_read(int sockFd, char* buf, size_t count);

ssize_t dollar_write(int sockFd, char* buf, size_t count);

ssize_t bulk_dollar_write(int fd, const void *buf, size_t size);

ssize_t bulk_dollar_read(int fd, void *buf, size_t size);

int get_msg_type (char buf[BUFLEN]);
