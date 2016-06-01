#include "client-display.h"


int get_int (char* msg, int* val)
{
	int v = 0;
	char buf [COMMANDLEN];
	memset(buf, 0x00, COMMANDLEN);
	fprintf (stdout, "%s\n", msg);
	fgets(buf, COMMANDLEN, stdin);
	v = atoi(buf);
	while (!v)
	{
		if (buf[0] == 'a')
		{
			return 0;
		}
		fprintf(stdout,"Wrong value. Please type correct integer.\n");
		memset(buf, 0x00, COMMANDLEN);
		fgets(buf, COMMANDLEN, stdin);
		v = atoi(buf);
	}
	*val = v;
	return 1;
}

int get_string (char* msg, char** buf, int len)
{
	memset(*buf, 0x00, len);
	fprintf (stdout, "%s\n", msg);
	fgets(*buf, len, stdin);
	*buf = strtok(*buf, "\n");
	if (strlen(*buf) == 1 && (*buf)[0] == 'a')
	{
		return 0;
	}
	while (!strlen(*buf))
	{
		memset(*buf, 0x00, len);
		fprintf (stdout, "Wrong value. Please type correct string.\n");
		fgets(*buf, len, stdin);
		if (strlen(*buf) == 1 && (*buf)[0] == 'a')
		{
			return 0;
		}
		*buf = strtok(*buf, "\n");
	}
	return 1;
}

int verify_3param_command(char* buf, int* arg1, int* arg2, int* arg3)
{
	int ret = sscanf (buf, "%*[^0123456789]%d %d %d", arg1, arg2, arg3);
	return ret == 3;
}


int get_comma_list (char* msg, char** buf, int len, int listlen, int maxVal)
{
	char* line = get_buffer(LINEMAX);
	int newVal;
	int prevVal;
	int i=1;
	int pos = 0;
	int written;
	memset(*buf, 0x00, len);
	while (i<=listlen)
	{
		fprintf (stdout, "%s %d\n", msg, i);
		fgets(line, LINEMAX, stdin);
		if (line[0] == 'a')
		{
			free(line);
			return 0;
		}
		if (sscanf (line, "%d", &newVal) != 1)
		{
			fprintf (stdout, "Wrong value.\n");
		}
		else if (newVal < prevVal)
		{
			fprintf (stdout, "Wrong values - distances are descending.\n");
		}
		else if (newVal >= maxVal)
		{
			fprintf (stdout, "Distance of checkpoint excedes length of track (%d)\n", maxVal);
		}
		else
		{
			if (i==1)
				written = snprintf(*buf+pos, len-pos, "%d", newVal);
			else
				written = snprintf(*buf+pos, len-pos, ",%d", newVal);
			pos+=written;
			i++;
			prevVal = newVal;
		}
	}
	
	free(line);
	return 1;
}

int get_players (char* msg, char** buf, int len)
{
	char* line = get_buffer(LINEMAX);
	int newId;
	int i=1;
	int pos = 0;
	int written;
	memset(*buf, 0x00, len);
	while (1)
	{
		fprintf (stdout, "%s %d (or type \'q\' to finish)\n", msg, i);
		fgets(line, LINEMAX, stdin);
		if (line[0] == 'q')
		{
			break;
		}
		else if (line[0] == 'a')
		{
			free(line);
			return 0;
		}
		if (sscanf (line, "%d", &newId) != 1)
		{
			fprintf (stdout, "Wrong value.\n");
		}
		else
		{
			if (i==1)
				written = snprintf(*buf+pos, len-pos, "%d", newId);
			else
				written = snprintf(*buf+pos, len-pos, ",%d", newId);
			pos+=written;
			i++;
		}
	}
	free(line);
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
	sscanf(p, "%[^:]:%d:%d:%d:%[0123456789,]", name, &chpoints, &laps, &length, chlengths);
	fprintf (stdout, "%-20s%-10d%-6d%-8d%-30s\n", name, chpoints, laps, length, chlengths);
		
	while ((p = strtok(NULL, &delim)) != NULL)
	{
		if (strlen(p) == 0)
			break;
		memset(name, 0x00, BUFLEN);
		if (sscanf(p, "%[^:]:%d:%d:%d:%[0123456789,]", name, &chpoints, &laps, &length, chlengths) != 5)
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
