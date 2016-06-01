#include "common.h"

ssize_t dollar_read(int sock, char* buf, size_t size)
{
	ssize_t len = bulk_dollar_read(sock, buf, size);
	if(len <0 && errno == ECONNRESET)
		return 0;
	fprintf (stdout, "Received %d bytes from socket.\n", (int)len);
	return len;
}

ssize_t dollar_write(int sock, char* buf, size_t size)
{
	ssize_t len = bulk_dollar_write(sock, buf, size);
	if(len < 0 && errno == EPIPE)
		return 0;
	if(len <0 && errno == ECONNRESET)
		return 0;
	fprintf (stdout, "Printed %d bytes to socket.\n", (int)len);
	return len;
}

ssize_t bulk_dollar_write(int fd, const void *buf, size_t size) 
{
	size_t nleft = size;
	ssize_t nwritten;
	const char *p = buf;

	while (nleft > 0)
	{
		if ((nwritten = (TEMP_FAILURE_RETRY(write(fd, p, nleft)))) == -1)
		{
			jb_print_error("write");
		}
		
		nleft -= nwritten;
		p += nwritten;
	}
	return size;
}

ssize_t bulk_dollar_read(int fd, void *buf, size_t size)
{
	size_t nleft = size;
	ssize_t nread;
	char *p = buf;

	while (nleft > 0)
	{
		if ((nread = (TEMP_FAILURE_RETRY(read(fd, p, nleft)))) == -1)
		{
			jb_print_error("read");
		}
		
		nleft -= nread;
		p += nread;
		/* EOF case */
		if (nread == 0)
			break;
		if (*(p-1) == MSG_TERMINATOR)
			break;
	}
	return size-nleft;
}

void print_error(int fd, char* msg)
{
	char buf[BUFLEN];
	memset(buf, 0x00, BUFLEN);
	snprintf(buf, BUFLEN, "%d;%s%c", ERRID, msg, MSG_TERMINATOR);
	dollar_write(fd, buf, BUFLEN);
}

void print_success(int fd)
{
	char buf[BUFLEN];
	memset(buf, 0x00, BUFLEN);
	snprintf(buf, BUFLEN, "%d%c", SUCCESSID, MSG_TERMINATOR);
	dollar_write(fd, buf, BUFLEN);
}

int get_msg_type (char buf[BUFLEN])
{
	int v = -1;
	
	if (strlen(buf) < 2)
		v = -1;
	else
	{
		v = (buf[0] - '0')*10 + buf[1] - '0';
		if (v < 0 || v > 99)
			v = -1;
	}
	return v;
}

char* get_buffer (int maxlen)
{
	char* buf = (char*) jb_malloc (sizeof(char) * BUFLEN);
	memset(buf, 0x00, BUFLEN);
	return buf;
}

char * chop_endings (char* buf, int len)
{
	char* buf2 = (char*) jb_malloc (sizeof(char) * len);
	memset(buf2, 0x00, len);
	sscanf(buf, "%*d;%[^$]", buf2);
	return buf2;
}
