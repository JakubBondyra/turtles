#include "server-data.h"
#include <pthread.h>

pthread_cond_t	cond;
pthread_mutex_t	mutex;
int listener_number;

struct thread_data
{
	char** newsBuffer;
	int fd;
};

void handle_add_turtle(int fd, int currRace, struct turtle_group* turtles, char* buf);
void handle_add_track(int fd, int currRace, struct track_group* tracks, char* buf);
void handle_start_race(int fd, char* buf, int* currRace, struct turtle_group* tu, struct track_group* tr);
void handle_end_race (int fd, char* buf, int* currRace, struct turtle_group* tu, char** news_buffer);
void handle_update_race (int fd, char* buf, int currRace, struct turtle_group* tu, struct track_group tr, char** news_buffer);
void handle_reset_table(int fd, struct turtle_group* tu, int currRace);
void handle_live_request (int fd, int currRace, struct turtle_group* turtles, struct track_group* tracks, char** news_buffer);

void print_sequence (int fd, int currRace, struct track_group tr, struct turtle_group tu);
void print_table(int fd, struct turtle_group tu);
void print_all_turtles (int fd, struct turtle_group tu);
void print_all_tracks (int fd, struct track_group tr);

void relay_news (char** news_buffer, char* who, int laps, int pos);
void relay_end_race (char** news_buffer);
void* wait_for_news (void* tdv);
int print_news (struct thread_data data);
