#include "../common/common.h"

/* server file paths */
#define TURTLEPATH "./turtles"
#define TRACKPATH "./tracks"
#define TABLEPATH "./table"

/* structures */
struct turtle{
	int id;
	char name [NAMELEN];
	int age;
	int weight;
	int currPoints;
	int currLap;
	int currChp;
};

struct turtle_group {
	int number;
	struct turtle* turtles;
};

struct track {
	char name [NAMELEN];
	int length;
	int checkpoint_number;
	int laps;
	char chlengths [CHPTLEN];
};

struct track_group {
	int number;
	struct track* tracks;
};

void read_turtles (char* filepath, struct turtle_group* t);
void save_turtle (char* filepath, struct turtle* t);
void save_turtles (char* filepath, struct turtle_group t);
void read_tracks (char* filepath, struct track_group* t);
void save_track (char* filepath, struct track* t);
void load_state (struct turtle_group** turtles, struct track_group** tracks);
void parse_turtle (struct turtle* t, char* line);
void parse_track (struct track* t, char* line);

int get_players_from_msg (struct turtle_group* tu, char* players);

void append_turtle (struct turtle_group* tu, struct turtle newTurtle);
void append_track (struct track_group* tr, struct track newTrack);
void free_all (struct turtle_group* tu, struct track_group* tr);
void init_all (struct turtle_group** tu, struct track_group** tr);

void append_int (int** p, int len, int val);
