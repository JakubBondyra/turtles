#include "common.h"

ssize_t dollar_read(int sock, char* buf, size_t size)
{
	ssize_t len = bulk_dollar_read(sock, buf, size);
	if(len <0 && errno == ECONNRESET)
		return 0;
	return len;
}

ssize_t dollar_write(int sock, char* buf, size_t size)
{
	ssize_t len = bulk_dollar_write(sock, buf, size);
	if(len < 0 && errno == EPIPE)
		return 0;
	if(len <0 && errno == ECONNRESET)
		return 0;
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
		if (*(p-1) == MSG_DELIMITER)
			break;
	}
	return size-nleft;
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
