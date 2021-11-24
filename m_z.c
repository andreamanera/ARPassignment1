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

char ch;

/* signals that we send from inspection needed to stop/reset motor x */

void handler(int sig){
	if(sig==SIGUSR1){
		ch = 'x';
	}
	if(sig==SIGUSR2){
		
		x = 0.0;
		ch = 'z';
	}
}

/* fd_from_comm / fd_to_mx helps in identifying an open file within a process while using input/output resources like network sockets or pipes */

int main(int argc, char* argv[])
{	

	/* declaration of needed variables */
	 
	int fd_from_comm;
	int fd_to_insp;
	int retval;
	double step = 0.1;
	double x = 0.0;
	struct timeval tv;
	struct sigaction sa; 
	fd_set rdset;

	/* pipes opening for reading from command and writing to insp */
	
	fd_from_comm = open(argv[1], O_RDONLY);
	fd_to_insp = open("fd_to_insp_z", O_WRONLY);
		
			
	if(fd_from_comm == -1){
		printf("Error opening FIFO from command to motor z");
		return(1);
	}
	
	if(fd_mx_to_ins == -1){
		printf("Error opening FIFO from motor z to inspection");
		return(1);
	}
	
	while(1){
		
		double error = (double) rand() /(double) (RAND_MAX/0.001);
		
		memset(&sa, 0, sizeof(sa));
		sa.sa_handler=&handler;
		sa.sa_flags=SA_RESTART;
		sigaction(SIGUSR1, &sa, NULL);
		sigaction(SIGUSR2,&sa,NULL);
	
		/* we want to read instantly the datas from command */
	
		tv.tv_sec = 0;
		tv.tv_usec = 0;
		
		/* file descriptor of datas that will be ready to be READ */
		
		FD_ZERO(&rdset);
		FD_SET(fd_from_comm, &rdset);
		retval = select(FD_SETSIZE, &rdset, NULL, NULL, &tv);
		
		if (retval == -1){
			perror("select()");
		}
		
		/* controll that fd_from_comm is put inside rdset */
		
		else if (retval >= 0){
			if(FD_ISSET(fd_from_comm, &rdset) != 0){
				read(fd_from_comm, &ch, sizeof(ch));
			}   
		}
		
		switch(ch){
			                
		    case 119: // case w
		    if(z >= 5){
		    }
		    	else{
		    	
					z += step;
					z += error;
				    
				}
					
					sleep(1);
			break;					

		    case 115: // case s
		    if (z <= 0){
		    }
		    	else{
		    
					z -= step;
					z += error;
		    	}
					
					sleep(1);
			break;
		}
		
			case 120: // case x
			
			sleep(1);
			break;
		}
		
		if (z > 5.0) 
			z = 5.0;
		
		if (z < 0.0) 
			z = 0.0;
		
		write(fd_mx_to_ins, &z, sizeof(x));;
	}
	
	close(fd_from_comm);
	close(fd_to_insp)
	
	return 0;
}

	
