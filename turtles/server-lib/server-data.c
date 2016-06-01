#include "server-data.h"

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
		if (NULL == fgets(buf, FILELINE, f))
			continue;
		if (strlen(buf) < 2)
			continue;
		memset(&newTurtle, 0x00, sizeof(newTurtle));
				
		parse_turtle(&newTurtle, buf);
		append_turtle(t, newTurtle);
	}
	
	if (-1 == fclose(f))
	{
		jb_print_error("fclose:");
	}
}

void save_turtle (char* filepath, struct turtle* t)
{
	FILE* f = fopen (filepath, "ab+");
	if (f==NULL)
	{
		jb_print_error("fopen:");
	}
	
	fprintf (f, "%d:%s:%d:%d:%d\n", t->id, t->name, t->age, t->weight, t->currPoints);
	
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
			continue;
		if (strlen(buf) < 2)
			continue;
		parse_track(&newTrack, buf);
		
		append_track(t, newTrack);
	}
	
	if (-1 == fclose(f))
	{
		jb_print_error("fclose:");
	}
}

void save_track (char* filepath, struct track* t)
{
	FILE* f = fopen (filepath, "ab+");

	if (f==NULL)
	{
		jb_print_error("fopen:");
	}
	
	fprintf (f, "%s:%d:%d:%d:%s", t->name, t->checkpoint_number, t->laps, t->length, t->chlengths);

	if (-1 == fclose(f))
	{
		jb_print_error("fclose:");
	}
}

void parse_turtle (struct turtle* t, char line[FILELINE])
{
	sscanf (line, "%d:%[^:]:%d:%d:%d", &(t->id), t->name, &(t->age), &(t->weight), &(t->currPoints));
	t->currChp = 0;
	t->currLap = 0;
}

void parse_track (struct track* t, char line[FILELINE])
{
	sscanf(line, "%[^:]:%d:%d:%d:%[0123456789,]", t->name, &(t->checkpoint_number), &(t->laps), &(t->length), t->chlengths);
}


void append_turtle (struct turtle_group* tu, struct turtle newTurtle)
{
	struct turtle* newTurtles = (struct turtle*) jb_malloc(sizeof(struct turtle) * (tu->number+1));
	if (tu->number > 0)
	{
		int i;
		for (i=0;i<tu->number;i++)
		{
			newTurtles[i].age = tu->turtles[i].age;
			newTurtles[i].weight = tu->turtles[i].weight;
			newTurtles[i].id = tu->turtles[i].id;
			newTurtles[i].currPoints = tu->turtles[i].currPoints;
			newTurtles[i].currChp = tu->turtles[i].currChp;
			newTurtles[i].currLap = tu->turtles[i].currLap;
			strncpy(newTurtles[i].name,tu->turtles[i].name, NAMELEN);
		}
	}

	newTurtles[tu->number].age = newTurtle.age;
	newTurtles[tu->number].weight = newTurtle.weight;
	newTurtles[tu->number].id = newTurtle.id;
	newTurtles[tu->number].currPoints = newTurtle.currPoints;
	newTurtles[tu->number].currChp = newTurtle.currChp;
	newTurtles[tu->number].currLap = newTurtle.currLap;
	strncpy(newTurtles[tu->number].name,newTurtle.name, NAMELEN);
	tu->number++;
	free(tu->turtles);
	tu->turtles = newTurtles;
}

void append_track (struct track_group* tr, struct track newTrack)
{
	struct track* newTracks = (struct track*) jb_malloc(sizeof(struct track) * (tr->number+1));
	
	if (tr->number > 0)
	{
		int i;
		for (i=0;i<tr->number;i++)
		{
			newTracks[i].length = tr->tracks[i].length;
			newTracks[i].checkpoint_number = tr->tracks[i].checkpoint_number;
			newTracks[i].laps = tr->tracks[i].laps;
			strncpy(newTracks[i].name, tr->tracks[i].name, NAMELEN);
			strncpy(newTracks[i].chlengths, tr->tracks[i].chlengths, CHPTLEN);
		}
	}
		
	newTracks[tr->number].length = newTrack.length;
	newTracks[tr->number].checkpoint_number = newTrack.checkpoint_number;
	newTracks[tr->number].laps = newTrack.laps;
	strncpy(newTracks[tr->number].name,newTrack.name, NAMELEN);
	strncpy(newTracks[tr->number].chlengths,newTrack.chlengths, CHPTLEN);
	tr->number++;
	free(tr->tracks);
	tr->tracks = newTracks;
}

void free_all (struct turtle_group* tu, struct track_group* tr)
{
	free(tu->turtles);
	free (tr->tracks);
	free(tu);
	free(tr);
}

void init_all (struct turtle_group** tu, struct track_group** tr)
{
	*tu = (struct turtle_group*) jb_malloc (sizeof(struct turtle_group));
	(*tu)->number = 0;
	(*tu)->turtles = NULL;
	*tr = (struct track_group*) jb_malloc (sizeof(struct track_group));
	(*tr)->number = 0;
	(*tr)->tracks = NULL;
}
