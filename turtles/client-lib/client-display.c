#include "client-display.h"


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

int verify_3param_command(char* buf, int* arg1, int* arg2, int* arg3)
{
	int ret = sscanf (buf, "%*[^0123456789]%d %d %d", arg1, arg2, arg3);
	return ret == 3;
}

int comma_list_check (char* buf, int len, int listlen, int maxval)
{
	int i;
	int commas = 0;
	char * p;
	char* tbuf = get_buffer(len);
	int currVal = -1;
	int lastVal;
	memcpy(tbuf, buf, sizeof(char)*len);
	for (i=0;i<strlen(buf);i++)
	{
		if (buf[i] == ',')
			commas++;
	}
	if (commas != listlen-1)
	{
		fprintf (stderr, "Invalid number of values.\n");
		free(tbuf);
		return 0;
	}
	
	p = strtok(tbuf, ",");
	sscanf(p, "%d", &currVal);
	if (currVal != 0)
	{
		fprintf (stderr, "String is not starting with 0 (length of first checkpoint)\n");
		free(tbuf);
		return 0;
	}
	lastVal = 0;

	while ((p = strtok(NULL, ",")) != NULL)
	{
		sscanf(p, "%d", &currVal);
		if (lastVal > currVal)
		{
			fprintf (stderr, "Lengths are not ascending\n");
			free(tbuf);
			return 0;
		}
		lastVal = currVal;
	}
	free(tbuf);
	return 1;
}


void get_comma_list (char* msg, char** buf, int len, int listlen, int maxval)
{
	memset(*buf, 0x00, len);
	fprintf (stdout, "%s\n", msg);
	fgets(*buf, len, stdin);
	*buf = strtok(*buf, "\n");
	
	while (!comma_list_check(*buf, len, listlen, maxval))
	{
		memset(*buf, 0x00, len);
		fprintf (stdout, "Wrong value. Please type correct string.\n");
		fgets(*buf, len, stdin);
		*buf = strtok(*buf, "\n");
	}
}

void get_players (char* msg, char** buf, int len)
{
	memset(*buf, 0x00, len);
	fprintf (stdout, "%s\n", msg);
	fgets(*buf, len, stdin);
	*buf = strtok(*buf, "\n");
	
	while (!players_list_check(*buf, len))
	{
		memset(*buf, 0x00, len);
		fprintf (stdout, "Wrong value. Please type correct string.\n");
		fgets(*buf, len, stdin);
		*buf = strtok(*buf, "\n");
	}
}

int players_list_check(char* buf, int len)
{
	char * p;
	char* tbuf = get_buffer(len);
	int val;
	memcpy(tbuf, buf, sizeof(char)*len);
	
	p = strtok(tbuf, ",");
	if (!sscanf(p, "%d", &val))
	{
		free(tbuf);
		return 0;
	}

	while ((p = strtok(NULL, ",")) != NULL)
	{
		if (!sscanf(p, "%d", &val))
		{
			free(tbuf);
			return 0;
		}
	}
	free(tbuf);
	return 1;
}

void display_formatted_turtles (char* buf)
{
	char* p;
	int age, weight, id, pts;
	char name [BUFLEN];
	char delim = MSG_PARTS_DELIMITER;

	fprintf (stdout, "All players known to server:\n");
	fprintf (stdout, "%-6s%-20s%-6s%-7s%-8s\n", "id", "name", "age", "weight", "points");
	p = strtok(buf, &delim);
	memset(name, 0x00, BUFLEN);
	sscanf(p, "%d:%[^:]:%d:%d:%d", &id, name, &age, &weight, &pts);
	fprintf (stdout, "%-6d%-20s%-6d%-7d%-8d\n", id, name, age, weight, pts);
		
	while ((p = strtok(NULL, &delim)) != NULL)
	{
		if (strlen(p) == 0)
			break;
		memset(name, 0x00, BUFLEN);
		if (sscanf(p, "%d:%[^:]:%d:%d:%d", &id, name, &age, &weight, &pts) != 5)
			continue;
		fprintf (stdout, "%-6d%-20s%-6d%-7d%-8d\n", id, name, age, weight, pts);
	}
}

void display_formatted_tracks (char* buf)
{
	char* p;
	int chpoints, laps, length;
	char name [BUFLEN];
	char chlengths[CHPTLEN];
	char delim = MSG_PARTS_DELIMITER;

	fprintf (stdout, "All tracks known to server:\n");
	fprintf (stdout, "%-20s%-10s%-6s%-8s%-30s\n", "name", "chpoints", "laps", "length", "chpoint locations");
	if (buf == NULL || strlen(buf) <4)
		return;
	p = strtok(buf, &delim);
	memset(name, 0x00, BUFLEN);
	memset(chlengths, 0x00, CHPTLEN);
	sscanf(p, "%[^:]:%d:%d:%d:%s", name, &chpoints, &laps, &length, chlengths);
	fprintf (stdout, "%-20s%-10d%-6d%-8d%-30s\n", name, chpoints, laps, length, chlengths);
		
	while ((p = strtok(NULL, &delim)) != NULL)
	{
		if (strlen(p) == 0)
			break;
		memset(name, 0x00, BUFLEN);
		if (sscanf(p, "%[^:]:%d:%d:%d%s", name, &chpoints, &laps, &length, chlengths) != 5)
			continue;
		fprintf (stdout, "%-20s:%-10d:%-6d:%-8d%-30s\n", name, chpoints, laps, length, chlengths);
	}
}

void display_formatted_sequence (char* buf)
{
	char* p;
	int lap, chpoint;
	char name [BUFLEN];
	char delim = MSG_PARTS_DELIMITER;

	fprintf (stdout, "Current race players' positions:\n");
	fprintf (stdout, "%-20s%-6s%-13s\n", "name", "lap", "last checkpoint");
	if (buf == NULL || strlen(buf) <4)
		return;
	p = strtok(buf, &delim);
	memset(name, 0x00, BUFLEN);
	sscanf(p, "%[^:]:%d:%d", name, &lap, &chpoint);
	fprintf (stdout, "%-20s%-6d%-13d\n", name, lap, chpoint);
		
	while ((p = strtok(NULL, &delim)) != NULL)
	{
		if (strlen(p) == 0)
			break;
		memset(name, 0x00, BUFLEN);
		if (sscanf(p, "%[^:]:%d:%d", name, &lap, &chpoint) != 3)
			continue;
		fprintf (stdout, "%-20s%-6d%-13d\n", name, lap, chpoint);
	}
}

void display_formatted_table (char* buf)
{
	char* p;
	int points;
	char name [BUFLEN];
	char delim = MSG_PARTS_DELIMITER;
	
	fprintf (stdout, "Current season's table:\n");
	fprintf (stdout, "%-20s%-6s\n", "name", "points");
	if (buf == NULL || strlen(buf) <4)
		return;
	p = strtok(buf, &delim);
	memset(name, 0x00, BUFLEN);
	sscanf(p, "%[^:]:%d", name, &points);
	fprintf (stdout, "%-20s%-6d\n", name, points);
		
	while ((p = strtok(NULL, &delim)) != NULL)
	{
		if (strlen(p) == 0)
			break;
		memset(name, 0x00, BUFLEN);
		if (sscanf(p, "%[^:]:%d", name, &points) != 2)
			continue;
		fprintf (stdout, "%-20s%-6d\n", name, points);
	}
}
