#include "ui.h"

void get_int (char* msg, int* val)
{
	int v = 0;
	char buf [COMMANDLEN];
	memset(buf, 0x00, COMMANDLEN);
	fprintf (stdout, "%s\n", msg);
	fgets(buf, COMMANDLEN, stdin);
	v = atoi(buf);
	while (!v)
	{
		fprintf(stdout,"Wrong value. Please type correct integer.\n");
		memset(buf, 0x00, COMMANDLEN);
		fgets(buf, COMMANDLEN, stdin);
		v = atoi(buf);
	}
	*val = v;
}

void get_string (char* msg, char** buf, int len)
{
	memset(*buf, 0x00, len);
	fprintf (stdout, "%s\n", msg);
	fgets(*buf, len, stdin);
	*buf = strtok(*buf, "\n");
	while (!strlen(*buf))
	{
		memset(*buf, 0x00, len);
		fprintf (stdout, "Wrong value. Please type correct string.\n");
		fgets(*buf, len, stdin);
		*buf = strtok(*buf, "\n");
	}
}

