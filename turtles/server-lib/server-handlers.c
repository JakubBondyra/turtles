#include "server-handlers.h"

void handle_add_turtle(int fd, int currRace, struct turtle_group* turtles, char buf[BUFLEN])
{
	struct turtle t;
	
	if (currRace != -1)
	{
		print_error(fd, "Adding not possible during ongoing race.");
		return;
	}
	
	if (sscanf(buf, "%*d;%d:%[^:]:%d:%d$", &(t.id), t.name, &(t.age), &(t.weight)) != 4)
	{
		print_error(fd, "Wrong request structure.");
		return;
	}
	
	t.currPoints = 0;
	t.currLap = 0;
	t.currChp = 0;
	append_turtle(turtles, t);
	save_turtle(TURTLEPATH, &t);
	print_success(fd);
}

void handle_add_track(int fd, int currRace, struct track_group* tracks, char buf[BUFLEN])
{
	int chuj = -1;
	struct track t;
	
	if (currRace != -1)
	{
		print_error(fd, "Adding not possible during ongoing race.");
		return;
	}
	
	if ((chuj=sscanf(buf, "%*d;%[^:]:%d:%d:%d:%[0123456789,]", t.name, &(t.checkpoint_number), &(t.laps), &(t.length), t.chlengths)) != 5)
	{
		fprintf (stdout, "Assigned: %d\n", chuj); 
		print_error(fd, "Wrong request structure");
		return;
	}
	
	append_track(tracks, t);
	save_track(TRACKPATH, &t);
	print_success(fd);
}

void handle_start_race(int fd, char buf[BUFLEN], int* currRace, struct track_group tr)
{
	char name [BUFLEN];
	char players[CHPTLEN];
	int i;
	memset(name, 0x00, BUFLEN);
	memset(players, 0x00, CHPTLEN);
	
	if (*currRace != -1)
	{
		print_error(fd, "There is already race ongoing.");
		return;
	}
	if (sscanf (buf, "%*d;%[^:]:%[^$]", name, players) != 1)
	{
		print_error(fd, "Wrong request structure.");
		return;
	}/*TODO! HANDLE PLAYERS IN RACE, ETC...*/
	for (i=0;i<tr.number;i++)
	{
		if (!strcmp(tr.tracks[i].name, name))
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
	
	print_success(fd);
}

void handle_end_race (int fd, char buf[BUFLEN], int* currRace, struct turtle_group* tu)
{
	int i;
	if (*currRace == -1)
	{
		print_error(fd, "There is no race to end.");
	}
	*currRace = -1;
	for (i=0;i<tu->number;i++)
	{
		tu->turtles[i].currLap = 0;
		tu->turtles[i].currChp = 0;
	}
	save_turtles(TURTLEPATH, *tu);
	print_success(fd);
}

void handle_update_race (int fd, char buf[BUFLEN], int currRace, struct turtle_group* tu, struct track_group tr)
{
	int who, pos, laps, rpos, rlaps, cpos, claps, i, ind = -1;
	
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
		if (tu->turtles[i].id == who)
		{
			ind = i;
			break;
		}
	}
	if (ind == -1)
	{
		print_error(fd, "Wrong player id specified.");
		return;
	}
	
	rpos = tr.tracks[currRace].checkpoint_number;
	rlaps = tr.tracks[currRace].laps;

	if (laps <= 0 || laps > rlaps+1 || pos < 0 || pos > rpos)
	{
		print_error(fd, "Wrong lap or checkpoint specified.");
		return;
	}
	
	cpos = tu->turtles[ind].currChp;
	claps = tu->turtles[ind].currLap;

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
		fprintf (stdout, "Turtle finished. Checking position.\n");
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
			fprintf (stdout, "Control message: turtle not granted any points\n");
		}
	}
	print_success(fd);
}

void handle_reset_table(int fd, struct turtle_group* tu, int currRace)
{
	int i;
	
	if (currRace != -1)
	{
		print_error(fd, "Race is ongoing. Cannot reset table.");
	}
	
	for (i=0;i<tu->number;i++)
	{
		tu->turtles[i].currPoints = 0;
	}
	save_turtles(TURTLEPATH, *tu);
	print_success(fd);
}

void print_sequence (int fd, int currRace, struct track_group tr, struct turtle_group tu)
{
	char buf [BUFLEN];
	int i;
	int written = 0;
	int pos = 0;
	memset(buf, 0x00, BUFLEN);
	
	/* header */
	written = snprintf(buf, BUFLEN, "%d%c", SEQREQUESTID, MSG_PARTS_DELIMITER);
	pos+=written;
	
	for (i=0;i<tu.number;i++)
	{
		written = snprintf(buf+pos,BUFLEN-pos, "%s:%d:%d%c", tu.turtles[i].name, tu.turtles[i].currLap, tu.turtles[i].currChp, MSG_PARTS_DELIMITER);
		pos+=written;
	}
	snprintf(buf+pos, BUFLEN-pos, "%c",  MSG_TERMINATOR);
	
	dollar_write(fd, buf, BUFLEN);
}

void print_table(int fd, struct turtle_group tu)
{
	char buf [BUFLEN];
	int i;
	int written = 0;
	int pos = 0;
	memset(buf, 0x00, BUFLEN);
	/* header */
	written = snprintf(buf, BUFLEN, "%d%c", TABREQUESTID, MSG_PARTS_DELIMITER);
	pos+=written;
	
	for (i=0;i<tu.number;i++)
	{
		written = snprintf(buf+pos,BUFLEN-pos, "%s:%d%c", tu.turtles[i].name, tu.turtles[i].currPoints, MSG_PARTS_DELIMITER);
		pos+=written;
	}
	snprintf(buf+pos, BUFLEN-pos, "%c", MSG_TERMINATOR);
	
	dollar_write(fd, buf, BUFLEN);
}

void print_all_turtles (int fd, struct turtle_group tu)
{
	char buf [BUFLEN];
	int i;
	int written = 0;
	int pos = 0;
	memset(buf, 0x00, BUFLEN);
	
	/* header */
	written = snprintf(buf, BUFLEN, "%d%c", GETTURTLESID, MSG_PARTS_DELIMITER);
	pos+=written;	
	
	for (i=0;i<tu.number;i++)
	{
		written = snprintf(buf+pos,BUFLEN-pos, "%d:%s:%d:%d:%d%c", tu.turtles[i].id, tu.turtles[i].name, tu.turtles[i].age, tu.turtles[i].weight, tu.turtles[i].currPoints, MSG_PARTS_DELIMITER);
		pos+=written;
	}
	snprintf(buf+pos, BUFLEN-pos, "%c", MSG_TERMINATOR);
	
	dollar_write(fd, buf, BUFLEN);
}

void print_all_tracks (int fd, struct track_group tr)
{
	char buf [BUFLEN];
	int i;
	int written = 0;
	int pos = 0;
	memset(buf, 0x00, BUFLEN);
	
	/* header */
	written = snprintf(buf, BUFLEN, "%d%c", GETTRACKSID, MSG_PARTS_DELIMITER);
	pos+=written;
	
	for (i=0;i<tr.number;i++)
	{
		written = snprintf(buf+pos,BUFLEN-pos, "%s:%d:%d:%d:%s%c", tr.tracks[i].name, tr.tracks[i].checkpoint_number, tr.tracks[i].laps, tr.tracks[i].length, tr.tracks[i].chlengths, MSG_PARTS_DELIMITER);
		pos+=written;
	}
	snprintf(buf+pos, BUFLEN-pos, "%c", MSG_TERMINATOR);
	
	dollar_write(fd, buf, BUFLEN);
}
