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
	
	len = strlen(buf);	
	for (i=0;i<ccount;i++)
	{
		if (len == strlen(commands[i]) && !strncmp(buf, commands[i], len))
		{
			get_handler (port, address, commands[i]);
			return 1;
		}
	}
	return 0;
}

void get_handler (int port, char* address, char* command)
{
	if (!strncmp (command, QUITCOMMAND, strlen(command)))
	{
		quit_handler();
	}
	else if (!strncmp(command, ADDTURTLECOMMAND, strlen(command)))
	{
		add_turtle_handler (port, address);
	}
	else if (!strncmp(command, ADDTRACKCOMMAND, strlen(command)))
	{
		add_track_handler (port, address);
	}
	else if (!strncmp(command, STARTRACECOMMAND, strlen(command)))
	{
		start_race_handler (port, address);
	}
	else if (!strncmp(command, ENDRACECOMMAND, strlen(command)))
	{
		end_race_handler (port, address);
	}
	else if (!strncmp(command, RACENEWSCOMMAND, strlen(command)))
	{
		race_news_handler (port, address);
	}
	else if (!strncmp(command, NEWSEASONCOMMAND, strlen(command)))
	{
		new_season_handler (port, address);
	}
	else if (!strncmp(command, LIVEREQUESTCOMMAND, strlen(command)))
	{
		live_request_handler (port, address);
	}
	else if (!strncmp(command, SEQREQUESTCOMMAND, strlen(command)))
	{
		seq_request_handler (port, address);
	}
	else if (!strncmp(command, TABREQUESTCOMMAND, strlen(command)))
	{
		tab_request_handler (port, address);
	}
	else if (!strncmp(command, GETTURTLESCOMMAND, strlen(command)))
	{
		get_turtles_handler (port, address);
	}
	else if (!strncmp(command, GETTRACKSCOMMAND, strlen(command)))
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
	get_int("Please specify weight of turtle", &age);
	get_int("Please specify weight of turtle", &weight);
	memset(buf, 0x00, BUFLEN);
	len = snprintf(buf, BUFLEN, "%d;%d:%s:%d:%d$", ADDTURTLEID, id, name, age, weight);
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
	len = snprintf(buf, BUFLEN, "%d;%s:%d:%d:%d$", ADDTRACKID, name, chpoints, laps, length);
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
	len = snprintf(buf, BUFLEN, "%d;%s$", STARTRACEID, name);
	send_message(port, address, buf, len);
	free(buf);
	free(name);
}
void end_race_handler (int port, char* address)
{
	char* buf = (char*) jb_malloc (sizeof(char) * BUFLEN);
	int len = 0;
	memset(buf, 0x00, BUFLEN);
	len = snprintf(buf, BUFLEN, "%d$", ENDRACEID);
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
	len = snprintf(buf, BUFLEN, "%d;%d:%d$", RACENEWSID, who, where);
	send_message(port, address, buf, len);
	free(buf);
}
void new_season_handler (int port, char* address)
{
	char* buf = (char*) jb_malloc (sizeof(char) * BUFLEN);
	int len = 0;
	memset(buf, 0x00, BUFLEN);
	len = snprintf(buf, BUFLEN, "%d$", NEWSEASONID);
	send_message(port, address, buf, len);
	free(buf);
}
void live_request_handler (int port, char* address)
{
	/* TODO */
	char* buf = (char*) jb_malloc (sizeof(char) * BUFLEN);
	int len = 0;
	memset(buf, 0x00, BUFLEN);
	len = snprintf(buf, BUFLEN, "%d$", LIVEREQUESTID);
	send_message(port, address, buf, len);
	free(buf);
}
void seq_request_handler (int port, char* address)
{
	char* buf = (char*) jb_malloc (sizeof(char) * BUFLEN);
	int len = 0;
	memset(buf, 0x00, BUFLEN);
	len = snprintf(buf, BUFLEN, "%d$", SEQREQUESTID);
	send_message(port, address, buf, len);
	free(buf);
}
void tab_request_handler (int port, char* address)
{
	char* buf = (char*) jb_malloc (sizeof(char) * BUFLEN);
	int len = 0;
	memset(buf, 0x00, BUFLEN);
	len = snprintf(buf, BUFLEN, "%d$", TABREQUESTID);
	send_message(port, address, buf, len);
	free(buf);
}
void get_turtles_handler (int port, char* address)
{
	char* buf = (char*) jb_malloc (sizeof(char) * BUFLEN);
	int len = 0;
	memset(buf, 0x00, BUFLEN);
	len = snprintf(buf, BUFLEN, "%d$", GETTURTLESID);
	send_message(port, address, buf, len);
	free(buf);
}
void get_tracks_handler (int port, char* address)
{
	char* buf = (char*) jb_malloc (sizeof(char) * BUFLEN);
	int len = 0;
	memset(buf, 0x00, BUFLEN);
	len = snprintf(buf, BUFLEN, "%d$", GETTRACKSID);
	send_message(port, address, buf, len);
	free(buf);
}

void send_message (int port, char* address, char* buf, int count)
{
	/* send message feature */
	int sock;
	jb_internet_stream_socket_client(&sock, address, (uint16_t) port);
	socket_write(sock, buf, count);
	handle_response(sock);
}

void handle_response (int sock)
{
	char* buf = (char*) jb_malloc (sizeof(char) * BUFLEN);
	int msgid;
	memset(buf, 0x00, BUFLEN);
	socket_read(sock, buf, BUFLEN);
	if (strlen(buf) < 2)
	{
		fprintf (stdout, "Error of communication (too short message)\n");
		free(buf);
		return;
	}
	msgid = buf[0]*10 + (int)buf[1];
	if (msgid == ERRID)
	{
		fprintf(stdout, "Server reported error.\n");
	}
	else if (msgid == SUCCESSID)
	{
		fprintf(stdout, "Success.\n");
	}
	else /* GET TABLE, GET SEQUENCE, GET ALL TURTLES or GET ALL TRACKS */
	{
		char buf2[BUFLEN];
		memset(buf2, 0x00, BUFLEN);
		sscanf(buf, "%d;%s$", &msgid, buf2);
		fprintf (stdout,"Result:\n");
		fprintf (stdout, "%s", buf2);
	}
	free(buf);
}
