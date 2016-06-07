#include "server-handlers.h"

pthread_cond_t	cond = PTHREAD_COND_INITIALIZER;
pthread_mutex_t	mutex = PTHREAD_MUTEX_INITIALIZER;
int listener_number = 0;

void handle_add_turtle(int fd, int currRace, struct turtle_group* turtles, char* buf)
{
	struct turtle t;
	int no = -1;
	fprintf (stderr, "Handling (%s)\n", buf);
	if ((no=sscanf(buf, "%*d;%d:%[^:]:%d:%d$", &(t.id), t.name, &(t.age), &(t.weight))) != 4)
	{
		print_error(fd, "Wrong request structure.");
		return;
	}
	
	t.currPoints = 0;
	t.currLap = -1;
	t.currChp = -1;
	append_turtle(turtles, t);
	save_turtle(TURTLEPATH, &t);
	print_success(fd);
}

void handle_add_track(int fd, int currRace, struct track_group* tracks, char* buf)
{
	int no = -1;
	struct track t;
	fprintf (stderr, "Handling (%s)\n", buf);
	if ((no=sscanf(buf, "%*d;%[^:]:%d:%d:%d:%[0123456789,]", t.name, &(t.checkpoint_number), &(t.laps), &(t.length), t.chlengths)) != 5)
	{
		print_error(fd, "Wrong request structure");
		return;
	}
	
	append_track(tracks, t);
	save_track(TRACKPATH, &t);
	print_success(fd);
}

void handle_start_race(int fd, char* buf, int* currRace, struct turtle_group* tu, struct track_group* tr)
{
	char* name = get_buffer(NAMELEN);
	char* players = get_buffer(CHPTLEN);
	int i;
	memset(name, 0x00, BUFLEN);
	memset(players, 0x00, CHPTLEN);
	fprintf (stderr, "Handling (%s)\n", buf);
	if (*currRace != -1)
	{
		print_error(fd, "There is already race ongoing.");
		return;
	}
	if (sscanf (buf, "%*d;%[^:]:%[^$]", name, players) != 2)
	{
		print_error(fd, "Wrong request structure.");
		return;
	}
	for (i=0;i<tr->number;i++)
	{
		if (!strncmp(tr->tracks[i].name, name, strlen(name)))
		{
			*currRace = i;
			break;
		}
	}
	if (*currRace == -1)
	{
		print_error(fd, "No track for given name.");
		return;
	}
	if (get_players_from_msg (tu, players))
	{
		print_success(fd);
	}
	else
	{
		print_error(fd, "Wrong players' ids.");
	}
	fprintf (stderr, "START RACE, place: %s\n", name);
	free(players);
	free(name);
}

void handle_end_race (int fd, char* buf, int* currRace, struct turtle_group* tu, char** news_buffer)
{
	int i;
	fprintf (stderr, "Handling (%s)\n", buf);
	if (*currRace == -1)
	{
		print_error(fd, "There is no race to end.");
		return;
	}

	*currRace = -1;
	for (i=0;i<tu->number;i++)
	{
		tu->turtles[i].currLap = -1;
		tu->turtles[i].currChp = -1;
	}
	save_turtles(TURTLEPATH, *tu);
	print_success(fd);
	relay_end_race(news_buffer);
	listener_number = 0;
	fprintf (stderr, "END RACE\n");
}

void handle_update_race (int fd, char* buf, int currRace, struct turtle_group* tu, struct track_group tr, char** news_buffer)
{
	int who, pos, laps, rpos, rlaps, cpos, claps, i, ind = -1;
	fprintf (stderr, "Handling (%s)\n", buf);
	if (currRace == -1)
	{
		print_error(fd, "No race is ongoing.");
		return;
	}
	if (sscanf(buf, "%*d;%d:%d:%d$", &who, &laps, &pos) != 3)
	{
		print_error(fd, "Wrong request structure.");
		return;
	}
	
	for (i=0;i<tu->number;i++)
	{
		if (tu->turtles[i].id == who && tu->turtles[i].currLap >=0 && tu->turtles[i].currChp >= 0)
		{
			ind = i;
			break;
		}
	}
	if (ind == -1)
	{
		print_error(fd, "Player doesn't exist or doesn't attend a race.");
		return;
	}
	
	rpos = tr.tracks[currRace].checkpoint_number;
	rlaps = tr.tracks[currRace].laps;
	cpos = tu->turtles[ind].currChp;
	claps = tu->turtles[ind].currLap;

	if (laps <= 0 || laps > rlaps+1 || pos < 0 || pos > rpos)
	{
		print_error(fd, "Wrong lap or checkpoint specified.");
		return;
	}
	if (cpos > pos && claps >= laps)
	{
		print_error(fd, "Turtle is going backwards.");
		return;
	}
	if (claps>rlaps)
	{
		print_error(fd, "Turtle has already finished race.");
		return;
	}
	
	tu->turtles[ind].currLap = laps;
	tu->turtles[ind].currChp = pos;
	
	if (laps > rlaps)
	{
		int won_count=0;
		int i;
		fprintf (stderr, "Turtle finished the race.\n");
		for (i=0;i<tu->number;i++)
		{
			if (tu->turtles[i].currLap > rlaps)
				won_count++;
		}
		if (won_count>4)
		{
			print_success(fd);
			return;
		}
		
		/* 25 pkt */
		if (won_count == 1)
		{
			tu->turtles[ind].currPoints += 25;
		} /* 10 pkt */
		else if (won_count == 2)
		{
			tu->turtles[ind].currPoints += 10;
		}/* 5 pkt */
		else if (won_count == 3)
		{
			tu->turtles[ind].currPoints += 5;
		}/* 1 pkt */
		else if (won_count == 4)
		{
			tu->turtles[ind].currPoints += 1;
		}
		else
		{
			fprintf (stderr, "Not granted any points.\n");
		}
	}
	print_success(fd);
	/* -1 as signal of finished race */
	relay_news(news_buffer, tu->turtles[ind].name, laps > rlaps ? -1 : laps, pos);
}

void handle_reset_table(int fd, struct turtle_group* tu, int currRace)
{
	int i;
	fprintf (stderr, "Handling reset race\n");
	if (currRace != -1)
	{
		print_error(fd, "Race is ongoing. Cannot reset table.");
		return;
	}
	
	for (i=0;i<tu->number;i++)
	{
		tu->turtles[i].currPoints = 0;
	}
	save_turtles(TURTLEPATH, *tu);
	print_success(fd);
}

void handle_live_request (int fd, int currRace, struct turtle_group* turtles, struct track_group* tracks, char** news_buffer)
{
	int err = 0;
	pthread_t tid;
	struct thread_data* threadData = (struct thread_data*) jb_malloc (sizeof(struct thread_data));
	fprintf (stderr, "Handling live request\n");
	if (currRace == -1)
	{
		print_error(fd, "No race is ongoing.");
		jb_close(fd);
		return;
	}
	threadData->newsBuffer = news_buffer;
	threadData->fd = fd;
	err = pthread_create(&tid, NULL, wait_for_news, (void*)threadData);
	if (err != 0){
		fprintf(stderr, "Thread creation failed: %d", err);
		print_error(fd, "Failed to create handler thread.");
		jb_close(fd);
		return;
	}
	/* error impossible */
	pthread_detach(tid);
	listener_number++;
}

void print_sequence (int fd, int currRace, struct track_group tr, struct turtle_group tu)
{
	char* buf = get_buffer(BUFLEN);
	int i;
	int written = 0;
	int pos = 0;
	fprintf (stderr, "Handling print sequence\n");
	if (currRace == -1)
	{
		print_error(fd, "No race right now.");
		free(buf);
		return;
	}
	/* header */
	written = snprintf(buf, BUFLEN, "%d%c", SEQREQUESTID, MSG_PARTS_DELIMITER);
	pos+=written;
	
	for (i=0;i<tu.number;i++)
	{
		if (tu.turtles[i].currLap >= 0 && tu.turtles[i].currChp >=0)
		{
			written = snprintf(buf+pos,BUFLEN-pos, "%s:%d:%d%c", tu.turtles[i].name, tu.turtles[i].currLap, tu.turtles[i].currChp, MSG_PARTS_DELIMITER);
			pos+=written;
		}
	}
	written = snprintf(buf+pos, BUFLEN-pos, "%c",  MSG_TERMINATOR);
	pos += written;
	dollar_write(fd, buf, pos);
	free(buf);
}

void print_table(int fd, struct turtle_group tu)
{
	char* buf = get_buffer(BUFLEN);
	int i;
	int written = 0;
	int pos = 0;
	memset(buf, 0x00, BUFLEN);
	fprintf (stderr, "Handling print table\n");
	
	written = snprintf(buf, BUFLEN, "%d%c", TABREQUESTID, MSG_PARTS_DELIMITER);
	pos+=written;
	
	for (i=0;i<tu.number;i++)
	{
		written = snprintf(buf+pos,BUFLEN-pos, "%s:%d%c", tu.turtles[i].name, tu.turtles[i].currPoints, MSG_PARTS_DELIMITER);
		pos+=written;
	}
	written = snprintf(buf+pos, BUFLEN-pos, "%c", MSG_TERMINATOR);
	pos += written;
	
	dollar_write(fd, buf, pos);
	free(buf);
}

void print_all_turtles (int fd, struct turtle_group tu)
{
	char* buf = get_buffer(BUFLEN);
	int i;
	int written = 0;
	int pos = 0;
	memset(buf, 0x00, BUFLEN);
	fprintf (stderr, "Handling print turtles\n");
	/* header */
	written = snprintf(buf, BUFLEN, "%d%c", GETTURTLESID, MSG_PARTS_DELIMITER);
	pos+=written;	
	
	for (i=0;i<tu.number;i++)
	{
		written = snprintf(buf+pos,BUFLEN-pos, "%d:%s:%d:%d:%d%c", tu.turtles[i].id, tu.turtles[i].name, tu.turtles[i].age, tu.turtles[i].weight, tu.turtles[i].currPoints, MSG_PARTS_DELIMITER);
		pos+=written;
	}
	written = snprintf(buf+pos, BUFLEN-pos, "%c", MSG_TERMINATOR);
	pos += written;
	dollar_write(fd, buf, pos);
	free(buf);
}

void print_all_tracks (int fd, struct track_group tr)
{
	char* buf = get_buffer(BUFLEN);
	int i;
	int written = 0;
	int pos = 0;
	memset(buf, 0x00, BUFLEN);
	fprintf (stderr, "Handling print tracks\n");
	/* header */
	written = snprintf(buf, BUFLEN, "%d%c", GETTRACKSID, MSG_PARTS_DELIMITER);
	pos+=written;
	
	for (i=0;i<tr.number;i++)
	{
		written = snprintf(buf+pos,BUFLEN-pos, "%s:%d:%d:%d:%s%c", tr.tracks[i].name, tr.tracks[i].checkpoint_number, tr.tracks[i].laps, tr.tracks[i].length, tr.tracks[i].chlengths, MSG_PARTS_DELIMITER);
		pos+=written;
	}
	written = snprintf(buf+pos, BUFLEN-pos, "%c", MSG_TERMINATOR);
	pos += written;
	dollar_write(fd, buf, pos);
	free(buf);
}

void* wait_for_news (void* tdv)
{
	struct thread_data* data = (struct thread_data*) tdv;
	int ret = -1;
	while (1)	
	{
		pthread_mutex_lock (&mutex);
		pthread_cond_wait(&cond,&mutex);
		ret = print_news(*data);
		pthread_mutex_unlock(&mutex);
		if (ret == 0)
		{
			listener_number--;
			break;
		}
	}
	jb_close(data->fd);
	free(data);
	return NULL;
}

void relay_news (char** news_buffer, char* who, int laps, int pos)
{
	int i;
	if (listener_number==0)
		return;
	fprintf (stderr, "Relaying new message to handler threads.\n");
	fflush(stdout);
	pthread_mutex_lock(&mutex);
	memset(*news_buffer, 0x00, BUFLEN);
	snprintf(*news_buffer, BUFLEN, "%d%c%s:%d:%d%c", NEWSPOSCHANGEDID, MSG_PARTS_DELIMITER, who, laps, pos, MSG_TERMINATOR);
	fflush(stdout);
	for (i=0;i<listener_number;i++)
		pthread_cond_signal(&cond);
	pthread_mutex_unlock(&mutex);
}

void relay_end_race (char** news_buffer)
{
	int i;
	if (listener_number==0)
		return;
	fprintf (stderr, "Relaying new message to handler threads.\n");
	fflush(stderr);
	pthread_mutex_lock(&mutex);
	memset(*news_buffer, 0x00, BUFLEN);
	snprintf(*news_buffer, BUFLEN, "%d%c", NEWSRACEENDID, MSG_TERMINATOR);
	fflush(stdout);
	for (i=0;i<listener_number;i++)
		pthread_cond_signal(&cond);
	pthread_mutex_unlock(&mutex);
}

int print_news (struct thread_data data)
{
	int ret;
	int id;
	char* mbuf = get_buffer(BUFLEN);
	strncpy(mbuf, *(data.newsBuffer), BUFLEN);
	ret = dollar_write(data.fd, mbuf, strlen(*(data.newsBuffer)));
	id = get_msg_type(mbuf);
	if (id == NEWSRACEENDID)
		ret = 0;
	free(mbuf);
	return ret;
}
