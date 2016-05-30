#include "../common/ui.h"

#define ADDTURTLECOMMAND "add-turtle"
#define ADDTRACKCOMMAND "add-track"
#define STARTRACECOMMAND "start-race"
#define ENDRACECOMMAND "end-race"
#define RACENEWSCOMMAND "update-pos"
#define NEWSEASONCOMMAND "reset-season"
#define LIVEREQUESTCOMMAND "get-live"
#define SEQREQUESTCOMMAND "get-positions"
#define TABREQUESTCOMMAND "get-table"
#define GETTURTLESCOMMAND "get-turtles"
#define GETTRACKSCOMMAND "get-tracks"
#define QUITCOMMAND "quit"

#define ADDTURTLEINFO "adds new turtle to server data"
#define ADDTRACKINFO "adds new track to server data"
#define STARTRACEINFO "start a new race for specified track"
#define ENDRACEINFO "ends current race (or not in another case)"
#define RACENEWSINFO "updates position of a turtle (id) to specified checkpoint (int)"
#define NEWSEASONINFO "resets current season"
#define LIVEREQUESTINFO "starts live relation for current race"
#define SEQREQUESTINFO "gets current positions of players in current race"
#define TABREQUESTINFO "gets point table for current season"
#define GETTURTLESINFO "lists all turtles"
#define GETTRACKSINFO "lists all tracks"
#define QUITINFO "quit this program"

void handle_args (int argc, char* argv[], int* port, char** address);
void run_spectator (int port, char* address);
void run_client (int port, char* address);
void run_ui (int port, char* address, char* commands[], char* infos[], int ccount);
int get_line (int port, char* address, char* commands[], int ccount);
void get_handler (int port, char* address, char* command);
void add_turtle_handler (int port, char* address);
void add_track_handler (int port, char* address);
void start_race_handler (int port, char* address);
void end_race_handler (int port, char* address);
void race_news_handler (int port, char* address);
void new_season_handler (int port, char* address);
void live_request_handler (int port, char* address);
void seq_request_handler (int port, char* address);
void tab_request_handler (int port, char* address);
void get_turtles_handler (int port, char* address);
void get_tracks_handler (int port, char* address);
void quit_handler ();

void send_message(int port, char* address, char buf[BUFLEN], int count);
void handle_response(int sock);

void print_formatted_turtles (char buf[BUFLEN]);
void print_formatted_tracks (char buf[BUFLEN]);

