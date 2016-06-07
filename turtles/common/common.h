#include <stdio.h>
#include <stdlib.h>
#include "jb_library.h"

/* buffer lengths */
#define FILELINE 256
#define BUFLEN 667
#define COMMANDLEN 32
#define CHPTLEN 128
#define LINEMAX 164

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
#define NEWSPOSCHANGEDID 44
#define NEWSRACEENDID 55
#define MSG_TERMINATOR '$'
#define MSG_PARTS_DELIMITER ';'

ssize_t dollar_read(int sockFd, char* buf, size_t count);
ssize_t dollar_read_signal_sensitive(int sockFd, char* buf, size_t count);
ssize_t dollar_write(int sockFd, char* buf, size_t count);
ssize_t bulk_dollar_write(int fd, const void *buf, size_t size);
ssize_t bulk_dollar_read(int fd, void *buf, size_t size);
ssize_t bulk_dollar_read_signal_sensitive(int fd, void *buf, size_t size);

void print_error(int fd, char* msg);
void print_success(int fd);
int get_msg_type (char* buf);
char* get_buffer (int maxlen);
char* chop_endings(char* buf, int len);
