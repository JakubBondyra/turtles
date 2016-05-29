#include "server-features.h"

void read_turtles (char* filepath, struct turtle_group* t)
{
	FILE* f = fopen (filepath, "ab+");
	char buf [FILELINE];
	if (f==NULL)
	{
		jb_print_error("fopen:");
	}
	t->number=0;
	
	while (!feof(f))
	{
		struct turtle newTurtle;
		
		memset(buf, 0x00, FILELINE);
		memset(&newTurtle, 0x00, sizeof(newTurtle));
		
		if (NULL == fgets(buf, FILELINE, f))
			exit(EXIT_FAILURE);
			
		parse_turtle(&newTurtle, buf);
		append_turtle(t, newTurtle);	
	}
	
	if (-1 == fclose(f))
	{
		jb_print_error("fclose:");
	}
}

void save_turtle (char* filepath, struct turtle t)
{
	FILE* f = fopen (filepath, "ab+");
	if (f==NULL)
	{
		jb_print_error("fopen:");
	}
	
	fprintf (f, "%d:%s:%d:%d:%d\n", t.id, t.name, t.age, t.weight, t.currPoints);
	
	if (-1 == fclose(f))
	{
		jb_print_error("fclose:");
	}
}

void save_turtles (char* filepath, struct turtle_group t)
{
	FILE* f;
	int i;
	
	jb_unlink(filepath);
	f = fopen (filepath, "ab+");
	if (f==NULL)
	{
		jb_print_error("fopen:");
	}
	for (i=0;i<t.number;i++)
	{
		fprintf (f, "%d:%s:%d:%d:%d\n", t.turtles[i].id, t.turtles[i].name, t.turtles[i].age, t.turtles[i].weight, t.turtles[i].currPoints);
	}
	if (-1 == fclose(f))
	{
		jb_print_error("fclose:");
	}
}

void read_tracks (char* filepath, struct track_group* t)
{
	FILE* f = fopen (filepath, "ab+");
	char buf [FILELINE];
	if (f==NULL)
	{
		jb_print_error("fopen:");
	}
	t->number=0;
	
	while (!feof(f))
	{
		struct track newTrack;
		
		memset(buf, 0x00, FILELINE);
		memset(&newTrack, 0x00, sizeof(newTrack));
		
		if (NULL == fgets(buf, FILELINE, f))
			exit(EXIT_FAILURE);
			
		parse_track(&newTrack, buf);
		
		append_track(t, newTrack);
	}
	
	if (-1 == fclose(f))
	{
		jb_print_error("fclose:");
	}
}

void save_track (char* filepath, struct track t)
{
	FILE* f = fopen (filepath, "ab+");

	if (f==NULL)
	{
		jb_print_error("fopen:");
	}
	
	fprintf (f, "%s:%d:%d:%d:", t.name, t.checkpoint_number, t.laps, t.length);

	if (-1 == fclose(f))
	{
		jb_print_error("fclose:");
	}
}

void parse_turtle (struct turtle* t, char line[FILELINE])
{
	sscanf (line, "%d:%s:%d:%d:%d", &(t->id), t->name, &(t->age), &(t->weight), &(t->currPoints));
}

void parse_track (struct track* t, char line[FILELINE])
{
	sscanf(line, "%s:%d:%d:%d", t->name, &(t->checkpoint_number), &(t->laps), &(t->length));
}

void handle_add_turtle(int fd, struct turtle_group* turtles, char buf[BUFLEN])
{
	struct turtle t;
	
	char buf2 [BUFLEN];
	int msg;
	memset(buf2, 0x00, BUFLEN);

	sscanf(buf, "%d;%s$", &msg, buf2);
	
	sscanf (buf2, "%d:%s:%d:%d", &(t.id), t.name, &(t.age), &(t.weight));
	t.currPoints = 0;
	t.currPos = 0;
	
	append_turtle(turtles, t);
	save_turtle(TURTLEPATH, t);
	print_short(fd, SUCCESSID);
}
void handle_add_track(int fd, struct track_group* tracks, char buf[BUFLEN])
{
	struct track t;
	
	char buf2 [BUFLEN];
	int msg;
	memset(buf2, 0x00, BUFLEN);

	sscanf(buf, "%d;%s$", &msg, buf2);
	
	sscanf(buf2, "%s:%d:%d:%d", t.name, &(t.checkpoint_number), &(t.laps), &(t.length));
	
	append_track(tracks, t);
	save_track(TRACKPATH, t);
	print_short(fd, SUCCESSID);
}

void handle_start_race(int fd, char buf[BUFLEN], int* currRace, struct track_group tr)
{
	char name [BUFLEN];
	int msg;
	int i;
	memset(name, 0x00, BUFLEN);
	if (*currRace != -1)
	{
		print_short(fd, ERRID);
		return;
	}
	sscanf (buf, "%d;%s$", &msg, name);
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
		print_short(fd, ERRID);
		return;
	}
	
	print_short(fd, SUCCESSID);
}
void handle_end_race (int fd, char buf[BUFLEN], int* currRace, struct turtle_group* tu)
{
	int i;
	if (*currRace == -1)
	{
		print_short(fd, ERRID);
	}
	*currRace = -1;
	for (i=0;i<tu->number;i++)
	{
		tu->turtles[i].currPos = 0;
	}
	
	print_short(fd, SUCCESSID);
}

void handle_update_race (int fd, char buf[BUFLEN], int currRace, struct turtle_group* tu, struct track_group tr)
{
	int msg, who, where, chpoints, pos;
	sscanf(buf, "%d;%d:%d$", &msg, &who, &where);
	if (who>=tu->number || currRace == -1)
	{
		print_short(fd, ERRID);
	}
	chpoints = tr.tracks[currRace].checkpoint_number*tr.tracks[currRace].laps +1;
	pos = tu->turtles[who].currPos;
	
	if (pos>=chpoints)
	{
		print_short(fd,ERRID);
	}
	tu->turtles[who].currPos = where;
	if (where >= chpoints)
	{
		int won_count=0;
		int i;
		for (i=0;i<tu->number;i++)
		{
			if (tu->turtles[i].currPos >= chpoints)
				won_count++;
		}
		if (won_count>4)
		{
			print_short(fd, SUCCESSID);
			return;
		}
		
		/* 25 pkt */
		if (won_count == 1)
		{
			tu->turtles[who].currPoints += 25;
		} /* 10 pkt */
		else if (won_count == 2)
		{
			tu->turtles[who].currPoints += 10;
		}/* 5 pkt */
		else if (won_count == 3)
		{
			tu->turtles[who].currPoints += 5;
		}/* 1 pkt */
		else if (won_count == 4)
		{
			tu->turtles[who].currPoints += 1;
		}
		save_turtles(TURTLEPATH, *tu);
	}
	print_short(fd, SUCCESSID);
}

void handle_reset_table(int fd, struct turtle_group* tu)
{
	int i;
	for (i=0;i<tu->number;i++)
	{
		tu->turtles[i].currPoints = 0;
		tu->turtles[i].currPos = 0;
	}
	save_turtles(TURTLEPATH, *tu);
	print_short(fd, SUCCESSID);
}

void print_sequence (int fd, int currRace, struct track_group tr, struct turtle_group tu)
{
	char buf [BUFLEN];
	int i;
	int written = 0;
	int pos = 0;
	int checkpoints = tr.tracks[currRace].checkpoint_number * tr.tracks[currRace].laps + 1;
	memset(buf, 0x00, BUFLEN);
	
	/* header */
	written = snprintf(buf, BUFLEN, "%d;", SEQREQUESTID);
	pos+=written;
	
	for (i=0;i<tu.number;i++)
	{
		written = snprintf(buf+pos,BUFLEN-pos, "%s\t%d/%d\n", tu.turtles[i].name, tu.turtles[i].currPos, checkpoints);
		pos+=written;
	}
	snprintf(buf+pos, BUFLEN-pos, "$");
	
	socket_write(fd, buf, BUFLEN);
}

void print_table(int fd, struct turtle_group tu)
{
	char buf [BUFLEN];
	int i;
	int written = 0;
	int pos = 0;
	memset(buf, 0x00, BUFLEN);
	
	/* header */
	written = snprintf(buf, BUFLEN, "%d;", TABREQUESTID);
	pos+=written;
	
	for (i=0;i<tu.number;i++)
	{
		written = snprintf(buf+pos,BUFLEN-pos, "%s\t%d pkt\n", tu.turtles[i].name, tu.turtles[i].currPoints);
		pos+=written;
	}
	snprintf(buf+pos, BUFLEN-pos, "$");
	
	socket_write(fd, buf, BUFLEN);
}
void print_all_turtles (int fd, struct turtle_group tu)
{
	char buf [BUFLEN];
	int i;
	int written = 0;
	int pos = 0;
	memset(buf, 0x00, BUFLEN);
	
	/* header */
	written = snprintf(buf, BUFLEN, "%d;", GETTURTLESID);
	pos+=written;	
	
	for (i=0;i<tu.number;i++)
	{
		written = snprintf(buf+pos,BUFLEN-pos, "%d\t%s\t%d\t%d\n", tu.turtles[i].id, tu.turtles[i].name, tu.turtles[i].age, tu.turtles[i].weight);
		pos+=written;
	}
	snprintf(buf+pos, BUFLEN-pos, "$");
	
	socket_write(fd, buf, BUFLEN);
}
void print_all_tracks (int fd, struct track_group tr)
{
	char buf [BUFLEN];
	int i;
	int written = 0;
	int pos = 0;
	memset(buf, 0x00, BUFLEN);
	
	/* header */
	written = snprintf(buf, BUFLEN, "%d;", GETTRACKSID);
	pos+=written;
	
	for (i=0;i<tr.number;i++)
	{
		written = snprintf(buf+pos,BUFLEN-pos, "%s:%d:%d:%d\n", tr.tracks[i].name, tr.tracks[i].checkpoint_number, tr.tracks[i].laps, tr.tracks[i].length);
		pos+=written;
	}
	snprintf(buf+pos, BUFLEN-pos, "$");
	
	socket_write(fd, buf, BUFLEN);
}

void append_turtle (struct turtle_group* tu, struct turtle newTurtle)
{
	struct turtle* newTurtles = (struct turtle*) jb_malloc(sizeof(struct turtle) * tu->number+1);
	if (tu->number > 0)
		memcpy (newTurtles, tu->turtles, sizeof(struct turtle) * tu->number);
		
	newTurtles[tu->number].age = newTurtle.age;
	newTurtles[tu->number].weight = newTurtle.weight;
	newTurtles[tu->number].id = newTurtle.id;
	newTurtles[tu->number].currPoints = newTurtle.currPoints;
	newTurtles[tu->number].currPos = newTurtle.currPos;
	strncpy(newTurtles[tu->number].name,newTurtle.name, NAMELEN);
	tu->number++;
	free(tu->turtles);
	tu->turtles = newTurtles;
}

void append_track (struct track_group* tr, struct track newTrack)
{
	struct track* newTracks = (struct track*) jb_malloc(sizeof(struct track) * tr->number+1);
	if (tr->number > 0)
		memcpy (newTracks, tr->tracks, sizeof(struct track) * tr->number);
		
	newTracks[tr->number].length = newTrack.length;
	newTracks[tr->number].checkpoint_number = newTrack.checkpoint_number;
	newTracks[tr->number].laps = newTrack.laps;
	strncpy(newTracks[tr->number].name,newTrack.name, NAMELEN);
	tr->number++;
	free(tr->tracks);
	tr->tracks = newTracks;
}

void print_short(int fd, int id)
{
	char buf[BUFLEN];
	memset(buf, 0x00, BUFLEN);
	snprintf(buf, BUFLEN, "%d$", id);
	socket_write(fd, buf, BUFLEN);
}

void free_all (struct turtle_group* tu, struct track_group* tr)
{
	free(tu->turtles);
	free (tr->tracks);
}

void init_all (struct turtle_group* tu, struct track_group* tr)
{
	tu->number = 0;
	tu->turtles = NULL;
	tr-> number = 0;
	tr->tracks = NULL;
}
