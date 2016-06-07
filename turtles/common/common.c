#include "common.h"

ssize_t dollar_read(int sock, char* buf, size_t size)
{
	ssize_t len = bulk_dollar_read(sock, buf, size);
	if(len <0 && errno == ECONNRESET)
	{
		fprintf (stderr, "Connection reset. Returning.\n");
		return 0;
	}
	return len;
}

ssize_t dollar_read_signal_sensitive(int sock, char* buf, size_t size)
{
	ssize_t len = bulk_dollar_read_signal_sensitive(sock, buf, size);
	if(len <0 && errno == ECONNRESET)
	{
		fprintf (stderr, "Connection reset. Returning.\n");
		return 0;
	}
	return len;
}

ssize_t dollar_write(int sock, char* buf, size_t size)
{
	ssize_t len = bulk_dollar_write(sock, buf, size);
	if(len < 0 && errno == EPIPE)
	{
		fprintf (stderr, "The other end is closed. Returning.\n");
		return 0;
	}
	if(len <0 && errno == ECONNRESET)
	{
		fprintf (stderr, "Connection reset. Returning.\n");
		return 0;
	}
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

ssize_t bulk_dollar_read_signal_sensitive(int fd, void *buf, size_t size)
{
	size_t nleft = size;
	ssize_t nread;
	char *p = buf;

	while (nleft > 0)
	{
		if (((nread = (read(fd, p, nleft))) == -1) && errno != EINTR)
		{
			jb_print_error("read");
		}
		if (errno == EINTR)
		{
			fprintf (stdout, "Signal detected. Aborting read operation.\n");
			return 0;
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
	char* buf = get_buffer(BUFLEN);
	int len = snprintf(buf, BUFLEN, "%d;%s%c", ERRID, msg, MSG_TERMINATOR);
	dollar_write(fd, buf, len);
	free(buf);
}

void print_success(int fd)
{
	char* buf = get_buffer(BUFLEN);
	int len = snprintf(buf, BUFLEN, "%d%c", SUCCESSID, MSG_TERMINATOR);
	dollar_write(fd, buf, len);
	free(buf);
}

int get_msg_type (char* buf)
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
