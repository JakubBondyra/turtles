#include "../common/common.h"

void get_int (char* msg, int* val);
void get_string (char* msg, char** buf, int len);
int verify_3param_command(char* buf, int* arg1, int* arg2, int* arg3);
int comma_list_check (char* buf, int len, int listlen, int maxval);
void get_comma_list (char* msg, char** buf, int len, int listlen, int maxval);
void get_players (char* msg, char** buf, int len);
int players_list_check(char* buf, int len);

void display_formatted_turtles (char* buf);
void display_formatted_tracks (char* buf);
void display_formatted_sequence (char* buf);
void display_formatted_table (char* buf);
