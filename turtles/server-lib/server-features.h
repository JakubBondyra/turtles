#include "../common/common.h"

/* server file paths */
#define TURTLEPATH "./turtles"
#define TRACKPATH "./tracks"
#define TABLEPATH "./table"

void read_turtles (char* filepath, struct turtle_group* t);
void save_turtle (char* filepath, struct turtle* t);
void save_turtles (char* filepath, struct turtle_group t);
void read_tracks (char* filepath, struct track_group* t);
void save_track (char* filepath, struct track* t);
void parse_turtle (struct turtle* t, char line[FILELINE]);
void parse_track (struct track* t, char line[FILELINE]);

void handle_add_turtle(int fd, struct turtle_group* turtles, char buf[BUFLEN]);
void handle_add_track(int fd, struct track_group* tracks, char buf[BUFLEN]);
void handle_start_race(int fd, char buf[BUFLEN], int* currRace, struct track_group tr);
void handle_end_race (int fd, char buf[BUFLEN], int* currRace, struct turtle_group* tu);
void handle_update_race (int fd, char buf[BUFLEN], int currRace, struct turtle_group* tu, struct track_group tr);
void handle_reset_table(int fd, struct turtle_group* tu);
void print_sequence (int fd, int currRace, struct track_group tr, struct turtle_group tu);
void print_table(int fd, struct turtle_group tu);
void print_all_turtles (int fd, struct turtle_group tu);
void print_all_tracks (int fd, struct track_group tr);

void append_turtle (struct turtle_group* tu, struct turtle newTurtle);
void append_track (struct track_group* tr, struct track newTrack);

void print_short(int fd, int id);

void free_all (struct turtle_group* tu, struct track_group* tr);
void init_all (struct turtle_group** tu, struct track_group** tr);
