#include "client-features.h"

char* client_commands [] = {QUITCOMMAND, ADDTURTLECOMMAND, ADDTRACKCOMMAND, STARTRACECOMMAND,
	ENDRACECOMMAND, RACENEWSCOMMAND, NEWSEASONCOMMAND};
char* client_infos [] = {QUITINFO, ADDTURTLEINFO, ADDTRACKINFO, STARTRACEINFO,
	ENDRACEINFO, RACENEWSINFO, NEWSEASONINFO};
	
char* spectator_commands [] = {QUITCOMMAND, LIVEREQUESTCOMMAND, SEQREQUESTCOMMAND, TABREQUESTCOMMAND,
	GETTURTLESCOMMAND, GETTRACKSCOMMAND};
char* spectator_infos [] = {QUITINFO, LIVEREQUESTINFO, SEQREQUESTINFO, TABREQUESTINFO,
	GETTURTLESINFO, GETTRACKSINFO};
	
void handle_args (int argc, char* argv[], int* port, char** address)
{
	if (argc != 3)
	{
		fprintf (stdout, "Usage: %s [ADDRESS] [PORT]\n", argv[0]);
		exit(EXIT_SUCCESS);
	}
	*port = atoi(argv[2]);
	if (*port == 0)
	{
		fprintf (stdout, "Wrong port argument!\n");
		exit(EXIT_FAILURE);
	}
	*address = argv[1];
	return;
}

void run_spectator (int port, char* address)
{
	run_ui (port, address, spectator_commands, spectator_infos, 6);
}

void run_client (int port, char* address)
{
	run_ui (port, address, client_commands, client_infos, 6);
}

void run_ui (int port, char* address, char* commands[], char* infos[], int ccount)
{
	int i;
	if (!get_line(port, address, commands, ccount))
	{
		fprintf (stdout, "Usage:\n");
		for (i=0;i<ccount;i++)
		{
			fprintf(stdout, "%s - %s\n", commands[i], infos[i]);
		}
		fprintf (stdout, "quit - to end this program\n");
	}
}

int get_line (int port, char* address, char* commands[], int ccount)
{
	int i;
	char buf[COMMANDLEN];
	int len;
	memset(buf, 0x00, COMMANDLEN);
	if (fgets(buf, COMMANDLEN, stdin) == NULL)
		return 0;
	buf[strlen(buf)-1] = '\0';
	for (i=0;i<ccount;i++)
	{
		len = strlen(commands[i]);
		if (!strncmp(buf, commands[i], len))
		{
			get_handler (port, address, commands[i]);
			return 1;
		}
	}
	return 0;
}

void get_handler (int port, char* address, char* command)
{
	if (!strncmp (command, QUITCOMMAND, strlen(QUITCOMMAND)))
	{
		quit_handler();
	}
	else if (!strncmp(command, ADDTURTLECOMMAND, strlen(ADDTURTLECOMMAND)))
	{
		add_turtle_handler (port, address);
	}
	else if (!strncmp(command, ADDTRACKCOMMAND, strlen(ADDTRACKCOMMAND)))
	{
		add_track_handler (port, address);
	}
	else if (!strncmp(command, STARTRACECOMMAND, strlen(STARTRACECOMMAND)))
	{
		start_race_handler (port, address);
	}
	else if (!strncmp(command, ENDRACECOMMAND, strlen(ENDRACECOMMAND)))
	{
		end_race_handler (port, address);
	}
	else if (!strncmp(command, RACENEWSCOMMAND, strlen(RACENEWSCOMMAND)))
	{
		race_news_handler (port, address);
	}
	else if (!strncmp(command, NEWSEASONCOMMAND, strlen(NEWSEASONCOMMAND)))
	{
		new_season_handler (port, address);
	}
	else if (!strncmp(command, LIVEREQUESTCOMMAND, strlen(LIVEREQUESTCOMMAND)))
	{
		live_request_handler (port, address);
	}
	else if (!strncmp(command, SEQREQUESTCOMMAND, strlen(SEQREQUESTCOMMAND)))
	{
		seq_request_handler (port, address);
	}
	else if (!strncmp(command, TABREQUESTCOMMAND, strlen(TABREQUESTCOMMAND)))
	{
		tab_request_handler (port, address);
	}
	else if (!strncmp(command, GETTURTLESCOMMAND, strlen(GETTURTLESCOMMAND)))
	{
		get_turtles_handler (port, address);
	}
	else if (!strncmp(command, GETTRACKSCOMMAND, strlen(GETTRACKSCOMMAND)))
	{
		get_tracks_handler (port, address);
	}
}

void quit_handler ()
{
	exit(EXIT_SUCCESS);
}

void add_turtle_handler (int port, char* address)
{
	int id, age, weight;
	char* name = (char*) jb_malloc (sizeof(char)*NAMELEN);
	char* buf = (char*) jb_malloc (sizeof(char) * BUFLEN);
	int len = 0;
	memset(name, 0x00, NAMELEN);
	get_int("Please specify id of turtle", &id);
	get_string("Please specify name of turtle", &name, NAMELEN);
	get_int("Please specify age of turtle", &age);
	get_int("Please specify weight of turtle", &weight);
	memset(buf, 0x00, BUFLEN);
	len = snprintf(buf, BUFLEN, "%d;%d:%s:%d:%d%c", ADDTURTLEID, id, name, age, weight, MSG_DELIMITER);
	send_message(port, address, buf, len);
	free(buf);
	free(name);
}
void add_track_handler (int port, char* address)
{
	int chpoints, laps, length;
	char* name = (char*) jb_malloc (sizeof(char)*NAMELEN);
	char* buf = (char*) jb_malloc (sizeof(char) * BUFLEN);
	int len = 0;
	get_string("Please specify name of track", &name, NAMELEN);
	get_int("Please specify number of checkpoints on each lap", &chpoints);
	get_int("Please specify number of laps", &laps);
	get_int("Please specify length of track", &length);
	memset(buf, 0x00, BUFLEN);
	len = snprintf(buf, BUFLEN, "%d;%s:%d:%d:%d%c", ADDTRACKID, name, chpoints, laps, length, MSG_DELIMITER);
	send_message(port, address, buf, len);
	free(buf);
	free(name);
}
void start_race_handler (int port, char* address)
{
	char* buf = (char*) jb_malloc (sizeof(char) * BUFLEN);
	int len = 0;
	char* name = (char*) jb_malloc (sizeof(char)*NAMELEN);
	get_string("Please specify name of track", &name, NAMELEN);
	len = snprintf(buf, BUFLEN, "%d;%s%c", STARTRACEID, name, MSG_DELIMITER);
	send_message(port, address, buf, len);
	free(buf);
	free(name);
}
void end_race_handler (int port, char* address)
{
	char* buf = (char*) jb_malloc (sizeof(char) * BUFLEN);
	int len = 0;
	memset(buf, 0x00, BUFLEN);
	len = snprintf(buf, BUFLEN, "%d%c", ENDRACEID, MSG_DELIMITER);
	send_message(port, address, buf, len);
	free(buf);
}
void race_news_handler (int port, char* address)
{
	int who, where;
	char* buf = (char*) jb_malloc (sizeof(char) * BUFLEN);
	int len = 0;
	get_int("Please specify turtle id", &who);
	get_int("Please specify checkpoint", &where);
	memset(buf, 0x00, BUFLEN);
	len = snprintf(buf, BUFLEN, "%d;%d:%d%c", RACENEWSID, who, where, MSG_DELIMITER);
	send_message(port, address, buf, len);
	free(buf);
}
void new_season_handler (int port, char* address)
{
	char* buf = (char*) jb_malloc (sizeof(char) * BUFLEN);
	int len = 0;
	memset(buf, 0x00, BUFLEN);
	len = snprintf(buf, BUFLEN, "%d%c", NEWSEASONID, MSG_DELIMITER);
	send_message(port, address, buf, len);
	free(buf);
}
void live_request_handler (int port, char* address)
{
	/* TODO */
	char* buf = (char*) jb_malloc (sizeof(char) * BUFLEN);
	int len = 0;
	memset(buf, 0x00, BUFLEN);
	len = snprintf(buf, BUFLEN, "%d%c", LIVEREQUESTID, MSG_DELIMITER);
	send_message(port, address, buf, len);
	free(buf);
}
void seq_request_handler (int port, char* address)
{
	char* buf = (char*) jb_malloc (sizeof(char) * BUFLEN);
	int len = 0;
	memset(buf, 0x00, BUFLEN);
	len = snprintf(buf, BUFLEN, "%d%c", SEQREQUESTID, MSG_DELIMITER);
	send_message(port, address, buf, len);
	free(buf);
}
void tab_request_handler (int port, char* address)
{
	char* buf = (char*) jb_malloc (sizeof(char) * BUFLEN);
	int len = 0;
	memset(buf, 0x00, BUFLEN);
	len = snprintf(buf, BUFLEN, "%d%c", TABREQUESTID, MSG_DELIMITER);
	send_message(port, address, buf, len);
	free(buf);
}
void get_turtles_handler (int port, char* address)
{
	char* buf = (char*) jb_malloc (sizeof(char) * BUFLEN);
	int len = 0;
	memset(buf, 0x00, BUFLEN);
	len = snprintf(buf, BUFLEN, "%d%c", GETTURTLESID, MSG_DELIMITER);
	send_message(port, address, buf, len);
	free(buf);
}
void get_tracks_handler (int port, char* address)
{
	char* buf = (char*) jb_malloc (sizeof(char) * BUFLEN);
	int len = 0;
	memset(buf, 0x00, BUFLEN);
	len = snprintf(buf, BUFLEN, "%d%c", GETTRACKSID, MSG_DELIMITER);
	send_message(port, address, buf, len);
	free(buf);
}

void send_message (int port, char* address, char* buf, int count)
{
	/* send message feature */
	int sock;
	jb_internet_stream_socket_client(&sock, address, (uint16_t) port);
	fprintf (stdout, "Trying to write: \"%s\" (total %d bytes)\n", buf, count);
	dollar_write(sock, buf, count);
	handle_response(sock);
}

void handle_response (int sock)
{
	char* buf = (char*) jb_malloc (sizeof(char) * BUFLEN);
	int msgid;
	memset(buf, 0x00, BUFLEN);
	dollar_read(sock, buf, BUFLEN);
	if (strlen(buf) < 2)
	{
		fprintf (stdout, "Error of communication (too short message)\n");
		free(buf);
		return;
	}
	msgid = get_msg_type(buf);
	if (msgid == ERRID)
	{
		fprintf(stdout, "Server reported error.\n");
	}
	else if (msgid == SUCCESSID)
	{
		fprintf(stdout, "Success.\n");
	}
	else if (msgid == COMMERROR)
	{
		fprintf (stderr, "Communication error. Malformed buffer. \n");
		fflush(stderr);
	}
	else if (msgid == GETTURTLESID || msgid == GETTRACKSID)
	{
		char* buf2 = (char*) jb_malloc (sizeof(char) * BUFLEN);
		memset(buf2, 0x00, BUFLEN);
		sscanf(buf, "%*d;%[^$]", buf2);
		fprintf (stdout,"Result:\n");
		if (msgid==GETTURTLESID)
		{
			print_formatted_turtles(buf2);
		}
		else
		{
			print_formatted_tracks(buf2);
		}
		free(buf2);
	}
	else /* GET TABLE, GET SEQUENCE, LIVE RELATION (FOR NOW) */
	{
		char buf2[BUFLEN];
		memset(buf2, 0x00, BUFLEN);
		sscanf(buf, "%*d;%[^$]", buf2);
		fprintf (stdout,"Result:\n");
		fprintf (stdout, "%s", buf2);
	}
	jb_close(sock);
	free(buf);
}

void print_formatted_turtles (char* buf)
{
	char* p;
	int age, weight, id, pts;
	char name [BUFLEN];

	fprintf (stdout, "%6s%20s%6s%7s%8s\n", "id", "name", "age", "weight", "points");
	p = strtok(buf, ";");
	memset(name, 0x00, BUFLEN);
	sscanf(p, "%d:%[^:]:%d:%d:%d", &id, name, &age, &weight, &pts);
	fprintf (stdout, "%6d%20s%6d%7d%8d\n", id, name, age, weight, pts);
	while ((p = strtok(NULL, ";")) != NULL)
	{
		if (strlen(p) == 0)
			break;
		memset(name, 0x00, BUFLEN);
		sscanf(p, "%d:%[^:]:%d:%d:%d", &id, name, &age, &weight, &pts);
		fprintf (stdout, "%6d%20s%6d%7d%8d\n", id, name, age, weight, pts);
	}
}
void print_formatted_tracks (char* buf)
{
	char* p;
	int chpoints, laps, length;
	char name [BUFLEN];

	fprintf (stdout, "%20s%10s%6s%8s\n", "name", "chpoints", "laps", "length");
	if (buf == NULL || strlen(buf) <4)
		return;
	p = strtok(buf, ";");
	memset(name, 0x00, BUFLEN);
	sscanf(p, "%[^:]:%d:%d:%d", name, &chpoints, &laps, &length);
	fprintf (stdout, "%20s%10d%6d%8d\n", name, chpoints, laps, length);
	while ((p = strtok(NULL, ";")) != NULL)
	{
		if (strlen(p) == 0)
			break;
		memset(name, 0x00, BUFLEN);
		sscanf(p, "%[^:]:%d:%d:%d", name, &chpoints, &laps, &length);
		fprintf (stdout, "%20s:%10d:%6d:%8d\n", name, chpoints, laps, length);
	}
}
