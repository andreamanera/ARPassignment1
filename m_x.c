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

/* fd_from_comm / fd_to_mx helps in identifying an open file within a process while using input/output resources like network sockets or pipes */

int main(void)
{	
	
	int ch;
	int fd_from_comm;
	int fd_to_insp;
	
	/* FIFO that reads from command console */
	
	char * myfifo = "/tmp/myfifo"; 
	mkfifo(myfifo, 0666); 
	
	/* FIFO that write to inspection console */

	char * myfifo2 = "/tmp/myfifo2"; 
	mkfifo(myfifo2, 0666); 

	double x = 0.0;
	double x_end = 10.0;
	double error;
	
	/* rimango nel while e ogni volta eseguo solo un if e poi il ciclo si ripete */
	
	while(1){
	
		fd_from_comm = open(myfifo, O_RDONLY);
		fd_to_insp = open(myfifo2, O_WRONLY);
		
		read(fd_from_comm, &ch, sizeof(ch));
		
		error = (double) rand() /(double) (RAND_MAX/0.025);
	
		if (ch == 'a' && x > 0.25){
			x -= 0.25;
			x = x + error; 
			printf("%f\n", x);
			fflush(stdout);	
			write(fd_to_insp, &x, sizeof(x));
		}
		
		if (ch == 'd' && x < 9.75){
			x += 0.25;
			x = x + error; 
			printf("%f\n", x);
			fflush(stdout);	
			write(fd_to_insp, &x, sizeof(x));
		}
		
		if (ch != 'a' && ch != 'd'){
			write(fd_to_insp, &x, sizeof(x));
		}
	
		close(fd_from_comm);
		close(fd_to_insp);
	}
	
	return 0;
}

	
