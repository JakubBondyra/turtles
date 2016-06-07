#include "client-handlers.h"

char* client_commands [] = {QUITCOMMAND, ADDTURTLECOMMAND, ADDTRACKCOMMAND, STARTRACECOMMAND,
	ENDRACECOMMAND, RACENEWSCOMMAND, NEWSEASONCOMMAND, LIVEREQUESTCOMMAND, SEQREQUESTCOMMAND, TABREQUESTCOMMAND,
	GETTURTLESCOMMAND, GETTRACKSCOMMAND};
char* client_infos [] = {QUITINFO, ADDTURTLEINFO, ADDTRACKINFO, STARTRACEINFO,
	ENDRACEINFO, RACENEWSINFO, NEWSEASONINFO, LIVEREQUESTINFO, SEQREQUESTINFO, TABREQUESTINFO,
	GETTURTLESINFO, GETTRACKSINFO};
	
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
	run_ui (port, address, client_commands, client_infos, 12);
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
		fprintf (stdout, "*** Type single \'a\' to abort the current command at any time.\n");
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
			return get_handler (port, address, buf);
		}
	}
	return 0;
}

int get_handler (int port, char* address, char* command)
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
		int who, lap, where;
		if (!verify_3param_command(command, &who, &lap, &where))
			return 0;

		race_news_handler (port, address, who, lap, where);
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
	else
		return 0;
		
	return 1;
}

void quit_handler ()
{
	exit(EXIT_SUCCESS);
}

void add_turtle_handler (int port, char* address)
{
	int id, age, weight;
	char* name = get_buffer(NAMELEN);
	char* buf = get_buffer(BUFLEN);
	int len = 0;
	memset(name, 0x00, NAMELEN);
	if (!get_int("Please specify id of turtle", &id))
	{
		abort_creation(buf, name);
		return;
	}
	if (!get_string("Please specify name of turtle", &name, NAMELEN))
	{
		abort_creation(buf, name);
		return;
	}
	if (!get_int("Please specify age of turtle", &age))
	{
		abort_creation(buf, name);
		return;
	}
	if (!get_int("Please specify weight of turtle", &weight))
	{
		abort_creation(buf, name);
		return;
	}
	len = snprintf(buf, BUFLEN, "%d;%d:%s:%d:%d%c", ADDTURTLEID, id, name, age, weight, MSG_TERMINATOR);
	send_request(port, address, buf, len);
	free(buf);
	free(name);
}
void add_track_handler (int port, char* address)
{
	int chpoints, laps, length;
	char* name = (char*) jb_malloc (sizeof(char)*NAMELEN);
	char* buf = get_buffer(BUFLEN);
	char* chlengths = get_buffer(CHPTLEN);
	int len = 0;
	if (!get_string("Please specify name of track", &name, NAMELEN))
	{
		free(chlengths);
		abort_creation(buf, name);
		return;
	}
	if (!get_int("Please specify number of checkpoints on each lap", &chpoints))
	{
		free(chlengths);
		abort_creation(buf, name);
		return;
	}
	if (!get_int("Please specify number of laps", &laps))
	{
		free(chlengths);
		abort_creation(buf, name);
		return;
	}
	if (!get_int("Please specify length of track", &length))
	{
		free(chlengths);
		abort_creation(buf, name);
		return;
	}
	fprintf (stdout, "Now, specify checkpoints' distances from start.\n");
	if (!get_comma_list("Please specify checkpoint", &chlengths, CHPTLEN, chpoints, length))
	{
		free(chlengths);
		abort_creation(buf, name);
		return;
	}
	len = snprintf(buf, BUFLEN, "%d;%s:%d:%d:%d:%s%c", ADDTRACKID, name, chpoints, laps, length, chlengths, MSG_TERMINATOR);
	send_request(port, address, buf, len);
	free(buf);
	free(chlengths);
	free(name);
}
void start_race_handler (int port, char* address)
{
	char* buf = get_buffer(BUFLEN);
	int len;
	char* name = get_buffer(NAMELEN);
	char* players = get_buffer(CHPTLEN);
	if (!get_string("Please specify name of track", &name, NAMELEN))
	{
		abort_creation(buf, name);
		return;
	}
	fprintf (stdout, "Now, specify participants' ids (turtle ids).\n");
	if (!get_players("Please specify player", &players, CHPTLEN))
	{
		abort_creation(buf, name);
		return;
	}
	len = snprintf(buf, BUFLEN, "%d;%s:%s%c", STARTRACEID, name, players, MSG_TERMINATOR);
	send_request(port, address, buf, len);
	free(buf);
	free(players);
	free(name);
}
void end_race_handler (int port, char* address)
{
	char* buf = get_buffer(BUFLEN);
	int len = snprintf(buf, BUFLEN, "%d%c", ENDRACEID, MSG_TERMINATOR);
	send_request(port, address, buf, len);
	free(buf);
}
void race_news_handler (int port, char* address, int who, int lap, int where)
{
	char* buf = get_buffer(BUFLEN);
	int len = snprintf(buf, BUFLEN, "%d;%d:%d:%d%c", RACENEWSID, who, lap, where, MSG_TERMINATOR);
	send_request(port, address, buf, len);
	free(buf);
}
void new_season_handler (int port, char* address)
{
	char* buf = get_buffer(BUFLEN);
	int len = snprintf(buf, BUFLEN, "%d%c", NEWSEASONID, MSG_TERMINATOR);
	send_request(port, address, buf, len);
	free(buf);
}
void live_request_handler (int port, char* address)
{
	char* buf = get_buffer(BUFLEN);
	int len = snprintf(buf, BUFLEN, "%d%c", LIVEREQUESTID, MSG_TERMINATOR);
	send_live_request(port, address, buf, len);
	free(buf);
}
void seq_request_handler (int port, char* address)
{
	char* buf = get_buffer(BUFLEN);
	int len = snprintf(buf, BUFLEN, "%d%c", SEQREQUESTID, MSG_TERMINATOR);
	send_request(port, address, buf, len);
	free(buf);
}
void tab_request_handler (int port, char* address)
{
	char* buf = get_buffer(BUFLEN);
	int len = snprintf(buf, BUFLEN, "%d%c", TABREQUESTID, MSG_TERMINATOR);
	send_request(port, address, buf, len);
	free(buf);
}
void get_turtles_handler (int port, char* address)
{
	char* buf = get_buffer(BUFLEN);
	int len = snprintf(buf, BUFLEN, "%d%c", GETTURTLESID, MSG_TERMINATOR);
	send_request(port, address, buf, len);
	free(buf);
}
void get_tracks_handler (int port, char* address)
{
	char* buf = get_buffer(BUFLEN);
	int len = snprintf(buf, BUFLEN, "%d%c", GETTRACKSID, MSG_TERMINATOR);
	send_request(port, address, buf, len);
	free(buf);
}

void send_request (int port, char* address, char* buf, int count)
{
	/* send message feature */
	int sock;
	jb_internet_stream_socket_client(&sock, address, (uint16_t) port);
	dollar_write(sock, buf, count);
	handle_response(sock);
}

void send_live_request(int port, char* address, char* buf, int count)
{
	/* send message feature */
	int sock;
	jb_internet_stream_socket_client(&sock, address, (uint16_t) port);
	dollar_write(sock, buf, count);
	listen_to_updates(sock);
}

void handle_response (int sock)
{
	char* buf = get_buffer(BUFLEN);
	int msgid;
	if (!dollar_read(sock, buf, BUFLEN))
	{
		fprintf (stdout, "No response received.\n");
		jb_close(sock);
		free(buf);
		return;
	}
	if (strlen(buf) < 2)
	{
		fprintf (stdout, "Error of communication (too short message)\n");
		free(buf);
		return;
	}
	msgid = get_msg_type(buf);
	if (msgid == ERRID)
	{
		char* buf2 = chop_endings(buf, BUFLEN);
		fprintf(stdout, "Server reported error: %s\n", buf2);
		free(buf2);
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
	else
	{
		char* buf2 = chop_endings(buf, BUFLEN);
		if (msgid==GETTURTLESID)
		{
			display_formatted_turtles(buf2);
		}
		else if (msgid == GETTRACKSID)
		{
			display_formatted_tracks(buf2);
		}
		else if (msgid == SEQREQUESTID)
		{
			display_formatted_sequence(buf2);
		}
		else if (msgid == TABREQUESTID)
		{
			display_formatted_table(buf2);
		}
		free(buf2);
	}
	jb_close(sock);
	free(buf);
}

void abort_creation (char* buf1, char* buf2)
{
	free(buf1);
	free(buf2);
	fprintf (stdout, "Aborted.\n");
}

void listen_to_updates(int sock)
{
	char* buf = get_buffer(BUFLEN);
	int msgid;
	fprintf (stdout, "Request sent. Listening to online features...\n");
	while (1)
	{
		if (!dollar_read_signal_sensitive(sock, buf, BUFLEN))
		{
			fprintf (stdout, "Nothing to read from socket, returning.\n");
			jb_close(sock);
			free(buf);
			return;
		}
		msgid = get_msg_type(buf);
		if (msgid == NEWSPOSCHANGEDID)
		{
			char* buf2 = chop_endings(buf, BUFLEN);
			if (!print_pos_changed(buf2, BUFLEN))
				break;
		}
		else if (msgid == NEWSRACEENDID)
		{
			fprintf (stdout, "Whole race has finished.\n");
			break;
		}
		else if (msgid == ERRID)
		{
			char* buf2 = chop_endings(buf, BUFLEN);
			fprintf(stdout, "Server reported error: %s\n", buf2);
			free(buf2);
			break;
		}
		else
		{
			fprintf (stdout, "Wrong response received from server.\n");
			break;
		}
		memset(buf, 0x00, BUFLEN);
	}
	fprintf (stdout, "Listening to online features finished.\n");
	free(buf);
	jb_close(sock);
}

