#include "jb_library.h"

/*cztery funkcje nizej wzorowane na przykładach p. Borkowskiego
* mini.pw.edu.pl/~marcinbo
*/

int jb_sethandler( void (*f)(int), int signal) {
	struct sigaction act;
	memset(&act, 0, sizeof(struct sigaction));
	act.sa_handler = f;
	if (-1==sigaction(signal, &act, NULL))
		{
			jb_print_error("sethandler:");
		}
	return 0;
}

void jb_sigchld_handler(int sig) {
	pid_t pid;
	for(;;){
		pid=waitpid(0, NULL, WNOHANG);
		if(pid==0) return;
		if(pid<=0) {
			if(errno==ECHILD) return;
			jb_print_error("waitpid:");
		}
	}
}

void jb_kill(pid_t pid, int sig)
{
	if (-1 == kill(pid, sig))
	{
		jb_print_error ("kill:");
	}
}

sigset_t jb_block_signal (int signal)
{
	sigset_t mask, oldmask;
	/* tutaj bledy sa niemozliwe, wiec pomijam:
	 */
	sigemptyset(&mask);
	if (sigaddset (&mask, signal) < 0)
	{
		jb_print_error("sigaddset:");
	}
	/* tutaj bledy sa niemozliwe, wiec pomijam:
	 */
	sigprocmask (SIG_BLOCK, &mask, &oldmask);
	return oldmask;
}

void jb_unblock_signal (int signal)
{
	sigset_t mask;
	/* tutaj bledy sa niemozliwe, wiec pomijam:
	 */
	sigemptyset(&mask);
	if (sigaddset (&mask, signal) < 0)
	{
		jb_print_error("sigaddset:");
	}
	/* tutaj bledy sa niemozliwe, wiec pomijam:
	 */
	sigprocmask (SIG_UNBLOCK, &mask, NULL);
}

void jb_total_wait ()
{
	while (TEMP_FAILURE_RETRY(wait(NULL)) > 0);
}

void jb_sleep (long miliseconds)
{
	struct timespec tt, t;
	time_t seconds = (miliseconds/1000) > 0 ? miliseconds/1000 : 0;
	long nanoseconds = (miliseconds - seconds*1000)*1000000;
	t.tv_sec = seconds; t.tv_nsec = nanoseconds;
	for (tt=t;nanosleep(&tt,&tt);)
		if (EINTR!=errno) jb_print_error("nanosleep:");
}

/* spanie angazujace alarm
 */
void jb_sleep_alrm (int seconds)
{
	int tt, t = seconds;
	for (tt=t; tt > 0; tt = sleep(tt));
}

void jb_print_error (const char* msg)
{
	perror(msg);
	exit(EXIT_FAILURE);
}

void jb_exit_failure (const char* msg)
{
	fprintf(stderr, "%s\n", msg);
	exit(EXIT_FAILURE);
}

void jb_fflush(FILE* stream)
{
	if (-1 == TEMP_FAILURE_RETRY(fflush(stream)))
	{
		jb_print_error("jb_fflush:");
	}
}

void* jb_malloc (size_t size)
{
	void * ptr = malloc (size);
	if (ptr == NULL)
	{
		jb_print_error ("malloc:");
	}
	return ptr;
}

ssize_t jb_bulk_write(int fd, const void *buf, size_t size) 
{
	size_t nleft = size;
	ssize_t nwritten;
	const char *p = buf;

	while (nleft > 0)
	{
		if ((nwritten = (TEMP_FAILURE_RETRY(write(fd, p, nleft)))) == -1)
		{
			jb_print_error("write");
		}
		
		nleft -= nwritten;
		p += nwritten;
	}
	return size;
}

ssize_t jb_bulk_read(int fd, void *buf, size_t size)
{
	size_t nleft = size;
	ssize_t nread;
	char *p = buf;

	while (nleft > 0)
	{
		if ((nread = (TEMP_FAILURE_RETRY(read(fd, p, nleft)))) == -1)
		{
			jb_print_error("write");
		}
		
		nleft -= nread;
		p += nread;
		/* EOF case */
		if (nread == 0)
			break;
	}
	return size-nleft;
}

void jb_setlock(int fd, int start, int len, int type) 
{
	struct flock l;
	l.l_whence = SEEK_SET;
	l.l_start = start;
	l.l_len = len;
	l.l_type = type;
	if (-1 == TEMP_FAILURE_RETRY(fcntl(fd, F_SETLKW, &l)))
		jb_print_error("fcntl");
}

int jb_open(char* filename, int flags, int mode)
{
	int fd;
	if ((fd = TEMP_FAILURE_RETRY(open(filename, flags, mode))) == -1)
	{
		fprintf (stderr,"%s\n", filename);
		jb_print_error("Cannot open");
	}
	return fd;
}

void jb_close(int fd)
{	
	if (TEMP_FAILURE_RETRY(close(fd)) == -1)
		jb_print_error("Cannot close");
}

DIR* jb_opendir (const char* name)
{
	DIR * dirp = opendir (name);
	if (dirp == NULL)
	{
		jb_print_error("opendir");
	}
	return dirp;
}

void jb_closedir (DIR* dirp)
{
	if (TEMP_FAILURE_RETRY(closedir(dirp)) == -1)
	{
		jb_print_error("closedir");
	}
}

void * jb_mmap (void * addr, size_t length, int prot, int flags,
	int fd, off_t offset)
{
	void *ptr;
	if ((ptr = mmap (addr,length,prot,flags,fd,offset)) == NULL)
	{
		jb_print_error("mmap:");
	}
	return ptr;
}
void jb_munmap(void* addr, size_t length)
{
	if (munmap(addr,length) == -1)
	{
		jb_print_error("unmap:");
	}
	
}

FILE* jb_fdopen (int fd, const char* mode)
{
	FILE* f = fdopen (fd,mode);
	if (f==NULL)
	{
		jb_print_error("fdopen:");
	}
	return f;
}

void jb_fclose (FILE* f)
{
	if (fclose(f) == EOF)
	{
		jb_print_error("fclose:");
	}
}

int jb_dup (int oldfd)
{
	int fd = TEMP_FAILURE_RETRY(dup (oldfd));
	if (fd == -1)
	{
		jb_print_error("dup:");
	}
	return fd;
}

int jb_dup2 (int oldfd, int newfd)
{
	int fd = TEMP_FAILURE_RETRY(dup2(oldfd, newfd));
	if (fd == -1)
	{
		jb_print_error("dup2:");
	}
	return fd;
}

void jb_lstat (const char* name, struct stat* buf)
{
	if (lstat(name,buf) == -1)
	{
		jb_print_error("lstat:");
	}
}

void jb_mkfifo (const char* pathname, mode_t mode)
{
	if (mkfifo (pathname, mode) == -1)
	{
		jb_print_error("mkfifo:");
	}
}

void jb_unlink (const char* pathname)
{
	if (unlink(pathname) == -1)
	{
		jb_print_error("unlink:");
	}	
}

void jb_pipe (int pipefd[2])
{
	if (pipe(pipefd) == -1)
	{
		jb_print_error("pipe:");
	}
}

int jb_select (int nfds, fd_set *readfds, fd_set *writefds, fd_set *exceptfds,
struct timeval* timeout)
{
	int ret = TEMP_FAILURE_RETRY(select(nfds, readfds, writefds, exceptfds, timeout));
	if (ret == -1)
	{
		jb_print_error("select:");
	}
	return ret;
}

int jb_poll (struct pollfd *fds, nfds_t nfds, int timeout)
{
	int ret = TEMP_FAILURE_RETRY(poll(fds, nfds, timeout));
	if (ret == -1)
	{
		jb_print_error("poll:");
	}
	return ret;
}

FILE * jb_popen (const char* command, const char* type)
{
	FILE* stream = popen(command, type);
	if (stream == NULL)
	{
		jb_print_error("inside popen:");
	}
	return stream;
}

void jb_pclose (FILE* stream)
{
	if (pclose(stream) == -1)
	{
		jb_print_error("pclose:");
	}
}

int jb_read_fifo (int fd, void* buf, size_t size)
{
	int len;
	if (size > PIPE_BUF)
	{
		fprintf (stderr, "Size>buf in jb_read_fifo\n");
		exit(EXIT_FAILURE);
	}
	len = TEMP_FAILURE_RETRY(read(fd, buf, size));
	if (len < 0) 
		jb_print_error("read:");
	if (0 == len) 
	{
		return 0;
	}
	return len;
}

int jb_open2 (const char* path, int flags)
{
	int fd = open(path, flags);
	if (fd == -1)
	{
		fprintf (stderr,"%s\n", path);
		jb_print_error ("open:");
	}
	return fd;
}

int jb_semget (key_t key, int nsems, int semflags)
{
	int id;
	if ((id=semget(key, nsems, semflags)) == -1)
	{
		jb_print_error("semget:");
	}
	return id;
}

int jb_semop (int semid, int semno, int op)
{
	struct sembuf buf;
	buf.sem_num = semno;
	buf.sem_op = op;
	buf.sem_flg = 0;
	
	if(TEMP_FAILURE_RETRY(semop(semid, &buf, 1))<0)
		return -1;
	return 0;
}

int jb_semops (int semid, struct sembuf* sops, unsigned int nsops)
{
	if(TEMP_FAILURE_RETRY(semop(semid, sops, nsops))<0)
		return -1;
	return 0;
}

void jb_seminit(int sem, int nr, int value)
{
	union semun init;
	init.val = value;
	if(semctl(sem, nr, SETVAL, init)<0)
		jb_print_error("seminit:");
}

void jb_remove_semaphores(int sset)
{
	if (semctl(sset, 0, IPC_RMID) == -1)
		jb_print_error("remove semaphores:");
}

int jb_shmget (key_t key, size_t size, int shmflg)
{
	int id;
	if ((id=shmget(key, size, shmflg)) == -1)
	{
		jb_print_error("shmget:");
	}
	return id;
}

void* jb_shmat (int shmid, const void* shmaddr, int shmflg)
{
	void* ptr = shmat(shmid, shmaddr, shmflg);
	if (ptr == (void*) -1)
	{
		jb_print_error("shmat:");
	}
	return ptr;
}

void jb_shmdt (const void* ptr)
{
	if (shmdt(ptr) == -1)
	{
		jb_print_error("shmdt");
	}
}

void jb_remove_shm(int shmid)
{
	if (shmctl(shmid, IPC_RMID, NULL) == -1)
		jb_print_error("remove shared memory:");
}

int jb_msgget(key_t key, int msgflg)
{
	int id;
	if ((id=msgget(key, msgflg)) == -1)
	{
		jb_print_error("msgget:");
	}
	return id;
}

void jb_remove_msg(int msqid)
{
	if (msgctl(msqid, IPC_RMID, NULL) == -1)
		jb_print_error("remove message queue:");
}

void jb_aio_fsync (int op, struct aiocb* aiocbp)
{
	if (aio_fsync(op, aiocbp) == -1)
	{
		jb_print_error("aio_fsync:");
	}
	jb_aio_suspend(aiocbp);
}

void jb_aio_read (struct aiocb* aiocbp)
{
	if (aio_read(aiocbp) == -1)
	{
		jb_print_error("aio_read:");
	}
}

void jb_aio_write (struct aiocb* aiocbp)
{
	if (aio_write(aiocbp) == -1)
	{
		jb_print_error("aio_read:");
	}
}

void jb_aio_suspend(struct aiocb* aiocbp)
{
	struct aiocb* aiolist[1];
	aiolist[0] = aiocbp;
	if(TEMP_FAILURE_RETRY(aio_suspend((const struct aiocb* const*) aiolist, 1, NULL)) <0)
		jb_print_error("aio_suspend:");
	if(aio_error(aiocbp) !=0)
		jb_print_error("aio_suspend:");
	if(aio_return(aiocbp) <0)
		jb_print_error("aio_suspend:");
}

int jb_local_stream_socket(int* sock, char* name, struct sockaddr_un* addr)
{
	int sfd;
	if(sock == NULL || name == NULL || addr == NULL) return -1;
	(*sock) = -1;
	if((sfd=socket(PF_UNIX, SOCK_STREAM, 0)) <0)
		return -1;
	(*sock)=sfd;
	memset(addr, 0, sizeof(struct sockaddr_un));
	addr->sun_family = AF_UNIX;
	strncpy(addr->sun_path, name, sizeof(addr->sun_path)-1);
	return 0;
}

int jb_local_stream_socket_mbl(char* name, struct sockaddr_un* addr, int backlog)
{
	int sock;
	
	if(unlink(name) <0 && errno != ENOENT)
		jb_print_error("unlink:");
		
	if(jb_local_stream_socket(&sock, name, addr) <0)
		jb_print_error("socket:");
		
	if(bind(sock, (struct sockaddr*) addr, SUN_LEN(addr))<0)
		jb_print_error("bind:");
		
	if(listen(sock, backlog)<0)
		jb_print_error("listen:");
		
	return sock;
}

void jb_connect_local_socket(int sock, struct sockaddr_un* addr)
{
	if(connect(sock, (struct sockaddr*) addr, SUN_LEN(addr)) <0)
	{
		if(errno != EINTR) 
			jb_print_error("connect:");
		else
		{
			fd_set wfds;
			int status;
			socklen_t size = sizeof(int);
			FD_ZERO(&wfds);
			FD_SET(sock, &wfds);
			if(TEMP_FAILURE_RETRY(select(sock+1, NULL, &wfds, NULL, NULL)) <0)
				jb_print_error("select:");
			if(getsockopt(sock, SOL_SOCKET, SO_ERROR, &status, &size) <0)
				jb_print_error("getsockopt:");
			if(status != 0)
			{
				fprintf (stderr,"socket error\n");
				exit(EXIT_FAILURE);
			}
		}
	}
}

void jb_accept(int sock, int* fd)
{
	(*fd)=-1;
	if(((*fd)=accept(sock, NULL, NULL))<0 && errno != EINTR)
		jb_print_error("accept:");
}

void jb_accept_set_rfds(int sock, int* fd, fd_set* rfds, int* maxFd)
{
	jb_accept(sock, fd);

	FD_SET((*fd), rfds);
	if((*fd) > (*maxFd))
		(*maxFd) = (*fd);
}

void jb_local_stream_socket_client(int* sockFd, char* name, struct sockaddr_un* addr)
{
	struct sockaddr_un add;
	if(jb_local_stream_socket(sockFd, name, &add) <0)
		jb_print_error("socket:");
	jb_connect_local_socket(*sockFd, &add);
	(*addr) = add;
}

ssize_t socket_read(int sock, char* buf, size_t size)
{
	ssize_t len = jb_bulk_read(sock, buf, size);
	if(len <0 && errno == ECONNRESET)
		return 0;
	return len;
}

ssize_t socket_write(int sock, char* buf, size_t size)
{
	ssize_t len = jb_bulk_write(sock, buf, size);
	if(len < 0 && errno == EPIPE)
		return 0;
	if(len <0 && errno == ECONNRESET)
		return 0;
	return len;
}

int jb_internet_stream_socket()
{
	int sock = socket(PF_INET, SOCK_STREAM, 0);
	if(sock <0) 
		jb_print_error("socket (internet):");
	return sock;
}

struct sockaddr_in jb_address_to_struct(char* address, uint16_t port)
{
	struct sockaddr_in addr;
	struct hostent* info;
	memset(&addr, 0, sizeof(struct sockaddr_in));
	
	addr.sin_family = AF_INET;
	addr.sin_port = htons(port);
	
	info = gethostbyname(address);
	if(info == NULL) 
	{
		fprintf (stderr, "host error: %d\n", h_errno);
		exit(EXIT_FAILURE);
	}
		
	addr.sin_addr = *((struct in_addr*)info->h_addr);
	return addr;
}

void jb_internet_stream_socket_client(int* sockFd, char* address, uint16_t port)
{
	struct sockaddr_in addr = jb_address_to_struct(address, port);
	(*sockFd) = jb_internet_stream_socket();
	/* make struct from address and connect */
	jb_internet_stream_socket_connect(*sockFd, &addr);
}

void jb_internet_stream_socket_connect(int sockFd, struct sockaddr_in* addr)
{
	if(connect(sockFd, (struct sockaddr*)addr, sizeof(struct sockaddr_in)) <0)
	{
		if(errno != EINTR) 
			jb_print_error("connect:");
		else
		{
			fd_set wfds;
			int status;
			socklen_t size = sizeof(int);
			FD_ZERO(&wfds);
			FD_SET(sockFd, &wfds);
			if(TEMP_FAILURE_RETRY(select(sockFd+1, NULL, &wfds, NULL, NULL)) <0)
				jb_print_error("select:");
			if(getsockopt(sockFd, SOL_SOCKET, SO_ERROR, &status, &size) <0)
				jb_print_error("getsockopt:");
			if(status != 0)
				jb_print_error("connect (status)");
		}
	}
}

void jb_internet_stream_socket_mbl(int* sockFd, uint16_t port, int backlog)
{
	struct sockaddr_in addr;
	int t=1;
	(*sockFd) = jb_internet_stream_socket();
	
	memset(&addr, 0, sizeof(struct sockaddr_in));
	addr.sin_family = AF_INET;
	addr.sin_port = htons(port);
	addr.sin_addr.s_addr = htonl(INADDR_ANY);

	if(setsockopt((*sockFd), SOL_SOCKET, SO_REUSEADDR, &t, sizeof(t))<0)
		jb_print_error("setsockopt:");
	if(bind((*sockFd), (struct sockaddr*) &addr, sizeof(addr))<0)
		jb_print_error("bind:");
	if(listen((*sockFd), backlog)<0)
		jb_print_error("listen:");

}

/* ends with error, accepts properly, or doesn't raise any error but has fd == -1 (caused by EINTR)*/
void jb_accept_eintr(int sock, int* fd)
{
	(*fd)=-1;
	while(((*fd)=accept(sock, NULL, NULL))<0)
	{
		if(errno != EINTR)
			jb_print_error("accept:");
		return;
	}
}

int jb_internet_datagram_socket()
{
	int sock;
	if((sock = socket(PF_INET, SOCK_DGRAM, 0))<0)
		jb_print_error("dgram socket:");
	return sock;
}

int jb_internet_datagram_socket_mb(char* address, uint16_t port)
{
	struct sockaddr_in name = jb_address_to_struct(address, port);
	int sock = jb_internet_datagram_socket();
	
	int t=1;
	if(setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &t, sizeof(t))<0)
		jb_print_error("setsockopt:");
	if(bind(sock, (struct sockaddr*) &name, sizeof(name)) <0)
		jb_print_error("bind:");
	return sock;
}

int jb_send_datagram(int sock, struct sockaddr_in addr, int16_t msg)
{
	int16_t buf = htons(msg);
	int status = TEMP_FAILURE_RETRY(sendto(sock, &buf, sizeof(int16_t), 0, (struct sockaddr*) &addr, sizeof(addr)));
	if(status <0 && errno != EPIPE && errno != ECONNRESET)
		jb_print_error("sendto:");
	if(status <0) 
		return 0;
	return status;
}

int jb_send_datagram_buf(int sock, struct sockaddr_in addr, char* buf, size_t count)
{
	int status = TEMP_FAILURE_RETRY(sendto(sock, buf, count, 0, (struct sockaddr*) &addr, sizeof(addr)));
	if(status <0 && errno != EPIPE && errno != ECONNRESET)
		jb_print_error("sendto (w/ buffer):");
	if(status <0)
		return 0;
	return status;
}

int jb_recv_datagram_any(int sock, char* buf, int count)
{
	int status;
	memset(buf, 0, count);
	
	status = TEMP_FAILURE_RETRY(recv(sock, &buf, count, 0));
	if(status <0 && errno != ECONNRESET)
		jb_print_error("recvfrom (w/ buffer):");
	if(status <0)
		return 0;
	return status;
}

int jb_recv_datagram(int sock, struct sockaddr_in *addr, int16_t* msg)
{
	int16_t buf;
	int status;
	socklen_t len;
	(*msg)=-1;
	len = sizeof(struct sockaddr_in);
	status = TEMP_FAILURE_RETRY(recvfrom(sock, &buf, sizeof(int16_t), 0, (struct sockaddr*) addr, &len));
	if(status <0 && errno != ECONNRESET)
		jb_print_error("recvfrom:");
	if(status <0)
		return 0;
	(*msg) = ntohs(buf);
	return status;
}

int jb_recv_datagram_buf(int sock, struct sockaddr_in *addr, char* buf, int count)
{
	int status;
	socklen_t len;
	memset(buf, 0, count);
	len = sizeof(struct sockaddr_in);
	
	status = TEMP_FAILURE_RETRY(recvfrom(sock, &buf, count, 0, (struct sockaddr*) addr, &len));
	if(status <0 && errno != ECONNRESET)
		jb_print_error("recvfrom (w/ buffer):");
	if(status <0)
		return 0;
	return status;
}

