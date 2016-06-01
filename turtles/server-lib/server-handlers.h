#include "server-data.h"

void handle_add_turtle(int fd, int currRace, struct turtle_group* turtles, char buf[BUFLEN]);
void handle_add_track(int fd, int currRace, struct track_group* tracks, char buf[BUFLEN]);
void handle_start_race(int fd, char buf[BUFLEN], int* currRace, struct turtle_group* tu, struct track_group* tr);
void handle_end_race (int fd, char buf[BUFLEN], int* currRace, struct turtle_group* tu);
void handle_update_race (int fd, char buf[BUFLEN], int currRace, struct turtle_group* tu, struct track_group tr);
void handle_reset_table(int fd, struct turtle_group* tu, int currRace);
void print_sequence (int fd, int currRace, struct track_group tr, struct turtle_group tu);
void print_table(int fd, struct turtle_group tu);
void print_all_turtles (int fd, struct turtle_group tu);
void print_all_tracks (int fd, struct track_group tr);
