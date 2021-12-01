#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>
#include <sys/wait.h>
#include <time.h>

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

int val;

// signals that we send from inspection needed to stop/reset motor x 

void handler(int sig){

	if(sig == SIGUSR1){

		val = 5;
	}

	if(sig == SIGUSR2){

		val = 2;
	}
}

// fd_from_comm / fd_to_mx helps in identifying an open file within a process while using input/output resources like network sockets or pipes 

int main(int argc, char* argv[]){	

	// declaration of needed variables 
	 
	int fd_from_comm;
	int fd_to_insp;
	int retval;

	double x = 0.0;
	double step = 0.1;

	struct timeval tv;
	struct sigaction sa;

	fd_set rdset;

	// pipes opening for reading from command and writing to insp 
	
	CHECK(fd_from_comm = open("/tmp/x", O_RDONLY));
	CHECK(fd_to_insp = open("/tmp/inspx", O_WRONLY));
	
	while(1){
		
		double error = (double) rand() /(double) (RAND_MAX/0.001);
		
		// only in motor the signal will change something 
		
		memset(&sa, 0, sizeof(sa));
		sa.sa_handler=&handler;
		sa.sa_flags=SA_RESTART;
		CHECK(sigaction(SIGUSR1, &sa, NULL));
		CHECK(sigaction(SIGUSR2,&sa,NULL));

		// we want to read instantly the datas from command, not waiting 
	
		tv.tv_sec = 0;
		tv.tv_usec = 0;
		
		// file descriptor of datas that will be ready to be READ 
		
		FD_ZERO(&rdset);
		FD_SET(fd_from_comm, &rdset);
		retval = select(FD_SETSIZE, &rdset, NULL, NULL, &tv);
		
		if (retval == -1){

			perror("select()");
		}
		
		// controll that fd_from_comm is put inside rdset 
		
		else if (retval >= 0){

			if(FD_ISSET(fd_from_comm, &rdset) != 0){

				CHECK(read(fd_from_comm, &val, sizeof(val)));
			}   
		}
		
		switch(val){
							
		    case 1: // case d
		    	if(x >= 5){

				}
					
				else{

					x += step;
					x = x + error;
				}

				CHECK(sleep(1));
		    break;					

		    case 2: // case a
				if (x <= 0){

				}
							
				else{

					x -= step;
					x = x + error;
				}

				CHECK(sleep(1));
        	break;
					
	    	case  5:
				CHECK(sleep(1));
		    break;
		}

	    if (x > 5.0){

			x = 5.0;
		}
			
	    if (x < 0.0){ 

			x = 0.0;
		}
					
	    CHECK(write(fd_to_insp, &x, sizeof(x)));
	
	}
	
	CHECK(close(fd_from_comm));
	CHECK(close(fd_to_insp));
	
	return 0;
}

	
