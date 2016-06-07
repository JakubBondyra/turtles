#include "server-handlers.h"

volatile sig_atomic_t running = 1;

void sigint_handler (int sig)
{
	running = 0;
}

void server_loop (int sock, struct turtle_group* turtles, struct track_group* tracks, int raceId)
{
	char* buf = get_buffer(BUFLEN);
	char* newsBuf = get_buffer(BUFLEN);
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
		msg = get_msg_type (buf);
		
		if (msg == ADDTURTLEID)
		{
			/* add turtle in msg to turtle_group and to file */
			handle_add_turtle(fd, raceId, turtles, buf);
			jb_close(fd);
		}
		else if (msg == ADDTRACKID)
		{
			/* add track in msg to track_group and to file */
			handle_add_track(fd, raceId, tracks, buf);
			jb_close(fd);
		}
		else if (msg == STARTRACEID)
		{
			/* start race (specified name of track) */
			handle_start_race(fd, buf, &raceId, turtles, tracks);
			jb_close(fd);
		}
		else if (msg == ENDRACEID)
		{
			/* end current race */
			handle_end_race (fd, buf, &raceId, turtles, &newsBuf);
			jb_close(fd);
		}
		else if (msg == RACENEWSID)
		{
			/* update position of turtle (turtle id) to checkpoint (checkpoint) */
			handle_update_race (fd, buf, raceId, turtles, *tracks, &newsBuf);
			jb_close(fd);
		}
		else if (msg == NEWSEASONID)
		{
			/* just reset table and write to file */
			handle_reset_table(fd, turtles, raceId);
			jb_close(fd);
		}
		else if (msg == LIVEREQUESTID)
		{
			/* live request feature, new thread */
			handle_live_request (fd, raceId, turtles, tracks, &newsBuf);
		}
		else if (msg == SEQREQUESTID)
		{
			/* get current sequence*/
			print_sequence (fd, raceId, *tracks, *turtles);
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
			fprintf (stderr, "Communication error. Malformed buffer.\n");
			fflush(stderr);
			print_error(fd, "Communication error. Malformed buffer.");
			jb_close(fd);
		}
	}
	free(buf);
	free(newsBuf);
}

void server_work(int port)
{
	struct turtle_group* turtles;
	struct track_group* tracks;
	int currRacePlaceId = -1;
	int sock;
	load_state(&turtles, &tracks);
	
	jb_internet_stream_socket_mbl(&sock, (uint16_t)port, BACKLOG);
	server_loop(sock, turtles, tracks, currRacePlaceId);
	
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
