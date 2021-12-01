#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>
#include <termios.h>
#include <time.h>
#include <sys/wait.h>

#define CHECK(X) (                                                 \
    {                                                              \
        int __val = (X);                                           \
        (__val == -1 ? (                                           \
                           {                                       \
                               fprintf(stderr, "ERROR ("__FILE__   \
                                               ":%d) -- %s\n",     \
                                       __LINE__, strerror(errno)); \
                               exit(EXIT_FAILURE);                 \
                               -1;                                 \
                           })                                      \
                     : __val);                                     \
    })

pid_t pid_m_x;
pid_t pid_m_z;
pid_t pid_command;
pid_t fd_c_to_wd;

void handler(int sig){

	if(sig==SIGUSR1){

		alarm(60);
	}

	if(sig==SIGALRM){

		CHECK(kill(pid_m_x, SIGUSR2));
		CHECK(kill(pid_m_z, SIGUSR2));
		CHECK(kill(pid_command, SIGUSR2));
	}
}

int main(int argc, char * argv[]){

	CHECK(fd_c_to_wd=open(argv[1], O_RDONLY));

	pid_m_x=atoi(argv[2]);
	pid_m_z=atoi(argv[3]);
	
	struct sigaction sa;
	memset(&sa, 0, sizeof(sa));
	sa.sa_handler=&handler;
	sa.sa_flags=SA_RESTART;
	
	alarm(60);
	
	CHECK(sigaction(SIGUSR1, &sa, NULL));
	CHECK(sigaction(SIGALRM,&sa,NULL));
	
	CHECK(read(fd_c_to_wd, &pid_command, sizeof(pid_command)));
	
	while(1){
		
		sleep(1);
	
	}

	CHECK(close(fd_c_to_wd));

	return 0;
}
