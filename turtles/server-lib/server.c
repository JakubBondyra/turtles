#include "server-handlers.h"

volatile sig_atomic_t running = 1;

void sigint_handler (int sig)
{
	running = 0;
}

void server_work(int port)
{
	struct turtle_group* turtles;
	struct track_group* tracks;
	int currRacePlaceId = -1;
	int sock;
	char buf[BUFLEN];
	init_all(&turtles, &tracks);
	read_turtles(TURTLEPATH, turtles);
	read_tracks(TRACKPATH, tracks);
	fprintf (stdout, "Server state loaded from files. Starting work...\n");
	jb_internet_stream_socket_mbl(&sock, (uint16_t)port, BACKLOG);
	while (running)
	{
		int fd, msg, read;
		memset(buf, 0x00, BUFLEN);
		
		jb_accept(sock, &fd);
		if (fd == -1)
		{
			fprintf (stdout, "Signal detected. Continuing...\n");
			continue;
		}
		if ((read=dollar_read(fd, buf, BUFLEN))==0)
		{
			jb_close(fd);
			continue;
		}
		fprintf (stdout,"Successfully read \"%s\" (%d bytes).\n", buf, read);
		fflush(stdout);
		msg = get_msg_type (buf);
		
		if (msg == ADDTURTLEID)
		{
			/* add turtle in msg to turtle_group and to file */
			handle_add_turtle(fd, currRacePlaceId, turtles, buf);
			jb_close(fd);
		}
		else if (msg == ADDTRACKID)
		{
			/* add track in msg to track_group and to file */
			handle_add_track(fd, currRacePlaceId, tracks, buf);
			jb_close(fd);
		}
		else if (msg == STARTRACEID)
		{
			/* start race (specified name of track) */
			handle_start_race(fd, buf, &currRacePlaceId, *tracks);
			jb_close(fd);
		}
		else if (msg == ENDRACEID)
		{
			/* end current race */
			handle_end_race (fd, buf, &currRacePlaceId, turtles);
			jb_close(fd);
		}
		else if (msg == RACENEWSID)
		{
			/* update position of turtle (turtle id) to checkpoint (checkpoint) */
			handle_update_race (fd, buf, currRacePlaceId, turtles, *tracks);
			jb_close(fd);
		}
		else if (msg == NEWSEASONID)
		{
			/* just reset table and write to file */
			handle_reset_table(fd, turtles, currRacePlaceId);
			jb_close(fd);
		}
		else if (msg == LIVEREQUESTID)
		{
			/* live request feature, new thread */
			/* TODO */
			jb_close(fd);
		}
		else if (msg == SEQREQUESTID)
		{
			/* get current sequence*/
			print_sequence (fd, currRacePlaceId, *tracks, *turtles);
			jb_close(fd);
		}
		else if (msg == TABREQUESTID)
		{
			/* respond with table in msg*/
			print_table(fd, *turtles);
			jb_close(fd);
		}
		else if (msg == GETTURTLESID)
		{
			/*response with all turtles in message */
			print_all_turtles (fd, *turtles);
			jb_close(fd);
		}
		else if (msg == GETTRACKSID)
		{
			/*response with all tracks in message */
			print_all_tracks (fd, *tracks);
			jb_close(fd);
		}
		else
		{
			fflush(stderr);
			print_error(fd, "Communication error. Malformed buffer.");
			jb_close(fd);
		}
	}
	free_all(turtles, tracks);
}

int main (int argc, char* argv[])
{
	int port;

	if (argc != 2)
	{
		fprintf (stdout, "Usage: %s [PORT]\n", argv[0]);
		exit(EXIT_SUCCESS);
	}
	port = atoi(argv[1]);
	if (port==0)
	{
		fprintf (stderr, "Wrong argument!\n");
		exit(EXIT_FAILURE);
	}
	
	jb_sethandler(sigint_handler, SIGINT);
	server_work(port);

	 
	exit (EXIT_SUCCESS);
}
