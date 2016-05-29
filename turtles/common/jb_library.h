#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <signal.h>
#include <errno.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <string.h>
#include <time.h>
#include <dirent.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <poll.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/msg.h>
#include <sys/shm.h>
#include <aio.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <netdb.h>

extern int h_errno;


/* makro do obsługi EINTR */
#define TEMP_FAILURE_RETRY(expression) \
(__extension__ ({ long int __result; \
do __result = (long int) (expression);\
while (__result == -1L && errno == EINTR);\
__result; }))

/*union do semaforów*/
union semun {
	int val;    /* Value for SETVAL */
	struct semid_ds *buf;    /* Buffer for IPC_STAT, IPC_SET */
	unsigned short  *array;  /* Array for GETALL, SETALL */
};

/* SIGNALS */
int jb_sethandler( void (*f)(int), int sigNo);
void jb_sigchld_handler(int sig);

void jb_kill(pid_t pid, int sig);

sigset_t jb_block_signal (int signal);
void jb_unblock_signal (int signal);

void jb_total_wait ();

/* SEN */
void jb_sleep (long miliseconds);

void jb_sleep_alrm (int seconds);

/* perror + exit_failure */
void jb_print_error (const char* msg);

void jb_exit_failure(const char* msg);

/* opakowane funkcje z stdlib */
void jb_fflush (FILE* stream);

void* jb_malloc (size_t size);

/* PLIKI + KATALOGI */

ssize_t jb_bulk_write(int fd, const void *buf, size_t size);
ssize_t jb_bulk_read(int fd, void *buf, size_t size);

/* fd- deskryptor, type - F_WRLCK, F_UNLCK, ... na append start=0 len=1 */
void jb_setlock(int fd, int start, int len, int type);

/* dotyczy deskryptorów */
int jb_open(char* filename, int flags, int mode);
void jb_close(int fd);

/* readdir nie ma sensu tak pisać, bo zawsze inaczej wygląda */
DIR* jb_opendir (const char* name);
void jb_closedir (DIR* dirp);

void * jb_mmap (void * addr, size_t length, int prot, int flags,
	int fd, off_t offset);
void jb_munmap(void* addr, size_t length);

FILE* jb_fdopen(int fd, const char* mode);
void jb_fclose (FILE* fp);

int jb_dup (int oldfd);
int jb_dup2 (int oldfd, int newfd);

void jb_lstat (const char* name, struct stat* buf);


/* laby numer 3 */
void jb_mkfifo(const char* pathname, mode_t mode);

void jb_unlink (const char* pathname);

void jb_pipe (int pipefd[2]);

int jb_select (int nfds, fd_set *readfds, fd_set *writefds, fd_set *exceptfds,
struct timeval* timeout);

int jb_poll (struct pollfd *fds, nfds_t nfds, int timeout);

int jb_read_fifo (int fd, void* buf, size_t size);

FILE * jb_popen (const char* command, const char* type);
void jb_pclose (FILE * stream);


int jb_open2 (const char* path, int flags);
/* 
valgrind --leak-check=full \
--show-leak-kinds=all --track-fds=yes
*/

/* IPC */
/* semafory */

int jb_semget(key_t key, int nsems, int semflags);

int jb_semop (int semid, int semno, int semop);
int jb_semops (int semid, struct sembuf* sops, unsigned int nsops);

void jb_seminit(int sem, int nr, int value);

void jb_remove_semaphores (int sset);

/* shm */
int jb_shmget (key_t key, size_t size, int shmflg);

void* jb_shmat (int shmid, const void* shmaddr, int shmflg);

void jb_shmdt (const void* ptr);

void jb_remove_shm (int shmid);

/* message queue */
typedef struct jb_msg
{
	long mtype;
	char mtext[144];
}jb_msg;

int jb_msgget (key_t key, int msgflg);

void jb_remove_msg (int msqid);

void jb_aio_fsync (int op, struct aiocb* aiocbp);

void jb_aio_read (struct aiocb* aiocbp);

void jb_aio_write (struct aiocb* aiocbp);

void jb_aio_suspend(struct aiocb* aiocbp);

/* socket - local */

int jb_local_stream_socket(int* sock, char* name, struct sockaddr_un* addr);

int jb_local_stream_socket_mbl(char* name, struct sockaddr_un* addr, int backlog);

void jb_connect_local_socket(int sock, struct sockaddr_un* addr);

void jb_accept(int sock, int* fd);

void jb_accept_set_rfds(int sockFd, int* fd, fd_set* rfds, int* maxFd);

void jb_local_stream_socket_client(int* sockFd, char* name, struct sockaddr_un* addr);

ssize_t socket_read(int sockFd, char* buf, size_t count);

ssize_t socket_write(int sockFd, char* buf, size_t count);

/* socket - tcp */
int jb_internet_stream_socket();

struct sockaddr_in jb_address_to_struct(char* address, uint16_t port);

void jb_internet_stream_socket_client(int* sockFd, char* address, uint16_t port);

void jb_internet_stream_socket_connect(int sockFd, struct sockaddr_in* addr);

void jb_internet_stream_socket_mbl(int* sockFd, uint16_t port, int backlog);

void jb_accept_eintr(int sockFd, int* fd);

/* socket - udp */
int jb_internet_datagram_socket();

int jb_internet_datagram_socket_mb(char* address, uint16_t port);

int jb_send_datagram(int sock, struct sockaddr_in addr, int16_t msg);

int jb_send_datagram_buf(int sock, struct sockaddr_in addr, char* buf, size_t count);

int jb_recv_datagram_any(int sock, char* buf, int count);

int jb_recv_datagram(int sock, struct sockaddr_in *addr, int16_t* msg);

int jb_recv_datagram_buf(int sock, struct sockaddr_in* addr, char* buf, int count); 

